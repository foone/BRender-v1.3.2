# Convert a C++ class file into a C header file
#
# makes alot of assumptions about the layout of the C++ file
#

while(<STDIN>) {

	# 'class' <class_name> [: ['public'] <super_class_name> ] '{'
	#
	if(/^\s*class\s+(\w+)\s*(:\s*(public)?\s*(\w*))?\s*{/) {
		$class = $1;
		$super = $4;
#		print "Class '$1' super='$4'\n";

		push(@classes,$class);

		# Construct class macro name
		#

		# Build macro name
		#
		@class_words = split("_",$class);

		# See if first word of class name is some generic prefix
		# and drop it if so
		#
		if($class_words[0] eq "br" || $class_words[0] eq "brp") {
			shift(@class_words);
		}

		$macro_name = "";
		foreach $w (@class_words) {
			$macro_name .= "\u$w";
		}

		$class_macro_names{$class} = $macro_name;

		# Initialise methods list from superclass
		#
		@this_methods = split(" ",$class_methods{$super});

		# Read class definition 
		#
		while(<STDIN>) {
			last if(/^\s*}/);

			# 'virtual' <return type> 'BR_METHOD' <name> '(' <args> [= 0] [';']
			#
			if(/^\s*virtual\s+(.*)\s+BR_METHOD\s+(\w+)\s*\(([^;=]*)\s*(=\s*0)?\s*(;)?\s*$/) {

				$method = "$class\:\:$2";
				$args = $3;

				push(@this_methods,$method);

				$method_return{$method} = $1;

				# Read any remaining argument lines until a terminating semicolon is found
				#
				if(!$5) {
					while(<STDIN>) {
						if(/\s*([^;=]*)\s*(=\s*0)?\s*(;?)/) {
							$args .= $1;

							last if($3);
						}
					}
				}

				# Tidy up argument list:
				#	squeeze out white space
				#	remove trailing ')'
				#	change '&' to '*' (references to pointers)
				#	change 'class' to 'struct'
				#	remove any leading and trailing space
				#
				$args =~ tr/\n\t \r/ /s;
				$args =~ s/\)\s*$//;
				$args =~ s/\&/\*/;
				$args =~ s/^\s*(.*)\s*/\1/;

				$method_args{$method} = $args if($args ne "void");

#				print "	   method[$method] args[$method_args{$method}] ret[$method_return{$method}]\n";
 			}

			# Any line with CG_PUBLIC in it gets added to front of class
			#
			if(/CG_PUBLIC/) {
				s/CG_PUBLIC//g;

				$class_public{$class} .= $_;
			}
		}

		# Remember class list
		#
		$class_methods{$class} = join(" ",@this_methods);

	}
}

# Start building output file
#

print <<END;

/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Automatically generated by classgen.pl
 */
#ifndef _\U$ARGV[0]\E_H_
#define _\U$ARGV[0]\E_H_

#ifdef __cplusplus
extern "C" {
#endif

#define class struct

END

# Class declarations
#
foreach $class (@classes) {
	print "struct  $class;\n";
}
print "\n\n";

# Classes
#
foreach $class (@classes) {
	@this_methods = split(" ",$class_methods{$class});

	print <<END;
struct  ${class}_dispatch;

#ifndef \U${class}\E_PRIVATE
typedef struct $class {
	struct  ${class}_dispatch *dispatch;
	$class_public{$class}
} $class;
#endif

struct  ${class}_dispatch {

END

	foreach $method (@this_methods) {
		$ret  = $method_return{$method};
		$args = $method_args{$method};

		($m_class, $m_name) = split("::",$method);

	# Generate vtbl entry
	#
		print "\t$ret BR_CMETHOD_PTR_DECL($m_class, $m_name)(\n";

		print "\t\tstruct $m_class *self";

		foreach $arg (split(",",$args)) {
			$arg =~ s/^\s*(.*)\s*/\1/;
			print ",\n\t\t$arg";
		}

		print ");\n";

	}

	print "};\n";
};

# Generate C macros for calling methods
#
foreach $method (sort(keys(%method_return))) {
		@args = split(",",$method_args{$method});
		($m_class, $m_name) = split("::",$method);

		# Construct comma seperated list of arguments for macro
		#
		$alist = "";
		foreach $a ((1 .. @args)) {
			$alist .= ", a$a";
		}

		$define_name = "$class_macro_names{$m_class}\u$m_name";

		print "#define $define_name(self$alist) BR_CMETHOD_CALL($m_class,$m_name,self)(($m_class *)self$alist)\n";
}

# Generate templates for dispatch tables
#

print <<END;
/*
 * Example templates for class dispatch tables
 */
#if 0
END

foreach $class (@classes) {
	@this_methods = split(" ",$class_methods{$class});

	print <<END;
/*
 * Example dispatch table for $class
 */
static struct ${class}_dispatch \l$class_macro_names{$class}Dispatch = {
END

	foreach $method (split(" ",$class_methods{$class})) {
		($m_class, $m_name) = split("::",$method);

		printf("\tBR_CMETHOD_REF(%-32s %s),\n", $m_class . "," , $m_name);
	}

	print "};\n\n";

}

print <<END;
#endif

#undef class

#ifdef __cplusplus
};
#endif
#endif
END
