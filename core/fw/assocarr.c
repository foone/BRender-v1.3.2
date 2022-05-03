/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: assocarr.c 1.3 1998/02/27 18:10:44 jon Exp $
 * $Locker: $
 *
 * Associative array handling code
 */

#include "fw.h"
#include <brassert.h>



#define MAX_ELEMENTS_INCREMENT 32


br_associative_array *BrAssociativeArrayAllocate()
{
	br_associative_array *array;

	array = (br_associative_array *)BrResAllocate(fw.res, sizeof(br_associative_array), BR_MEMORY_TOKEN_VALUE);
	if (array == NULL)
		return NULL;

	array->tv = NULL;
	array->max_elements = 0;
	array->num_elements = 0;

	return array;
}


br_error BrAssociativeArraySet(br_associative_array *array, br_token t, br_value v)
{
	br_token_value *tv = NULL;
	char *string;
	br_uint_32 i;

	ASSERT(array);

	/*
	 * Look for the key in the array
	 */
	for (i = 0; i < array->num_elements; i++)

		if (array->tv[i].t == t) {
			tv = &array->tv[i];
			break;
		}

	/*
	 * Add the key to the array if necessary
	 */
	if (tv == NULL) {

		/*
		 * If necessary, expand the array
		 */
		if (array->num_elements >= array->max_elements) {

			tv = (br_token_value *)BrResAllocate(array, (array->max_elements + MAX_ELEMENTS_INCREMENT) * sizeof(br_token_value), BR_MEMORY_APPLICATION );
			if (tv == NULL)
				return BRE_FAIL;

			if (array->tv != NULL) {
				BrMemCpy(tv, array->tv, array->num_elements * sizeof(br_token_value));
				BrResFree(array->tv);
			}

			array->tv = tv;
			array->max_elements += MAX_ELEMENTS_INCREMENT;
		}

		tv = &array->tv[array->num_elements];

		array->num_elements++;

		tv->t = t;
	}

	/*
	 * Copy the data, duplicating all strings.  Other references are
	 * copied so they must remain valid if they are to be used
	 */
    switch (BrTokenType(tv->t)) {

	case BRT_STRING:

		if (v.str != NULL) {

			string = BrResStrDup(array, v.str);
			if (string == NULL)
				return BRE_FAIL;
	 
		} else

			string = NULL;

		if (tv->v.str != NULL)
			BrResFree(tv->v.str);

		tv->v.str = string;
		break;

	default:

		tv->v = v;
		break ;
	}

	return BRE_OK;
}


br_error BrAssociativeArrayQuery(br_associative_array *array, br_token t, br_value *v)
{
	br_uint_32 i;

	ASSERT(array);

	/*
	 * Look for the key in the array
	 */
	for (i = 0; i < array->num_elements; i++)

		if (array->tv[i].t == t) {

			/*
			 * Copy the data - string references are not duplicated, so
			 * they must be duplicated if they will be needed after the array
			 * is freed or the key is set to a new value
			 */
			*v = array->tv[i].v;
			return BRE_OK;
		}

	return BRE_FAIL;
}


br_error BrAssociativeArrayRemove(br_associative_array *array, br_token t)
{
	br_uint_32 i;

	/*
	 * Look for the key in the array
	 */
	for (i = 0; i < array->num_elements; i++)

		if (array->tv[i].t == t) {

			/*
			 * Delete the old string copy if necessary
			 */
			if (BrTokenType(array->tv[i].t) == BRT_STRING && array->tv[i].v.str != NULL)
				BrResFree(array->tv[i].v.str);

			/*
			 * Copy the last element over to replace this one
			 */
			array->num_elements--;

			array->tv[i] = array->tv[array->num_elements];

			return BRE_OK;
		}

	return BRE_FAIL;
}








