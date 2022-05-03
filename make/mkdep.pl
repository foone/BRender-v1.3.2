#############################################################
#
# File    : MKDEP 
# Version : 3.0
# Author  : Steve Williams
# Author  : Sam Littlewood
#
# Purpose : Generate source file dependency lists.
#
#############################################################

# Add include directories from environment
#
@include_path = split(";",$ENV{"INCLUDE"});

# Process the comand line
#
$ignore_sys_flag = 0;
@files = ();
$ext = "obj";
$outdir = "";
&Process_Command_Line(@ARGV);

# Make sure include directories have no trailing seperator
#
@include_path = grep(s/[\\\/]?$//,@include_path);

#print STDERR join("\n",@include_path) . "\n";

# Go through input files and find dependencies
#
foreach $f (@files) {

	next if( $f eq "");

	$base = $f;
	$base =~ s/^(.*[\/\\:])?([^.\\\/]+)(\.[^.]*)?$/$2/;
	print "$outdir$base.$ext: $f\\\n";
	
	print "\t" . join("\\\n\t",sort(split(" ",&FindDependencies($f)))) . "\n\n";
}

exit;


# Return a whitespace separated list of dependencies for the named file
#
sub FindDependencies
{
	local ($file) = @_;
	local (@includes);
	local (%deps);
	local ($i);
	local ($j);
	local ($f);

	if(!$dependencies{$file}) {

		# Have not seen this file before - scan it
		#

		$dependencies{$file} = " "; # dummy value to trap self-inclusion

		@includes = &FindIncludes($file);

		%deps = ();

		foreach $f (@includes) {
			$i = &QualifyInclude($f);
			$i =~ tr/\\/\//;
			$deps{$i} = 1;

			# Add dependencies from included files
			#
			foreach $j (split(" ",&FindDependencies($i))) {
				$deps{$j} = 1;
			}
		}

		$dependencies{$file} = join(" ", keys(%deps));
	}

	return $dependencies{$file};
}


# FindIncludes
#
# Given a file, extract a list of included file names
#
sub FindIncludes
{
	local ($first);
	local ($file_name) = @_;
	local (@include_list,@contents);
	local ($file_ext);

	$file_ext = $file_name;
	$file_ext =~ s/^.*\.([^.]*)$/\1/;

	# If first line has {{NO_DEPENDENCIES}} in it, then ignore this file
	#

	open(F,$file_name) || return () ;
	$first = <F>;
	close(F);

	return() if ($first =~ /\{\{NO_DEPENDENCIES\}\}/);

	# Scan rest of file
	#
	open(F,$file_name) || return () ;

	if($file_ext =~ /c|cpp|h|hpp|cxx|hxx/i) {
		if($ignore_sys_flag) {
			@include_list = grep (/^\s*\#\s*include\s*\"\s*(\S*)\s*\".*$/ && ($_ = $1),<F>);
		} else {
			@include_list = grep (/^\s*\#\s*include\s*[<"]\s*(\S*)\s*[>"].*$/ && ($_ = $1),<F>);
		}
	} elsif($file_ext =~ /s|asm|inc/i) {
		@include_list = grep (/^\s*include\s*['"]?\s*(\S*)\s*['"]?.*$/ && ($_ = $1),<F>);
	} else {
		@include_list = ();
	}

	close(F);

	return @include_list;
}

# QualifyInclude
#
# Given a file name, try to fully qualify it, possibly using include paths
#
sub QualifyInclude
{
	local ($f) = @_;
	local ($dir);

	return "$f" if( -e "$f");

	foreach $dir (@include_path) {
		return "$dir/$f" if( -e "$dir/$f");
	}
	return $f;
}

sub Expand_Wildcard
{
	local ($filename) ;
	local (@files);
	local (@temp);

	$filename = $_[0] ;

	@temp = grep ( s/\*//, ($filename) );

	die 'Wildcard * not currently supported in @ files.' if ( $#temp >= 0 );

	@temp = grep ( s/\?//, ($filename) );

	die 'Wildcard ? not currently supported in @ files.' if ( $#temp >= 0 );

	$filename ;
}

sub ShowHelp
{
   print "
MKDEP is a Perl script used for generating dependancy lists.

Usage : MKDEP <dependency list> [-o <obj file dir>] [-x extension] [-s]

Switches
========

-s  Ignore system includes.

";

}

sub Process_Command_Line
{
   local (@args);
   local ($arg);
   local ($hint);
   local (@switch); 
   local (@new_args);
   local (@arg_file);
   local (@arg_files);
   local ($filename);

   @args = @_ ;

   foreach $arg ( @args )
   {
      if ( substr( $arg, 0, 1 ) eq "@" )
      {
         $filename = substr( $arg, 1 );

		open(F,$filename) || die "Could not open $filename";
		@arg_file = <F>;
		close(F);

         foreach( @arg_file ) {
            @new_args = ( split( /\s/g ), @new_args )
		}

         &Process_Command_Line( @new_args );
      }
      elsif ( $arg =~ /-o/i )
      {
         $hint = 1 ;
      }
      elsif ( $arg =~ /-x/i )
      {
         $hint = 2 ;
      }
      elsif ( $arg =~ /-i/i )
      {
         $hint = 3 ;
      }
      elsif ( $arg =~ /-s/i )
      {
         $ignore_sys_flag = 1 ;
      }
      elsif ( $arg =~ /-h/i )
      {
         &ShowHelp() ;
         exit ;
      }
      elsif ( $arg eq "-\?" )
      {
         &ShowHelp() ;
         exit ;
      }
      elsif ( $arg =~ /-help/i  )
      {
         &ShowHelp() ;
         exit ;
      }
      elsif ( $hint == 1 )
      {
         $outdir = $arg ;
         $hint = 0 ;
      }
      elsif ( $hint == 2 )
      {
         $ext = $arg ;
         $ext  =~ s/\.// ;  # Ensure there is no dot in the user specified extension.

         $hint = 0 ;
      }
      elsif ( $hint == 3 )
      {
         @include_path = ( @include_path, split(";",$arg));
         $hint = 0 ;
      }
      else
      {
         @switch = grep ( s/-//, ($arg) );
 
         if ( $#switch >= 0 )
         {
            print "Unrecognized parameter $arg\n" ;
         } 
         else
         {
            @new_files = &Expand_Wildcard( $arg );

            @files = ( @files, @new_files );
            $hint = 0 ;
         }
      }
   }
}


