/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: template.h 1.1 1997/12/10 16:52:57 jon Exp $
 * $Locker: $
 *
 * per-device store of allocated token-value templates
 */
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#ifdef __cplusplus
extern "C" {
#endif

struct device_templates {
    /*
     * Object query templates
     */
    struct br_tv_template *deviceTemplate;
    struct br_tv_template *rendererFacilityTemplate;
    struct br_tv_template *rendererTemplate;
    struct br_tv_template *rendererStateStoredTemplate;
    struct br_tv_template *geometryV1ModelTemplate;
    struct br_tv_template *geometryV1BucketsTemplate;
    struct br_tv_template *geometryPrimitivesTemplate;
    struct br_tv_template *geometryLightingTemplate;

    /*
     * Argument processing templates
     */
    struct br_tv_template *rendererNewTemplate;

    /*
     * Parts templates
     */
    struct br_tv_template *partCullTemplate;
    struct br_tv_template *partSurfaceTemplate;
    struct br_tv_template *partMatrixTemplate;
    struct br_tv_template *partEnableTemplate;
    struct br_tv_template *partHiddenSurfaceTemplate;
    struct br_tv_template *partBoundsTemplate;
    struct br_tv_template *partLightTemplate;
    struct br_tv_template *partClipTemplate;
};

#ifdef __cplusplus
};
#endif
#endif

