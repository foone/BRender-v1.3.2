# Converts primtive info file (*.ifg) into an array initialiser
#
# infogen <input-file>
#
# Read each primitive line
#
%global_properties = ();

foreach (@ARGV) {

	/^\s*(.*)(=.*)?\s*/;

	if(/^\s*(.*)=(.*)/) {
		$global_properties{$1} = $2;
	} elsif(/^\s*(.*)/) {
		$global_properties{$1} = "TRUE";
	}
}

while (<STDIN>) {

	# Ignore comments
	#
	next if(/^#/);

	if(/^\s*\[(.*)\]/) {
		$props = $1;
		# Common properties
		#
		# Go through each property
		#
		%common_properties = %global_properties;

		foreach (split(",",$props)) {

			/^\s*(.*)(=.*)?\s*/;

			if(/^\s*(.*)=(.*)/) {
				$common_properties{$1} = $2;
			} elsif(/^\s*(.*)/) {
				$common_properties{$1} = "TRUE";
			}
		}
	} elsif(/^\s*(\w+\:)?\s*(\w+)\s*=\s*\[(.*)\]/) {

#		print "[$1] [$2] [$3] [$4]\n";

		# Remember image and renderer name
		#
		$image = $1;
		$render = $2;
		$props = $3;

		$image =~ s/^\s*(.*)\s*:/\1/;


		# Set default state
		#
		&set_default;

		# Go through each property
		#
		%properties = %common_properties;

		foreach (split(",",$props)) {

			/^\s*(.*)(=.*)?\s*/;

			if(/^\s*(.*)=(.*)/) {
				$properties{$1} = $2;
			} elsif(/^\s*(.*)/) {
				$properties{$1} = "TRUE";
			}
		}
		# This rather nasty way of doing things ensures that the subroutines are called
		# in a defiend order, irrespective of the input order
		#
		&property_mmx($properties{"mmx"}) if $properties{"mmx"};

		&property_point if $properties{"point"};
		&property_line if $properties{"line"};
		&property_triangle if $properties{"triangle"};
		&property_quad if $properties{"quad"};
		&property_sprite if $properties{"sprite"};

		&property_index8 if $properties{"index8"};
		&property_rgb555 if $properties{"rgb555"};
		&property_rgb565 if $properties{"rgb565"};
		&property_rgb888 if $properties{"rgb888"};
		&property_rgbx888 if $properties{"rgbx888"};

		&property_z_buffered if $properties{"z_buffered"};

		&property_interpolated_intensity if $properties{"interpolated_intensity"};
        &property_range_zero if $properties{"range_zero"};

		&property_constant_intensity if $properties{"constant_intensity"};
		&property_interpolated_colour if $properties{"interpolated_colour"};
		&property_constant_colour if $properties{"constant_colour"};
		&property_interpolated_alpha if $properties{"interpolated_alpha"};
		&property_constant_alpha if $properties{"constant_alpha"};
		&property_linear_depth if $properties{"linear_depth"};

		&property_no_depth_write if $properties{"no_depth_write"};

		&property_duplicate if $properties{"duplicate"};

        &property_shade_table if $properties{"shade_table"};

		&property_texture if $properties{"texture"};
		&property_texture8x8 if $properties{"texture8x8"};
		&property_texture16x16 if $properties{"texture16x16"};
		&property_texture32x32 if $properties{"texture32x32"};
		&property_texture64x64 if $properties{"texture64x64"};
		&property_texture128x128 if $properties{"texture128x128"};
		&property_texture256x256 if $properties{"texture256x256"};
		&property_texture512x512 if $properties{"texture512x512"};
		&property_texture1024x1024 if $properties{"texture1024x1024"};

		&property_texture_index8 if $properties{"texture_index8"};
   		&property_texture_index8_palette if $properties{"texture_index8_palette"};
		&property_texture_rgb555 if $properties{"texture_rgb555"};
		&property_texture_rgb565 if $properties{"texture_rgb565"};
		&property_texture_rgbx888 if $properties{"texture_rgbx888"};
   		&property_texture_power2 if $properties{"texture_power2"};
		&property_texture_stride_positive if $properties{"texture_stride_positive"};
		&property_texture_no_skip if $properties{"texture_no_skip"};
		&property_unscaled_texture_coords if $properties{"unscaled_texture_coords"};

		&property_perspective if $properties{"perspective"};
		&property_perspective_subdivide if $properties{"perspective_subdivide"};

		&property_dithered if $properties{"dithered"};

		&property_colour7bit if $properties{"colour7bit"};
	
		&property_dithered_map if $properties{"dithered_map"};
		&property_colour_index if $properties{"colour_index"};
		&property_colour_rgb if $properties{"colour_rgb"};

		&property_rgb_shade if $properties{"rgb_shade"};
		&property_decal if $properties{"decal"};
		&property_bump if $properties{"bump"};
		&property_blend_table if $properties{"blend_table"};

        &property_blendrgb if $properties{"blendrgb"};
        &property_screendoor if $properties{"screendoor"};

        &property_fog if $properties{"fog"};

		&property_float_components if $properties{"float_components"};
		&property_integer_components if $properties{"integer_components"};
		&property_fixed_components if $properties{"fixed_components"};

		&property_float_homogeneous_coords if $properties{"float_homogeneous_coords"};
		&property_float_coords if $properties{"float_coords"};
		&property_float_depth if $properties{"float_depth"};
		&property_float_colour if $properties{"float_colour"};
		&property_float_texture_coords if $properties{"float_texture_coords"};
		&property_float_intensity if $properties{"float_intensity"};
		&property_float_alpha if $properties{"float_alpha"};
		&property_float_linear_depth if $properties{"float_linear_depth"};

		&property_integer_homogeneous_coords if $properties{"integer_homogeneous_coords"};
		&property_integer_coords if $properties{"integer_coords"};
		&property_integer_depth if $properties{"integer_depth"};
		&property_integer_colour if $properties{"integer_colour"};
		&property_integer_texture_coords if $properties{"integer_texture_coords"};
		&property_integer_intensity if $properties{"integer_intensity"};
		&property_integer_alpha if $properties{"integer_alpha"};
		&property_integer_linear_depth if $properties{"integer_linear_depth"};

		&property_fixed_homogeneous_coords if $properties{"fixed_homogeneous_coords"};
		&property_fixed_coords if $properties{"fixed_coords"};
		&property_fixed_depth if $properties{"fixed_depth"};
		&property_fixed_colour if $properties{"fixed_colour"};
		&property_fixed_texture_coords if $properties{"fixed_texture_coords"};
		&property_fixed_intensity if $properties{"fixed_intensity"};
		&property_fixed_alpha if $properties{"fixed_alpha"};
		&property_fixed_linear_depth if $properties{"fixed_linear_depth"};

		&property_generic_setup($properties{"generic_setup"}) if $properties{"generic_setup"};

		&property_pixel_stride($properties{"pixel_stride"}) if $properties{"pixel_stride"};
		&property_parameter_struct($properties{"parameter_struct"}) if $properties{"parameter_struct"};
		&property_area_test($properties{"area_test"}) if $properties{"area_test"};

		# Print out entry
		#
		&print_entry;

		# Print externs ?
		#
		# &print_externs;
	}
}

sub set_default {
	$identifier = "";
	$type = "BR_PRIMT_UNKNOWN";
    $autoloader = "RenderAutoloadThunk";
	@flags = ();
	@prim_flags = ();
	@constant_components = ();
	@vertex_components = ();
	@convert_mask_f = ();
	@convert_mask_x = ();
	@convert_mask_i = ();
	@use_buffers = ();

	@match_flags_set = ();	
	@match_flags_clear = ();

	@flags_mask = ();	
	@flags_cmp = ();
	@range_flags = ();

	$colour_type = "PMT_NONE";
	$depth_type = "PMT_NONE";
	$texture_type = "PMT_NONE";
	$shade_type = "PMT_NONE";
	$blend_type = "PMT_NONE";
	$bump_type = "PMT_NONE";
	$lighting_type = "PMT_NONE";
	$screendoor_type = "PMT_NONE";
	$fog_type = "PMT_NONE";

	$input_colour_type = "0";

	$colour_row_size = 0;
	$depth_row_size = 0;

	$texture_width = 0;
	$texture_height = 0;
	$decal_flag = "FALSE";

	$colour_base = "BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)";
	$colour_scale = "BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)";

    $alpha_base = "BR_SCALAR(0)";
	$alpha_scale = "BR_SCALAR(0)";

	%integer_component = ();
	%float_component = ();

	$generic_setup = "";
	$pixel_stride = 0;
	$param_size = "0";
	$area_limit = "0.0f";
	$param_struct = "";
}

sub print_entry {

	# Strip trailing comma from name
	# 		
	$identifier =~ s/, $//;

	# Split components by type
	#
	foreach $component (@constant_components, @vertex_components) {
		if ($integer_component{$component}) {
			push(@convert_mask_i,($component));
		} elsif ($float_component{$component}) {
			push(@convert_mask_f,($component));
		} else {
			push(@convert_mask_x,($component));
		}
    }

	@flags_mask = (@match_flags_set, @match_flags_clear);
	@flags_cmp = (@match_flags_set);

	# Convert flag arrays to strings
	#
	$prim_flags_string = "0";
	$prim_flags_string = "BR_PRIMF_" . join("|BR_PRIMF_",@prim_flags) if(@prim_flags);

	$constant_components_string = "0";
	$constant_components_string = "CM_" . join("|CM_",@constant_components) if(@constant_components);

	$vertex_components_string = "0";
	$vertex_components_string = "CM_" . join("|CM_",@vertex_components) if(@vertex_components);

	$convert_mask_f_string = "0";
	$convert_mask_f_string = "(1<<C_" . join(")|(1<<C_",@convert_mask_f) . ")" if(@convert_mask_f);
	$convert_mask_x_string = "0";
	$convert_mask_x_string = "(1<<C_" . join(")|(1<<C_",@convert_mask_x) . ")" if(@convert_mask_x);
	$convert_mask_i_string = "0";
	$convert_mask_i_string = "(1<<C_" . join(")|(1<<C_",@convert_mask_i) . ")" if(@convert_mask_i);

	$constant_slots_string = "0";
	$constant_slots_string = "(1<<C_" . join(")|(1<<C_",@constant_components) . ")" if(@constant_components);

	$use_buffers_string = "0";
	$use_buffers_string = "BUFFER_" . join("_MASK|BUFFER_",@use_buffers) ."_MASK" if(@use_buffers);

	$flags_mask_string = "0";
	$flags_mask_string = "PRIMF_" . join("|PRIMF_",@flags_mask) if(@flags_mask);
	
	$flags_cmp_string = "0";
	$flags_cmp_string = "PRIMF_" . join("|PRIMF_",@flags_cmp) if(@flags_cmp);

	$range_flags_string = "0";
	$range_flags_string = "RF_" . join("|RF_",@range_flags) if(@range_flags);


	if($image) {
		$image = "\"$properties{image_prefix}$image$properties{image_suffix}\"";
                if($generic_setup){
                        $image_entry = "$render";
                }else{
                        $image_entry = "\"_$render\"";
                }
		$entry = $autoloader;
	} else {
		$image = "NULL";
		$image_entry = "NULL";
		$entry = $render;
	}

	print <<END;
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)$entry, NULL,
#else
		(brp_render_fn *)$render, NULL,
#endif

		"$identifier", NULL,
		BRT_$type, $prim_flags_string,

		/* components - constant and per vertex
		 */
		$constant_components_string,
		$vertex_components_string,

		/* Component slots as - float, fixed or integer
		 */
		$convert_mask_f_string,
		$convert_mask_x_string,
		$convert_mask_i_string,

		/* Constant slots
	 	 */
		$constant_slots_string,
	},

	/* Offset and scale for R,G,B,A
	 */
	{$colour_base,$alpha_base},
	{$colour_scale,$alpha_scale},

	/* range flags
	 */
	$range_flags_string,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	$flags_mask_string,
	$flags_cmp_string,

	/* Texture, depth and shade type
	 */
	$depth_type,
	$texture_type,
	$shade_type,
	$blend_type,
	$screendoor_type,
	$lighting_type,
	$bump_type,
	$fog_type,

	/* Colour & Depth  row size
	 */
	$colour_row_size,$depth_row_size,

	/* Texture size
	 */
	$texture_width,$texture_height,

	/* Input colour type
	 */
	$input_colour_type,

	/* Autoload info
	 */
#if AUTOLOAD
	$image,(void *)$image_entry,
#else
	NULL,NULL,
#endif
END
	print <<END if($generic_setup);
	/* Generic setup info.
	 */
	{
		$area_limit,
		$param_size,
		$pixel_stride,
		$generic_setup,

#if AUTOLOAD
                (void *)\"_$rasterise_rl_l\",
                (void *)\"_$rasterise_lr_l\",
                (void *)\"_$rasterise_rl_s\",
                (void *)\"_$rasterise_lr_s\",
#else
		$rasterise_rl_l,
		$rasterise_lr_l,
		$rasterise_rl_s,
		$rasterise_lr_s,
#endif
	}
END

	print "},\n";

}

sub print_externs
{
	# Generate some externs
	#
	if($generic_setup) {
		print "void BR_ASM_CALL $generic_setup(void);\n";
		print "void BR_ASM_CALL $rasterise_lr_l(void);\n";
		print "void BR_ASM_CALL $rasterise_rl_l(void);\n";
		print "void BR_ASM_CALL $rasterise_lr_s(void);\n" if($rasterise_lr_l ne $rasterise_lr_s);
		print "void BR_ASM_CALL $rasterise_rl_s(void);\n" if($rasterise_rl_l ne $rasterise_rl_s);
	} else {
		print "void BR_ASM_CALL $render(brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);\n";
	}
}

# Routines for each property
#

sub property_range_zero
{
        push(@match_flags_set,("RANGE_ZERO"));
}

sub property_index8
{
	$identifier .=	"Indexed, ";
	$colour_type = "BR_PMT_INDEX_8";
}

sub property_rgb555
{
	$identifier .=	"RGB 555, ";
	$colour_type = "BR_PMT_RGB_555";
}

sub property_rgb565
{
	$identifier .=	"RGB 565, ";
	$colour_type = "BR_PMT_RGB_565";
}

sub property_rgb888
{
	$identifier .=	"RGB 888, ";
	$colour_type = "BR_PMT_RGB_888";
}

sub property_rgbx888
{
	$identifier .=	"RGBX 888, ";
	$colour_type = "BR_PMT_RGBX_888";
}

sub property_z_buffered
{
	$identifier .=	"Z Buffered, ";
	$depth_type = "BR_PMT_DEPTH_16";
	push(@vertex_components,("SZ"));
	push(@use_buffers,("DEPTH"));
}

sub property_shade_table
{
	if($properties{"constant_intensity"} || $properties{"interpolated_intensity"}) {
		push(@use_buffers,("SHADE"));

 		$shade_type = $colour_type;
	}
}


# Texture mapped
#
sub shared_texture
{
	push(@vertex_components,("U","V"));

	push(@use_buffers,("TEXTURE"));

	if($properties{"constant_intensity"} || $properties{"interpolated_intensity"}) {
		push(@use_buffers,("SHADE"));

 		$shade_type = $colour_type;
	}

	if($properties{"constant_intensity"} || $properties{"interpolated_intensity"} ||
	   $properties{"constant_colour"} || $properties{"interpolated_colour"}) {
		push(@match_flags_set,("MODULATE"));
	}

	$texture_type = $colour_type;
}

sub property_texture
{
	$identifier .=	"Textured, ";
	$texture_width = 0;
	$texture_height = 0;
	&shared_texture;
}

sub property_texture8x8
{
	$identifier .=  "Textured 8x8, ";
	$texture_width = 8;
	$texture_height = 8;
	&shared_texture;
}

sub property_texture16x16
{
	$identifier .=  "Textured 16x16, ";
	$texture_width = 16;
	$texture_height = 16;
	&shared_texture;
}

sub property_texture32x32
{
	$identifier .=  "Textured 32x32, ";
	$texture_width = 32;
	$texture_height = 32;
	&shared_texture;
}

sub property_texture64x64
{
	$identifier .=	"Textured 64x64, ";
	$texture_width = 64;
	$texture_height = 64;
	&shared_texture;
}

sub property_texture128x128
{
	$identifier .=	"Textured 128x128, ";
	$texture_width = 128;
	$texture_height = 128;
	&shared_texture;
}

sub property_texture256x256
{
	$identifier .=	"Textured 256x256, ";
	$texture_width = 256;
	$texture_height = 256;
	&shared_texture;
}

sub property_texture512x512
{
	$identifier .=	"Textured 512x512, ";
	$texture_width = 512;
	$texture_height = 512;
	&shared_texture;
}

sub property_texture1024x1024
{
	$identifier .=	"Textured 1024x1024, ";
	$texture_width = 1024;
	$texture_height = 1024;
	&shared_texture;
}

# Extra properties for texture mapping
#
sub property_texture_index8
{
	$texture_type = "BR_PMT_INDEX_8";
}

sub property_texture_index8_palette
{
	$texture_type = "BR_PMT_INDEX_8";
	push(@match_flags_set,("PALETTE"));
}

sub property_texture_rgb555
{
	$texture_type = "BR_PMT_RGB_555";
}

sub property_texture_rgb565
{
	$texture_type = "BR_PMT_RGB_565";
}

sub property_texture_rgbx888
{
	$texture_type = "BR_PMT_RGBX_888";
}

sub property_unscaled_texture_coords
{
	$identifier .=  "Unscaled_texture_coords, ";
	push(@range_flags,("UNSCALED_TEXTURE_COORDS"));
}

sub property_texture_power2
{
	push(@match_flags_set,("POWER2"));
}

sub property_texture_stride_positive
{
        push(@match_flags_set,("STRIDE_POSITIVE"));
}

sub property_texture_no_skip
{
        push(@match_flags_set,("NO_SKIP"));
}

sub property_decal
{
	$identifier .=	"Decal, ";
	push(@match_flags_set,("DECAL"));
	push(@range_flags,("DECAL"));
}

sub property_blend
{
	$identifier .=	"Blended, ";

	if($properties{"constant_alpha"} || $properties{"interpolated_alpha"}) {
		$blend_type = $colour_type;
	}
    push(@prim_flags,("BLENDED"));
}

sub property_blendrgb
{
    $identifier .=  "RGB blended, ";
    push(@match_flags_set,("BLEND"));
    push(@prim_flags,("BLENDED"));
}

sub property_screendoor
{
    $identifier .=  "Screendoor, ";
    push(@match_flags_set,("BLEND"));
    push(@prim_flags,("BLENDED"));
}

sub property_perspective
{
	$identifier .=	"Perspective Correct, ";
	push(@vertex_components,("W"));
	push(@match_flags_set,("PERSPECTIVE"));
}

sub property_perspective_subdivide
{
	$identifier .=	"Perspective Correct, ";
	push(@match_flags_set,("PERSPECTIVE"));
	push(@prim_flags,("SUBDIVIDE"));
	push(@range_flags,("NEED_SUBDIVIDE"));
}

sub property_dithered_map
{
	$identifier .=	"Dithered Map, ";
	push(@match_flags_set,("DITHER_MAP"));
}

sub property_dithered
{
	$identifier .=	"Dithered, ";
	push(@match_flags_set,("DITHER_COLOUR"));
}

sub property_colour7bit
{
}

sub property_interpolated_intensity
{
	$identifier .=	"Interpolated Intensity, ";
	push(@vertex_components,("I"));
	push(@match_flags_set,("SMOOTH"));
}

sub property_constant_intensity
{
	$identifier .=	"Constant Intensity, ";
	push(@constant_components,("I"));
	push(@match_flags_clear,("SMOOTH"));
}

sub property_interpolated_colour
{
	$identifier .=	"Interpolated Colour, ";
	push(@vertex_components,("R","G","B"));
	push(@match_flags_set,("SMOOTH"));
	$colour_base = "BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(1)";

	$colour_scale = "BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(254)" if(!$properties{"dithered"} && !$properties{"colour7bit"});
	$colour_scale = "BR_SCALAR(126),BR_SCALAR(126),BR_SCALAR(126)" if(!$properties{"dithered"} &&  $properties{"colour7bit"});

	$colour_scale = "BR_SCALAR(247),BR_SCALAR(251),BR_SCALAR(247)" if( $properties{"dithered"} && !$properties{"colour7bit"});
	$colour_scale = "BR_SCALAR(123),BR_SCALAR(125),BR_SCALAR(123)" if( $properties{"dithered"} &&  $properties{"colour7bit"});
}

sub property_constant_colour
{
	$identifier .=	"Constant Colour, ";
	push(@constant_components,("R","G","B"));
	push(@match_flags_clear,("SMOOTH"));
	$colour_base = "BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(1)";

	$colour_scale = "BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(254)" if(!$properties{"dithered"} && !$properties{"colour7bit"});
	$colour_scale = "BR_SCALAR(126),BR_SCALAR(126),BR_SCALAR(126)" if(!$properties{"dithered"} &&  $properties{"colour7bit"});

	$colour_scale = "BR_SCALAR(247),BR_SCALAR(251),BR_SCALAR(247)" if( $properties{"dithered"} && !$properties{"colour7bit"});
	$colour_scale = "BR_SCALAR(123),BR_SCALAR(125),BR_SCALAR(123)" if( $properties{"dithered"} &&  $properties{"colour7bit"});
}

sub property_interpolated_alpha
{
	push(@vertex_components,("A"));

	$alpha_base = "BR_SCALAR(0)";
	$alpha_scale = "BR_SCALAR(1)";
}

sub property_constant_alpha
{
	push(@constant_components,("A"));

	$alpha_base = "BR_SCALAR(0)";
	$alpha_scale = "BR_SCALAR(1)";
}

sub property_linear_depth
{
	push(@vertex_components,("SW"));
}

sub property_no_depth_write
{
#        push(@match_flags_clear,("DEPTH_WRITE"));
}

sub property_duplicate
{
	push(@prim_flags,("CONST_DUPLICATE"));
}

sub property_colour_index
{
	$input_colour_type = "BRT_INDEX";
}

sub property_colour_rgb
{
	$input_colour_type = "BRT_RGB";
}

sub property_rgb_shade
{
	$identifier .=	"RGB Shading, ";
	$shade_type = "BR_PMT_RGBX_888";
	push(@range_flags,("RGB_SHADE"));
}

sub property_bump
{
	$identifier .=	"Bump, ";
 	$bump_type = "BR_PMT_INDEX_8";
 	$lighting_type = "BR_PMT_INDEX_8";
}

sub property_blend_table
{
	$identifier .=	"Blend, ";
 	$blend_type = "BR_PMT_INDEX_8";
    push(@prim_flags,("BLENDED"));
}

sub property_fog
{
	$identifier .=  "Fog, ";
	$fog_type = "BR_PMT_INDEX_8";
}

sub property_point
{
	$type = "POINT";
	push(@vertex_components,("SX","SY"));
	push(@use_buffers,("COLOUR"));
}

sub property_line
{
	$type = "LINE";
	push(@vertex_components,("SX","SY"));
	push(@use_buffers,("COLOUR"));
}

sub property_triangle
{
	$type = "TRIANGLE";
	push(@vertex_components,("SX","SY"));
	push(@use_buffers,("COLOUR"));
}

sub property_quad
{
	$type = "QUAD";
	push(@vertex_components,("SX","SY"));
	push(@use_buffers,("COLOUR"));
}

sub property_sprite
{
	$type = "SPRITE";
	push(@vertex_components,("SX","SY"));
	push(@use_buffers,("COLOUR"));
}

sub property_fixed_components
{
	&property_fixed_homogeneous_coords;
	&property_fixed_coords;
	&property_fixed_depth;
	&property_fixed_colour;
	&property_fixed_texture_coords;
	&property_fixed_intensity;
	&property_fixed_alpha;
	&property_fixed_linear_depth;
}

sub property_integer_components
{
	&property_integer_homogeneous_coords;
	&property_integer_coords;
	&property_integer_depth;
	&property_integer_colour;
	&property_integer_texture_coords;
	&property_integer_intensity;
	&property_integer_alpha;
	&property_integer_linear_depth;
}

sub property_float_components
{
	&property_float_homogeneous_coords;
	&property_float_coords;
	&property_float_depth;
	&property_float_colour;
	&property_float_texture_coords;
	&property_float_intensity;
	&property_float_alpha;
	&property_float_linear_depth;
}

sub property_fixed_homogeneous_coords
{
	$integer_component{"X"} = 0;
	$integer_component{"Y"} = 0;
	$integer_component{"Z"} = 0;
	$integer_component{"W"} = 0;

	$float_component{"X"} = 0;
	$float_component{"Y"} = 0;
	$float_component{"Z"} = 0;
	$float_component{"W"} = 0;
}

sub property_fixed_coords
{
	$integer_component{"SX"} = 0;
	$integer_component{"SY"} = 0;

	$float_component{"SX"} = 0;
	$float_component{"SY"} = 0;
}

sub property_fixed_depth
{
	$integer_component{"SZ"} = 0;

	$float_component{"SZ"} = 0;
}

sub property_fixed_colour
{
	$integer_component{"R"} = 0;
	$integer_component{"G"} = 0;
	$integer_component{"B"} = 0;

	$float_component{"R"} = 0;
	$float_component{"G"} = 0;
	$float_component{"B"} = 0;
}

sub property_fixed_texture_coords
{
	$integer_component{"U"} = 0;
	$integer_component{"V"} = 0;

	$float_component{"U"} = 0;
	$float_component{"V"} = 0;
}

sub property_fixed_intensity
{
	$integer_component{"I"} = 0;

	$float_component{"I"} = 0;
}

sub property_fixed_alpha
{
	$integer_component{"A"} = 0;

	$float_component{"A"} = 0;
}

sub property_fixed_linear_depth
{
	$integer_component{"SW"} = 0;

	$float_component{"SW"} = 0;
}

sub property_integer_homogeneous_coords
{
	$integer_component{"X"} = 1;
	$integer_component{"Y"} = 1;
	$integer_component{"Z"} = 1;
	$integer_component{"W"} = 1;

	$float_component{"X"} = 0;
	$float_component{"Y"} = 0;
	$float_component{"Z"} = 0;
	$float_component{"W"} = 0;
}

sub property_integer_coords
{
	$integer_component{"SX"} = 1;
	$integer_component{"SY"} = 1;

	$float_component{"SX"} = 0;
	$float_component{"SY"} = 0;
}

sub property_integer_depth
{
	$integer_component{"SZ"} = 1;

	$float_component{"SZ"} = 0;
}

sub property_integer_colour
{
	$integer_component{"R"} = 1;
	$integer_component{"G"} = 1;
	$integer_component{"B"} = 1;

	$float_component{"R"} = 0;
	$float_component{"G"} = 0;
	$float_component{"B"} = 0;
}

sub property_integer_texture_coords
{
	$integer_component{"U"} = 1;
	$integer_component{"V"} = 1;

	$float_component{"U"} = 0;
	$float_component{"V"} = 0;
}

sub property_integer_intensity
{
	$integer_component{"I"} = 1;

	$float_component{"I"} = 0;
}

sub property_integer_alpha
{
	$integer_component{"A"} = 1;

	$float_component{"A"} = 0;
}

sub property_integer_linear_depth
{
	$integer_component{"SW"} = 1;

	$float_component{"SW"} = 0;
}

sub property_float_homogeneous_coords
{
	$integer_component{"X"} = 0;
	$integer_component{"Y"} = 0;
	$integer_component{"Z"} = 0;
	$integer_component{"W"} = 0;

	$float_component{"X"} = 1;
	$float_component{"Y"} = 1;
	$float_component{"Z"} = 1;
	$float_component{"W"} = 1;
}

sub property_float_coords
{
	$integer_component{"SX"} = 0;
	$integer_component{"SY"} = 0;

	$float_component{"SX"} = 1;
	$float_component{"SY"} = 1;
}

sub property_float_depth
{
	$integer_component{"SZ"} = 0;

	$float_component{"SZ"} = 1;
}

sub property_float_colour
{
	$integer_component{"R"} = 0;
	$integer_component{"G"} = 0;
	$integer_component{"B"} = 0;

	$float_component{"R"} = 1;
	$float_component{"G"} = 1;
	$float_component{"B"} = 1;
}

sub property_float_texture_coords
{
	$integer_component{"U"} = 0;
	$integer_component{"V"} = 0;

	$float_component{"U"} = 1;
	$float_component{"V"} = 1;
}

sub property_float_intensity
{
	$integer_component{"I"} = 0;

	$float_component{"I"} = 1;
}

sub property_float_alpha
{
	$integer_component{"A"} = 0;

	$float_component{"A"} = 1;
}

sub property_float_linear_depth
{
	$integer_component{"SW"} = 0;

	$float_component{"SW"} = 1;
}

sub property_pixel_stride
{

	($pixel_stride) = @_;
}

sub property_parameter_struct
{
	($s) = @_;

	($param_size) = "sizeof(struct $s)";
}

sub property_generic_setup
{
	($r) = @_;

	$rasterise_lr_l = $render . "_LR";
	$rasterise_rl_l = $render . "_RL";

	$rasterise_lr_s = $render . "_LR";
	$rasterise_rl_s = $render . "_RL";

        if($float_component{"X"}) {
		($generic_setup) = "GenericSetupFloat" . $r;
		$render = "GenericSetupTriangleFloat_A";
        } elsif($integer_component{"X"}) {
		($generic_setup) = "GenericSetup" . $r;
		$render = "GenericSetupTriangle_A";
	} else {
		($generic_setup) = "GenericSetupFixed" . $r;
		$render = "GenericSetupTriangleFixed_A";
	}

    $autoloader = "GenericAutoloadThunk";
}

sub property_area_test
{
	($generic_area_limit) = @_;

	$rasterise_lr_s .= "S";
	$rasterise_rl_s .= "S";
}

sub property_mmx
{
	$identifier .=  "MMX, ";
	push(@range_flags,("OFFSET_Y"));
}

