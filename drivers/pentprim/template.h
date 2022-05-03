/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: template.h 1.1 1997/12/10 16:48:59 jon Exp $
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
    struct br_tv_template *primitiveLibraryTemplate;
    struct br_tv_template *primitiveStateTemplate;
    struct br_tv_template *bufferStoredTemplate;

    /*
     * Parts templates
     */
    struct br_tv_template *partPrimitiveTemplate;
    struct br_tv_template *partOutputTemplate;
};

#ifdef __cplusplus
};
#endif
#endif

