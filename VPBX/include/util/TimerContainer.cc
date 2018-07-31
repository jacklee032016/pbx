/*
* $Id: TimerContainer.cc,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <algorithm>
using namespace std;

template < class Msg >
TimerContainer < Msg > ::TimerContainer()
{
}



template < class Msg >
TimerContainer < Msg > ::~TimerContainer()
{
}



template < class Msg >
TimerEntryId
TimerContainer < Msg > ::add(
    Sptr < Msg > msg,
    milliseconds_t when,
    bool relative
)
{
    TimerEntry < Msg > newTimerEntry(msg, when, relative);

    postAdd(newTimerEntry);
    
    return ( newTimerEntry.getId() );
}


template < class Msg >
TimerEntryId
TimerContainer < Msg > ::add(
    Sptr < Msg > msg,
    const TimeVal & when,
    bool relative
)
{
    TimerEntry < Msg > newTimerEntry(msg, when, relative);
    postAdd(newTimerEntry);
    return ( newTimerEntry.getId() );
}


template < class Msg >
bool
TimerContainer < Msg > ::cancel(TimerEntryId msg_id)
{
   //Lock lock(myMutex);
   typename map<TimerEntryId, TimerEntry<Msg> >::iterator j = _idToTimer.find(msg_id);
   if (j == _idToTimer.end())
   {
      return false;
   }
   
   pair<typename TimerSet::iterator, typename TimerSet::iterator> 
      range = equal_range(_timerSet.begin(),
                          _timerSet.end(),
                          j->second);
   _idToTimer.erase(j);
   for (typename TimerSet::iterator i = range.first; i != range.second; i++)
   {
      if (i->getId() == msg_id)
      {
         _timerSet.erase(i);
         return true;
      }
   }
   assert(0);
}      

template < class Msg >
int
TimerContainer < Msg > ::getTimeout()
{
   //Lock lock(myMutex);
   return (_timerSet.size() > 0
           ? _timerSet.begin()->getTimeout()
           : -1);
}


template < class Msg >
TimerEntryId
TimerContainer < Msg > ::getFirstTimerEntryId()
{
   //Lock lock(myMutex);
   return (_timerSet.size() > 0 
           ? _timerSet.begin()->getId() 
           : 0);
}


template < class Msg >
bool
TimerContainer < Msg > ::messageAvailable()
{
   bool msgAvailable = (_timerSet.size() > 0 && _timerSet.begin()->hasExpired());
   return msgAvailable;
}


template < class Msg >
Sptr < Msg >
TimerContainer < Msg > ::getMessage()
throw ( VMissingDataException )
{
    if ( !messageAvailable() )
    {
        throw VMissingDataException("TimerContainer::getMessage: no message available.",
                                    __FILE__, __LINE__);
    }
    typename TimerSet::iterator it = _timerSet.begin();
    _idToTimer.erase(it->getId());

    Sptr<Msg> msg = it->getMessage();
    _timerSet.erase(it);

    return msg;
}

template < class Msg >
ostream &
TimerContainer < Msg > ::writeTo(ostream & out) const
{
   out << "timer container (size: " << size() << ") = { ";
   bool first = true;

   for ( typename TimerSet::const_iterator it = _timerSet.begin(); it != _timerSet.end(); it++ )
   {
      if ( first )
      {
         out << " (";
         first = false;
      }
      else
      {
         out << ", (";
      }
      
      it->writeTo(out);
      out << " )";
   }
   return ( out << " }" );
}


template < class Msg >
unsigned int
TimerContainer < Msg > ::size() const
{
   //Lock lock(myMutex);
   return _timerSet.size();
}

template < class Msg >
void    
TimerContainer < Msg > ::postAdd(const TimerEntry<Msg> & newTimerEntry)
{
    //Lock lock(myMutex);
   _timerSet.insert(newTimerEntry);
   _idToTimer[newTimerEntry.getId()] = newTimerEntry;
}
