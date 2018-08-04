#if !defined( VOCAL_COMMON_DOT_HXX )
#define VOCAL_COMMON_DOT_HXX
/*
 * $Id: VocalCommon.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include <cstddef>
#include "vtypes.h"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/**
 *  Typedef used for methods returning status.<br><br>
 *
 *  @see    Vocal::SUCCESS
 */
typedef 	int         ReturnCode;


/**
 *  ReturnCode indicating success.<br><br> 
 *
 *  Also known as 0.
 *
 *  @see    Vocal::ReturnCode
 */
extern const  	ReturnCode  SUCCESS;


} // namespace Vocal


#endif // !defined( VOCAL_COMMON_DOT_HXX )
