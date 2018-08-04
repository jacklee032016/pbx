#ifndef VFILTER_HXX_
#define VFILTER_HXX_
/*
 * $Id: VFilter.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "global.h"
#include "VNamedPipeFC.hxx"
#include <string>

// simple filtering class.


// To enable VFilter, you must meet all of these conditions:

// 1.  run unix.  no vxworks support

// 2.  define the environment variable SIP_FILTER !  This is
// important!  use

// setenv SIP_FILTER 1

// in tcsh, or

// SIP_FILTER=1 ; export SIP_FILTER

// in bash to enable.

// VFilter prints out information as it starts running about this.

// 3.  run the appropriate infilter.pl and outfilter.pl code to filter
// the incoming and outgoing sides of the connections.  this code is in
// the vfilter directory under tools .

// There is a readme with more information in tools/vfilter .

#ifdef WIN32
using std::string;
#endif

class VFilter
{
    public:
        static string inFilter(const string& input);
        static string outFilter(const string& input);

    private:
        VFilter();
#ifndef __vxworks
        VNamedPipeFC inPipe;
        VNamedPipeFC outPipe;
        bool useFilter;
        static VFilter* instance_;
#endif
};


#endif
