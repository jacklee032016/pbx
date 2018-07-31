#ifndef SDP2EXTERNS_HXX_
#define SDP2EXTERNS_HXX_

/*
* $Log: Sdp2Externs.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:46  lizhijie
* no message
*
* $Id: Sdp2Externs.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

/** Infrastructure common to VOCAL.
 */
namespace Assist 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


// The following const char []s are located in SdpSession.cxx

extern const char SdpNetworkTypeIN[];

extern const char SdpAddressTypeIP4[];
extern const char SdpAddressTypeIP6[];

extern const char SdpMediaTransportRTPAVP[];
extern const char SdpMediaTransportUDP[];
extern const char SdpMediaTransportMPA[];

extern const char SdpBandwidthModifierAS[];
extern const char SdpBandwidthModifierCT[];
extern const char SdpBandwidthModifierRR[];

extern const char SdpMediaTypeAudio[];
extern const char SdpMediaTypeVideo[];

extern const char SdpAttributeRecvonly[];
extern const char SdpAttributeSendrecv[];
extern const char SdpAttributeSendonly[];

extern const char SdpEncryptkeyMethodClear[];
extern const char SdpEncryptkeyMethodBase64[];
extern const char SdpEncryptkeyMethodURI[];
extern const char SdpEncryptkeyMethodPrompt[];

}

}

#endif
