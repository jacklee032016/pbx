#ifndef RTPTYPES_H
#define RTPTYPES_H
/*
 * $Id: rtpTypes.h,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
 */

#include <stdlib.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <limits.h>
#include "vovida-endian.h"

#include "NtpTime.hxx"

/// Version of this RTP, always 2
const int RTP_VERSION = 2;

const int RTP_SEQ_MOD = 1 << 16;
const u_int32_t RTP_TIME_MOD = UINT_MAX; 

/// Maximum UDP packet size, 8129
const int RTP_MTU = 8129;

/// 32-bit sequence number
//typedef u_int16_t RtpSeqNumber;
typedef int RtpSeqNumber;

/// Middle 32-bit of NTP
typedef u_int32_t RtpTime;

/// 32-bit source number
typedef u_int32_t RtpSrc;




/* ----------------------------------------------------------------- */
/* --- RTP Structures ---------------------------------------------- */
/* ----------------------------------------------------------------- */

typedef void* RtpPacketPtr;
typedef void* RtpReceiverPtr;
typedef void* RtpTransmitterPtr;
typedef void* RtpSessionPtr;

class RtpTransmitter;
class RtpReceiver;


/** Supported types of RTP payloads (encodings).
 */
typedef enum
{
    rtpPayloadPCMU = 0,
    rtpPayload1016 = 1,
    rtpPayloadG726_32 = 2,
    rtpPayloadGSM = 3,
    rtpPayloadG723 = 4,
    rtpPayloadDVI4_8KHz = 5,
    rtpPayloadDVI4_16KHz = 6,
    rtpPayloadLPC = 7,
    rtpPayloadPCMA = 8,
    rtpPayloadG722 = 9,
    rtpPayloadL16_stereo = 10,
    rtpPayloadL16_mono = 11,
    rtpPayloadQCELP = 12,
    rtpPayloadMPA = 14,
    rtpPayloadG728 = 15,
    rtpPayloadDVI4_11KHz = 16,
    rtpPayloadDVI4_22KHz = 17,
    rtpPayloadG729 = 18,
    rtpPayloadCN = 19,
    rtpPayloadGSMEFR = 20,
    rtpPayloadG726_40 = 21,
    rtpPayloadG726_24 = 22,
    rtpPayloadG726_16 = 23,
    rtpPayloadCelB = 25,
    rtpPayloadJPEG = 26,
    rtpPayloadNV = 28,
    rtpPayloadH261 = 31,
    rtpPayloadMPV = 32,
    rtpPayloadMP2T = 33,
    rtpPayloadH263 = 34,
    rtpPayloadDynMin = 96,
    rtpPayLoadDyn98 = 98,
    rtpPayLoadDyn99 = 99,
    rtpPayLoadDyn100 = 100,
    rtpPayloadDTMF_RFC2833 = 100,
    rtpPayLoadDyn101 = 101,
    rtpPayloadCiscoRtp = 121,
    rtpPayloadL16_8k_mono = 122,
    rtpPayloadDynMax = 127,         /* max int of 7-bit unsigned int */
    rtpPayloadUndefined = 128    
} RtpPayloadType;


/// RTP packet header
struct RtpHeaderStruct
{
#if (__BYTE_ORDER) == (__LITTLE_ENDIAN)
    u_int32_t count:4;              // csrc count
    u_int32_t extension:1;          // header extension flag
    u_int32_t padding:1;            // padding flag - for encryption
    u_int32_t version:2;            // protocol version


    u_int32_t type:7;               // payload type
    u_int32_t marker:1;             // marker bit - for profile

    u_int32_t sequence:16;          // sequence number of this packet 
                                    // (needs endian conversion here, 
                                    // done in the access methods )


#elif __BYTE_ORDER == __BIG_ENDIAN

    u_int8_t version:2;             // protocol version
    u_int8_t padding:1;             // padding flag - for encryption
    u_int8_t extension:1;           // header extension flag
    u_int8_t count:4;               // csrc count

    u_int8_t marker:1;              // marker bit - for profile
    u_int8_t type:7;                // payload type

    u_int16_t sequence:16;          // sequence number of this packet 
                                    // (needs endian conversion here, 
                                    // done in the access methods )

#else
#error "Problem in <endian.h>"
#endif

#if 1

    /// timestamp of this packet
    RtpTime timestamp;
    /// source of packet
    RtpSrc ssrc;
    /// list of contributing sources
    RtpSrc startOfCsrc;
#endif
};
typedef struct RtpHeaderStruct RtpHeader;


// Transmitter errors
typedef enum
{
    tran_success = 0
} RtpTransmitterError;


// Receiver errors
typedef enum
{
    recv_success = 0,
    recv_bufferEmpty = 20,
    recv_lostPacket = 21
} RtpReceiverError;


/* ----------------------------------------------------------------- */
/* --- RTCP Structures --------------------------------------------- */
/* ----------------------------------------------------------------- */

typedef void* RtcpPacketPtr;
typedef void* RtcpReceiverPtr;
typedef void* RtcpTransmitterPtr;


// Supported RTCP types
typedef enum
{
    rtcpTypeSR = 200,
    rtcpTypeRR = 201,
    rtcpTypeSDES = 202,
    rtcpTypeBYE = 203,
    rtcpTypeAPP = 204             // not implemented
} RtcpType;


// Supported SDES types
typedef enum
{
    rtcpSdesEnd = 0,
    rtcpSdesCname = 1,
    rtcpSdesName = 2,
    rtcpSdesEmail = 3,
    rtcpSdesPhone = 4,
    rtcpSdesLoc = 5,
    rtcpSdesTool = 6,
    rtcpSdesNote = 7,
    rtcpSdesPriv = 8              // not implemented
} RtcpSDESType;



/// RTCP header
struct RtcpHeaderStruct
{
#if __BYTE_ORDER == __LITTLE_ENDIAN
u_int32_t count:
    5;
u_int32_t padding:
    1;
u_int32_t version:
    2;
#elif __BYTE_ORDER == __BIG_ENDIAN
/// protocal version
u_int32_t version:
    2;
    /// padding flag
u_int32_t padding:
    1;
    /// depending on packet type
u_int32_t count:
    5;
#else
#error "Problem in <endian.h>"
#endif
    /// type of RTCP packet
u_int32_t type:
    8;
    /// lenght of RTCP packet in octlets minus 1
u_int32_t length:
    16;
};
typedef struct RtcpHeaderStruct RtcpHeader;


/// report block
struct RtcpReportStruct
{
    /// source being reported
    RtpSrc ssrc;

    /* endian problem here? - kle */
    /// fraction lost since last report
u_int32_t fracLost:
    8;
    /// cumulative packet lost - signed
    unsigned char cumLost[3];

    /// number of cycles
u_int32_t recvCycles:
    16;
    /// last seq number received
u_int32_t lastSeqRecv:
    16;
    /// interval jitter
    u_int32_t jitter;
    /// last SR packet received from ssrc
    u_int32_t lastSRTimeStamp;
    /// delay since last SR packet
    u_int32_t lastSRDelay;
};
typedef struct RtcpReportStruct RtcpReport;


/// sender info
struct RtcpSenderStruct
{
    /// source of sender
    RtpSrc ssrc;
    /// seconds of NTP
    RtpTime ntpTimeSec;
    /// fractional seconds of NTP
    RtpTime ntpTimeFrac;
    /// transmitter RTP timestamp
    RtpTime rtpTime;
    /// number of packets sent
    u_int32_t packetCount;
    /// number of octlets sent
    u_int32_t octetCount;
};
typedef struct RtcpSenderStruct RtcpSender;


/// bye reason item
struct RtcpByeStruct
{
    /// lenght of text
    u_int8_t length;
    /// reason for leaving, not null-term
    char startOfText;
};
typedef struct RtcpByeStruct RtcpBye;


/// source descrp item
struct RtcpSDESItemStruct
{
    /// type of description
    u_int8_t type;
    /// lenght of item
    u_int8_t length;
    /// text description not null-term
    char startOfText;
};
typedef struct RtcpSDESItemStruct RtcpSDESItem;


/// source descrp chunk
struct RtcpChunkStruct
{
    /// source being described
    RtpSrc ssrc;
    /// list of SDES information, ending with rtcpSdesEnd
    RtcpSDESItem startOfItem;
};
typedef struct RtcpChunkStruct RtcpChunk;


/// SDES information
struct SDESdataStruct
{
    /// CNAME for this source
    char cname [256];
    /// NAME for this source
    char name [256];
    /// EMAIL for this source
    char email [256];
    /// PHONE for this source
    char phone [256];
    /// LOC for this source
    char loc [256];
    /// TOOL for this source
    char tool [256];
    /// NOTE for this source
    char note [256];
};
typedef struct SDESdataStruct SDESdata;

/// receiver information
struct RtpTranInfoStruct
{
    /// SSRC number for recv
    RtpSrc ssrc;
    /// pointer to receiver for specific information
    RtpReceiver* recv;
    /// number packets expected in last interval
    int expectedPrior;
    /// number of packets actually received in last RTCP interval
    int receivedPrior;

    /// SDES information
    SDESdata SDESInfo;

    /// LSR timestamp which will be one of the fields of the next SR sent out
    u_int32_t lastSRTimestamp;

    /// receiveing time of the last SR received
    NtpTime recvLastSRTimestamp;

};
typedef struct RtpTranInfoStruct RtpTranInfo;




/* ----------------------------------------------------------------- */
/* --- RTP Session ------------------------------------------------- */
/* ----------------------------------------------------------------- */


// States
typedef enum
{
    rtp_session_undefined = -1,
    rtp_session_inactive = 0,
    rtp_session_sendonly = 1,
    rtp_session_recvonly = 2,
    rtp_session_sendrecv = 3
} RtpSessionState;


// Session errors
typedef enum
{
    session_success = 0,
    session_wrongState = 20
} RtpSessionError;


/* ----------------------------------------------------------------- */
/* --- RTP Events -------------------------------------------------- */
/* ----------------------------------------------------------------- */


struct RtpEventDTMFRFC2833Struct
{
u_int32_t event:
    8;
#if __BYTE_ORDER == __LITTLE_ENDIAN
u_int32_t volume:
    6;
u_int32_t reserved:
    1;
u_int32_t edge:
    1;
#elif __BYTE_ORDER == __BIG_ENDIAN
u_int32_t edge:
    1;
u_int32_t reserved:
    1;
u_int32_t volume:
    6;
#else
#error "Problem in <endian.h>"
#endif
u_int32_t duration:
    16;
};
typedef struct RtpEventDTMFRFC2833Struct RtpEventDTMFRFC2833;


struct RtpEventDTMFCiscoRtp
{
u_int32_t sequence:
    8;
#if __BYTE_ORDER == __LITTLE_ENDIAN
u_int32_t level:
    5;
u_int32_t reserved0:
    3;
u_int32_t edge:
    5;
u_int32_t digitType:
    3;
u_int32_t digitCode:
    5;
u_int32_t reserved1:
    3;
#elif __BYTE_ORDER == __BIG_ENDIAN
u_int32_t reserved0:
    3;
u_int32_t level:
    5;
u_int32_t digitType:
    3;
u_int32_t edge:
    5;
u_int32_t reserved1:
    3;
u_int32_t digitCode:
    5;
#else
#error "Problem in <endian.h>"
#endif
};
typedef struct RtpEventDTMFCiscoRtpStruct RtpEventCiscoRtp;
     

/** Events for DTMF (touch tone) events in RTP streams.
 */
enum DTMFEvent
{
    DTMFEventNULL = -1,
    DTMFEventDigit0,
    DTMFEventDigit1,
    DTMFEventDigit2,
    DTMFEventDigit3,
    DTMFEventDigit4,
    DTMFEventDigit5,
    DTMFEventDigit6,
    DTMFEventDigit7,
    DTMFEventDigit8,
    DTMFEventDigit9,
    DTMFEventDigitStar,
    DTMFEventDigitHash
};

enum KeyEvent
{
    KeyEventNULL,
    KeyEventOn,
    KeyEventEdge,
    KeyEventOff
};


#endif // RTPTYPES_H
