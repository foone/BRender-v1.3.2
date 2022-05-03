typedef struct {
  br_scalar near_z,far_z;
  br_scalar near_index,far_index;
  br_pixelmap *map;
  br_pixelmap *shade;
} br_strip;

typedef struct {
    int nstrips;		/* Number of textures */
    br_strip *strips;
#if 0
    br_vector4 n;	
    br_vector4 u;
    br_vector4 v;
    br_scalar *d;	/* Z values over which to use each texture */
    br_scalar *i;	/* Index values for attenuation */
    br_pixelmap **t;	/* Filtered textures */
    br_pixelmap *s;	/* Shade table for attenuation */
#endif
    br_matrix34 m;
} br_horizon;
