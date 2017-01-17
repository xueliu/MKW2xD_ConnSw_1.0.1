/*!=================================================================================================
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
\file       app_ethernet_usb.c
\brief      This is a public source file for the virtual usb ethernet interface.

==================================================================================================*/

/*==================================================================================================
Include Files
==================================================================================================*/
#include "app_stack_config.h"
#include "stack_config.h"

#include "EmbeddedTypes.h"

#include "stack_manager_if.h"

#include "ip_if_management.h"
#include "ip.h"
#include "ip6.h"
#include "nd.h"
#include "nd_events.h"
#include "dhcp_client.h"
#include "dhcp6_client.h"

#include "nvm_adapter.h"

#include "event_manager.h"

#include "thread_utils.h"
#if ND_OPT_DNS_RCV_ENABLED
#include "thread_dns.h"
#endif

extern const mediaIfStruct_t* gUsbEnetMediaIfPtr;

#if USBENET_ROUTER
/*==================================================================================================
Private macros
==================================================================================================*/

/* ND ROUTER CONFIGURATIONS */
#define ND6_ROUTER_CONFIGURATION  \
        /* Supported features */ \
        .devType                                = gNd6DevRouter_c, \
        .messages                               = ND_PIB_MSG_RA_RCV_MASK \
                                                 | ND_PIB_MSG_RS_RCV_MASK, \
        .options                                = 0U, \
        .behaviors                              = ND_PIB_SEND_PERIODIC_RA_MASK, \
        /* Variables Defaults */ \
        .variablesDefaults.hopLimit             = 255U, \
        .variablesDefaults.linkMTU              = 1280U, \
        .variablesDefaults.regLifetime          = 0U, \
        /* Constants */ \
        .constants.maxInitialRtrAdvInterval     = 5U,      /*!< sec */ \
        .constants.maxInitialRtrAdv             = 20U, \
        .constants.maxFinalRtrAdv               = 3U, \
        .constants.minDelayBetweenRas           = 3U,       /*!< sec */ \
        .constants.maxRaDelayTime               = 500U,     /*!< ms */ \
        .constants.maxRtrSolicitationDelay      = 1U,       /*!< sec */ \
        .constants.rtrSolicitationInterval      = 4U,       /*!< sec */ \
        .constants.maxRtrSolicitations          = 3U, \
        .constants.maxMulticastSolicit          = 3U, \
        .constants.maxUnicastSolicit            = 3U, \
        .constants.maxAnycastDelayTime          = 1U,       /*!< sec */ \
        .constants.maxNeighborAdvertisement     = 3U, \
        .constants.reachableTime                = 30U,      /*!< sec */ \
        .constants.retransTime                  = 1U,       /*!< sec */ \
        .constants.delayFirstProbeTime          = 5U,       /*!< sec */ \
        .constants.minRandomFactor              = 500U,     /*!< ms */ \
        .constants.maxRandomFactor              = 1500U,    /*!< ms */ \
        .constants.minContextChangeDelay        = 0U,       /*!< sec */ \
        .constants.tentativeNceLifetime         = 0U,       /*!< sec */ \
        .constants.multihopHopLimit             = 0U, \
        .constants.maxRtrSolicitationInterval   = 0U        /*!< sec */

/* ENET interface config  */
#define ETH_ROUTER_CONFIGURATION \
        .ifUniqueId                             = gIpIfEth0_c, \
        /* Ethernet / Virtual Enet */ \
        .macAddress.eui                         = {0x00, 0x04, 0x9F, 0x00, 0xFA, 0x5D}, \
        .macAddress.addrSize                    = gLlayerAddrEui48_c, \
        /* ND */ \
        .pNdPib                                 = (ndPib_t*)&mEthRouterNdPibCfg, \


/* ND HOST CONFIGURATIONS */
#define ND6_HOST_CONFIGURATION  \
        /* Supported features */ \
        .devType                                = gNd6DevHost_c, \
        .messages                               = ND_PIB_MSG_RA_RCV_MASK, \
        .options                                = 0U, \
        .behaviors                              = 0U, \
        /* Variables Defaults */ \
        .variablesDefaults.hopLimit             = 255U, \
        .variablesDefaults.linkMTU              = 1280U, \
        .variablesDefaults.regLifetime          = 0U, \
        /* Constants */ \
        .constants.maxInitialRtrAdvInterval     = 16U,      /*!< sec */ \
        .constants.maxInitialRtrAdv             = 3U, \
        .constants.maxFinalRtrAdv               = 3U, \
        .constants.minDelayBetweenRas           = 3U,       /*!< sec */ \
        .constants.maxRaDelayTime               = 500U,     /*!< ms */ \
        .constants.maxRtrSolicitationDelay      = 1U,       /*!< sec */ \
        .constants.rtrSolicitationInterval      = 4U,       /*!< sec */ \
        .constants.maxRtrSolicitations          = 3U, \
        .constants.maxMulticastSolicit          = 3U, \
        .constants.maxUnicastSolicit            = 3U, \
        .constants.maxAnycastDelayTime          = 1U,       /*!< sec */ \
        .constants.maxNeighborAdvertisement     = 3U, \
        .constants.reachableTime                = 30U,      /*!< sec */ \
        .constants.retransTime                  = 1U,       /*!< sec */ \
        .constants.delayFirstProbeTime          = 5U,       /*!< sec */ \
        .constants.minRandomFactor              = 500U,     /*!< ms */ \
        .constants.maxRandomFactor              = 1500U,    /*!< ms */ \
        .constants.minContextChangeDelay        = 0U,       /*!< sec */ \
        .constants.tentativeNceLifetime         = 0U,       /*!< sec */ \
        .constants.multihopHopLimit             = 0U, \
        .constants.maxRtrSolicitationInterval   = 0U        /*!< sec */

#define ETH_HOST_CONFIGURATION \
        .ifType                                 = gIfTypeEth_c, \
        /* Ethernet / Virtual Enet */ \
        .macAddress.eui                         = {0x00, 0x04, 0x9F, 0xFF, 0xFF, 0xFF}, \
        .macAddress.addrSize                    = gLlayerAddrEui48_c, \
        /* ND */ \
        .pNdPib                                 = (ndPib_t*)&mStaticNdHostPibCfg, \

/*==================================================================================================
Private type definitions
==================================================================================================*/

/*==================================================================================================
Private prototypes
==================================================================================================*/
#if 0
static void NET_SetAddr(ifHandle_t* pIfHandle, ipAddr_t *pIpAddr, void *pData);
#endif
static void NET_BrAddExternaIfPrefix(uint32_t thrInstanceId);

/*==================================================================================================
Private global variables declarations
==================================================================================================*/

#if USBENET_ROUTER
    static const ndPib_t mEthRouterNdPibCfg         = {ND6_ROUTER_CONFIGURATION};
    static const externalIfConfig_t mUsbEnetCfg                     = {ETH_ROUTER_CONFIGURATION};
    static bool_t mIsGlobalOnMeshPrefixConfigured   = TRUE;
#elif USBENET_HOST
    static const ndPib_t mStaticNdHostPibCfg        = {ND6_HOST_CONFIGURATION};
    static const externalIfConfig_t mUsbEnetCfg                     = {ETH_HOST_CONFIGURATION};
    static bool_t mIsGlobalOnMeshPrefixConfigured   = FALSE;
#endif

#if ND_OPT_DNS_RCV_ENABLED
    static bool_t mThrDNSServicesRegistered = FALSE;
#endif

static uint32_t mThrInstanceId = 0;
static ifHandle_t mIfHandleEnet = NULL;
#if USBENET_HOST
static dhcp6ClientStartParams_t mDhcp6ClientStartParams;
#endif

/*==================================================================================================
Public global variables declarations
==================================================================================================*/

taskMsgQueue_t *gpUSBEthTaskMsgQueue = NULL;

/*==================================================================================================
Public functions
==================================================================================================*/
/*!*************************************************************************************************
\fn     void IFENET_Start()
\brief  This is a public function used to start the Ethernet IP Media Interface.
\return void
***************************************************************************************************/
void IFUSBENET_Start
(
    taskMsgQueue_t* pTaskMsgQueue,
    instanceId_t thrInstanceID
)
{
    if (NULL == gpUSBEthTaskMsgQueue)
    {
        uint16_t protocols = gIpProtv4_c | gIpProtv6_c;
        void *enetIfPtr = NULL;
        uint32_t error;

        gpUSBEthTaskMsgQueue = pTaskMsgQueue;

        error = IP_IF_Add(gIpIfUsbRndis_c, enetIfPtr, (mediaIfStruct_t *)gUsbEnetMediaIfPtr, protocols);
        mIfHandleEnet = IP_IF_GetIfHandle(gIpIfUsbRndis_c);

        if(mUsbEnetCfg.pNdPib && (NULL != mIfHandleEnet) && (gIpOk_c == error))
        {
             /* assign routing function to interface */
             mIfHandleEnet->ip6If.ip6UnicastForward = IP6_Forward;
             mIfHandleEnet->ip6If.ip6McastForward = IP6_MulticastForward;


             /* start ND */
             ND_Open(mIfHandleEnet, mUsbEnetCfg.pNdPib);

#if USBENET_ROUTER
             if(mUsbEnetCfg.pNdPib->devType == gNd6DevRouter_c)
             {

                 thrPrefixAttr_t externalIfPrefix = THR_GetAttr_BrExternalIfPrefix(thrInstanceID);
                 thrPrefixAttr_t globalOnMeshPrefix = THR_GetAttr_BrGlobalOnMeshPrefix(thrInstanceID);

                 /* set Router Lifetime to 0 seconds - the border router is not a default router */
                 ND_DefaultRouterConfig(mIfHandleEnet, ND_ROUTER_LIFETIME_ZERO, gNdRoutePrefMedFlag_c);

                 /* configure external global pPrefix */
                 ND_PrefixListAdd(mIfHandleEnet, &externalIfPrefix.prefix,
                                 externalIfPrefix.prefixLenBits,
                                 IP6_ADDRESS_LIFETIME_INFINITE, IP6_ADDRESS_LIFETIME_INFINITE, 1, 1);

                 /* set on-mesh route to advertise */
                 ND_RouteInfoSet(globalOnMeshPrefix.prefix, globalOnMeshPrefix.prefixLenBits,
                                 gNdRoutePrefMedFlag_c, ND_LIFETIME_INFINITE);

                 /* configure IP address based on global pPrefix */
                 IP_IF_BindAddr6(gIpIfUsbRndis_c, &externalIfPrefix.prefix,
                                 ip6AddrTypeAutoconfigurable_c, IP6_ADDRESS_LIFETIME_INFINITE,
                                  externalIfPrefix.prefixLenBits);

                 /* set static route for the enet prefix */
                 //IP6_SetStaticRoute(&externalIfPrefix.prefix, externalIfPrefix.prefixLenBits,
                 //             IP6_STATIC_ROUTE_DEFAULT_METRIC,(ipAddr_t*)&in6addr_any,mIfHandleEnet);

             }
 #endif
 #if USBENET_HOST
             if(mUsbEnetCfg.pNdPib->devType == gNd6DevHost_c)
             {
                 /****************************************/
                 /* Start DHCP Client for Global Address */
                 /****************************************/
                 DHCP6_Client_Init(mpTaskMsgQueue);

                 mDhcp6ClientStartParams.clientStartMode = gDhcp6Pd_c;
                 mDhcp6ClientStartParams.ipIfId = gIpIfUsbRndis_c
                 mDhcp6ClientStartParams.deviceType = DHCP6_HW_TYPE_EUI64;
                 mDhcp6ClientStartParams.relayAddr = NULL;
                 mDhcp6ClientStartParams.pPrefix = NULL;
                 DHCP6_Client_Start(&mDhcp6ClientStartParams);
                 DHCP6_Client_RegisterSetAddr(NET_SetAddr);

                 DHCP4_Client_Init(mpTaskMsgQueue);
                 DHCP4_Client_Start(mIfHandleEnet, NULL);
             }
#endif
        }
    }
}

/*!*************************************************************************************************
\fn     void IFUSBENET_ThreadStarted(uint32_t thrInstanceId)
\brief  This is a public function which handles the steps that should be done after a device with
        Ethernet functionality transitions to Thread Leader.

\param  [in] thrInstanceId  instance ID of the Thread interface

\return void
***************************************************************************************************/
void IFUSBENET_ThreadStarted
(
    uint32_t thrInstanceId
)
{
    ipAddr_t mCastGroup = {0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0xAD};

    if ( (mIsGlobalOnMeshPrefixConfigured == TRUE) &&
         THR_GetAttr_IsDevConnected(thrInstanceId))
    {
        thrOtaBrPrefixSet_t prefixSet;
        thrPrefixAttr_t onMeshPrefix = THR_GetAttr_BrGlobalOnMeshPrefix(thrInstanceId);
        bool_t  brPrefixSync = FALSE;


        /* Add the global on-mesh prefix */
        if(onMeshPrefix.prefixLenBits)
        {
            FLib_MemSet(&prefixSet, 0, sizeof(prefixSet));
            prefixSet.thrBrPrefixLength = onMeshPrefix.prefixLenBits;
            FLib_MemCpy(prefixSet.thrBrPrefixValue,
                onMeshPrefix.prefix.addr8,
                onMeshPrefix.prefixLenBits >> 3);
            prefixSet.thrBrPrefixFlags[1] = THR_BR_PREFIX_FLAGS_P_SLAAC_MASK;
            prefixSet.thrBrPrefixFlags[1] |= THR_BR_PREFIX_FLAGS_P_ON_MESH_MASK;
            if(THR_GetAttr_BrDefaultRoute(thrInstanceId))
            {
              prefixSet.thrBrPrefixFlags[1] |= THR_BR_PREFIX_FLAGS_P_DEFAULT_MASK;
            }
            htonal(prefixSet.thrBrPrefixLifetime, THR_ALL_FFs32);
            htonal(prefixSet.thrBrExtRouteLifetime, THR_ALL_FFs32);
            prefixSet.thrBrPrefixAdvertised = TRUE;
            prefixSet.thrBrExtRouteAdvertised = FALSE;
            THR_BrPrefixAttrAddEntry(thrInstanceId, &prefixSet);
            brPrefixSync = TRUE;
        }

#if USBENET_ROUTER
        /* For ND_ROUTER, the external prefix is already known */
        {
            if(THR_GetAttr_BrExternalIfPrefix(thrInstanceId).prefixLenBits)
            {
                /* Add External Route prefix */
                NET_BrAddExternaIfPrefix(thrInstanceId);
                brPrefixSync = TRUE;
            }
        }
#endif
        /* Distribute Network Data */
        if(brPrefixSync == TRUE)
        {
            THR_BrPrefixAttrSync(thrInstanceId);
        }

        /* Register to multicast group and add forwarding rule */
        IP6_SetStaticRoute(&mCastGroup, 128, IP6_STATIC_ROUTE_DEFAULT_METRIC,
                          (ipAddr_t*)&in6addr_any, gIpIfSlp0_c);
        IP_IF_AddMulticastGroup6(gIpIfUsbRndis_c, &mCastGroup);

    }
}

static void NET_BrAddExternaIfPrefix(uint32_t thrInstanceId)
{
    thrOtaBrPrefixSet_t prefixSet;
    thrPrefixAttr_t externalIfPrefix = THR_GetAttr_BrExternalIfPrefix(thrInstanceId);

    if(externalIfPrefix.prefixLenBits)
    {
        /* Add External Route prefix */
        FLib_MemSet(&prefixSet, 0, sizeof(prefixSet));

        prefixSet.thrBrPrefixLength = externalIfPrefix.prefixLenBits;
        FLib_MemCpy(prefixSet.thrBrPrefixValue, externalIfPrefix.prefix.addr8, externalIfPrefix.prefixLenBits >> 3);
        htonal(prefixSet.thrBrPrefixLifetime, THR_ALL_FFs32);
        htonal(prefixSet.thrBrExtRouteLifetime, THR_ALL_FFs32);
        prefixSet.thrBrPrefixAdvertised = FALSE;
        prefixSet.thrBrExtRouteAdvertised = TRUE;

        THR_BrPrefixAttrAddEntry(thrInstanceId, &prefixSet);
    }
}
/*!*************************************************************************************************
\fn     NET_RaReceived
\brief  This is a private function which is called when the router allocates a prefix for us(in an
        ND Router Advertisement packet)

\param  [in] pEvent     pointer to the event structure

\return void
***************************************************************************************************/
void NET_RaReceived
(
    void *pEvent
)
{
    evmParams_t *pEvmParam = pEvent;

#if USBENET_HOST
    ipPrefix_t *pPrefix = (ipPrefix_t*)pEvmParam->pBuff;

    if(pEvmParam && pEvmParam->pBuff && mIfHandleEnet &&
       !NWKU_CmpAddrPrefix6(pPrefix->aPrefix, THR_GetAttr_BrExternalIfPrefix(mThrInstanceId).prefix.addr8,
                            pPrefix->prefixLen) )
    {
        thrPrefixAttr_t externalPrefix;

        FLib_MemSet(&externalPrefix, 0, sizeof(externalPrefix));
        /* Store the external IF prefix*/
        externalPrefix.prefixLenBits = pPrefix->prefixLen;
        FLib_MemCpy(&externalPrefix.prefix.addr8[0],
                    pPrefix->aPrefix,
                    pPrefix->prefixLen >> 3);
        THR_SetAttr(mThrInstanceId, gNwkAttrId_BrExternalIfPrefix_c, 0,
            sizeof(thrPrefixAttr_t), &externalPrefix);

        /* Propagate the external prefix */
        NET_BrAddExternaIfPrefix(mThrInstanceId);
        THR_BrPrefixAttrSync(mThrInstanceId);

        /* Set a static route for the prefix */
        //IP6_SetStaticRoute(&externalPrefix.prefix, externalPrefix.prefixLenBits,IP6_STATIC_ROUTE_DEFAULT_METRIC,
        //                  (ipAddr_t*)&in6addr_any, mIfHandleEnet);

        /* If a RA was received, solicit an address*/
        DHCP6_Client_SolicitAddress(&mDhcp6ClientStartParams);

    }
#endif /* USBENET_HOST */

    if(pEvmParam)
    {
      if(pEvmParam->pBuff)
      {
        MEM_BufferFree(pEvmParam->pBuff);
      }
      MEM_BufferFree(pEvmParam);
    }
}


/*!*************************************************************************************************
\fn     NET_RaRouteInfoReceived
\brief  This is a private function which is called when a Route Information option is present in an
        ND Router Advertisement packet.

\return void
***************************************************************************************************/
void NET_RaRouteInfoReceived
(
    void *pEvent
)
{
    evmParams_t *pEvmParam = pEvent;
    ndOptionRouteInfo_t *pNdRouteInfoOption = (ndOptionRouteInfo_t *)(pEvmParam->pBuff);
    ndOptionRouteInfo_t *pCurrOption = NULL;
    uint8_t iCount, noOptions = pEvmParam->buffSize / sizeof(ndOptionRouteInfo_t);
    thrOtaBrPrefixSet_t prefixSet = { 0 };

    for (iCount = 0; iCount < noOptions; iCount++)
    {
        pCurrOption = pNdRouteInfoOption + iCount;

        prefixSet.thrBrPrefixLength = pCurrOption->prefixLength;
        FLib_MemCpy(prefixSet.thrBrPrefixValue, pCurrOption->prefix.addr8, pCurrOption->prefixLength >> 3);

        /* If the received route's lifetime is zero, the route is removed from the Routing Table */
        if (ntohal(pCurrOption->lifetime) == ND_LIFETIME_ZERO)
        {
            THR_BrPrefixAttrRemoveEntry(mThrInstanceId, prefixSet.thrBrPrefixLength, prefixSet.thrBrPrefixValue);
        }
        else
        {
            FLib_MemCpy(prefixSet.thrBrExtRouteLifetime, pCurrOption->lifetime, sizeof(pCurrOption->lifetime));
            THR_BR_FLAGS_SET(prefixSet.thrBrExtRouteFlags, pCurrOption->prf >> 3,
                THR_BR_EXT_ROUTE_FLAGS_R_PREF_MASK, THR_BR_EXT_ROUTE_FLAGS_R_PREF_OFFSET);
            prefixSet.thrBrPrefixAdvertised = FALSE;
            prefixSet.thrBrExtRouteAdvertised = TRUE;

            THR_BrPrefixAttrAddEntry(mThrInstanceId, &prefixSet);
        }

        FLib_MemSet(&prefixSet, 0, sizeof(thrOtaBrPrefixSet_t));
    }

    THR_BrPrefixAttrSync(mThrInstanceId);

    MEM_BufferFree(pEvmParam->pBuff);
    MEM_BufferFree(pEvmParam);
}


#if ND_OPT_DNS_RCV_ENABLED
/*!*************************************************************************************************
\fn     NET_RaDnsInfoReceived
\brief  This is a private function which is called when a RDNSS or DNSSL option is present in an
        ND Router Advertisement packet.

\return void
***************************************************************************************************/
void NET_RaDnsInfoReceived
(
    void *pEvent
)
{
    evmParams_t *pEvmParam = pEvent;
    uint8_t flag = *pEvmParam->pBuff;

    uint8_t iCount;
    bool_t attrSync = FALSE;

    if (!mThrDNSServicesRegistered)
    {
        mThrDNSServicesRegistered = TRUE;
        THR_DNS_BRInit();
    }

    /* Set Border Router flag P_nd_dns for all prefixes. */
    for (iCount = 0; iCount < gBrPrefixTlbSize; iCount++)
    {
        if (NULL != gpaBrPrefixSetAttr[iCount])
        {
            thrOtaBrPrefixSet_t **pEntry = &gpaBrPrefixSetAttr[iCount];
            NVNG_MoveToRam((void **)pEntry);

            THR_BR_FLAGS_SET((*pEntry)->thrBrPrefixFlags[0], flag,
                THR_BR_PREFIX_FLAGS_P_ND_DNS_MASK, THR_BR_PREFIX_FLAGS_P_ND_DNS_OFFSET);

            NVNG_Save((void**)pEntry);
            attrSync = TRUE;
        }
    }

    if (attrSync)
    {
        THR_BrPrefixAttrSync(mThrInstanceId);
    }

    MEM_BufferFree(pEvmParam);
}
#endif /* ND_OPT_DNS_RCV_ENABLED */

/*==================================================================================================
Private functions
==================================================================================================*/
#if 0
/*!*************************************************************************************************
\private
\fn     NET_SetAddr
\brief  This is a private function Which is called when the router allocates a prefix for us(as
        response to DHCP Prefix Delegation request)

\param  [in] pIfHandle  pointer to the interface handle
\param  [in] pIpAddr    pointer to the IP prefix assigned by the router(:: in case of none)
\param  [in] pData      pointer to a variable containing the length of the prefix(NULL in case of no
                        prefix)

\return void
***************************************************************************************************/
static void NET_SetAddr
(
    ifHandle_t* pIfHandle,
    ipAddr_t *pIpAddr,
    void* pData
)
{
    /* Router could assign a prefix */
    if(pData && (!IP_IsAddrEqual(pIpAddr, &in6addr_any)))
    {
        uint8_t prefixLen = *(uint8_t*)pData;

        thrPrefixAttr_t globalOnMeshPrefix;

        /* If we received a prefix length smaller than 64 we expand it to 64 for Thread assignment
         * as we don't need to create any new sub-networks inside Thread */
        if(prefixLen < 64)
        {
            prefixLen = 64;
        }

        FLib_MemSet(&globalOnMeshPrefix, 0, sizeof(globalOnMeshPrefix));
        globalOnMeshPrefix.prefixLenBits = prefixLen;
        FLib_MemCpy(&globalOnMeshPrefix.prefix.addr8[0],
                    pIpAddr->addr8,
                    prefixLen >> 3);
        THR_SetAttr(mThrInstanceId, gNwkAttrId_BrGlobalOnMeshPrefix_c, 0,
                    sizeof(thrPrefixAttr_t), &globalOnMeshPrefix);

        mIsGlobalOnMeshPrefixConfigured = TRUE;
        IFUSBENET_ThreadStarted(mThrInstanceId);

    }
}
#endif
#endif /* USBENET_ROUTER */
