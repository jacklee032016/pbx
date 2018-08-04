/*
 * $Id: StateBeingXferred.cxx,v 1.2 2007/03/02 04:19:28 lizhijie Exp $
 */


#include "global.h"
#include "StateBeingXferred.hxx"

#include "OpBlindXferred.hxx"
#include "OpConsultXferred.hxx"
#include "OpBeingXferred.hxx"
#include "OpXfereeComplete.hxx"

using namespace Vocal;


StateBeingXferred::StateBeingXferred()
{
    addEntryOperator( new OpBeingXferred );
    addOperator( new OpBlindXferred );
    addOperator( new OpConsultXferred );
    addOperator( new OpXfereeComplete );
}


StateBeingXferred::~StateBeingXferred()
{
}


const char* const
StateBeingXferred::name() const
{
    return "StateBeingXferred";
}
