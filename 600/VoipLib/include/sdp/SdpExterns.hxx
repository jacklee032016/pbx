#ifndef SDPEXTERNS_HXX_
#define SDPEXTERNS_HXX_
/*
 * $Id: SdpExterns.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */


// The following const char []s are located in SdpSession.cxx

extern const char SdpNetworkTypeIN[];

extern const char SdpAddressTypeIP4[];
extern const char SdpAddressTypeIP6[];

extern const char SdpMediaTransportRTPAVP[];
extern const char SdpMediaTransportUDP[];

extern const char SdpBandwidthModifierAS[];
extern const char SdpBandwidthModifierCT[];

extern const char SdpMediaTypeAudio[];
extern const char SdpMediaTypeVideo[];

extern const char SdpAttributeRecvonly[];
extern const char SdpAttributeSendrecv[];
extern const char SdpAttributeSendonly[];

extern const char SdpEncryptkeyMethodClear[];
extern const char SdpEncryptkeyMethodBase64[];
extern const char SdpEncryptkeyMethodURI[];
extern const char SdpEncryptkeyMethodPrompt[];

#endif
