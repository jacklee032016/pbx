#ifndef DTMFInterface_HXX
#define DTMFInterface_HXX
/*
* $Log: DTMFInterface.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:04:36  lizhijie
* no message
*
* $Id: DTMFInterface.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

class DTMFInterface
{
    public:
        /**
	   Callback that is called when a particular DTMF event has
	   arrived.
	   @param event the DTMFEvent event
	   @param duration the length of the DTMF event
	   @see DTMFEvent
	 */
        virtual void sendDTMF( int event, int duration ) = 0;
};


/**
   @deprecated Unused.
*/
template < class T >
class RtpDevice : public DTMFInterface
{
    private:
        T* _device;

    public:
        ///
        RtpDevice( T* t )
        {
            _device = t;
        }
        void sendDTMF( int a, int b );
};


template < class T >
void RtpDevice < T > ::sendDTMF( int a, int b )
//void RtpDevice<class T>::sendDTMF( int a, int b )
{
    _device->recvRTPDTMF( a, b );
}


#endif
