#ifndef Sip_trans_debugger__hxx
#define Sip_trans_debugger__hxx
/*
* $Log: SipTransDebugger.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipTransDebugger.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
