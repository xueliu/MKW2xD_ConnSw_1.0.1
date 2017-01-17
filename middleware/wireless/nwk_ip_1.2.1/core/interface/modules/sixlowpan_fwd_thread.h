/*!
* \file      sixlowpan_fwd_thread.h
* \brief     This is a header file for the 6LowPan Thread unicast and multicast forwarding. In
*            thread the 6LoWPAN multicast forwarding is not used because MPL (Trickle-Multicast) is
*            used in the IP Layer. Only unicast forwarding is used in 6LoWPAN.
*
* Copyright (c) 2015, Freescale Semiconductor, Inc.
* All rights reserved.
*
* Freescale Confidential Proprietary
*
* No part of this document must be reproduced in any form - including copied,
* transcribed, printed or by any electronic means - without specific written
* permission from Freescale.
*
*/

#ifndef _SIXLOWPAN_FWD_THREAD_H
#define _SIXLOWPAN_FWD_THREAD_H

/*!=================================================================================================
\file       sixlowpan_fwd_thread.h
\brief      This is a header file for the sixlowpan forwarding functionality for THREAD.
==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
/* General Includes */
#include "EmbeddedTypes.h"
#include "stack_config.h"

#include "network_utils.h"
#include "mac_abs_types.h"


/*==================================================================================================
Public macros
==================================================================================================*/

/*==================================================================================================
Public type definitions
==================================================================================================*/

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

/*==================================================================================================
Public function prototypes
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
/*!*************************************************************************************************
\fn     llAddr_t adpThreadGetParent(llAddr_t* pMyAddr, llAddr_t* pFinalAddr, llAddr_t *pMacSrcAddr,
        instanceId_t instanceId)
\brief  Function used to get the parent node Link-Layer address.

\param  [in]    pMyAddr         Pointer to device Link-Layer address.
\param  [in]    pFinalAddr      Pointer to final destination Link-Layer address.
\param  [in]    pMacSrclAddr    Pointer to neighbor mac source Link-Layer address.
\param  [in]    instanceId      Instance ID.

\return         llAddr_t        Parent node Link-Layer address.
***************************************************************************************************/
llAddr_t adpThreadGetParent(llAddr_t* myAddr, llAddr_t* finalAddr, llAddr_t *pMacSrcAddr,
                                      instanceId_t instanceId);


/*!*************************************************************************************************
\fn     bool_t adpThreadIsAddrOnLink(ipAddr_t *pIpAddr, uint32_t interfaceId)
\brief  Function used to determine the on link status of a destination address.

\param  [in]    pIpAddr         Pointer to the IPv6 address.
\param  [in]    interfaceId     Interface ID.

\retval         TRUE            Address is ON LINK.
\retval         FALSE           Address is OFF LINK.
***************************************************************************************************/
bool_t adpThreadIsAddrOnLink(ipAddr_t *pIpAddr, uint32_t interfaceId);


/*!*************************************************************************************************
\fn     bool_t adpThreadIsMeshNeeded(void * param)
\brief  Function used to determine if the mesh header is required for forwarding the packet.

\param  [in]    param         Pointer to the 6LoWPAN info structure.

\retval         TRUE            Mesh Header is needed.
\retval         FALSE           Mesh Header is not needed.
***************************************************************************************************/
bool_t adpThreadIsMeshNeeded(void * param);

#if THR_ROUTING_ENABLE
/*!*************************************************************************************************
\fn     llAddr_t adpThreadGetNextHop(llAddr_t* pMyAddr, llAddr_t* pFinalAddr, llAddr_t *pMacSrcAddr,
                                     instanceId_t instanceId)
\brief  Function used to get the next hop Link-Layer address.

\param  [in]    pMyAddr         Pointer to device Link-Layer address.
\param  [in]    pFinalAddr      Pointer to final destination Link-Layer address.
\param  [in]    pMacSrclAddr    Pointer to neighbor mac source Link-Layer address.
\param  [in]    instanceId      Instance ID.

\return         llAddr_t        Next hop Link-Layer address.
***************************************************************************************************/
llAddr_t adpThreadGetNextHop(llAddr_t* myAddr, llAddr_t* finalAddr, llAddr_t *pMacSrcAddr,
                                        instanceId_t instanceId);

/*!*************************************************************************************************
\fn     void adpThreadSetRoutingTable(void * pMeshRouteTbl, instanceId_t instanceId)
\brief  Function used to set the whole routing table.

\param  [in]    pMeshRouteTbl   Pointer to routing table.
\param  [in]    instanceId      Instance ID.
***************************************************************************************************/
void adpThreadSetRoutingTable(void * pMeshRouteTbl, instanceId_t instanceId);

#ifdef __cplusplus
}
#endif
/*================================================================================================*/

#endif
#endif  /* _SIXLOWPAN_FWD_THREAD_H */
