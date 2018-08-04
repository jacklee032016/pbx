#ifndef RETRANSMIT_CONTENTS_HXX
#define RETRANSMIT_CONTENTS_HXX

/*
* $Log: RetransmitContents.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: RetransmitContents.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Sptr.hxx"
#include "symbols.hxx"
#include "SipMsg.hxx"

#include "SipTransactionLevels.hxx"

namespace Assist
{

class RetransmitContents
{
    public:
  ///
  RetransmitContents();

  ///TimetoGo is 0 by default
  RetransmitContents(SipMsgContainer* msg, int count);
  ///    
  ~RetransmitContents();
  ///     
  RetransmitContents(const RetransmitContents& src );
  ///
  RetransmitContents& operator =(const RetransmitContents& src);
  ///
  void setCount(const int i);
  ///
  int getCount() const;
  ///
  void setMsg(SipMsgContainer* );
  ///
  SipMsgContainer* getMsg() const;
  ///
  int getTimeToGo() const;
  ///
  void setTimeToGo(const int newtimeToGo);
  
private:
  
  SipMsgContainer* sipMsg;
  int retransmitt;
  int timeToGo;
};
 
}

#endif
