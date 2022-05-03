/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prelight.c 1.2 1998/07/16 17:56:14 johng Exp $
 * $Locker: $
 *
 * Bits of the lighting model for indexed pixels
 */
#include "brender.h"
#include "shortcut.h"
#include "brassert.h"
#include "formats.h"
#include "v1db.h"

BR_RCS_ID("$Id: prelight.c 1.2 1998/07/16 17:56:14 johng Exp $")

/*
 * Set the per-vertex i,r,g,b values for the model.
 *
 * This must be called during rendering - eg: as part of a model callback.
 * If you simply want to light a few models, then sandwich the call between
 * ZbSceneRenderBegin() and ZbSceneRenderEnd()
 * 
 * If 'a' != NULL, The values are generated as if the model were attached to
 * the actor 'a', otherwise the model will be in the current frame
 */
void BR_PUBLIC_ENTRY BrSceneModelLight(br_model *model,
            br_material *default_material, br_actor *root, br_actor *a)
{
    int i;
    struct v11model *v11m;
    struct v11group *group;
    br_matrix34 m2v;

    UASSERT_MESSAGE("NULL model pointer passed to BrSceneModelLight",
                    model);
    UASSERT_MESSAGE("NULL material pointer passed to BrSceneModelLight",
                    default_material);
    UASSERT_MESSAGE("Unprepared model passed to BrSceneModelLight",
                    model->prepared);
    UASSERT_MESSAGE("Unprepared material passed to BrSceneModelLight",
                    default_material->stored);

    v11m = model->prepared;

    /* Store the current renderer state */
    RendererStatePush(v1db.renderer, BR_STATE_ALL);

    /* Alter state to suite chosen material */
    RendererStateRestore(v1db.renderer, default_material->stored,
        BR_STATE_ALL);

    /* If root & actor pointers are passed in, then set the model to view
     * matrix to be the inverse of [camera_to_root][root_to_model]
     */
    if((root != NULL) && (a != NULL)){
        BrActorToActorMatrix34(&m2v, root, a);
        BrMatrix34Pre(&m2v, &v1db.camera_path[0].m);
        RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
                        BRT_AS_MATRIX34_SCALAR(MODEL_TO_VIEW),
                        (br_uint_32)&m2v);
        RendererModelInvert(v1db.renderer);
    }

    /* Light vertices in all groups */
    for(i = 0, group = v11m->groups; i < v11m->ngroups; i++, group++){
        if((group->stored))
            RendererStateRestore(v1db.renderer, group->stored,
                BR_STATE_ALL);
        else
            RendererStateRestore(v1db.renderer, default_material->stored,
                BR_STATE_ALL);
#if 0
        GeometryLightingRender(v1db.format_lighting, v1db.renderer,
            &group->vertices->p, &group->vertices->n,
            group->vertex_colours,
            (br_colour *)(&model->vertices->index),
            group->vertex_user,
            sizeof(*group->vertices),
            sizeof(*group->vertices),
            sizeof(*group->vertex_colours),
            sizeof(*model->vertices), group->nvertices);
#endif 
    }

    /* Restore previous state */
    RendererStatePop(v1db.renderer, BR_STATE_ALL);
}

