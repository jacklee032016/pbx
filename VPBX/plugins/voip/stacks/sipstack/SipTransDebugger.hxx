#ifndef Sip_trans_debugger__hxx
#define Sip_trans_debugger__hxx
/*
* $Log: SipTransDebugger.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTransDebugger.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipTransactionLevels.hxx"
#include "SipTransHashTable.hxx"
#include "Data.hxx"

namespace Assist
{
    
class SipTransDebugger
{
    public:
	static Data printDebug(SipTransHashTable* table);

	//// add one each for any more as needed...

    protected:
	SipTransDebugger();
	~SipTransDebugger();

    private:
	SipTransDebugger(const SipTransDebugger&);
	SipTransDebugger& operator = (const SipTransDebugger&);
};
 
}

#endif
