/*
 * $Id: TimerEntry.cc,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

template < class Msg >
TimerEntry < Msg > ::TimerEntry(
    Sptr < Msg >        msg, 
    milliseconds_t      time,
    bool                relative
)
    : msg_(msg)
{
    if ( relative )
    {
        absExpires_.now();
    }

    if ( time > 0 )
    {
        absExpires_ += time;
    }
}


template < class Msg >
TimerEntry < Msg > ::TimerEntry(
    Sptr < Msg >        msg, 
    const TimeVal &     time,
    bool                relative
)
    :   absExpires_(time),
        msg_(msg)
        
{
    if ( relative )
    {
        absExpires_ += TimeVal().now();
    }
}


template < class Msg >
TimerEntry < Msg > ::TimerEntry(const TimerEntry & src)
        : absExpires_(src.absExpires_),
        msg_(src.msg_)
{}

template < class Msg >
TimerEntry < Msg > ::TimerEntry()
   : msg_(0)
{}

template < class Msg >
TimerEntry < Msg > ::~TimerEntry()
{}



template < class Msg >
TimerEntry < Msg > &
TimerEntry < Msg > ::operator=(const TimerEntry & src)
{
    if ( this != &src )
    {
        absExpires_ = src.absExpires_;
        msg_ = src.msg_;
    }
    return ( *this );
}


template < class Msg >
milliseconds_t
TimerEntry < Msg > ::getTimeout() const
{
    TimeVal timeDifference = absExpires_ - TimeVal().now();

    milliseconds_t timeout = timeDifference.milliseconds();

    return ( timeout < 0 ? 0 : timeout );
}


template < class Msg >
TimerEntryId
TimerEntry < Msg > ::getId() const
{
    return ( (TimerEntryId)(msg_.operator->()) );
}


template < class Msg >
Sptr < Msg >
TimerEntry < Msg > ::getMessage() const
{
    return ( msg_ );
}


template < class Msg >
bool
TimerEntry < Msg > ::hasExpired() const
{
    return ( absExpires_ < TimeVal().now() );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator==(const TimerEntry & src) const
{
    return ( absExpires_ == src.absExpires_ );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator!=(const TimerEntry & src) const
{
    return ( !operator==(src) );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator<(const TimerEntry & src) const
{
    return ( absExpires_ < src.absExpires_ );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator<=(const TimerEntry & src) const
{
    return ( operator<(src) || operator==(src) );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator>(const TimerEntry & src) const
{
    return ( !operator<=(src) );
}


template < class Msg >
bool
TimerEntry < Msg > ::operator>=(const TimerEntry & src) const
{
    return ( !operator<(src) );
}


template < class Msg >
ostream &
TimerEntry < Msg > ::writeTo(ostream & out) const
{
    return ( out << "id = " << getId()
             << ", absolute time of expiration = "
             << "{ sec: " << absExpires_.tv_sec
             << ", usec: " << absExpires_.tv_usec << " }"
             << ", relative time of expiration = " << getTimeout()
           );
}
