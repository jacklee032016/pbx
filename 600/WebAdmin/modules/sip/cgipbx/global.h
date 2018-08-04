/*$Id: global.h,v 1.5 2007/03/14 18:57:47 lizhijie Exp $*/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#if DEBUG

// #define debug(a) printf(a)
// #define debug2(a,b) printf(a,b)
#define debug3(a,b,c) printf(a,b,c)
#define debug4(a,b,c,d,e) printf(a,b,c,d,e)
#else
//#define debug(a) 
//#define debug2(a,b) 
#define debug3(a,b,c)
#define debug4(a,b,c,d,e)
#endif

// global macros

//query name
//#define	CTYPE	"ct"
//#define	VTYPE	"vt"

//query value (pbx configuration pages)
#define	PBXPC	"pc"	//pbx conf page
#define	PBXBC	"bc"	//branch conf page
#define   PBXBCS  "bcs"
#define	PBXLC	"lc"	//line conf page
#define	PBXCC	"cc"
#define	PBXAC	"ac"
#define	PBXSC	"sc"
#define	PBXIC	"ic"
#define	PBXHC	"hc"

//input item
#define	PC01	"pc01"	// Ring differ
#define	PC02	"pc02"	// callID mode
#define	PC03	"pc03"	// do dial action 
#define	PC04	"pc04"	// line to line
#define	PC05	"pc05"	// passwd
#define	PC06	"pc06"	// PSNT prefix 
#define	PC07	"pc07"	// system password
#define	PC08	"pc08"	// special number first 
#define	PC09	"pc09"	// group call
#define	PC10	"pc10"	// ip prefix 
#define	PC99	"pc99"	// reset 

//input BC item
#define	BC01 	"bc01"
#define	BC02	"bc02"	// dial out mode: a:ip b:pstn
#define	BC03	"bc03"
#define	BC04	"bc04"
#define	BCS05	"bcs05"	// busy transfer 
#define	BCS06	"bcs06"	// no response transfer 
#define	BCS07	"bcs07"	// secret transfer
#define	BC08	"bc08"
#define	BC09	"bc09"
#define	BCS10	"bcs10"	// night service
#define	BC11	"bc11"
#define	BCS12	"bcs12"	/* secret transfer ??? */
#define	BC13	"bc13"
#define	BC18	"bc18"
#define	BCS14	"bcs14"	// unconditional transfer
#define   BCS15    "bcs15"
#define   BCS17    "bcs17"	// my group
#define   BCS25    "bcs25"	// differ ring for ext
#define   BC22      "bc22"
#define   BC26      "bc26"

//input AC item
#define	AC01	"ac01"
#define	AC02	"ac02"
#define	AC03	"ac03"
#define	AC04	"ac04"

//input LC item
#define	LC01	"lc01"
#define	LC02	"lc02"
#define	LC03	"lc03"

//input CC item
#define	CC01	"cc01"
#define	CC02	"cc02"
#define	CC03	"cc03"

//input SC item
#define	SC02	"sc02"

//global vars

#endif

