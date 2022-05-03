# Takes a list of assembler identifiers, and produces
# a header file that has any comiler dependant
# glue for linking to ASM functions
#

# Pull in all the identifiers
#
@idents = <ARGV>;

# Prologue
#
print "\
/*\
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.\
 *\
 * Extra glue for C<->ASM generated via gemasmpr.pl\
 */\
\
#ifdef __cplusplus\
	extern \"C\" {\
#endif\
";

# IBM CSet++
#
print "\n#ifdef __IBMC__\n";
for (@idents) {
	next if(!/^\w+/);
	print "#pragma map($&,\"_$&\")\n";
}
print "#endif /* __IBMC__ */\n";

# Metaware HighC
#
print "\n#ifdef __HIGHC__\n";
for (@idents) {
	next if(!/^\w+/);
	print "#pragma Alias($&,\"_$&\")\n";
}
print "#endif /* __IBMC__ */\n";

# Epilogue
#
printf "\
#ifdef __cplusplus\
	};\
#endif\
"

