#ifndef Sip_trans_debugger__hxx
#define Sip_trans_debugger__hxx
/*
 * $Id: SipTransDebugger.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "SipTransactionLevels.hxx"
#include "SipTransHashTable.hxx"
#include "Data.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
