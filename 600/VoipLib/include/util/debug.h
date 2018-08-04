#ifndef DEBUG_H_
#define DEBUG_H_
/*
 * $Id: debug.h,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

extern int MY_DEBUG_LEVEL;

extern bool DEBUG_SDP;
extern bool DEBUG_PARSE;
extern bool DEBUG_NTWK;
extern bool DEBUG_NTWK_TIME;
extern bool DEBUG_LOWLEVEL;

#if defined(NDEBUG) || defined(NO_IOSTREAMS_)
#define DEBUG(x) ;
#define HIGH_DEBUG(x) ;
#define XDEBUG(y, x) ;
#define ODEBUG(z, y, x) ;
#define CERR(x) ;
#else
#define DEBUG(x) if(MY_DEBUG_LEVEL > 0) { x; }
#define HIGH_DEBUG(x) if(MY_DEBUG_LEVEL > 2) { x; }
#define XDEBUG(y, x) if(MY_DEBUG_LEVEL > y) { x; }
#define ODEBUG(z, y, x) if(z && (MY_DEBUG_LEVEL > y)) { x; }
#define CERR(x) { x; }
void breakpoint();
#endif


#endif
