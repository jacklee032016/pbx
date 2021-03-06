#ifndef  __VOIP_SIP_HXX__
#define __VOIP_SIP_HXX__
/*
* $Id: voip_sip.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#define 	WITH_CALLWAIT					0

//Informational
#define	SIP_STATUS_CODE_INFO_TRYING								100
#define	SIP_STATUS_CODE_INFO_RINGING							180
#define	SIP_STATUS_CODE_INFO_FORWARDED							181
#define	SIP_STATUS_CODE_INFO_QUEUED								182
#define	SIP_STATUS_CODE_INFO_PROGRESS							183

//Success
#define	SIP_STATUS_CODE_SUCCESS_OK								200
#define	SIP_STATUS_CODE_SUCCESS_ACCEPTED						202

//Redirection
#define	SIP_STATUS_CODE_REDIRECT_MULTIPLE_CHOICE				300
#define	SIP_STATUS_CODE_REDIRECT_MOVE_PERMANENTLY			301
#define	SIP_STATUS_CODE_REDIRECT_MOVE_TEMPLY					302
#define	SIP_STATUS_CODE_REDIRECT_PROXY							305
#define	SIP_STATUS_CODE_REDIRECT_ALTER_SERVICE					380

//Client-Error
#define	SIP_STATUS_CODE_CERROR_BAD_REQUEST						400
#define	SIP_STATUS_CODE_CERROR_UNAUTHORIZED					401
#define	SIP_STATUS_CODE_CERROR_PAYMENT_REQUIREMENT			402
#define	SIP_STATUS_CODE_CERROR_FORBIDDEN						403
#define	SIP_STATUS_CODE_CERROR_NOT_FOUND						404
#define	SIP_STATUS_CODE_CERROR_METHOD_NOT_ALLOW				405
#define	SIP_STATUS_CODE_CERROR_NOT_ACCEPTABLE					406
#define	SIP_STATUS_CODE_CERROR_PROXY_AUTHEN_NEEDED			407
#define	SIP_STATUS_CODE_CERROR_REQUEST_TIMEOUT				408
#define	SIP_STATUS_CODE_CERROR_CONFLICT							409
#define	SIP_STATUS_CODE_CERROR_GONE								410

#define	SIP_STATUS_CODE_CERROR_ENTITY_TOO_LARGE				413
#define	SIP_STATUS_CODE_CERROR_URL_TOO_LARGE					414
#define	SIP_STATUS_CODE_CERROR_MEDIA_UNSUPPORT				415
#define	SIP_STATUS_CODE_CERROR_BAD_EXTENSION					420

#define	SIP_STATUS_CODE_CERROR_TMP_UNAVAILABLE				480
#define	SIP_STATUS_CODE_CERROR_CALLLEG_NOT_EXIST				481		/* call transaction is not exist */
#define	SIP_STATUS_CODE_CERROR_LOOP_DETECT						482
#define	SIP_STATUS_CODE_CERROR_TOO_MANY_HOPS					483
#define	SIP_STATUS_CODE_CERROR_ADDRESS_INCOMPLETE				484
#define	SIP_STATUS_CODE_CERROR_AMBIGUOUS						485
#define	SIP_STATUS_CODE_CERROR_BUSY_HERE						486
#define	SIP_STATUS_CODE_CERROR_REQUEST_TERMINATED			487
#define	SIP_STATUS_CODE_CERROR_NOT_ACCPTABLE_HERE				488
        //{ 411, "Length Required" },    removed in bis-03 (11/2001)

//Server-Error
#define	SIP_STATUS_CODE_SERROR_INTERNAL_ERROR					500
#define	SIP_STATUS_CODE_SERROR_NOT_IMPLEMENTED				501
#define	SIP_STATUS_CODE_SERROR_BAD_GATEWAY					502
#define	SIP_STATUS_CODE_SERROR_SERVICE_UNAVAILABLE			503
#define	SIP_STATUS_CODE_SERROR_SERVER_TIMEOUT					504
#define	SIP_STATUS_CODE_SERROR_VERSION_NOT_SUPPORT			505
#define	SIP_STATUS_CODE_SERROR_MSG_TOO_LARGE					513

//Global-Failure
#define	SIP_STATUS_CODE_GERROR_BUSY								600
#define	SIP_STATUS_CODE_GERROR_DECLINE							603
#define	SIP_STATUS_CODE_GERROR_NOT_EXIST						604
#define	SIP_STATUS_CODE_GERROR_NOT_ACCEPTABLE					606

#define	SIP_STATUS_CODE_UNKNOWN									0

#endif

