#ifndef __MSG_SETUP_HXX__
#define __MSG_SETUP_HXX__
/*
* $Id: MsgSetup.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IsdnMsg.hxx"

namespace Assist
{

/* CC_SETUP INDICATION */
class MsgSetup : public IsdnMsg
{
	public:
		MsgSetup(const msg_t *_msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) ;
		MsgSetup(IsdnCtrlDevice *ctrl);
		MsgSetup(const MsgSetup&);
		~MsgSetup();

		MsgSetup& operator =(const MsgSetup&);
		bool operator ==(const MsgSetup&);

		ISDN_METHOD getType() const;
		int getMsgType() const ;

		Sptr <IeBearer> getBearer() const;
		void setBearer(Sptr <IeBearer> bear);

		Sptr <IeCallingPN> getCallingPN() const;
 		void setCallingPN(Sptr <IeCallingPN> callingPN);
		Sptr <IeCallSub> getCallingSub() const;
		void setCallingSub(Sptr <IeCallSub> _callingSub);
		
		Sptr <IeCalledPN> getCalledPN() const;
		void setCalledPN(Sptr <IeCalledPN> calledPN);
		Sptr <IeCallSub> getCalledSub() const;
		void setCalledSub(Sptr <IeCallSub> _calledSub);
		
		Sptr <IeChanId> getChanId() const ;
		void setChanId(Sptr <IeChanId> chanId);
		
		Sptr <IeProgress> getProgress() const;
		void setProgress(Sptr <IeProgress> _pregress);
		
		Sptr <IeKeypad> getKeypad() const;
		void setKeypad(Sptr <IeKeypad> _keypad);
		
		Sptr <IeDisplay> getDisplay() const;
		void setDisplay(Sptr <IeDisplay> _display);
		
		Sptr <IeSignal> getSignal() const;
		void setSignal(Sptr <IeSignal> _signal);
		
#ifdef CENTREX
		Sptr <IeCentrex>		getCentrex() const;
		void setCentrex(Sptr <IeCentrex> centrex);
#endif


#if 0
		Sptr <IeComplete> getComplete() const;
		void setComplete(Sptr <IeComplete> complete);
#endif

		Sptr <IeRedirNR> getRedirNR() const;
		void setRedirNR(Sptr <IeRedirNR> redirNr);

#if WITH_DEBUG
		void debugInfo();
#endif

		int getCCType() const ;
		
		virtual const char *getName();

	protected:
	
		virtual void encode( msg_t *msg) ;
		int decode(const unsigned char *iesData)  throw(IsdnMsgParserException &) ;
//		int decode(const iframe_t *_frm)  throw(IsdnMsgParserException &) ;

	private:

#if 0
		/* not offered in INS*/
		Sptr <IeComplete>	ieComplete;
#endif
#if 0
		/* optional IE */
		Sptr <IeRepeat>		ieRepeat;
#endif		

		/* unique mandatory IE , length :4-12*/
		Sptr <IeBearer> 		ieBearer;

		/* optional IE, length :2-**/
		Sptr <IeChanId>		ieChanId;

		/* optional IE, 2-4 */
		Sptr <IeProgress>		ieProgress;

		/* IE is optional and n-->u */
		Sptr <IeDisplay> 		ieDisplay;
		/* IE is optional and u-->u */
		Sptr <IeKeypad>		ieKeypad;
		/* IE is optional and u-->u */
		Sptr <IeSignal>		ieSignal;

		/* length :2-36*/
		Sptr <IeCallingPN> 	ieCallingPN;
		/* length 2-23 */
		Sptr <IeCallSub> 		ieCallingSub;

		/* length : 2-35 */
		Sptr <IeCalledPN>		ieCalledPN;
		/* length : 2-23 */
		Sptr <IeCallSub>		ieCalledSub;
		
		Sptr <IeRedirNR>		ieRedirNR;

		Sptr <IeFacility>		ieFacility;
		
#ifdef CENTREX
		Sptr <IeCentrex>		ieCentrex;
#endif
		
};
 
}

#endif

