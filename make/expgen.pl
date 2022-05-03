#!/bin/perl
#
# Generate linker script for exported symbols
#

# Consume each line of input
#
@symbols = ();


# Allows math type to be appended to image name. Fixes memory leak.



if ( $#ARGV == 1 )
{
   $postfix = $ARGV[1];
}


while(<STDIN>) {
	# Ignore comments
	#
	next if(/^#/);

	# ='<module_name>'
	#
	if(/^=\s*(\w\w*)/) {
		$base = $1 . $postfix ;
		next;
	}


	# <symbol>
	#	
	if(/\w\w*/) {
		push(@symbols,$&);
		next;
	}
}

@symbols = sort(@symbols);

# Print out list of symbols
#
#$n = 0;
#foreach $sym (@symbols) {
#	print "Symbol $n $sym\n";
#	$n++;
#}

#open(STDOUT,">$ARGV[0]");

# Generate Watcom WLINK script
#
if($ARGV[0] eq "w") {
	foreach $sym (@symbols) {
		print "export _$sym\n";
	}
}

if($ARGV[0] eq "m") {
	print "LIBRARY $base\n";
	print "EXPORTS\n";
	foreach $sym (@symbols) {
		print "$sym\n";
	}
}

#close(STDOUT);

