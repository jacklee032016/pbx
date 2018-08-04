/*
 * $Id: SipVector.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#ifndef SIPVECTOR_H
#define SIPVECTOR_H
#include <vector>

namespace Vocal
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

 
} // namespace Vocal

#endif
