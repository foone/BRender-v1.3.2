# Remove structure alignment from some structures
#
while (<>) {
	s/(br_fvector[234](_f|_x)?\s*STRUCT)\s*4t/\1 ; 4t H2INCFIX/;
	s/(fmt_vertex(_f|_x)?\s*STRUCT)\s*4t/\1 ; 4t H2INCFIX/;
	s/(v11face(_f|_x)?\s*STRUCT)\s*4t/\1 ; 4t H2INCFIX/;
	s/(v11group(_f|_x)?\s*STRUCT)\s*4t/\1 ; 4t H2INCFIX/;

	print;
}
