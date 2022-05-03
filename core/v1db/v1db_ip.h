/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: v1db_ip.h 1.5 1998/06/17 16:14:18 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to V1 database
 */
#ifndef _V1DB_IP_H_
#define _V1DB_IP_H_
#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * actsupt.c
 */
br_boolean ActorToRoot(br_actor *a, br_actor *root, br_matrix34 *m);
br_boolean ActorToRootTyped(br_actor *a, br_actor *world, br_matrix34 *m, br_int_32 *type);
br_token CameraToScreenMatrix4(br_matrix4 *mat, br_actor *camera);

/*
 * prepmap.c
 */
void BrBufferUpdate(br_pixelmap *pm, br_token use, br_uint_16 flags);
void BrBufferClear(br_pixelmap *map);

/*
 * prepmatl.c
 */
void BrMaterialClear(br_material *mat);

/*
 * prepmesh.c
 */
void BrModelClear(br_model *model);
void BrModelClearPrepared(br_model *model);

/*
 * enables.c
 */
void BrSetupLights(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt);
void BrSetupClipPlanes(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt, br_matrix4 *view_to_screen);
void BrSetupEnvironment(br_actor *world, br_matrix34 *world_to_view, br_int_32 w2vt);
void BrSetupHorizons(br_actor *world,	br_matrix34 *world_to_view, br_int_32 w2vt);

void BrActorEnableCheck(br_actor *a);

br_error BrLightCullReset(void);

/*
 * modrend.c
 */
extern void (*RenderStyleCalls[])(
				br_actor *actor,
				br_model *model,
				br_material *material,
				void *render_data,
				br_uint_8 style,
				int on_screen);

/*
 * otable.c
 */
void RenderPrimaryOrderTable(void);
void RenderOrderTableList(void);
void SetOrderTableRange(br_order_table *order_table);
void InsertOrderTableList(br_order_table *order_table);
void SetOrderTableBounds(br_bounds *bounds, br_order_table *order_table);

#ifdef __cplusplus
};
#endif

#endif
#endif


