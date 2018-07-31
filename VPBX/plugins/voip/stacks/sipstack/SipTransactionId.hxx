#ifndef Sip_Transaction_Id__hxx
#define Sip_Transaction_Id__hxx
/*
* $Log: SipTransactionId.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipTransactionId.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Data.hxx"

namespace Assist
{

class SipMsg;
    
class SipTransactionId
{
    public:
	/**
	 * the first level key type - it's been chosen to be Data
	 * instead of MD5 unless we can be assured that MD5 will
	 * provide BETTER randomness (in hashing)than a simple Data
	 * string.
	 *
	 * this level will consist of To (NameAddress only), From
	 * (NameAddress only) and the CallID - specifically the
	 * CallLeg (w/o the tags)
	 *
	 * this level will group together all the messages for a
	 * single CallLeg.
	 */
	typedef Data KeyTypeI;

	/**
	 * the second level key type will consist of [CSeq
	 * Number]+[top Via Branch]
	 * 
	 * this is the level at which cancel and retransmissions take
	 * effect
	 */
	typedef Data KeyTypeII;

	/**
	 * the third level key will consist of [Cseq Method], and is
	 * required to * keep distinct request-response sequences
	 * during the same Cseq & branch.
	 *
	 * its been chosen to be Data and not "Method" (enum from
	 * symbols.hxx) 'coz SipCSeq returns method param as Data and
	 * to convert it to enum will have to compare and test thru
	 * all method strings, which can be ammortized by string
	 * compare at lookup time - the plus being not to do the ugly
	 * work here and keep maintaning it!
	 *
	 * NOTE: the method tag of Ack msgs will not be changed to
	 * INVITE. it will be keyed as ACK, and for 200 filtering the
	 * user will search in level3 list for key type ACK (there's
	 * ack only for responses of invites, hence there'll be only
	 * one ACK in the level3 list).
	 */
	typedef Data KeyTypeIII;

	SipTransactionId(const SipMsg& sipMsg);
	SipTransactionId(const SipTransactionId& sipTransactionId);
	~SipTransactionId();

	SipTransactionId& operator= (const SipTransactionId& sipTransactionId);
	bool operator==(const SipTransactionId& sipTransactionId) const;
	bool operator<(const SipTransactionId& sipTransactionId) const;

	KeyTypeI& getLevel1() const;
	KeyTypeII& getLevel2() const;
	KeyTypeIII& getLevel3() const;
	bool getValid() const;
      SipTransactionId();
    protected:
	

    private:
	bool               valid;
	mutable KeyTypeI   level1;
	mutable KeyTypeII  level2;
	mutable KeyTypeIII level3;
};
 
}

#endif
