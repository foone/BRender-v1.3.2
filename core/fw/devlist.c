/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devlist.c 1.1 1997/12/10 16:41:06 jon Exp $
 * $Locker: $
 *
 * XXX Need to keep track of images that devices came from and dereference
 * them in BrDevRemove
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: devlist.c 1.1 1997/12/10 16:41:06 jon Exp $")

extern br_framework_state BR_ASM_DATA fw ;

/* Add drivers specified in the BRENDER_DRIVERS configuration entry 
 * This is only done once, so check it has not already been done    
 */

static br_error AddRequestedDrivers()
{
	char devstr[256];
    static br_boolean bAlreadyDone = BR_FALSE ;

    if ( !fw.bAlreadyLoadedDrivers )
    {
       BrSystemConfigQueryString( BRT_BRENDER_DRIVERS_STR, devstr, BR_ASIZE(devstr) );

       if ( *devstr != '\0' )
          BrDevAddConfig(devstr);
      
       fw.bAlreadyLoadedDrivers = BR_TRUE ;
    }

    return BRE_OK ;
}




/*
 * Instantiate a device and add it to internal list given entry point
 */
static br_error devAdd(br_device **pdev, br_device_begin_fn *dev_begin, char *args, br_image *image)
{
	int i;
	br_device *dev;
	struct br_open_device *new_slots;

	/*
	 * Cal the driver entry point, which should return a pointer to
	 * device object, or NULL if things went wrong
	 */
	dev = dev_begin(args);

	if(dev == NULL)
		return BRE_FAIL;
	
	/*
	 * Got device, find free slot
	 */
	for(i=0; i < fw.ndev_slots; i++) {
		if(fw.dev_slots[i].dev == NULL) {
			/*
			 * Fill in slot, including optional image pointer
			 */
			fw.dev_slots[i].dev = dev;
            fw.dev_slots[i].image = image;

			if(pdev)
				*pdev = dev;
			return BRE_OK;
		}
	}

	/*
	 * Need to expand array
	 */
	new_slots = BrResAllocate(dev, (fw.ndev_slots+NDEV_SLOTS) * sizeof(*fw.dev_slots), BR_MEMORY_DRIVER );

	if(new_slots == NULL) {
		ObjectFree(dev);
		return BRE_NO_MEMORY;
	}

	for(i=0; i < fw.ndev_slots; i++) 
		new_slots[i] = fw.dev_slots[i];

	BrResFree(fw.dev_slots);

	/*
	 * Fill in slot, including optional image pointer
	 */
	fw.dev_slots = new_slots;
	new_slots[fw.ndev_slots].dev = dev;
	new_slots[fw.ndev_slots].image = image;

	fw.ndev_slots += NDEV_SLOTS;

	if(pdev)
		*pdev = dev;

	return BRE_OK;
}

/*
 * Instantiate a new device and add it to internal list given image name
 */
br_error BR_RESIDENT_ENTRY BrDevAdd(br_device **pdev, char * image, char *args)
{
	br_image *dev_image;
	br_device_begin_fn *dev_begin;

	ASSERT(image != NULL);

	/*
	 * Try and load up device
	 */
	dev_image = BrImageReference(image);
	
	if(dev_image == NULL) {
		return BRE_FAIL;
	}

	/*
	 * Look up the driver entry point
	 */
	dev_begin = BrImageLookupName(dev_image, "BrDrv1Begin",0);

	if(dev_begin == NULL) {
        BrImageDereference(dev_image);
		return BRE_FAIL;
    }

	/*
	 * Add device to internal list, remebering the iamge pointer for later
	 */
	return devAdd(pdev, dev_begin, args, dev_image);
}

/*
 * Add a driver that is already in memory by some other means (like being statically linked)
 */
br_error BR_RESIDENT_ENTRY BrDevAddStatic(br_device **pdev, br_device_begin_fn *dev_begin, char *args)
{
	/*
	 * Add to internal list, with a NULL image pointer
	 */
    return devAdd(pdev, dev_begin, args, NULL);
}

/*
 * See if a device is loaded, if not, load it
 */
br_error BR_RESIDENT_ENTRY BrDevCheckAdd(br_device **pdev, char *name, char *args)
{
	/*
	 * Is device already there?
	 */
	if(BrDevFind(pdev, name) == BRE_OK)
		return BRE_OK;

	/*
	 * Add it
	 */
	return BrDevAdd(pdev, name, args);
}

/*
 * Take a config string and split it into <dev_args>{;<dev_args>}
 * 
 * Add each device in turn with it's arguments
 */
br_error BR_RESIDENT_ENTRY BrDevAddConfig(char *config)
{
	char *end, *dev, *arg;
	char tmp[512];

	int n;

	for(;;) {
		/*
		 * Find next semi. or end of string
		 */
		for(end = config; *end != '\0' && *end != ';' ; end++)
			;
		
		n = end - config;
		
		if(n) {
			/*
			 * Make a copy of this device string
			 */	
			BrMemCpy(tmp, config, n);
			tmp[n] = '\0';
			dev = tmp;

			/*
			 * Find device arguments
			 */
			for(arg = tmp; *arg != '\0' && *arg != ',' && *arg != ':'; arg++)
				;

			if(*arg) {
				*arg = '\0';
				arg++;
			}

			/*
			 * Add the device
			 */
			while(BrIsSpace(*dev))
				dev++;

			n = BrStrLen(dev);
			
			if(n) {
				while(BrIsSpace(dev[n-1])) {
					dev[n-1] = '\0';
					n--;
				}
				
				BrDevAdd(NULL, dev, arg);
			}
		}

		if(*end == ';')
			config = end+1;
		else
			break;
	}

	return BRE_OK;
}

/*
 * Close down device and remove it from list
 */
br_error BR_RESIDENT_ENTRY BrDevRemove(br_device *dev)
{
	int i;

	/*
	 * Find device in list
	 */
	for(i=0; i < fw.ndev_slots; i++) {
		if(fw.dev_slots[i].dev == dev) {

			/*
			 * Found it - release object
			 */
			ObjectFree(dev);

			/*
			 * If there was an image associated with device, dereference it
			 */
			if(fw.dev_slots[i].image) {
                BrImageDereference(fw.dev_slots[i].image);
			}

			fw.dev_slots[i].dev = NULL;
			fw.dev_slots[i].image = NULL;
		}
	}

	return BRE_OK;
}

/*
 * Find a named device
 */
br_error BR_RESIDENT_ENTRY BrDevFind(struct br_device **pdev, char *pattern)
{
	int i,c;

   AddRequestedDrivers();

	for(i=0,c=0; i < fw.ndev_slots; i++) {
		if(fw.dev_slots[i].dev) {
			if(BrNamePatternMatch(pattern,ObjectIdentifier(fw.dev_slots[i].dev))) {
				if(pdev)
					*pdev = fw.dev_slots[i].dev;
				return BRE_OK;
			}				
		}
	}

	return BRE_FAIL;
}

/*
 * Fill in an array of pointers to loaded devices mathcing pattern
 */
br_error BR_RESIDENT_ENTRY BrDevFindMany(struct br_device **devices, br_int_32 *ndevices, br_int_32 max_devices, char *pattern)
{
	int i,c;

   AddRequestedDrivers();

	for(i=0,c=0; i < fw.ndev_slots; i++) {
		if(fw.dev_slots[i].dev &&
			BrNamePatternMatch(pattern,ObjectIdentifier(fw.dev_slots[i].dev))) {

			if(++c >= max_devices)
				return BRE_OVERFLOW;

			devices[c] = fw.dev_slots[i].dev;
		}
	}

	if(ndevices)
		*ndevices = c;

	return BRE_OK;
}

/*
 * Find number of loaded devices
 */
br_error BR_RESIDENT_ENTRY BrDevCount(br_int_32 *ndevices, char *pattern)
{
	int i,c;

   AddRequestedDrivers();

	for(i=0,c=0; i < fw.ndev_slots; i++)
		if(fw.dev_slots[i].dev &&
			BrNamePatternMatch(pattern,ObjectIdentifier(fw.dev_slots[i].dev)))
			c++;

	if(ndevices)
		*ndevices = c;

	return BRE_OK;
}

/*
 * Search over all loaded devices for a type of object
 */
br_error BR_RESIDENT_ENTRY BrDevContainedFind(struct br_object **ph, br_token type, char *pattern, br_token_value *tv)
{
	int i;

   AddRequestedDrivers();

	for(i=0; i < fw.ndev_slots; i++)
		if(fw.dev_slots[i].dev && (ObjectContainerFind(fw.dev_slots[i].dev, ph, type, pattern, tv) == BRE_OK))
				return BRE_OK;

	return BRE_FAIL;
}

br_error BR_RESIDENT_ENTRY BrDevContainedFindMany(struct br_object **objects, br_int_32 max_objects,
			br_int_32 *pnum_objects, br_token type, char *pattern, br_token_value *tv)
{
	int i;
	br_int_32 n,total = 0;
	br_error r;

   AddRequestedDrivers();

	for(i=0; i < fw.ndev_slots; i++) {
		if(fw.dev_slots[i].dev) {
			r = ObjectContainerFindMany(fw.dev_slots[i].dev, objects, max_objects, &n, type, pattern, tv);
			if(r != BRE_OK)
				return r;

			objects += n;
			max_objects -= n;
			total += n;
		}
	}

	if(pnum_objects)
		*pnum_objects = total;

	return BRE_OK;
}

br_error BR_RESIDENT_ENTRY BrDevContainedCount(br_int_32 *pcount, br_token type, char *pattern, br_token_value *tv)
{
	int i;
	br_int_32 n,total = 0;
	br_error r;

   AddRequestedDrivers();

	for(i=0; i < fw.ndev_slots; i++) {
		if(fw.dev_slots[i].dev) {
			r = ObjectContainerCount(fw.dev_slots[i].dev, &n, type, pattern, tv);
			if(r != BRE_OK)
				return r;

			total += n;
		}
	}

	if(pcount)
		*pcount = total;

	return BRE_OK;
}
