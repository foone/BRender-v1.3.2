#!/bin/perl
#
# Generate static initialisers for a resident image
#

# Consume each line of input
#
@functions = ();

while(<STDIN>) {
	# Ignore comments
	#
	next if(/^#/);

	# ='<module_name>'
	#
	if(/^=\s*(\w\w*)/) {
		$base = $1;
		next;
	}

	# <class>:<method>
	# <class>::<method>
	#
	if(/^\s*(\w\w*)::?(\w\w*)/) {
		push(@functions,"_M_$1_$2");
		next;
	}

	# <function_name>
	#	
	if(/\w\w*/) {
		push(@functions,$&);
		next;
	}

}

@functions = sort(@functions);

# Print out list of functions
#
#$n = 0;
#foreach $fn (@functions) {
#	print "Function $n $fn\n";
#	$n++;
#}

open(STDOUT,">$ARGV[0]");

# Print table of names
#
print <<END;
/*
 * Table of exported names
 */
static char * namePointers_$base\[\] = {
END

foreach $fn (@functions) {
	print "\t\"_$fn\",\n";
}

print <<END;
};

END


# Print table of ordinals 
#
print <<END;
/*
 * Table of ordinals
 */
static br_uint_16 nameOrdinals_$base\[\] = {
END

$n = 0;
foreach $fn (@functions) {
	print "\t$n,\n";
	$n++;
}

print <<END;
};

END

# Print table of entry points
#
print <<END;
/*
 * Table of ordinals
 */
static void * functionPointers_$base\[\] = {
END

$n = 0;
foreach $fn (@functions) {
	printf("\t%-64s, /* %d */\n", $fn ,$n);
	$n++;
}

print <<END;
};

static br_image Image_$base = {
	{0},
	"$base",						/* Identifier				*/
	BR_IMG_RESIDENT,				/* Image type				*/
	0,								/* Reference count			*/

	1,									/* Ordinal base				*/
	BR_ASIZE(functionPointers_$base),	/* Number of functions		*/
	functionPointers_$base,				/* Table of functions		*/

	BR_ASIZE(namePointers_$base),	/* Number of names			*/
	namePointers_$base,				/* Table of names 			*/
	nameOrdinals_$base,				/* Table of name ordinals	*/
	
	0,								/* Number of imports		*/
	NULL,							/* Table of imports			*/

	0,								/* Number of sections		*/
	NULL,							/* Table of sections		*/

	NULL,							/* Type pointer				*/
};

END

close(STDOUT);

# Generate librarian scripts for import libraries
#
open(STDOUT,">$ARGV[1]");

# Microsoft LIB
#
if($ARGV[2] eq "m") {
	print "LIBRARY $base\n";
	print "EXPORTS\n";
	foreach $fn (@functions) {
		print "${fn}\n";
	}
}

# Watcom WLIB
#
if($ARGV[2] eq "w") {
	foreach $fn (@functions) {
		print "++_${fn}.$base\n";
	}
}

close(STDOUT);
