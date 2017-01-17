#ifndef _RIPNG_CFG_H
#define _RIPNG_CFG_H
/*!
\file       ripng_cfg.h

\copyright  (c) Copyright 2014, Freescale, Inc.  All rights reserved.

\brief      This is a header file for a configuration header file. It contains default values for
            module configuration defines.


\details    This file contains the folowing configuration options:

            RIPNG_ENABLE                                            0|1
            RIPNG_TIMER_INTERVAL_SEC                        configurable value
            RIPNG_SPLIT_HORIZON_ENABLED                             0|1
            RIPNG_METRIC_INFINITY_VALUE                     configurable value
            RIPNG_ROUTE_TIMEOUT_SEC                         configurable value
            RIPNG_GARBAGE_COLLECTION_TIME_SEC               configurable value
            RIPNG_TRIGGERD_UPDATE_HOLDOFF_TIME_MIN_SEC      configurable value
            RIPNG_TRIGGERD_UPDATE_HOLDOFF_TIME_MAX_SEC      configurable value
            RIPNG_ROUTING_UPDATE_TIME_MIN_SEC               configurable value
            RIPNG_ROUTING_UPDATE_TIME_MAX_SEC               configurable value

*/

/*==================================================================================================
Include Files
==================================================================================================*/

/* General Includes */
#include "EmbeddedTypes.h"
#include "app_stack_config.h"

/*==================================================================================================
Private macros
==================================================================================================*/

#ifndef RIPNG_ENABLE
   #define RIPNG_ENABLE 0
#endif

#ifndef RIPNG_ROUTE_TIMEOUT_SEC
   #define RIPNG_ROUTE_TIMEOUT_SEC  180
#endif


#ifndef RIPNG_TRIGGERD_UPDATE_HOLDOFF_TIME_MIN_SEC
   #define RIPNG_TRIGGERD_UPDATE_HOLDOFF_TIME_MIN_SEC  1
#endif

#ifndef RIPNG_TRIGGERD_UPDATE_HOLDOFF_TIME_MAX_SEC
   #define RIPNG_TRIGGERD_UPDATE_HOLDOFF_TIME_MAX_SEC   5
#endif

#ifndef RIPNG_ROUTING_UPDATE_TIME_MIN_SEC
   #define RIPNG_ROUTING_UPDATE_TIME_MIN_SEC 15
#endif

#ifndef RIPNG_ROUTING_UPDATE_TIME_MAX_SEC
   #define RIPNG_ROUTING_UPDATE_TIME_MAX_SEC 45
#endif

#endif  /*_RIPNG_CFG_H */
