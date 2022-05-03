#
# Expand a template according to substitution
#
# $Id: makegen.pl 1.1 1997/12/10 16:43:29 jon Exp $
# $Locker: $
#
# The input script has the following form:
#
# Input is line based, '#' indicates a comment to the end of the line
#
# ?<identifier> ;<pattern>;<replacement>;	# rewrite all values associated with identifier
# <identifier>=<value>						# associate a new value with identifier
#


# Parse input file into identifers and associated rewrite rules
#
while(<STDIN>) {

	# Ignore comments
	#
	next if(/^#/);

	if(/^\s*\?(\w+)\s+;(.*);\s*/) {
		if($identifiers_rewrite{$1}) {
			$identifiers_rewrite{$1} .= " " . $2;
		} else {
			$identifiers_rewrite{$1} = $2;
		}
	}

	if(/^\s*(\w+)\s+(.*)\s*/) {
		($ident, $value) = ($1,$2);

		# Apply any rewrite rules to the value
		#
		foreach $rw (split(" ",$identifiers_rewrite{$ident})) {
			($p,$r) = split(";",$rw);

			$value =~ s/$p/$r/g;
		}

		# Accumulate string of values
		#
		if($identifiers{$ident}) {
			$identifiers{$ident} .= " " . $value;
		} else {
			$identifiers{$ident} = $value;
		}
	}
}

# Insert information into compiler specific makefile template
#
# For every line that contains {{identifier}}, output it for each occurence of the identifer
# in the input, with {{identifier}} replace with the value
#
# For every line that contains {?identifier?}, if identifier has any values, output it with
# {?identifier?} part stripped out
#
# For every line that contains {!identifier!}, if identifier has any no values, output it with
# {!identifier!} part stripped out
#

open(template,$ARGV[0]) || die "Cannot open template!\n";

while(<template>) {

	if(/^(.*){{(\w*)}}(.*)$/) {
		# If this is a substitution line, duplicate it for each value of idenitifer
		#
		($head,$ident,$tail) = ($1,$2,$3);

		foreach $value (split(" ", $identifiers{$ident})) {
			print "$head$value$tail\n";
		}

	} elsif(/^(.*){\?(\w*)\?}(.*)$/) {
		# If this is a true conditional line and the identifer has 1 or more values,
		# output the line with the {?identifier?} part stripped out
		#
		($head,$ident,$tail) = ($1,$2,$3);

        print "$head$tail\n" if ($identifiers{$ident});

	} elsif(/^(.*){\!(\w*)\!}(.*)$/) {
		# If this is a false conditional line and the identifer has 1 or more values,
		# output the line with the {?identifier?} part stripped out
		#
		($head,$ident,$tail) = ($1,$2,$3);

        print "$head$tail\n" if (!$identifiers{$ident});

	} else {
		print;
	}
}
