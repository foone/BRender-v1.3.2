#
# Generate initial data structures and header file for pre-defined tokens
#

$token_prefix = "BRT_";
$current = 1;

# Read existing token ids
#

open(DB,"<tokens.db");

while (<DB>) {
	# Ignore comments
	#
	next if(/^#/);

	if(/^=(\w\w*)/) {
		$current = $1;
		next;
	}

	# name type id
	# 
	if(/\s*(\w\w*)\s*(\w\w*)\s*(\w\w*)/) {
		$token_val{$1} = $2;
		$token_id{$1} = $3;
	}
}

close(DB);

# Read all tokens into an array
#
while (<>) {

	# Ignore comments
	#
	next if(/^#/);

	# Look for a type definition
	#
	if(/^\$\s*(\w\w*)\s*(\w*)/) {
		# Add type to type table
		#
		$name = "\U$1";

		$type_ext{$1} = $2;
		$type_tok{$1} = $name;

		# Create token for type
		#
		if(!$token_val{$name}) {
			$token_val{$name} = "none";
			$token_id{$name} = $current++;
		}

		next;
	}

	# Look for a token definition
	#
	if(/\s*(\w\w*)\s*(\w\w*)/) {
		die "Unknown type: $2" unless $type_tok{$2} ;

		$name = "\U$1$type_ext{$2}";
		$value = $2;

		if(!$token_val{$name}) {
			$token_val{$name} = $value;
			$token_id{$name} = $current++;
		}
	}
}

#foreach $t (sort(keys %token_val)) {
#	print "$t = $token_val{$t} $token_id{$t} $token_len{$t}\n";
#}

# Header file
#
open(STDOUT,">pretok.h");

	print <<END;
/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Predefined tokens generated via tokgen.pl
 */
#if BR_TOKEN_ENUM

typedef enum {
END

foreach $t (sort(keys %token_val)) {
	printf("	%-30s = %8s,\n",
		"$token_prefix$t",
		"$token_id{$t}");
}
	print <<END;
	_BRT_MAX = 0x7ffffff	/* Forces enum to be 32 bit */
} br_token;
#else
END

foreach $t (sort(keys %token_val)) {
	printf("#define %-30s %8s\n",
		"$token_prefix$t",
		"$token_id{$t}");
}

	print <<END;
#endif
END

# Spit out some macros for glueing a token base name to a type
#
foreach $t (sort(keys %type_tok)) {
	print("#define BRT_AS_$type_tok{$t}(tok)	BRT_##tok##$type_ext{$t}\n") if ($type_ext{$t});
}
close(STDOUT);

# Predefined table
#
open(STDOUT,">pretok.c");

foreach $t (sort(keys %token_val)) {
	$len = length($t) - length($type_ext{$token_val{$t}});

	printf("\t{ {0,}, %-32s%-24s%-32s%8d},\n",
		"\"$t\",",
		"$token_prefix$type_tok{$token_val{$t}},",
        "$token_prefix$t,",
		$len);
}

print "#define NEXT_FREE_TOKEN $current\n" ;

close(STDOUT);

# Type table, sorted by suffix length, largest first
#
open(STDOUT,">toktype.c");

foreach $t (sort({length($type_ext{$b}) <=> length($type_ext{$a})} keys %type_tok)) {

	printf("\t{ %-24s%-12s%8d},\n",
	    "\"$type_ext{$t}\",",
	    "$token_prefix$type_tok{$t},",
	    length($type_ext{$t}));
}

close(STDOUT);

# Dump out a new database
#
open(STDOUT,">tokens.new");

foreach $t (sort(keys %token_val)) {
	printf("%s %s %s\n", $t, $token_val{$t}, $token_id{$t});
}
printf("=%d\n",$current);

close(STDOUT);
