/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loader.c 1.1 1997/12/10 16:41:08 jon Exp $
 * $Locker: $
 *
 * Win32 Image Loader - Can load EXEs and DLLs
 *
 * 
 * XXX Todo:
 * 	Add exceptions
 *	Make more paranoid about reading sections and applying relocations
 *	Don't load unnecessary segments
 *	Throw away any segments that are not needed after loading (relocation)
 */
#include "fw.h"
#include "wincoff.h"

BR_RCS_ID("$Id: loader.c 1.1 1997/12/10 16:41:08 jon Exp $")

#define READ_BLOCK(var,fh) do { if (BrFileRead(&(var), 1, sizeof(var),(fh)) != sizeof(var)) return NULL; } while (0)

/*
 * Attempts to load an image file into memory
 */
br_image *ImageLoad(char *name)
{
	void *fh;
	int mode = BR_FS_MODE_BINARY;
	struct msdos_header dos_header;
	struct coff_header coff_header;
	struct nt_optional_header nt_header;
	struct section_header section_header;
	br_uint_32 pe;
	br_image *img;
	br_uint_8 *arena_base;
	int arena_size,arena_align;
	int i;
	br_uint_32 offset;
	unsigned int n;

	/*
	 * Try name as is.
	 */
	fh = BrFileOpenRead(name,0, NULL, &mode);

	if(fh == NULL)
		return NULL;

	/*
	 * Read MSDOS header
	 */
	READ_BLOCK(dos_header,fh);

	/*
	 * Validate MSDOS header
	 */
	if(dos_header.magic != 0x5A4D)
		return NULL;

	/*
	 * Read PE signature
	 */
	if(dos_header.new_header_offset  < sizeof(dos_header))
		return NULL;

	BrFileAdvance(dos_header.new_header_offset - sizeof(dos_header), fh);

	READ_BLOCK(pe,fh);

	/*
	 * Check magic number
	 */
	if(pe != 0x00004550)
		return NULL;

	/*
	 * Read base COFF header
	 */
	READ_BLOCK(coff_header,fh);

	/*
	 * Check machine type
	 */
	if(coff_header.machine != IMAGE_FILE_MACHINE_I386)
		return NULL;	/* Wrong processor type */

	if(coff_header.flags & IMAGE_FILE_RELOCS_STRIPPED)
		return NULL;	/* No base relocation */

	if(!(coff_header.flags & IMAGE_FILE_EXECUTABLE_IMAGE))
		return NULL;	/* Not executable file */

	if(!(coff_header.flags & IMAGE_FILE_32BIT_MACHINE))
		return NULL;	/* Not 32 bit machine */

	if(coff_header.opt_header_size != sizeof(nt_header))
		return NULL;	/* Expecting NT coff file */
	
	/*
	 * Read optional header 
	 */
	READ_BLOCK(nt_header,fh);

	img = BrResAllocate(NULL,sizeof(*img), BR_MEMORY_IMAGE);

	img->type = BR_IMG_FRAMEWORK;

	img->sections = BrResAllocate(img,
		sizeof(br_image_section) * coff_header.n_sections,
		BR_MEMORY_IMAGE_SECTIONS);

	/*
	 * Check image alignment is a power of 2
	 */
	if((nt_header.section_alignment-1) & nt_header.section_alignment)
		return NULL;

	/*
	 * Read each section header
	 */
	for(i=0; i < coff_header.n_sections; i++) {
		READ_BLOCK(section_header,fh);
	 
		/*
		 * Make copies of the parts of the header that
		 * are needed
		 */
		img->sections[i].name		 = BrResStrDup(img, section_header.section_name);
		img->sections[i].mem_offset	 = section_header.rva;
		img->sections[i].mem_size	 = section_header.virtual_size;
		img->sections[i].data_size 	 = section_header.data_size;
		img->sections[i].data_offset = section_header.data_offset;
	}

	/*
	 * Allocate image arena (aligned)
	 */
	arena_size = nt_header.image_size;
	arena_align = nt_header.section_alignment;

	arena_base = BrResAllocate(img, arena_size + (arena_align-1), BR_MEMORY_IMAGE_ARENA);
	arena_base = (br_uint_8 *) (((int)arena_base+arena_align-1) & (~(arena_align-1)));

	/*
	 * Remember current offset into file
	 */
	offset = dos_header.new_header_offset + 
		sizeof(pe) +
		sizeof(coff_header) +
		sizeof(nt_header) +
		coff_header.n_sections * sizeof(section_header);

	/*
	 * Read each section into image arena
	 */
	for(i=0; i < coff_header.n_sections; i++) {

		/*
		 * Ignore sections that have no raw data
		 */
		if((img->sections[i].data_offset == 0) ||
		   (img->sections[i].data_size == 0))
			continue;

		/*
		 * make sure we only ever go forwards in file
		 */
		if(offset > img->sections[i].data_offset)
			return NULL;	/* Section data not in order */

		/*
		 * XXX Check that block is within arena
		 */

		/*
		 * Advance to start of section in file
		 */
		BrFileAdvance(img->sections[i].data_offset - offset, fh);
		
		/*
		 * Record pointer to start of section
		 */
		img->sections[i].base = arena_base+img->sections[i].mem_offset;

		/*
		 * Read the section into memory
		 */
		if((br_uint_32)BrFileRead(img->sections[i].base, 1, img->sections[i].data_size,fh)
		 != img->sections[i].data_size)
			return NULL;

		offset = img->sections[i].data_offset + img->sections[i].data_size;
	}

	/*
	 * Close file
	 */
	BrFileClose(fh);

	/*
	 * Exports
	 */
	if(nt_header.directories[DIRECTORY_EXPORT].size != 0) {
		struct export_directory *ed;

		ed = (export_directory *)(arena_base +
			nt_header.directories[DIRECTORY_EXPORT].rva);

		/*
		 * Fill in image structure with info from export directory
		 */
		img->identifier = (char *)(arena_base + ed->name);
		img->ordinal_base = ed->ordinal_base;
		img->n_functions = ed->n_entries;
		img->functions = (void **)(arena_base + ed->export_table);
		img->n_names = ed->n_names;
		img->names = (char **)(arena_base + ed->name_table);
		img->name_ordinals = (br_uint_16 *)(arena_base + ed->ordinal_table);

		/*
		 * Relocate the tables of pointers
		 */
		for(n=0; n < img->n_functions; n++)
			img->functions[n] = (void *)((br_uint_32)(img->functions[n]) + arena_base);

		for(n=0; n < img->n_names; n++)
			img->names[n] = (char *)((br_uint_32)(img->names[n]) + arena_base);
	}

	/*
	 * Imports
	 */
	if(nt_header.directories[DIRECTORY_IMPORT].size != 0) {
		struct import_directory *id;
		void **at;
		br_uint_32 *lt;
		br_image *import_img;

		/*
		 * Count number of import DLLs
		 */
		n = 0;
		for(id = (import_directory *)(arena_base +
				nt_header.directories[DIRECTORY_IMPORT].rva);
			id->lookup_table;
			id++)
			n++;

		/*
		 * Allocate a table of pointers to imported DLL names
		 */
		img->imports = BrResAllocate(img, n * sizeof(*img->imports), BR_MEMORY_IMAGE_NAMES);
		img->n_imports = n;

		/*
		 * For each entry in the directory table...
		 */
		for(n = 0, id = (import_directory *)(arena_base +
				nt_header.directories[DIRECTORY_IMPORT].rva);
			id->lookup_table;
			id++, n++) {

			/*
			 * Get a handle onto referenced DLL
			 */
			import_img = BrImageReference((char *)(id->name + arena_base));
			if(import_img == NULL) {
				BR_ERROR1("Could not reference image %s",
					(char *)(id->name + arena_base));
				return NULL; /* could not find DLL */
			}

			img->imports[n] = import_img;

			/*
			 * For each  entry in lookup table
			 */
			at = (void **)(arena_base + id->address_table);

			for(
				lt = (br_uint_32 *)(arena_base + id->lookup_table);
				*lt; lt++, at++) {
				if(*lt & 0x80000000) 
					*at = BrImageLookupOrdinal(import_img, *lt & 0x7fffffff);

    				if(*at == NULL) {
       					BR_ERROR2("Could not resolve imported ordinal %s:%d",
           					(char *)(id->name + arena_base),*lt & 0x7fffffff);

					    return NULL;
                    }
				else {
					*at = BrImageLookupName(import_img, (char *)(*lt+arena_base+2),
						*((br_uint_16 *)(*lt+arena_base)));
    				if(*at == NULL) {
          					BR_ERROR3("Could not resolve imported symbol %s:%s (%d)",
		        			(char *)(id->name + arena_base),
	    					(char *)(*lt+arena_base+2), *((br_uint_16 *)(*lt+arena_base)));

					    return NULL; /* Could not resolve symbol */
       				}
                }
			}
		}
	}

	/*
	 * Base Relocation (only if the base needs to be moved)
	 */
	if(((br_uint_32)arena_base != nt_header.image_base) &&
		(nt_header.directories[DIRECTORY_BASERELOC].size != 0)) {

		basereloc_header *header;
		br_uint_16 *entry;
		br_uint_8 *fixup;
		br_int_32 delta;
		br_int_16 delta_h, delta_l;

		offset = 0;
		delta = (br_int_32)arena_base - nt_header.image_base;
		delta_h = (br_uint_16)(delta >> 16);
		delta_l = (br_uint_16)(delta & 0xFFFF);

		/*
		 * Loop for each block in section
		 */
		while(offset < nt_header.directories[DIRECTORY_BASERELOC].size) {
			header = (basereloc_header *)
				(nt_header.directories[DIRECTORY_BASERELOC].rva + arena_base + offset);
			offset += header->size;

			/*
			 * Loop for each entry in block
			 */
			for(n=0, entry = (br_uint_16 *)(header+1);
				n < (header->size - sizeof(basereloc_header)) / sizeof(br_uint_16);
				n++, entry++) {

				fixup = ENTRY_OFFSET(*entry) + header->rva + arena_base;

				/*
				 * Check fixup is within arena
				 */
				if(fixup >= arena_base + arena_size)
					return NULL;					/* Fixup overrun */

				/*
				 * Apply delta
				 */
				switch(ENTRY_TYPE(*entry)) {
				case IMAGE_REL_BASED_ABSOLUTE:
					/* Do nothing */
					break;

				case IMAGE_REL_BASED_HIGH:
					*(br_int_16 *)fixup += delta_h;
					break;

				case IMAGE_REL_BASED_LOW:
					*(br_int_16 *)fixup += delta_l;
					break;

				case IMAGE_REL_BASED_HIGHLOW:
					*(br_int_32 *)fixup += delta;
					break;

				case IMAGE_REL_BASED_HIGHADJ:
				case IMAGE_REL_BASED_MIPS_JMPADDR:
					/* XXX todo */

				default:
					return NULL;					/* Unknown fixup type */
				}
			}
		}
	}

	return img;
}
