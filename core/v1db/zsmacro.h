/*
 * Calculate the sort value for a triangle
 */
#define SORT_VALUE_TRIANGLE(type,z0,z1,z2) do { \
	zprim=(z0); \
	switch(type) { \
		case BR_SORT_MIN: \
			if((z1)<zprim) zprim=(z1); \
			if((z2)<zprim) zprim=(z2); \
			break; \
		case BR_SORT_MAX: \
			if((z1)>zprim) zprim=(z1); \
			if((z2)>zprim) zprim=(z2); \
			break; \
		case BR_SORT_AVERAGE: \
			zprim=BR_ADD(zprim,BR_ADD((z1),(z2))); \
			zprim=BR_MUL(zprim,BR_SCALAR(1.0/3.0)); \
			break; \
		case BR_SORT_FIRST_VERTEX: \
			break; \
	} \
} while(0)

/*
 * Calculate the sort value for an edge
 */
#define SORT_VALUE_EDGE(type,z0,z1) do { \
	zprim=(z0); \
	switch(type) { \
		case BR_SORT_MIN: \
			if((z1)<zprim) zprim=(z1); \
			break; \
		case BR_SORT_MAX: \
			if((z1)>zprim) zprim=(z1); \
			break; \
		case BR_SORT_AVERAGE: \
			zprim=BR_ADD(zprim,(z1)); \
			zprim=BR_MUL(zprim,BR_SCALAR(1.0/2.0)); \
			break; \
		case BR_SORT_FIRST_VERTEX: \
			break; \
	} \
} while(0)


