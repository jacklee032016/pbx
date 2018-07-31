#ifndef HOSTMATCH_HXX_
#define HOSTMATCH_HXX_

/*
* $Id: HostMatch.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <string>

/** compare two hosts and determine if they are equal (e.g. their IP
 * addresses, and each name, are equal) 
 */
bool hostsEqual(const std::string& first, const std::string& second);

/** determines if the two sets are equal, less than, or greater than.
 * returns -1 if first < second, 0 if first == second, and 1 if first
 * > second 
 */
int hostsCompare(const std::string& first, const std::string& second);

/** compare two hosts and determine if they intersect (e.g. if they
 * share any names)  
 */
bool hostsIntersect(const std::string& first, const std::string& second);

#endif
