#if !defined( ASSIST_COMMON_DOT_HXX )
#define ASSIST_COMMON_DOT_HXX

/*
* $Id: AssistCommon.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <cstddef>
#include "vtypes.h"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{


/**
 *  Typedef used for methods returning status.<br><br>
 *
 *  @see    Assist::SUCCESS
 */
typedef 	int         ReturnCode;


/**
 *  ReturnCode indicating success.<br><br> 
 *
 *  Also known as 0.
 *
 *  @see    Assist::ReturnCode
 */
extern const  	ReturnCode  SUCCESS;


} // namespace Assist


#endif // !defined( ASSIST_COMMON_DOT_HXX )
