#ifndef RETRANSMIT_CONTENTS_HXX
#define RETRANSMIT_CONTENTS_HXX
/*
 * $Id: RetransmitContents.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Sptr.hxx"
#include "symbols.hxx"
#include "SipMsg.hxx"

#include "SipTransactionLevels.hxx"

namespace Vocal
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
 
} // namespace Vocal

#endif
