#ifndef SipVia_HXX
#define SipVia_HXX
/*
* $Id: IeShift.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx" 
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "IsdnIE.hxx"

namespace Assist
{
    
enum SipViaErrorType
{
   DECODE_VIA_FAILED,
   SCAN_VIA_FAILED,
   MANDATORY_VIA_ITEM_MISSING,
   COMMENT_ERR,
   PARSE_VIAPRAM_ERR
  
};

/// Exception handling class SipViaParserException
class SipViaParserException :public VException
{
   public:
      ///
      SipViaParserException( const string& msg,
			     const string& file,
			     const int line,
			     const int error = 0 ); 
      ///
      SipViaParserException( const string& msg, 
			     const string& file,
			     const int line,
			     SipViaErrorType i)
	 : VException( msg, file, line, static_cast<int>(i))
      {
	 value = i;
      }
      ///
      const SipViaErrorType& getError() const{return value;}
      ///
      string getName() const ;
   private:
      SipViaErrorType value;

}; 


/// data container for Via header 
class IsdnIeShift : public IsdnIE
{
    public:
        /// Create one with default values
        IsdnIeShift();
        ///
        IsdnIeShift( const Data& data ) throw(SipViaParserException &);
        ///
        IsdnIeShift( const IsdnIeShift& src );
        /// 
        const IsdnIeShift& operator =( const IsdnIeShift& src );
        ///
        bool operator<( const IsdnIeShift& src ) const;
        ///
        bool operator>( const IsdnIeShift& src ) const;
        ///
        bool operator ==( const IsdnIeShift& src ) const;
        ///
        Data encode() const;
        ///
        void setprotoName(const Data& protoName);
        ///
        Data getprotoName() const;
        ///
        void setprotoVersion(const Data& protoVer);
        ///
        Data getprotoVersion() const;
        ///
        void setTransport(const Data& trans);
        ///
        Data getTransport() const;
        ///
        void setHost(const Data& data);
        ///
        const Data& getHost() const;
        ///
        void setPort(int data);
        ///
        void setPortData(const Data& data);
        ///
        Data getPortData() const;
        ///
        int getPort() const;
        ///
        Data getExtensionValue() const ;
        ///
        Data getExtensionParm() const;
        ///
        void setExtensionParm(const Data & newextparm);
        ///
        void setExtensionValue(const Data& newextvalue);

        ///
        void sethidden();
        ///
        void setTtl(const Data& newttl);
        ///
        Data getTtl() const ;
        ///
        void setMaddr(const Data& newmaddr);
        ///
        Data getMaddr() const;
        ///
        Data getReceivedhost() const;
        ///
        void setReceivedhost(const Data& recvdhost);
        ///
        Data getReceivedport() const;
        ///
        void setReceivedport(const Data& recvdport);
        ///
        void setBranch(const Data& newtoken);
        ///
        Data getBranch() const;
        ///
        void setHidden(const Data& hidden);
        ///
        bool getHidden() const;
        ///
        void setComment(const Data& newcomment);
        ///
        Data getComment() const;
        /// set the rport
        void setRPort(const Data& port);
        /// set the rport
        void setRPort(const bool flag);
        /// get the rport
        Data getRPort() const;
        /// true if the via is received
        bool isViaReceived() const;      
	/// method for copying sip headers of any type without knowing which type
	IsdnIE* duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareIsdnIE(IsdnIE* msg) const;
    private:
        //Data sipVia; //this is returned in encode.  
    
        Data host;
        Data port;
        Data protocolName;
        Data protocolVersion;
        Data transport;

        //Via specific details.
        Data receivedHost;
        Data receivedPort;

        Data branchToken;
        Data comment;
        Data ttl;
        Data extparm;
        Data extvalue;
        Data maddr;
        bool viaBranch;
        bool viaComment;
        bool viaReceived;
        bool viaHidden;
        bool viaTtl;
        bool viaMaddr;
        bool rport;
        Data rportValue;
        ///
        void parseRestVia(const Data& restdata);
        ///
        void parseViaParms( const Data& data);
        ///
        void parseViaParm( const Data& data, const Data& value);
        ///
        void parseViaParmHidden( const Data& data, const Data& value);
        ///
        void parseBranch(const Data& branchdata);
        ///
        void parseRecevied(const Data& recevieddata);
        ///
        void parseExtension(const Data& extensiondata, const Data& extensionvalue);
        ///
        void parseHidden(const Data& hiddendata);
        ///
        void scanSipVia(const Data &tmpdata);
        ///
        void parseComment(const Data& cdata);
        ///
        void parseTransport(const Data& tdata);
        ///
        void parseHost(const Data& data);
        void decode(const Data& data);
        Data formOtherparms() const; 
};

 
} // namespace Assist

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
