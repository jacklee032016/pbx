#ifndef SIPCOMMAND_HXX_
#define SIPCOMMAND_HXX_

/*
* $Log: SipCommand.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipCommand.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <vector>
#include "SipMsg.hxx"
#include "SipRequestLine.hxx"

namespace Assist
{
class SipAlso;
class SipAuthorization;
class SipContentDisposition;
class SipHide;
class SipMaxForwards;
class SipOrganization;
class SipOspAuthorization;
class SipPriority;
class SipProxyAuthorization;
class SipProxyRequire;
class SipRequire;
class SipResponseKey;
class SipRoute;
class SipSubject;
class StatusMsg;

//All command classes throw exceptions of this type.
enum SipCommandErrorType
{
    COMMAND_CSEQ_OUT_OF_RANGE,
    COMMAND_VIA_NOT_PRESENT,
    COMMAND_PARSE_FAILED,
    COMMAND_INDEX_OUT_OF_BOUND
};

/// exception class
class SipCommandException
{
    public:
        SipCommandException(SipCommandErrorType i)
            {
                value = i;
            }
        SipCommandErrorType value;
};

///Base class for all SIP requests
class SipCommand : public SipMsg
{
    public:
        ///
        SipCommand();
        ///
        SipCommand& operator =(const SipCommand&);
        ///
        bool operator ==(const SipCommand& src) const;
        ///
        SipCommand(const SipCommand&); 

        ///
        SipCommand(const SipCommand&, const SipVia&, const SipCSeq& ); 

        ///
        SipCommand(const StatusMsg&); 

        /** decode a SipCommand.
            @return            true if there is an error while parsing, false
            if there is no error

            @param inputData   the message to be parsed
        */
        bool decode(const Data& inputData);
        
        /// Set's the branch number on top (most recent) via
        void setViaBranch( int i );

        /// Get's the branch number on top (most recent) via
        int getViaBranch();

        /**@name RequestLine Header Methods 
          */
        //@{

        /// Returns a COPY of the request line
        const SipRequestLine& getRequestLine() const;

        /// Returns a reference to the request line.
        SipRequestLine& getMutableRequestLine() ;

        /// Set the RequestLine header
        void setRequestLine( const SipRequestLine& );
        
        //@}
        
        /**@name Authorization Header Methods
          */
        //@{

        ///
        const SipAuthorization& getAuthorization() const;

        /// Set the Authorization header
        void setAuthorization( const SipAuthorization& );

        /// Set the Authorization header
        void setAuthorization( const Data& textData);
       
        //@}

        ///
        const SipHide& getHide() const;
        /// Set the Hide header
        void setHide( const SipHide& );
        ///
        void setHide( const Data& textData);

        ///
        const SipAlso& getAlso() const;
        ///
        void setAlso(const SipAlso& );

        ///
        const SipContentDisposition& getContentDisposition() const;
        ///
        void setContentDisposition(const SipContentDisposition&);
        ///
        void setContentDisposition(const Data&);

        /**@name MaxForwards Header Methods
          */
        //@{

        ///
        const SipMaxForwards& getMaxForwards() const;
        
        /// Set the MaxForwards header
        void setMaxForwards( const SipMaxForwards& );

        /// Set the MaxForwards header
        void setMaxForwards( const Data& textData);
        
        ///returns false , if maxforwards becomes < 0.
        bool checkMaxForwards();

        /**returns false , if maxforwards becomes < 0. also adds MaxForwards
          *header if did not exist and sets value = 70. see rfc 
          */
        bool decrementMaxForwards();
        
        //@}

        /**@name Organization Header Methods
          */
        //@{
        ///
        const SipOrganization& getOrganization() const;

        /// Set the Organization header
        void setOrganization( const SipOrganization& );

        /// Set the Organization header
        void setOrganization( const Data& textData);
        //@}

        /**@name Priority Header Methods
          */
        //@{

        ///
        const SipPriority& getPriority() const;

        /// Set the Priority header
        void setPriority( const SipPriority& );

        /// Set the Priority header
        void setPriority( const Data& textData);

        //@}

        // ----------------- ProxyAuthorization Header Methods --------

        /// get the proxyauthorization header
        const SipProxyAuthorization& getProxyAuthorization() const;
        
        /// Set the ProxyAuthorization header
        void setProxyAuthorization( const SipProxyAuthorization& );

        /// Set the ProxyAuthorization header
        void setProxyAuthorization( const Data& textData);

        /// get the number of Require items
        int getNumProxyRequire() const;

        /// Get the i'th Require item. If i is -1, it gets the last one
        const SipProxyRequire& getProxyRequire( int i = -1) const;
       
        /** set or add another Require itme, if the index is -1, it is
          * appended to the current list  
          */
        void setProxyRequire(SipProxyRequire item, int index = -1);
      
        /** set or add another Require itme, if the index is -1, it is
          * appended to the current list  
          */
        void setProxyRequire(const Data&item, int index = -1);

        /** Set number of Require items.
            If i is less than current number then the extras are deleted. */
        void setNumProxyRequire(int i);

        ///
        void copyProxyRequireList(const SipCommand& src);

        ///
        void flushProxyrequireList();

        ///
        const vector < SipProxyRequire* >& getProxyrequireList() const;

        ///
        void addsipProxyrequire(SipProxyRequire* sipProxyrequire);

        // ----------------- ResponseKey Header Methods ------------------

        /// get the responsekey
        const SipResponseKey& getResponseKey() const;

        /// Set the ResponseKey header
        void setResponseKey( const SipResponseKey& );

        // ----------------- Require Header Methods  ----------------
        /// get the number of requires
        int getNumRequire() const;

        /// Get the i'th Require item. If i is -1, it gets the last one
        const SipRequire& getRequire( int i = -1) const;

        /** set or add another Require itme, if the index is -1, it is appended
            to the current list */
        void setRequire(const SipRequire& item, int index = -1);

        /** set or add another Require itme, if the index is -1, it is appended
            to the current list */
        void setRequire(const Data&item, int index = -1);

        /** Set number of Require items.
            If i is less than current number then the extras are deleted. */
        void setNumRequire(int i);
        
        ///
        void copyRequireList(const SipCommand& src);
        
        ///
        void flushrequireList();
        
        ///
        const vector < SipRequire* >& getrequireList() const;
        
        ///
        void addsiprequire(SipRequire* siprequire);
        
        /// ----------------- Route Header Methods  -----------------
        int getNumRoute() const;

        /// Get the i'th Route item. If i is -1, it gets the last one 
        const SipRoute& getRoute( int i = -1) const;

        /** set or add another Route itme, if the index is -1, it is
          * appended to the current list  
          */
        void setRoute(const SipRoute& item, int index = -1);

        /** set or add another Route itme, if the index is -1, it is
          * appended to the current list  
          */
        void setRoute(const Data&item, int index = -1);

        /** Set number of Route items.If i is less than current number then
          * the extras are deleted.  
          */
        void setNumRoute(int i);

        /// highest in the command, next element to be sent to
        void routePushFront(const SipRoute& item);

        /// lowest in the command, last element to be sent to
        void routePushBack(const SipRoute& item);
        void routePopFront();
        void routePopBack();
        const SipRoute& routeFront() const;
        const SipRoute& routeBack() const;
        bool routeEmpty() const;
        
        ///
        void copyRouteList(const SipCommand& src);

        ///
        void flushrouteList();
    
        ///
        const vector < Sptr<SipRoute> > getRouteList() const;

        ///
        void addsiproute(SipRoute* siproute);

        ///
        void setRouteList(const vector < SipRoute* > sipRouteList);
        
        /// replacement for previous interface vector <SipRoute*> 
        void setRouteList(const vector < Sptr<SipRoute > > list);
        
        ///
        void removeRoute(int index = -1);

        /// ----------------- Subject Header Methods ------------------
        /// get the subject header
        const SipSubject& getSubject() const;
        
        /// Set the Subject header
        void setSubject( const SipSubject& );

        /// Set the Subject header
        void setSubject( const Data& textData);

        /// Basic auth details
        void setAuthBasic(const Data& data, const Data& pwd = "");
        
        ///
        bool checkAuthBasic(const Data& data, const Data& pwd = "");

        ///
        void setProxyAuth(const Data& data, const Data& pwd = "");
        
        ///
        bool checkProxyAuth(const Data& data, const Data& pwd = "");

        /// Digest Proxy-Authentication Details
        void setProxyAuthDigest(const Data& nonce, const Data& user,
                                const Data& pwd, const Data& method,
                                const Data& realm, const Data& requestURI,
                                const Data& qop, const Data& cnonce,
                                const Data& alg, const Data& noncecount,
                                const Data& opaque);

        /// Digest Authentication Details
        void setAuthDigest(const Data& nonce, const Data& user,
                           const Data& pwd, const Data& method,
                           const Data& realm, const Data& requestURI,
                           const Data& qop, const Data& cnonce,
                           const Data& alg, const Data& noncecount,
                           const Data& opaque);   

        /** verify the digest returned by the user is correct.  Here,
         *  pass the following items to be verified.  The other
         *  headers should not be needed (e.g. requestURI, algorithm)
         *  and can be gotten from the message
         *
         * */
        bool checkAuthDigest(const Data& nonce, 
                             const Data& user, 
                             const Data& pwd,
                             const Data& requestURI,
                             const Data& realm, 
                             const bool authReg = false);
        
        ///
        void setSipOspAuthorization(const SipOspAuthorization &);
    
        ///
        void setSipOspAuthorization( const Data& data);
         
        ///
        const SipOspAuthorization& getSipOspAuthorization() const;

        /// 
        virtual ~SipCommand();
    
        /** Form cryptographically unique branch, should be combined with
          * results of computeBranch to satisfy draft bis09. 
          */
        static Data computeUniqueBranch();

        /** return a branch to be used by a proxy for via (for loop detection)
           As per draft bis09, this computes a branch using computeUniqueBranch
           combined with a cryptographically random suffix
           "z9hG4bK" + computeUniqueBranch() + "." + cryptokey
        */
        Data computeProxyBranch() const;

        /** This version does not add a cryptographically random suffix but is
          * otherwise the same as computeProxyBranch
          */
        Data computeStatelessProxyBranch() const;
        
        /** detect loops in this command - only if branch was computed using
            computeProxyBranch
        */
        bool detectLoop() const;

        ///form MD5 hash for the branch here.
        Data computeBranch(Data hashBranch = "") const;
 
        
        /// alternate form which uses local data as well
        Data computeBranch2() const;
            
        /// Moved from public to private
        Data encodeProxyRequireList() const;
        
        ///
        Data encodeRequireList() const;
        
        ///
        Data encodeRouteList() const;
          
        bool compareProxyRequireList(const SipCommand& src) const;

        bool compareRouteList(const SipCommand& src) const;

        bool compareRequireList(const SipCommand& src) const;

        ///
        virtual Data encode() const;

        ///Check to see if message is a candidate for retransmission
        virtual bool toBeRetransmitted() const;

        /** return the next hop - based on the message's request-uri 
          * and/or route-set  see 16.6 section 6 and section 7. 
          * If a client wants to send a SipCommand with a req-uri of tel:xyz,
          * they must load a route into the ROUTE header to tell the stack where
          * to send the message. 
         */
        Sptr<SipUrl> postProcessRouteAndGetNextHop();
        int isLocaladdr(const char *addr);
    protected:
        SipRequestLine myRequestLine;

        bool isMethod(const Data& data);
        bool parseStartLine(const Data& line1);
        void sipdecode(const Data& fgdata);
       
};
    
}

#endif
