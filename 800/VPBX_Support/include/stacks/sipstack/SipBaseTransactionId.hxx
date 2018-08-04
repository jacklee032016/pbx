#ifndef SIPBASETRANSACTIONID_HXX_
#define SIPBASETRANSACTIONID_HXX_

/*
* $Log: SipBaseTransactionId.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipBaseTransactionId.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "SipCallId.hxx"
#include "SipCSeq.hxx"
#include "Sptr.hxx"
#include "SipMsg.hxx"
#include "Md5.hxx"

#if defined(__linux__)
#include <hash_map>
#endif

namespace Assist
{

using Crypto::Md5;

/// data container for TransactionId header
class SipBaseTransactionId
{
    public:
        /// Create one with default values
        SipBaseTransactionId();
        ///
        SipBaseTransactionId(const SipMsg&, bool useVia = true);
        ///
        SipBaseTransactionId(const SipBaseTransactionId& other);

        ///
        void setToTag(const Data& toTag);

	/// get the To tag
	Data getToTag() const;

        ///
        void setFromTag(const Data& fromTag);

        ///
        bool operator == (const SipBaseTransactionId& other) const;
        ///
        bool operator < (const SipBaseTransactionId& other) const;
        ///
        bool operator > (const SipBaseTransactionId& other) const;
        ///
        const SipBaseTransactionId& operator =(const SipBaseTransactionId& src);
    
        ///
        void print() const;
        ///
	Data getSummary() const;
       
        void printComparison( const SipBaseTransactionId& other) const;
	///
        void replaceCSeqMethod( Method method );
        ///
        bool toBeDeleted(time_t timeNow) const ;

	///compare two transactionids, omitting the Via
	bool compareOmitVia(const SipBaseTransactionId& other) const;
	

	///
	size_t hashfn() const;
    private:
	void setMethodFromData(const Data& data);
	///
	Md5 mySum;
	///
	Data myToTag;
	///
	Data myFromTag;
	///
	Data myMethod;
        ///
	Data myViaHost;
        ///
	Data myViaPort;
        ///
	Data myBranchTag;

	///
        time_t myTimeCreated;
};

#if defined(__linux__)
struct hash<SipBaseTransactionId>
{
	inline size_t  
	operator()(const SipBaseTransactionId& s) const 
	{
	    return s.hashfn();
	}
};
#endif


inline
size_t
SipBaseTransactionId::hashfn() const
{
    return mySum.hashfn();
}


inline
SipBaseTransactionId::SipBaseTransactionId()
{
}


inline
SipBaseTransactionId::SipBaseTransactionId(const SipMsg& msg, bool useVia)
{
    Data data = msg.getCallId().getData();
    data += "|";
    data += msg.getCSeq().getCSeqData();
    data += "|";
    data += msg.getTo().getUrl()->getUniqueKey();
    data += "|";
    data += msg.getFrom().getUrl()->getUniqueKey();

    mySum.digest(data);
    myToTag = msg.getTo().getTag();
    myFromTag = msg.getFrom().getTag();
    myMethod = msg.getCSeq().getMethod();
    myViaHost = msg.getVia(0).getHost();
    myViaPort = msg.getVia(0).getPort();
    myBranchTag = msg.getVia(0).getBranch();

    cpLog(LOG_DEBUG_STACK,"TransID is digest[%s] ToTag[%s]...",data.getData(),myToTag.getData());
}


inline
SipBaseTransactionId::SipBaseTransactionId(const SipBaseTransactionId& other)
    :
    mySum(other.mySum),
    myToTag(other.myToTag),
    myFromTag(other.myFromTag),
    myMethod(other.myMethod),
    myViaHost(other.myViaHost),
    myViaPort(other.myViaPort),
    myBranchTag(other.myBranchTag),
    myTimeCreated(other.myTimeCreated)
{
}


inline void 
SipBaseTransactionId::setToTag(const Data& toTag)
{
    myToTag = toTag;
}


inline Data 
SipBaseTransactionId::getToTag() const
{
    return myToTag;
}


inline void 
SipBaseTransactionId::setFromTag(const Data& fromTag)
{
    myFromTag = fromTag;
}

inline void 
SipBaseTransactionId::replaceCSeqMethod( Method method )
{
    switch (method)
    {
        case SIP_INVITE:
        {
            myMethod = INVITE_METHOD;
            break;
        }
        case SIP_ACK:
        {
            myMethod = ACK_METHOD;
            break;
        }
        case SIP_CANCEL:
        {
            myMethod = CANCEL_METHOD;
            break;
        }
        case SIP_BYE:
        {
            myMethod = BYE_METHOD;
            break;
        }
        case SIP_INFO:
        {
            myMethod = INFO_METHOD;
            break;
        }
        case SIP_REGISTER:
        {
            myMethod = REGISTER_METHOD;
            break;
        }
        case SIP_OPTIONS:
        {
            myMethod = OPTIONS_METHOD;
            break;
        }
        case SIP_TRANSFER:
        {
            myMethod = TRANSFER_METHOD;
            break;
        }
        case SIP_SUBSCRIBE:
        {
            myMethod = SUBSCRIBE_METHOD;
            break;
        }
        case SIP_NOTIFY:
        {
            myMethod = NOTIFY_METHOD;
            break;
        }
        default:
        {
            break;
        }
    }//end switch
}


inline bool
SipBaseTransactionId::operator == (const SipBaseTransactionId& other) const
{
    if(&other == this)
    {
	return true;
    }
    else
    {
	return ((mySum == other.mySum) &&
		(myToTag == other.myToTag) &&
		(myFromTag == other.myFromTag) &&
		(myMethod == other.myMethod) &&
		(myViaHost == other.myViaHost) &&
		(myViaPort == other.myViaPort) &&
		(myBranchTag == other.myBranchTag)
	    );
    }
}

inline bool
SipBaseTransactionId::operator < (const SipBaseTransactionId& other) const
{
    if(&other == this)
    {
	return false;
    }
    else
    {
	if(mySum < other.mySum)
	    return true;
	else if (mySum > other.mySum)
	    return false;
	else if(myToTag < other.myToTag)
	    return true;
	else if(myToTag > other.myToTag)
	    return false;
	else if(myFromTag < other.myFromTag)
	    return true;
	else if(myFromTag > other.myFromTag)
	    return false;
	else if(myMethod < other.myMethod)
	    return true;
	else if(myMethod > other.myMethod)
	    return false;
	else if(myViaHost < other.myViaHost)
	    return true;
	else if(myViaHost > other.myViaHost)
	    return false;
	else if(myViaPort < other.myViaPort)
	    return true;
	else if(myViaPort > other.myViaPort)
	    return false;
	else if(myBranchTag < other.myBranchTag)
	    return true;
	else if(myBranchTag > other.myBranchTag)
	    return false;
	else
	    return false;
    }

}


inline bool
SipBaseTransactionId::operator > (const SipBaseTransactionId& other) const
{
    if(&other == this)
    {
	return false;
    }
    else
    {
	if(mySum < other.mySum)
	    return false;
	else if (mySum > other.mySum)
	    return true;
	else if(myToTag < other.myToTag)
	    return false;
	else if(myToTag > other.myToTag)
	    return true;
	else if(myFromTag < other.myFromTag)
	    return false;
	else if(myFromTag > other.myFromTag)
	    return true;
	else if(myMethod < other.myMethod)
	    return false;
	else if(myMethod > other.myMethod)
	    return true;
	else if(myViaHost < other.myViaHost)
	    return false;
	else if(myViaHost > other.myViaHost)
	    return true;
	else if(myViaPort < other.myViaPort)
	    return false;
	else if(myViaPort > other.myViaPort)
	    return true;
	else if(myBranchTag < other.myBranchTag)
	    return false;
	else if(myBranchTag > other.myBranchTag)
	    return true;
	else
	    return false;
    }
}


inline const SipBaseTransactionId& 
SipBaseTransactionId::operator =(const SipBaseTransactionId& src)
{
    if(&src != this)
    {
	mySum = src.mySum;
	myToTag = src.myToTag; 
	myFromTag = src.myFromTag;
	myMethod = src.myMethod; 
	myTimeCreated = src.myTimeCreated;
	myViaHost = src.myViaHost;
	myViaPort = src.myViaPort;
	myBranchTag = src.myBranchTag;
	
    }
    return *this;
}

       
inline void
SipBaseTransactionId::printComparison( const SipBaseTransactionId& other) const
{

}


inline bool 
SipBaseTransactionId::toBeDeleted(time_t timeNow) const
{
    return false;
}


inline bool 
SipBaseTransactionId::compareOmitVia(const SipBaseTransactionId& other) const
{
    if(&other == this)
    {
	return true;
    }
    else
    {
	return ((mySum == other.mySum) &&
		(myToTag == other.myToTag) &&
		(myFromTag == other.myFromTag) &&
		(myMethod == other.myMethod));
    }
}
 
}

#endif
