#ifndef SNMP_COMMON_HXX_
#define SNMP_COMMON_HXX_
/*
 * $Id: SnmpCommon.h,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */

#include "vtypes.h"

enum ServerType
{
    SERVER_Unknown = 0,
    SERVER_FS,
    SERVER_MS,
    SERVER_RS,
    SERVER_JS,
    SERVER_Agent,
    SERVER_NetMgnt,
    SERVER_SipStack,
    SERVER_CDR,
    SERVER_MgcpStack,
    SERVER_H323,
    SERVER_UA,
    SERVER_HB,
    SERVER_POS,
    SERVER_CA,
    SERVER_PS,
    SERVER_UAVM
};

enum ServerStatus
{
    Inactive = 0,
    Active,
    ActivePending,
    UnknownStatus
};


enum DataType
{
    UnknownData = 0,
    HeartbeatData,
    RedundancyData,
    MaintenanceData
};


typedef struct
{
    u_int16_t dataType;
    u_int16_t padding;
    u_int16_t port;
    u_int16_t load;
}
heartbeatPortData;


typedef struct
{
    u_int16_t dataType;
    u_int16_t padding;
    u_int16_t port;
    u_int16_t info;
    u_int32_t misc;
}
redundancyPortData;


/* the following may need to be at a higher level */
enum voReturnStatus
{
    voSuccess,
    voSuccessWithIssues,
    voNoMoreData,
    voFailure,
    voUnknown,
    voTimeout,
    voInvalidInParm
};


#endif
