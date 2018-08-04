#if !defined(ASSIST_NAME_VALUE_HXX)
#define ASSIST_NAME_VALUE_HXX

/*
* $Id: NameValue.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include <string>
#include <list>
#include <map>


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Assist 
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
} // namespace Assist


#endif // !defined(ASSIST_NAME_VALUE_HXX)
