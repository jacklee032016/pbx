#ifndef SDPSESSION_HXX
#define SDPSESSION_HXX
/*
* $Log: SdpSession.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:05:22  lizhijie
* no message
*
* $Id: SdpSession.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <string>
#include "Data.hxx"
#include <vector>
#include <sys/types.h>
#include "support.hxx"
#include "SdpMedia.hxx"
#include "SdpConnection.hxx"
#include "SdpTime.hxx"
#include "SdpAttributes.hxx"
#include "SdpBandwidth.hxx"
#include "SdpEncryptkey.hxx"
#include "SdpZoneAdjustment.hxx"

#include "SdpExceptions.hxx"


enum SdpProtocolType
{
    SdpProtocolTypeSDP = 0,     // Conforms to RFC 2327
    SdpProtocolTypeNCS         // Conforms to additional NCS requirements
};


/** Implement Session Description Protocol (RFC 2327)
 *
 */
class SdpSession
{
    public:
        ///
        SdpSession ();
        ///
        SdpSession (const SdpSession& x);

        ///
        ~SdpSession();

        ///
        bool isValidDescriptor()
        {
            return isValid;
        }

        ///
        void setProtocolType (SdpProtocolType protocol);
        ///
        SdpProtocolType getProtocolType ();
        /// Verify if this a well formed SDP according to protocol type
        bool verify (SdpProtocolType protocol = SdpProtocolTypeSDP);

        ///
        // Do we need this? Comment out to see what happens
        //    void setValidDescriptor(bool valid) { isValid = valid; }

        // No setProtocolVersion() is needed for now. It is always 0.
        ///
        int getProtocolVersion()
        {
            return protocolVersion;
        }

        ///
        void setUserName(char* name)
        {
            username = name;
            isValid = true;
        }
        ///
        void setUserName(const string& name)
        {
            username = name;
            isValid = true;
        }
        ///
        string getUserName()
        {
            return username;
        }

        ///
        void setSessionId(unsigned int session)
        {
            sessionId = session;
            isValid = true;
        }
        ///
        unsigned int getSessionId()
        {
            return sessionId;
        }

        ///
        void setVersion(unsigned int vers)
        {
            version = vers;
            isValid = true;
        }
        ///
        unsigned int getVersion()
        {
            return version;
        }

        // void setNetworkType (NetworkType ntwkype);
        ///
        NetworkType getNetworkType()
        {
            return networkType;
        }

        // void setAddressType(AddressType addrType);
        ///
        AddressType getAddressType()
        {
            return addressType;
        }

        ///
        void setSessionName(char* name)
        {
            sessionName = name;
        }
        ///
        void setSessionName(const string& name)
        {
            sessionName = name;
        }
        ///
        string getSessionName()
        {
            return sessionName;
        }

        ///
        void setAddress(char* addr)
        {
            address = string (addr);
        }
        ///
        void setAddress(const string& addr)
        {
            address = addr;
        }
        ///
        string getAddress()
        {
            return address;
        }

        ///
        void setSessionInfo(char* info)
        {
            sessionInfo = info;
        }
        ///
        void setSessionInfo(const string& info)
        {
            sessionInfo = info;
        }
        ///
        string getSessionInfo()
        {
            return sessionInfo;
        }

        ///
        void setURIInfo(char* uri)
        {
            uriInfo = uri;
        }
        ///
        void setURIInfo(const string& uri)
        {
            uriInfo = uri;
        }
        ///
        string getURIInfo()
        {
            return uriInfo;
        }

        ///
        void setEmailAddr(const char* emailAddr);
        ///
        void setEmailAddr(const string& emailAddr);
        ///
        vector < string > getEmailList()
        {
            return emailList;
        }

        ///
        void setPhoneNum(const char* phoneNum);
        ///
        void setPhoneNum(const string& phoneNum);
        ///
        vector < string > getPhoneList()
        {
            return phoneList;
        }


        ///
        void setConnection (const SdpConnection& conn);
        ///
        SdpConnection* getConnection();

        ///
        void setBandwidth (SdpBandwidth& bw);
        ///
        SdpBandwidth* getBandwidth ()
        {
            return bandwidth;
        }

        ///
        vector < SdpTime > getSdpTimeList() const
        {
            return sdpTimeList;
        }

	void clearSdpTimeList();

        ///
        void addTime (const SdpTime& time);

        vector < SdpZoneAdjustment > getZoneAdjustmentList() const
        {
            return zoneAdjustmentList;
        }
        ///
        void addZoneAdjustment (SdpZoneAdjustment& adjustment)
        {
            zoneAdjustmentList.push_back(adjustment);
        }

	    ///
        void setEncryptkey (const SdpEncryptkey& conn);
        ///
        SdpEncryptkey* getEncryptkey();

        ///
        SdpAttributes* getAttribute() const
        {
            return attribute;
        }
        ///
        void setAttribute (SdpAttributes* attrib)
        {
            attribute = attrib;
        }

        ///
        void addMedia (SdpMedia* media)
        {
            mediaList.push_back(media);
        }
        ///
        vector < SdpMedia* > getMediaList() const
        {
            return mediaList;
        }

        /// Encode object into text string
        string encode();

        /// Reset all member variables so the object can be re-used
        void reset();

        /// Decode text string into object
        bool decode(const string& buffer);
        ///
        bool decode(split_t& parms);

        /// assignment operator
        SdpSession& operator= (const SdpSession& x);

#if 0
        /// equality operator -- not implemented
        bool operator == (const SdpSession& x) const;
#endif

        /// Clear all data in media list
        void flushMediaList();

        /// Implement INVITE(HOLD)
        void setHold(); 	// set "c="'s <connection address> to "0.0.0.0"
        bool isHold();

    private:
        ///
        void encodeVersion (ostrstream& s);
        ///
        string networkTypeString();
        ///
        string addressTypeString();
        ///
        void encodeOrigin (ostrstream& s);
        ///
        void encodeSessionName (ostrstream& s);
        ///
        void encodeSessionInformation (ostrstream& s);
        ///
        void encodeURI (ostrstream& s);
        ///
        void encodeEmailAddress (ostrstream& s);
        ///
        void encodePhoneNumber (ostrstream& s);
        ///
        void encodeTime (ostrstream& s);
        ///
        void encodeTimeZoneAdjustment (ostrstream& s);
        ///
        void encodeMedia (ostrstream& s);

        /// Set this SDP according to the PacketCable NCS requirements
        void setNcs ();
        /// Check if this SDP has all the mandatory fields
        bool conformToSdp ();
        /// Check if this SDP conforms to the PacketCable NCS spec
        bool conformToNcs ();


    private:

        /// ostrstream buffer during encoding
        char buf[4096];
        /// set to true if this session descriptor is meant to be valid
        bool isValid;
        /** SDP Protocol Type indicates this SDP descriptor must conform to a
         *  specific protocol.
         *  Default is SDP which means no additional requirements.
         *  For example, NCS means this SDP conforms to Packet Cable NCS protocol
         */
        SdpProtocolType protocolType;


        /// Session Description data

        /// Protocol Version in "v=" line. Default is 0
        int protocolVersion;

        /** Origin
         *  o=<username> <session id> <version> <network type> <address type> <address>
         */
        /// <username> in "o=" line. Default is "-"
        string username;
        /// <session id> in "o=" line. TODO: Use NTP time
        unsigned int sessionId;
        /// <version> in "o=" line. TODO: Use NTP time
        unsigned int version;
        /// <network type> in "o=" line. Default is IN
        NetworkType networkType;
        /// <address type> in "o=" line. Default is IP4
        AddressType addressType;
        /// <address> in "o=" line
        string address;  // contains the address in the o field

        /// Session Name
        /// <session name> in "s=" line. Default is "-"
        string sessionName;

        /// Session and Media Information - optional
        string sessionInfo;

        //URI for more information "u=*" - optional
        string uriInfo;

        //"e=*" email address list - optional
        vector < string > emailList;

        //"p=*" phone list - optional
        vector < string > phoneList;

        /// Connection Data
        /// Session level connection information("c=") - not required if include in all media
        SdpConnection* connection;

        /// Bandwidth
        /// Session level bandwidth information("b=") - optional
        SdpBandwidth* bandwidth;

        /// Times, Repeat Times and Time Zones
        vector < SdpTime > sdpTimeList;
        //"t="
        /// Repeat times("r=") - optional, done in SdpTime
        ///"z=*" - optional
        vector < SdpZoneAdjustment > zoneAdjustmentList;

        /// Encryption Key
        /// Session level encryption ket("k=") lines - optional
        SdpEncryptkey* encryptkey;

        /// Attributes
        /// Session level attribute("a=") lines - optional
        SdpAttributes* attribute;

        /// Media Announcements
        /// List of Media descriptions
        vector < SdpMedia* > mediaList;
};

typedef SdpSession SessionDescriptor;

#endif
