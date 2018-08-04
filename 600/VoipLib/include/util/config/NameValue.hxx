#if !defined(VOCAL_NAME_VALUE_HXX)
#define VOCAL_NAME_VALUE_HXX
/*
 * $Id: NameValue.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include <string>
#include <list>
#include <map>


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{


/** 
 */
namespace Configuration
{


using std::string;
using std::list;
using std::map;

/**
 */
enum        ValueType
{
    NO_VALUE   	    = 0,
    VALUE_PRESENT   = 1,
    VALUE_OPTIONAL  = 2
};


/**
 */
struct Value
{
    Value() : type(NO_VALUE), count(0) {}

    ValueType       type;
    list<string>    value;
    size_t          count;
};


/**
 */
typedef map<string, Value>    NameValueMap;
        

} // namespace Configuration
} // namespace Vocal


#endif // !defined(VOCAL_NAME_VALUE_HXX)
