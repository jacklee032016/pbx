#if !defined(VOCAL_STRING_HXX)
#define VOCAL_STRING_HXX
/*
 * $Id: vstring.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


#include <string>
#include "vtypes.h"


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Vocal 
{

using std::string;
using std::istream;

extern const size_t     MAX_LINE_LENGTH;
extern const string     SEPARATORS;
extern const string     COMMENT_CHARS;
extern const string     QUOTE_CHARS;
extern const char       NL;
extern const string     TAB1;
extern const string     TAB2;
extern const string     TAB3;


extern string &     upper(string & src);
extern string &     lower(string & src);


extern string &     remove_lws(string & src);
extern string &     remove_tws(string & src);
extern string &     remove_ws(string & src);


extern string &     remove_comments(
    string          &   src, 
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);

extern string &     remove_quotes(
    string          &   src,
    const string    &   quote_chars = Vocal::QUOTE_CHARS
);


extern bool     getline(
    istream         &   in, 
    string          &   line, 
    size_t          &   line_num,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);
extern string   getnext(
    string          &   line, 
    const string    &   separators = Vocal::SEPARATORS,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);


extern bool     parse_property(
    string          &   line, 
    const string    &   name, 
    string          &   new_prop,
    const string    &   separators = Vocal::SEPARATORS,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);

extern bool     parse_value(
    string          &   line, 
    const string    &   name,
    int             &   new_value,
    const string    &   separators = Vocal::SEPARATORS,
    const string    &   comment_chars = Vocal::COMMENT_CHARS
);

        
} // namespace Vocal


#endif // !defined(VOCAL_STRING_HXX)
