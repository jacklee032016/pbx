#ifndef STATUSMSG_HXX_
#define STATUSMSG_HXX_
/*
* $Log: StatusMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: StatusMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include "cpLog.h"
#include "SipStatusLine.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipServer.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipSession.hxx"
#include "SipMsg.hxx"
#include "SipInReplyTo.hxx"
#include "symbols.hxx"
#include "SipWarning.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"

namespace Assist
{
    
class SipUnsupported;
class SipCommand;

enum SipStatusErrorType
{
    STATUS_UNKNOWN_NETTYPE,
    STATUS_UNKNOWN_ADDRTYPE,
    STATUS_PARAM_NUMERR,
    STATUS_UNDEF_MODIFIER,
    STATUS_UNKNOWN_FIELD,
    STATUS_FAILED,
    STATUS_PARAM_NOTSET
};


///class to handle exceptions of SDPMedia.
class SipStatusException
{
    public:
        SipStatusException(SipStatusErrorType i)
        {
            value = i;
        }
        SipStatusErrorType value;
};


/**
   Class for all SIP STATUS messages.
   <p>
   <b>Example</b>
   <p>
   <pre>
      Data url;
      url = "sip:destination@192.168.5.12";
      Sptr<SipUrl> tourl = new SipUrl (url);
      int listenPort = 5060;
      int rtpPort = 3456;

      InviteMsg invite(tourl, listenPort, rtpPort);

      //Create Status message off the Invite
      StatusMsg status(invite, 183);
      SipSession testSession("Media");
      status.setSession(testSession);
   </pre>
*/
class StatusMsg : public SipMsg
{
    public:
        ///
        StatusMsg(SipCommand& command, int statusCode);
        ///
        StatusMsg(const Data &) throw (SipParserException&);
        ///
        StatusMsg();
        ///
        StatusMsg(const StatusMsg&);
        ///
        const StatusMsg& operator = (const StatusMsg& src);
        ///
        bool operator ==(const StatusMsg& statusMsg); 
        ///
        void decode(const Data& data);
        ///
        virtual ~StatusMsg();

        ///mime
        void mimeDecode( const Data& gdata );
     
	// ----------------- StatusLine Header Methods ------------------
        /// Get the current StatusLine header
        const SipStatusLine& getStatusLine() const;
        /// Set the StatusLine header
        void setStatusLine( const SipStatusLine& );
        /// Set the StatusLine header
        void setStatusLine( const Data& textData);

        // ------------ ProxyAuthenticate Header Methods ------------------
        /// Get the current ProxyAuthenticate header
        const SipProxyAuthenticate& getProxyAuthenticate() const;
        /// Set the ProxyAuthenticate header
        void setProxyAuthenticate( const SipProxyAuthenticate& );
        /// Set the ProxyAuthenticate header
        void setProxyAuthenticate( const Data& textData);

        // ----------------- Server Header Methods ------------------
        /// Get the current Server header
        const SipServer& getServer() const;
        /// Set the Server header
        void setServer( const SipServer& );
        /// Set the Server header
        void setServer( const Data& textData); 

        /**@name Unsupported Header Methods
          */
        //@{

        /// Get the i'th Via item. If i is -1, it gets the last one 
        const SipUnsupported& getUnsupported( int i=-1) const;

        /** set or add another Unsupported itme, if the index is -1,
            it is appended to the current list */

        void setUnsupported(const SipUnsupported& item, int index = -1);

         /** set or add another Unsupported itme, if the index is -1,
             it is appended to the current list */
        void setUnsupported(const Data& item, int index = -1);
        /** Set number of Unsupported items.
        If i is less than current number then the extras are deleted. */
        void setNumUnsupported(int i);
        ///
        void flushUnsupportedList();
        ///
        void copyUnsupportedList( const StatusMsg& srcMsg);
        ///
        void addsipUnsupported(SipUnsupported* sipUnsupported);
        ///
        const vector < SipUnsupported* > & getUnsupportedList() const;
        ///return true if both the objects contain identical unsupported lists.
        bool compareUnsupportedList(const StatusMsg& src) const; 
        /// get the number of Via items 
        int getNumUnsupported() const;      

        //@}
     
        // ----------------- Warning Header Methods  -----------------
        /// get the number of Warning item

        /** set or add another Warning itme, if the index is -1, it is appended
        to the current list */
        void setWarning(const SipWarning& item, int index = -1);
    
        /** set or add another Warning itme, if the index is -1, it is appended
        to the current list */
        void setWarning(const Data& item, int index = -1);
        /** Set number of Warning items.
        If i is less than current number then the extras are deleted. */
        void setNumWarning(int i);
        ///
        const vector < SipWarning* > & getWarningList() const;
        ///
        void copyWarningList( const StatusMsg& srcMsg);
        ///
        void flushWarningList();

        /** return true if both the objects contain identical
            unsupported lists. */
        bool compareWarningList(const StatusMsg& src) const;       
        /// get the number of Via items 
        int getNumWarning() const;
        /// Get the i'th Via item. If i is -1, it gets the last one 
        const SipWarning& getWarning( int i=-1) const;

        // ----------------- WwwAuthenticate Header Methods ------------------
        /// Get the current WwwAuthenticate header
        const SipWwwAuthenticate& getWwwAuthenticate() const;
        /// Set the WwwAuthenticate header
        void setWwwAuthenticate( const SipWwwAuthenticate& );
        /// Set the WwwAuthenticate header
        void setWwwAuthenticate( const Data& textData);
        ///
        Data encodeWarningList() const;
        ///
        Data encodeUnsupportedList() const;
        ///
        virtual Data encode() const;
        ///
        void setReasonPhrase(const Data& reason);
        ///
        Data getReasonPhrase() const;
        ///
        Method getType() const;
	/**@name InRePlyTo
          */
        //@{

	///
	void setInReplyTo(const SipInReplyTo& item, int index = -1 );
	///
	void setInReplyTo(const Data& item, int index = -1 );
	///
	void setNumInReplyTo(int index);
	///
	const vector < SipInReplyTo* > & getInReplyToList() const;
        ///
        void addsipInReplyTo(SipInReplyTo* sipwarning);
        ///
        void copyInReplyToList( const StatusMsg& srcMsg);
        ///
        void flushInReplyToList();
        ///
        Data encodeInReplyToList() const;
        ///return true if both the objects contain identical InReplyTo lists.
        bool compareInReplyToList(const StatusMsg& src) const;       
        /// get the number of InReplyTo items 
        int getNumInReplyTo() const;
        /// Get the i'th InReplyTo item. If i is -1, it gets the last one 
        const SipInReplyTo& getInReplyTo( int i=-1) const;
         
        //@}

        /**@name SessionTimer headers
          */
        //@{

        ///
        const SipSessionExpires& getSessionExpires() const;
        ///
        const SipMinSE& getMinSE() const ;
        ///
        void setSessionExpires(const SipSessionExpires& sess);
        ///
        void setMinSE(const SipMinSE& minSe);

        //@}

	//Session methods.
	///
	void setSession(const SipSession& newSession);
	///
	void setSession(const Data& newSession);
	///
	const SipSession& getSession() const;
	
	///Check to see if message is a candidate for retransmission
	virtual bool toBeRetransmitted() const;

        // ------------------------------------------------------------
        // authentication stuff
        // ------------------------------------------------------------

        /**@name authentication methods
          */
        //@{

        /** add a basic-based Proxy-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
        **/
        void challengeBasicProxyAuthenticate(Data realm);

        /** add a digest-based Proxy-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
            @param nonce       nonce 
        **/
        void challengeDigestProxyAuthenticate(Data realm, Data nonce);


        /** add a basic-based Www-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
        **/
        void challengeBasicWwwAuthenticate(Data realm);

        /** add a digest-based Www-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
            @param nonce       nonce 
        **/
        void challengeDigestWwwAuthenticate(Data realm, Data nonce);

        //@}

    private:
        SipStatusLine statusLine;

        bool isStatusCodes(const Data& data);
        bool isprotocolSIP(const Data& header);  //only returns true for SIP
        void parseHeaders(const Data& sdata);
        void parseStartLine(const Data& line1);
        void sipdecode(const Data& fgdata);
        void setStatusDetails();
        void addsipWarning(SipWarning* sipwarning);
};
 
} // namespace Assist

#endif
