#ifndef __CALL_ID_HXX__
#define __CALL_ID_HXX__
/*
 * $Id: CallId.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "VException.hxx"
#include "AudioChannel.hxx"

#include "Data2.hxx"

namespace Assist
{

typedef enum
{
	dtmf_signal_out_band = 0,	/* default is out band signal */
	dtmf_signal_in_band	
}dtmf_signal_type_t;


enum CallIdErrorType
{
	DECODE_VPBXCALLID_FAILED
    //may need to change this to be more specific
};

/// Exception handling class 
class CallIdParserException : public VException
{
	public:
		CallIdParserException( const string& msg,
                                  const string& file, const int line,const int error = 0 );

		CallIdParserException( const string& msg,
                                  const string& file, const int line, CallIdErrorType i)
			: VException( msg, file, line, static_cast < int > (i))
		{
			value = i;
		}

		const CallIdErrorType& getError() const
		{
			return value;
		}

		string getName() const ;
		
	private:
		CallIdErrorType value;

};


/// data container for CallId header
class CallId
{
	public:
		CallId();
		~CallId();

		CallId( const CallId& ohter );
		CallId( const Data& data );

		Data getLocalId() const
		{
			return localid;
		}
		
		void setLocalId(const Data & newlocalid)
		{
			localid = newlocalid;
		}

		bool operator == ( const CallId& other ) const;
		bool operator != (const CallId& other) const
		{
			return !(*this == other);
		}

		const CallId& operator = ( const CallId& src );
		bool operator < ( const CallId& other ) const;
//		bool operator >( const CallId& other ) const;

		/*** Create by decoding the data string passed in. This is the decode
		or parse. This call should only be used inside the stack and is not part of the API */

		/*** return the encoded string version of this. This call should only be
		used inside the stack and is not part of the API */
		Data encode() const;
		Data getCallIdData() const;

		size_t hashfn()const;

		void setData(const Data& data);
		Data getData() const;
		/// method for copying sip headers of any type without knowing which type
//		IsdnIE* duplicate() const;
		/// compare two headers of (possibly) the same class
//		virtual bool compareIsdnIE(IsdnIE* msg) const;

	private:
		Data localid;
		size_t hash;
		void decode(const Data& data);
		void getHash();
		void parse(const Data& data);
		void parseLocalId(const Data& data);

//        friend class IsdnMsg;
//        Data host;

		Sptr <AudioChannel> 			audioChannel;

		int							dinfo;			/* primary key for CallId */	
};

 //	ostream& operator<<(ostream& strm, const CallId& id);
	
}

inline bool operator<( const Sptr<Assist::CallId>& left, const Sptr<Assist::CallId>&  right)
{
	return (*left < *right);
}

#endif

