var group___mac_types =
[
    [ "macSuperframeSpec_t", "group___mac_types.html#structmac_superframe_spec__t", [
      [ "beaconOrder", "group___mac_types.html#a56aedcf832ba1272975b632af36eb12c", null ],
      [ "superframeOrder", "group___mac_types.html#ad45fed3b708b4f50f969290933af4c1e", null ],
      [ "finalCapSlot", "group___mac_types.html#a4e9355b1c9c9a09e71482df7cb2e2842", null ],
      [ "ble", "group___mac_types.html#aa0109fb6b3ed4d2ea5b3b298c40948f2", null ],
      [ "reserved", "group___mac_types.html#a9679aa728c7de44faad09e63313485b0", null ],
      [ "panCoordinator", "group___mac_types.html#ac33893ac09aee33a821aa7e5707f20ee", null ],
      [ "associationPermit", "group___mac_types.html#a270a00e6fcd7a249743d6286e0671408", null ]
    ] ],
    [ "gtsCharacteristics_t", "group___mac_types.html#structgts_characteristics__t", [
      [ "gtsLength", "group___mac_types.html#a5b062c6484f805ff9cdf8f7fc7791e65", null ],
      [ "gtsDirection", "group___mac_types.html#aed406a52c0fa4d2454ba8c332c236f72", null ],
      [ "characteristicsType", "group___mac_types.html#acd3651a883d6f03c16bc134404d18d23", null ],
      [ "reserved", "group___mac_types.html#a5fc606dbee134afb2f329d2390245414", null ]
    ] ],
    [ "panDescriptor_tag", "group___mac_types.html#structpan_descriptor__tag", [
      [ "coordAddress", "group___mac_types.html#a852b793e13afd06a5b8738db9b2dd558", null ],
      [ "coordPanId", "group___mac_types.html#ad9f456969e7846029d96b5b495b176fe", null ],
      [ "coordAddrMode", "group___mac_types.html#a19023d10f2ef76f69e4e5545d75a57f0", null ],
      [ "logicalChannel", "group___mac_types.html#a68acd1229c0bb64da2900c1b0d3ba947", null ],
      [ "securityFailure", "group___mac_types.html#afa5f9b3bf67b6a7ca824d1a7965c7a0a", null ],
      [ "superframeSpec", "group___mac_types.html#a2bcc0f4ac317d6d45a7d0f9986b81702", null ],
      [ "gtsPermit", "group___mac_types.html#ad8d829c368cf27382ced34d1665788b5", null ],
      [ "linkQuality", "group___mac_types.html#a3e207eddf206084fcecbf8b7ad647f0c", null ],
      [ "timeStamp", "group___mac_types.html#aa76bcc740898df05513da041d20b5531", null ],
      [ "securityLevel", "group___mac_types.html#ab0a1095d2e1c8db24926ad4d5d826ed1", null ],
      [ "keyIdMode", "group___mac_types.html#a01a6b022c717ee0d0a6e7edaf97cf0e7", null ],
      [ "keySource", "group___mac_types.html#a4869f3b018346d2dfb9bc6f29e92b82c", null ],
      [ "keyIndex", "group___mac_types.html#a3a96b492fe73cab40ecb95d3b6fc68a5", null ]
    ] ],
    [ "keyIdLookupDescriptor_t", "group___mac_types.html#structkey_id_lookup_descriptor__t", [
      [ "lookupData", "group___mac_types.html#ac2cbb6b19fe73ac3b3ff786070b9ecf9", null ],
      [ "lookupDataSize", "group___mac_types.html#a098b436b58620f1ac2adcecc6eba5b85", null ]
    ] ],
    [ "deviceAddrDescriptor_t", "group___mac_types.html#structdevice_addr_descriptor__t", [
      [ "extAddress", "group___mac_types.html#af9c26f85498588baa1de98c0504912c9", null ],
      [ "PANId", "group___mac_types.html#a5ff3dbd1c44d2c078b65cd4590ee895e", null ],
      [ "shortAddress", "group___mac_types.html#a4267dad680dc89ef1461619176ea3f18", null ],
      [ "usageCnt", "group___mac_types.html#a33199270df3369e24870b4d5251daad8", null ]
    ] ],
    [ "deviceDescriptor_t", "group___mac_types.html#structdevice_descriptor__t", [
      [ "frameCounter", "group___mac_types.html#a285114da3a41070529ba77c7af7dda1c", null ],
      [ "exempt", "group___mac_types.html#a60c3c56d4dc399586baa2a27d9ba1085", null ],
      [ "deviceAddrDescriptorHandle", "group___mac_types.html#a82b85cddb7b9c4f52d41cb1d76c9e995", null ]
    ] ],
    [ "keyDeviceDescriptor_t", "group___mac_types.html#structkey_device_descriptor__t", [
      [ "deviceDescriptorHandle", "group___mac_types.html#a8b595e3999c9ccbae1b5681b0cb430f6", null ],
      [ "uniqueDevice", "group___mac_types.html#ad9457f025d9698f1292e35145aa54cf4", null ],
      [ "blackListed", "group___mac_types.html#af7a5eb3c23328bea008dbc90e98c9dd0", null ]
    ] ],
    [ "keyUsageDescriptor_t", "group___mac_types.html#structkey_usage_descriptor__t", [
      [ "frameType", "group___mac_types.html#a5a182d53478138cb0a178fc6b54c18dd", null ],
      [ "commandFrameIdentifier", "group___mac_types.html#a543814b1834f491d9aaf50283c4fc869", null ]
    ] ],
    [ "keyDescriptor_t", "group___mac_types.html#structkey_descriptor__t", [
      [ "keyIdLookupList", "group___mac_types.html#ad468c746aeab802704b7e692f2d4d1cf", null ],
      [ "keyUsageList", "group___mac_types.html#af7e3686040893e2961cb181d3ab2728b", null ],
      [ "keyDeviceList", "group___mac_types.html#a4b4cde8d7c85ac5c16ea54c6241516ea", null ],
      [ "keyDeviceListEntries", "group___mac_types.html#a52101bd8890868cfbac857086902110f", null ],
      [ "keyIdLookupListEntries", "group___mac_types.html#a0c07cb7c01ef598419224d59a236ee7b", null ],
      [ "keyUsageListEntries", "group___mac_types.html#a402db1b422963611bb45c6e3cdf7569e", null ],
      [ "key", "group___mac_types.html#a86077c1034919deb3358e15673990a1b", null ]
    ] ],
    [ "securityLevelDescriptor_t", "group___mac_types.html#structsecurity_level_descriptor__t", [
      [ "frameType", "group___mac_types.html#ac18ec5ebd0aee6446a5c55ce08e1ff77", null ],
      [ "commandFrameIdentifier", "group___mac_types.html#ac7b95fe1cecbb7aafd7347c6a1215141", null ],
      [ "securityMinimum", "group___mac_types.html#a5671156f0754881196f5c231f010d0e9", null ],
      [ "deviceOverrideSecurityMinimum", "group___mac_types.html#af28ee298e8f0b33dce3fc87d80dee200", null ]
    ] ],
    [ "macCslEntry_t", "group___mac_types.html#structmac_csl_entry__t", [
      [ "nextSample", "group___mac_types.html#a731ee917e416424620f3b668777f7bab", null ],
      [ "checksum", "group___mac_types.html#a99b21d21edb2c67d3d0da1edcd4b18f6", null ],
      [ "period", "group___mac_types.html#a92c8e7c0008d55c8d7a3b6589af3d119", null ],
      [ "inUse", "group___mac_types.html#a514b15fe337f051096da7426ccfd2078", null ]
    ] ],
    [ "macRitIe_tag", "group___mac_types.html#structmac_rit_ie__tag", [
      [ "T0", "group___mac_types.html#ae0bb64780dc012555ef745c0615105cb", null ],
      [ "N", "group___mac_types.html#a965585b2a9c60bf3ff9f30da02a16188", null ],
      [ "T", "group___mac_types.html#a3f5218f52528c7c9e6bab9de62be6b68", null ]
    ] ],
    [ "macRitEntry_t", "group___mac_types.html#structmac_rit_entry__t", [
      [ "lastTimestamp", "group___mac_types.html#a1fe8ddfc01f0bd4cbb068ab98a27fde0", null ],
      [ "ritIe", "group___mac_types.html#ad78d274d42fdcb46e7c732a07fc0e2a4", null ],
      [ "checksum", "group___mac_types.html#ab1fbefb702f53427e2a712c7b1cd287a", null ],
      [ "inUse", "group___mac_types.html#a317508eeaf13786aef67a3a137bc49b5", null ]
    ] ],
    [ "macLinkOptions_t", "group___mac_types.html#structmac_link_options__t", [
      [ "tx", "group___mac_types.html#a2e57abe7c22719c34d91351782d2bc42", null ],
      [ "rx", "group___mac_types.html#a7980fffbc0064c0623f87b3bf012f543", null ],
      [ "shared", "group___mac_types.html#af82350598357a0432e05e56f5a01c4c4", null ],
      [ "timekeeping", "group___mac_types.html#a784eaec71e9521a261e044043df46c56", null ],
      [ "reserved", "group___mac_types.html#ae2c6ee53581b5e3aec202522cd8b87ba", null ]
    ] ],
    [ "macSlotframeIe_tag", "group___mac_types.html#structmac_slotframe_ie__tag", [
      [ "macSlotframeHandle", "group___mac_types.html#a2652229d7238dde6dd1ea9a04a032620", null ],
      [ "macSlotframeSize", "group___mac_types.html#a7686a5bbdb7d2d33534d75cbc2f4b46e", null ]
    ] ],
    [ "macTschLinkIe_tag", "group___mac_types.html#structmac_tsch_link_ie__tag", [
      [ "timeslot", "group___mac_types.html#aaf1bd0a38bb4560e069f0aca9b13536f", null ],
      [ "channelOffset", "group___mac_types.html#ad210ef9d96333af796bdf19c30a7706f", null ],
      [ "macLinkOptions", "group___mac_types.html#a2b7b05cf4eac63d26041fd8f23bae22c", null ]
    ] ],
    [ "macSlotframe_t", "group___mac_types.html#structmac_slotframe__t", [
      [ "macSlotframeIe", "group___mac_types.html#a1746d34d4ae4ff1d4a8e5c900ac2db14", null ],
      [ "inUse", "group___mac_types.html#a4820ed1d8453bac9ce984c6e3f47eb42", null ]
    ] ],
    [ "macLink_t", "group___mac_types.html#structmac_link__t", [
      [ "macLinkHandle", "group___mac_types.html#a2f04b59c3e067b3d10b82063111647c9", null ],
      [ "macNodeAddress", "group___mac_types.html#ae5f500ef2e0b2875e92236b2c8c99d8a", null ],
      [ "macLinkType", "group___mac_types.html#a9425eb66e330bee0d1686fe8f7f946e4", null ],
      [ "slotframeHandle", "group___mac_types.html#a1fb92bc0d0d3a95b7da4d90963c6ace0", null ],
      [ "macLinkIe", "group___mac_types.html#a98ba9b11065c92cb56f666c38b46940c", null ],
      [ "inUse", "group___mac_types.html#a2dcefb283d8e3cdab20d375c0b12aa2d", null ]
    ] ],
    [ "macTimeslotTemplate_tag", "group___mac_types.html#structmac_timeslot_template__tag", [
      [ "macTimeslotTemplateId", "group___mac_types.html#a3c6a1510a937d8083976f3fe4efe9db4", null ],
      [ "macTsCCAOffset", "group___mac_types.html#a4615198fdc6c35647030575d145d1f99", null ],
      [ "macTsCCA", "group___mac_types.html#aca4584e00fbdb3fca3c1677546683b54", null ],
      [ "macTsTxOffset", "group___mac_types.html#a8f8aae03c930e44496f0822c55e37295", null ],
      [ "macTsRxOffset", "group___mac_types.html#a682eba166fb3a169f0713e0d4680d147", null ],
      [ "macTsRxAckDelay", "group___mac_types.html#a1176d98488289f0926108de8995a98ef", null ],
      [ "macTsTxAckDelay", "group___mac_types.html#a48339acd640d8cd3c475da05588d64f6", null ],
      [ "macTsRxWait", "group___mac_types.html#ae4c7b1c478125f4ac38821525fe14597", null ],
      [ "macTsAckWait", "group___mac_types.html#a0c8f331bbc69c2984f5cc9e5afa2e3c4", null ],
      [ "macTsRxTx", "group___mac_types.html#a3b4fe4472a75f6842dd8c932cb4aeed9", null ],
      [ "macTsMaxAck", "group___mac_types.html#a0f39819d2e6299d64186006f85643c06", null ],
      [ "macTsMaxTx", "group___mac_types.html#a9c510898f320a181559f4d9b2c8a00d2", null ],
      [ "macTsTimeslotLength", "group___mac_types.html#adda2d26c8b00f760a529017beebd76c7", null ]
    ] ],
    [ "macHoppingSequenceIe_tag", "group___mac_types.html#structmac_hopping_sequence_ie__tag", [
      [ "macHoppingSequenceId", "group___mac_types.html#ab5418f612d9b90176b797fecb1ec46f9", null ],
      [ "macChannelPage", "group___mac_types.html#ab6ea275aaca11b777bfa278375cbbfb1", null ],
      [ "macNumberOfChannels", "group___mac_types.html#a04a58316fe31675142392882b0d685e5", null ],
      [ "macPhyConfiguration", "group___mac_types.html#a69b9aa5dd1a4e269917ace7e56aedaec", null ]
    ] ],
    [ "macTschSynchronizationIe_tag", "group___mac_types.html#structmac_tsch_synchronization_ie__tag", [
      [ "macASN", "group___mac_types.html#adbe25cec9bf6b81a644e5717a7b16a1e", null ],
      [ "macJoinPriority", "group___mac_types.html#adfbfbe4407650284f2a3ea32214e03de", null ]
    ] ],
    [ "macTschSlotframeAndLinkIe_tag", "group___mac_types.html#structmac_tsch_slotframe_and_link_ie__tag", [
      [ "numberOfSlotframes", "group___mac_types.html#a1e3d944482968e0e21fedb6a16eacb6c", null ]
    ] ],
    [ "macTschSlotframeIe_tag", "group___mac_types.html#structmac_tsch_slotframe_ie__tag", [
      [ "slotframe", "group___mac_types.html#a7d648727de5834b1ee87113b1ad51c75", null ],
      [ "numberOfLinks", "group___mac_types.html#ae6c21efb309013a242d356926bb3d8e4", null ]
    ] ],
    [ "macAckTimeCorrectionIe_tag", "group___mac_types.html#structmac_ack_time_correction_ie__tag", [
      [ "timeSyncInfo", "group___mac_types.html#abccedae2cfeb511db11b595174a8dd17", null ]
    ] ],
    [ "macTschNeighbor_t", "group___mac_types.html#structmac_tsch_neighbor__t", [
      [ "nodeAddr", "group___mac_types.html#aa1edd20b09ce538c222fd085d9b56702", null ],
      [ "keepAlivePeriod", "group___mac_types.html#afd7f3c427f7264f558f5d1e2da76908c", null ],
      [ "cntTsSinceLastPacket", "group___mac_types.html#a5bb521a25dd344201234ec127376d437", null ]
    ] ],
    [ "gScanResultsPerBlock_c", "group___mac_types.html#ga6e3c22113655341e75291e8470031ab1", null ],
    [ "gMaxScanResults_c", "group___mac_types.html#gac2745b43d083d59c006c18ec52cd0e48", null ],
    [ "macCapabilityInfo_t", "group___mac_types.html#gae80b4e32473a2ca8de4890a110aa05f1", null ],
    [ "pibId_t", "group___mac_types.html#ga84ca8f499a7b5653e76893dab9308a9a", null ],
    [ "macEBIEList_t", "group___mac_types.html#gaba3f819664846ff040bf3466f81b727c", null ],
    [ "resultType_t", "group___mac_types.html#gaecc0456826d0a2a4f16d06688077c760", [
      [ "gSuccess_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a200eb5c7ac253124999a6c8c9c18f22f", null ],
      [ "gPanAtCapacity_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ac10e2d4094d6ee98a53b68463ffb2035", null ],
      [ "gPanAccessDenied_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a1b4387339bf64a456cb7bcf02c0209e4", null ],
      [ "gSlotframeNotFound_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ae97d3367b14fd36d20df595b5cbe53cc", null ],
      [ "gMaxSlotframesExceeded_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760af665f4e67326ee2553c7439f491df85f", null ],
      [ "gUnknownLink_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760aeb138e5eeb01c4731a9711bfd5ff9494", null ],
      [ "gMaxLinksExceeded_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a3231288f6a146d2ebb226d9e1eed70b8", null ],
      [ "gNoSync_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a1072cbc3eef77ea2d71df5fe42ff668d", null ],
      [ "gRxEnableDone_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a9f5af0440050c70325a409641b905088", null ],
      [ "gPollReqDecryptionError_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a0e8fc6adf6cf3113f789e647fb1cae6e", null ],
      [ "gCounterError_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a772755737ff69e94bc33f2aa506ccdd5", null ],
      [ "gImproperKeyType_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760adb99f6d14d8a0a9725f51b5e2276804c", null ],
      [ "gImproperSecurityLevel_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a46a3d4a5dba6baa9e842a2fbd8dffac5", null ],
      [ "gUnsupportedLegacy_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a524617b38c06d7c155c7426b76a0903e", null ],
      [ "gUnsupportedSecurity_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a5e88a14c42791d4e07fe623b872c88d9", null ],
      [ "gBeaconLoss_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760aa9eab497d92a6bef15113ab6287b3699", null ],
      [ "gChannelAccessFailure_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a20d6cc3f6debb119770f3fa2d2e5b9cf", null ],
      [ "gDenied_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a980a085a0bc37264be14fa6867afd443", null ],
      [ "gDisableTrxFailure_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a1cffaa7edaf1eec4cd8e05438baf7d43", null ],
      [ "gSecurityError_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a5cae5f69c6bcb562746938c77a644a27", null ],
      [ "gFrameTooLong_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a77327db04aa6663ec5f4efda6bfce3ba", null ],
      [ "gInvalidGts_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a0a4e5bc3a0f93290fed6bc04489ddb00", null ],
      [ "gInvalidHandle_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a43a958116d8db5d6f557bb352035b562", null ],
      [ "gInvalidParameter_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a4d85e291a52e635b36cdf74f6482b611", null ],
      [ "gNoAck_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a097ddd1293871a96ae1edf44afab1c13", null ],
      [ "gNoBeacon_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ae826a2138aa27a1ae8eae263f4022a29", null ],
      [ "gNoData_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ab0ccb41a7feb728e55b14744ce6e2546", null ],
      [ "gNoShortAddress_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a90e27cc493906ebedaacafd37ce77a57", null ],
      [ "gOutOfCap_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a00d2cfd197da236c8859f419329996c9", null ],
      [ "gPanIdConflict_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a47978528d92e3fb92b8e468a5a420465", null ],
      [ "gRealignment_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a68c83e5dbd98e81d4270b7d096d901d5", null ],
      [ "gTransactionExpired_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a658adf5d07bd44b765b9c471e07004c1", null ],
      [ "gTransactionOverflow_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a2f5ef62dfd63b95a3f3f641d62af64fd", null ],
      [ "gTxActive_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a47eaa79917e51b6890d7d24f7a25d695", null ],
      [ "gUnavailableKey_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760afe200b86fc9c2adf456c4cc840e4bf25", null ],
      [ "gUnsupportedAttribute_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ae705c5da6ac78c8241bc8e3ec32a2fdd", null ],
      [ "gInvalidAddress_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a4dfb1946626d3f6309be12680ce94286", null ],
      [ "gOnTimeTooLong_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a4132961b5ba2e825066af476af2b143b", null ],
      [ "gPastTime_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ab085f6a9bb28d4ebadf92345483ffe20", null ],
      [ "gTrackingOff_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a356ab207e1429718856e9d543a0cacea", null ],
      [ "gInvalidIndex_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ab86919fe951cb1693b08014e148791d5", null ],
      [ "gLimitReached_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760adea195c9134db265b8d0d301153c117c", null ],
      [ "gReadOnly_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760abdf4f8e607927aad11c050a75e91157a", null ],
      [ "gScanInProgress_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760ab4405064b333011341b12fe642deef62", null ],
      [ "gSuperframeOverlap_c", "group___mac_types.html#ggaecc0456826d0a2a4f16d06688077c760a5055561240e3031818d5b270ba8ac13a", null ]
    ] ],
    [ "addrModeType_t", "group___mac_types.html#ga2e57fe9062d7cbfc5450afecccd3f1cc", [
      [ "gAddrModeNoAddress_c", "group___mac_types.html#gga2e57fe9062d7cbfc5450afecccd3f1ccaf28a5aa1581b8ce42e1a42ce9468d087", null ],
      [ "gAddrModeReserved_c", "group___mac_types.html#gga2e57fe9062d7cbfc5450afecccd3f1ccaab3be282eb81365efec67422ea8c9936", null ],
      [ "gAddrModeShortAddress_c", "group___mac_types.html#gga2e57fe9062d7cbfc5450afecccd3f1cca77f1acb0c618d5159982a7abb795707a", null ],
      [ "gAddrModeExtendedAddress_c", "group___mac_types.html#gga2e57fe9062d7cbfc5450afecccd3f1cca9f1c3a5ce659fedef14b0548e22f9b30", null ]
    ] ],
    [ "macTxOptions_t", "group___mac_types.html#ga9b0a936192ee9d52e5a6b2f9c08dc5b1", [
      [ "gMacTxOptionsNone_c", "group___mac_types.html#gga9b0a936192ee9d52e5a6b2f9c08dc5b1a1c7d1f49880431068c7517324ac0fbb8", null ],
      [ "gMacTxOptionsAck_c", "group___mac_types.html#gga9b0a936192ee9d52e5a6b2f9c08dc5b1a26c869961bfb523a4b96d0065a2ecd17", null ],
      [ "gMacTxOptionGts_c", "group___mac_types.html#gga9b0a936192ee9d52e5a6b2f9c08dc5b1aa89969b85da9b0181baf67f5edbbf73e", null ],
      [ "gMacTxOptionIndirect_c", "group___mac_types.html#gga9b0a936192ee9d52e5a6b2f9c08dc5b1a1603402b8909b202275e1747045e7b3a", null ],
      [ "gMacTxOptionAltExtAddr_c", "group___mac_types.html#gga9b0a936192ee9d52e5a6b2f9c08dc5b1a765129780506b55064e2f38a8cd0575c", null ]
    ] ],
    [ "macSecurityLevel_t", "group___mac_types.html#ga3577dc87f11ac25a000f1e5685450d37", [
      [ "gMacSecurityNone_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37a6db9bca0b144256b5ee305db2c8c4189", null ],
      [ "gMacSecurityMic32_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37a5305e2dc16e42cafe308913078024e8b", null ],
      [ "gMacSecurityMic64_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37a7ed759bf4dd15e7b9534cef4ee7af549", null ],
      [ "gMacSecurityMic128_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37a1c2e0c203b3441195662e3babbce3dbe", null ],
      [ "gMacSecurityEnc_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37ac3ff9d2b21373f8e0c4157476b67eb5a", null ],
      [ "gMacSecurityEncMic32_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37ad5a076664b7eb601821533ac0df7829a", null ],
      [ "gMacSecurityEncMic64_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37aaccce9a52c72fd2a575b9f3dd3dc7643", null ],
      [ "gMacSecurityEncMic128_c", "group___mac_types.html#gga3577dc87f11ac25a000f1e5685450d37a292867b9a0fb2821101230dc24b39d65", null ]
    ] ],
    [ "keyIdModeType_t", "group___mac_types.html#gafee0ee2f93b9a61ae7cb3a2480f31867", [
      [ "gKeyIdMode0_c", "group___mac_types.html#ggafee0ee2f93b9a61ae7cb3a2480f31867a88fe0f6371f725494280cc7c5b426d2f", null ],
      [ "gKeyIdMode1_c", "group___mac_types.html#ggafee0ee2f93b9a61ae7cb3a2480f31867af50a271b09523e4efa1e052ec2bec7d3", null ],
      [ "gKeyIdMode2_c", "group___mac_types.html#ggafee0ee2f93b9a61ae7cb3a2480f31867a0e9f9d1db66b7a2fd5fc92715f6062ba", null ],
      [ "gKeyIdMode3_c", "group___mac_types.html#ggafee0ee2f93b9a61ae7cb3a2480f31867abd8ccaca4855b09f002d745d00090684", null ]
    ] ],
    [ "logicalChannelId_tag", "group___mac_types.html#gac0ae18bb8add1c463e75192a9ed70da6", null ],
    [ "channelMask_tag", "group___mac_types.html#gaaf29cde21971a087947257db1069cedd", [
      [ "gChannelMask11_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda96e430f9d97cd32c68a3c97135059135", null ],
      [ "gChannelMask12_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda49f8a6d60ce24bedcc13f223af4a7d9a", null ],
      [ "gChannelMask13_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069ceddae4bb772b66b406bd98d5d44f04836b71", null ],
      [ "gChannelMask14_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda2161d682553fdb12319dd9f737070711", null ],
      [ "gChannelMask15_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda8f5ae4be46c76ca2e71cb31410bea33d", null ],
      [ "gChannelMask16_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda1f26a4cfb091be82225af01a9304ba61", null ],
      [ "gChannelMask17_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda06d8c3f72b3d674b0d43c81f24adc4a9", null ],
      [ "gChannelMask18_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda7ac45cd6d5ed623c80eca9fd1b958b30", null ],
      [ "gChannelMask19_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069ceddac528083695c06fe84a6a90f33769f0b5", null ],
      [ "gChannelMask20_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda3f096c4ce14af9b54ba3a582b509bca4", null ],
      [ "gChannelMask21_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda6ae1546d0ad3ab4a53245de0f0e895c3", null ],
      [ "gChannelMask22_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda7b48fdedf7d3bf9474b147da397cec7c", null ],
      [ "gChannelMask23_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069ceddaf63ce19f4b074665e14ce06e7fbc40f8", null ],
      [ "gChannelMask24_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda2e32da029c1c60e51aad557b43f8b18a", null ],
      [ "gChannelMask25_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069ceddae6536152ca5d62653ed1c977d9c1dd8a", null ],
      [ "gChannelMask26_c", "group___mac_types.html#ggaaf29cde21971a087947257db1069cedda3194eb63c8e3076d8ff6496a8da886f7", null ]
    ] ],
    [ "channelPageId_t", "group___mac_types.html#gafaa20b71e82d5ddd84df6d0f62e123d3", [
      [ "gChannelPageId0_c", "group___mac_types.html#ggafaa20b71e82d5ddd84df6d0f62e123d3a7aee6f4e3c885a41db070fb6d5e0bca8", null ],
      [ "gChannelPageId1_c", "group___mac_types.html#ggafaa20b71e82d5ddd84df6d0f62e123d3aee936faa8dc44053edf2f40a687e885e", null ],
      [ "gChannelPageId2_c", "group___mac_types.html#ggafaa20b71e82d5ddd84df6d0f62e123d3a87e326c64dbafdac2df1b719b742db8c", null ]
    ] ],
    [ "macCapabilityInfo_tag", "group___mac_types.html#gaeeb329f9912faec9efd64b716af97f19", [
      [ "gCapInfoAltPanCoord_c", "group___mac_types.html#ggaeeb329f9912faec9efd64b716af97f19a595aa19a7b220a95623b3bf766c42ed2", null ],
      [ "gCapInfoDeviceFfd_c", "group___mac_types.html#ggaeeb329f9912faec9efd64b716af97f19a54500a6ad4cda427dcf54abcb055ebbc", null ],
      [ "gCapInfoPowerMains_c", "group___mac_types.html#ggaeeb329f9912faec9efd64b716af97f19a595cc6058a6fff3332a85a53c8c9dabb", null ],
      [ "gCapInfoRxWhenIdle_c", "group___mac_types.html#ggaeeb329f9912faec9efd64b716af97f19a887f939ad49f9145b93ffe3446e32beb", null ],
      [ "gCapInfoSecurity_c", "group___mac_types.html#ggaeeb329f9912faec9efd64b716af97f19ad5a7ac1f4ba6c2592889e86212238a3d", null ],
      [ "gCapInfoAllocAddr_c", "group___mac_types.html#ggaeeb329f9912faec9efd64b716af97f19a872e4e776be59fbe13c2092f54b10b3d", null ]
    ] ],
    [ "macDisassociateReason_t", "group___mac_types.html#ga3e5847d4e5abd070da33d01a6c6ecece", [
      [ "gDisassociateCoord_t", "group___mac_types.html#gga3e5847d4e5abd070da33d01a6c6ececea3c597844bf698c28912b1bc026d45197", null ],
      [ "gDisassociateDevice_t", "group___mac_types.html#gga3e5847d4e5abd070da33d01a6c6ececeaac8c1db265601100a607126c3546d65c", null ]
    ] ],
    [ "macScanType_t", "group___mac_types.html#gaa3e4e0a62e802303266b60fc1da378f3", [
      [ "gScanModeED_c", "group___mac_types.html#ggaa3e4e0a62e802303266b60fc1da378f3a5be20a2a930c917e26d45a01bf1236a4", null ],
      [ "gScanModeActive_c", "group___mac_types.html#ggaa3e4e0a62e802303266b60fc1da378f3a897007d455d0f16fd607f222f0ccb658", null ],
      [ "gScanModePassive_c", "group___mac_types.html#ggaa3e4e0a62e802303266b60fc1da378f3a9530ec84675e00dd841630d9464167af", null ],
      [ "gScanModeOrphan_c", "group___mac_types.html#ggaa3e4e0a62e802303266b60fc1da378f3a8b09fdbd423646a2dd6c3bee274ba91e", null ],
      [ "gScanModeFastED_c", "group___mac_types.html#ggaa3e4e0a62e802303266b60fc1da378f3a8c84cb618f698b4b0e534b83f3dddfde", null ]
    ] ],
    [ "macPibId_t", "group___mac_types.html#ga874aec6271ac38b4939dbb170e249930", [
      [ "gMPibRole_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a1da4eefcbc0e6e11c6de72940df2d3e2", null ],
      [ "gMPibLogicalChannel_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a7312e1c9f3dce05ae6d2aaca670fdaad", null ],
      [ "gMPibTreemodeStartTime_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a4473c2708f46b3429c1580080cadc321", null ],
      [ "gMPibPanIdConflictDetection_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aaa908274ce58bc21da8620153444c396", null ],
      [ "gMPibBeaconResponseDenied_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a5f310d350715d68dfc4eb07317503a9f", null ],
      [ "gMPibNBSuperFrameInterval_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aea09624089f945bed0232fc686d01396", null ],
      [ "gMPibBeaconPayloadLengthVendor", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ae5d81def6aa2a2d3ced5c81cadca42d6", null ],
      [ "gMPibBeaconResponseLQIThreshold_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aea964394241e7f67c28bfe575409ac66", null ],
      [ "gMPibUseExtendedAddressForBeacon_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a6f7bd306e8e3ede29e7f616bea42dbc8", null ],
      [ "gMPibCsmaDisableMask_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aacd488300b12679efae7f31f61e7d200", null ],
      [ "gMPibAlternateExtendedAddress_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a29b80ea5bad36c1e6c288a9b2083214c", null ],
      [ "gMPibMac2003Compatibility_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a400fee7b5fe49656610e64a42302add1", null ],
      [ "gMPibExtendedAddress_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a71033d9bd315ecd63caf0703a8ef65a8", null ],
      [ "gMPibASN_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a8a33e06d2ae4ec394c39377037d19f7e", null ],
      [ "gMPibEBSN_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aa87e432e1f7c6133fd93e94df144f181", null ],
      [ "gMPibAckWaitDuration_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a13f1fd77e50a3a415e77d93da7a3369f", null ],
      [ "gMPibAssociationPermit_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930af2bfc2f28dc01c3f20b638e8d630cee0", null ],
      [ "gMPibAutoRequest_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aab97fc137cdb954f3cbad1847bd831dc", null ],
      [ "gMPibBattLifeExt_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a0e3420d5391d1ec907bd440bb9c70b7e", null ],
      [ "gMPibBattLifeExtPeriods_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ad3ad5b9ca793d6841006e3ec654178d7", null ],
      [ "gMPibBeaconPayload_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a677495771570ead40de98f61c9a43391", null ],
      [ "gMPibBeaconPayloadLength_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a7d536d169b0f61a73c540a5335d77d93", null ],
      [ "gMPibBeaconOrder_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a56739025be75be480d20f5f64348c0fb", null ],
      [ "gMPibBeaconTxTime_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a717dea321c7578d32974c3c08e1ae363", null ],
      [ "gMPibBSN_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a657b625e2fcbf8f3de5f8aba80ea56b8", null ],
      [ "gMPibCoordExtendedAddress_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ad1eee71e5ce39d796c9643f8a0eda6ec", null ],
      [ "gMPibCoordShortAddress_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ae848531a2d84eeff3919375ba55ad846", null ],
      [ "gMPibDSN_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930af8b20767664ea65dcdaeb4560c4d1d2c", null ],
      [ "gMPibGtsPermit_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aaf07bf4647dd75f2aba660781ac798f3", null ],
      [ "gMPibMaxCSMABackoffs_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a7fa52791e755ec5c89aa9e37d21c9374", null ],
      [ "gMPibMinBE_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a7f44ed8cf55c33f8421ca2e2d6c79533", null ],
      [ "gMPibPanId_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930abe40eb59fb05700350e6f6ce46f42511", null ],
      [ "gMPibPromiscuousMode_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a624517849a5379c64a3b0df92afe117a", null ],
      [ "gMPibRxOnWhenIdle_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a9fa6fd2f7dfbf18975db8955759109d7", null ],
      [ "gMPibShortAddress_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a259c93314b39696c6050d1f61690fac6", null ],
      [ "gMPibSuperframeOrder_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a7faaffd602e4cb46b5f5b5e8d2d48deb", null ],
      [ "gMPibTransactionPersistenceTime_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ab769533a2a33e782def37ebfa68e6a65", null ],
      [ "gMPibAssociatedPANCoord_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a5263355f43f52f651bcf80c53e98d244", null ],
      [ "gMPibMaxBE_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ae647cb7ed31ce788643d2121c2b9c988", null ],
      [ "gMPibMaxFrameTotalWaitTime_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930aa80e7dc28b0f60cdb0872f601f21eb77", null ],
      [ "gMPibMaxFrameRetries_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a4d1b4694cf404839dfd7feba142d1fde", null ],
      [ "gMPibResponseWaitTime_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a9fb93e9ce6555655176eb59adbf9a63b", null ],
      [ "gMPibSyncSymbolOffset_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a033eb3a568a6b6474c29aa1aa49fc515", null ],
      [ "gMPibTimestampSupported_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a6861ba3319eadb16858ff304ba8991f6", null ],
      [ "gMPibSecurityEnabled_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930af2b3b08d8316a2cdfbfd06ff7d37e530", null ],
      [ "gMPibMinLIFSPeriod_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a2e377f0856e87fcae3718263a0d038ad", null ],
      [ "gMPibMinSIFSPeriod_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a527129a06548190effc591ac43d4d762", null ],
      [ "gMPibKeyTableEntries_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a2baec64793e28e56e3bac80983ec1e2f", null ],
      [ "gMPibDeviceTableEntries_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a83a6647972ccc14f17659f30287b43ab", null ],
      [ "gMPibSecurityLevelTableEntries_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930af5ef6ec50db1912945d761fb5fc351c7", null ],
      [ "gMPibAutoRequestSecurityLevel_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930ad3f703754a4d534cf55d87ca010ed0ff", null ],
      [ "gMPibAutoRequestKeyIdMode_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a2971602636959df7581bfe5c85e0f740", null ],
      [ "gMPibAutoRequestKeySource_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930afc0708d4afec657f0523150d65f7dedc", null ],
      [ "gMPibAutoRequestKeyIndex_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a48a8bd8eb09319ad6d1bd2972c4c6b6c", null ],
      [ "gMPibKeyIdLookupListEntries_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a60eee8f3519b8b535fc27edc59a6f072", null ],
      [ "gMPibKeyUsageListEntries_c", "group___mac_types.html#gga874aec6271ac38b4939dbb170e249930a0ecd12ce46e59c00a4a0d7684274dff1", null ]
    ] ],
    [ "beaconType_t", "group___mac_types.html#ga81556d44cc8b4743425567e9b2d80fa0", null ],
    [ "macPayloadIeId_t", "group___mac_types.html#ga7a073f89808af25625775396dc9ab5f0", null ],
    [ "macLinkType_t", "group___mac_types.html#ga34de6640474e864a711092958eb3623d", [
      [ "gMacLinkTypeNormal_c", "group___mac_types.html#gga34de6640474e864a711092958eb3623da8dfbdec6afc526780866b1000029762d", null ],
      [ "gMacLinkTypeAdvertising_c", "group___mac_types.html#gga34de6640474e864a711092958eb3623da4b528ce057a97ecf078e948ebd45b9d7", null ]
    ] ],
    [ "macSetSlotframeOp_t", "group___mac_types.html#gaaa565a42018bfc46f050dcc64a09c535", null ],
    [ "macSetLinkOp_t", "group___mac_types.html#gad94c8601beddc652e6a16303c530fa20", null ],
    [ "macTschMode_t", "group___mac_types.html#ga3d0664c4447c4788eeb7f285b8469169", null ],
    [ "macTschRole_t", "group___mac_types.html#ga3f96166cf43999c67d0b16d759117e46", [
      [ "gMacTschRolePANCoordinator_c", "group___mac_types.html#gga3f96166cf43999c67d0b16d759117e46a9f37ad7b48b3dab550f012e40b51f8e3", null ],
      [ "gMacTschRoleDevice_c", "group___mac_types.html#gga3f96166cf43999c67d0b16d759117e46a364f0d6d807ebb946c729dc14f35af02", null ]
    ] ]
];