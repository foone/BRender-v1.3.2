void BrQuantAddPixelmap(br_pixelmap *);
br_pixelmap * BrQuantMapPixelmap(int,br_pixelmap *);
void DoQuantize(br_pixelmap **,int,int,int,br_pixelmap *);
br_pixelmap * Remap(int,int,br_pixelmap *);
void DoRemap(br_pixelmap **,int,int,int);
void DoRemapWithSupermaps(br_pixelmap **mtable,int nmaps,br_pixelmap **smtable,int nsmaps,int smap_power,int base,int range);
br_pixelmap * DoConvert(br_pixelmap *,int);
int SortMapTable(br_pixelmap **,int);
