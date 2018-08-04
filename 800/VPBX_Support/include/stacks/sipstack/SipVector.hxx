#ifndef SIPVECTOR_H
#define SIPVECTOR_H
/*
* $Log: SipVector.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipVector.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <vector>

namespace Assist
{
    
template < class T >
class SipVector: public std::vector < T >
{
    public:
        SipVector() : std::vector < T > ()
        {}
        ;
        SipVector(int size) : std::vector < T > ( size )
        {}
        ;
        ~SipVector()
        {}
        ;


        const T getContainer()
        {
            return container;
        };

    private:
        std::vector < T > container;
};

 
} // namespace Assist

#endif
