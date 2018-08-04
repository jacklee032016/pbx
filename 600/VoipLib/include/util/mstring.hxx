#ifndef __MSTRING_H__
#define __MSTRING_H__
/*
 * $Id: mstring.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include "global.h"
#include <string>
#include <vector>
#include <stdlib.h>

namespace std
{
class mstring;

///mstr_vector definition typedef vector<mstring> mstr_vector
typedef std::vector < mstring > mstr_vector;

/**mstring class provides basic parser methods as geting tokens,
 *splitting, brackets processing an so on. */
class mstring : public string
{
    public:

        /** Separator flag sep_single or sep_multiple.
         *
         *  sep_single - Single mode - each symbol is a single separator 
         *                            (1,2,,,3,4 will be "1"  "2"  ""  ""  "3"  "4")
         *  sep_multiple - Multiple mode - all symbols are one separator 
         *                            (1,,2,,3,,4 will be "1"  "2"  "3"  "4") 
         *  sep_whole_str - Whole string is a single separator
         *                            (1,,2,3,,4 will be "1"  "2,3"  "4") 
         */
        enum sep_flag
        {
            sep_single,
            sep_multiple,
            sep_whole_str
    };

        ///Default Constructor
        mstring() : string()
        {}

        ///Constructor from char*

        mstring(const char *str) : string(str)
        {}

        ///Copy Constructor

        mstring(const string &str) : string(str)
        {}

        ///Assignment from int

        mstring & assigni(int val, const char *format);

        ///Assignment from double
        mstring & assignd(double val, const char *format);


        ///Assignment from int
        mstring & operator = (int val)
        {
            return assigni(val, "%d");
        }

        //        ///Assignment from double
        //        mstring & operator = (double val) { return assignd(val, "%.1f"); }

        ///Conversion to int
        operator int() const
        {
            return atoi(c_str());
        }

        ///Conversion to double
        operator double() const
        {
            return atof(c_str());
        }

        ///Trim left all the characters in s
        mstring & ltrims(const string &s);

        ///Trim right all the characters in s
        mstring & rtrims(const string &s);

        ///Trim left and right all the characters in s
        mstring & atrims(const string &s)
        {
            ltrims(s);
            return rtrims(s);
        }

        ///Trim left the character c
        mstring & ltrimc(char c)
        {
            char s[2] = {c, 0};
            return ltrims(s);
        }

        ///Trim right the character c
        mstring & rtrimc(char c)
        {
            char s[2] = {c, 0};
            return rtrims(s);
        }

        ///Trim left and right the character c
        mstring & atrimc(char c)
        {
            char s[2] = {c, 0};
            return atrims(s);
        }

        ///Trim left whitespaces
        mstring & ltrim()
        {
            return ltrims("\t ");
        }

        ///Trim right whitespaces
        mstring & rtrim()
        {
            return rtrims("\t ");
        }

        ///Trim left and right whitespaces
        mstring & atrim()
        {
            return atrims("\t ");
        }

        ///Left align to 'newlen' with filler 'fill'
        mstring & toleft(unsigned newlen, char fill = ' ', bool cut = false);

        ///Right align to 'newlen' with filler 'fill'
        mstring & toright(unsigned newlen, char fill = ' ', bool cut = false);

        ///Find and replace all the 'find_str' to 'repl_str'
        unsigned replace_all(const string &find_str, const string &repl_str);

        /// Convert all tabs to spaces
        mstring & tab2sp(unsigned tab_size = 8);

        ///Compare two strings. The function is provided just for convinience and uniformity
        int compare(const string &str, size_type len = npos) const;

        ///Compare two strings ignoring case. The function is provided just for convinience and uniformity
        int comparei(const string &str, size_type len = npos) const;

        ///Compare two strings. The function is provided just for convinience and uniformity
        friend int compare(const mstring &str1, const mstring &str2, size_type len = npos)
        {
            return str1.compare(str2, len);
        }

        ///Compare two strings ignoring case. The function is provided just for convinience and uniformity
        friend int comparei(const mstring &str1, const mstring &str2, size_type len = npos)
        {
            return str1.comparei(str2, len);
        }

        ///Insert pair-symb into the string
        mstring & ins_pair(const string &chrset, char pair_symb);

        ///Delete pair-symb from the string
        mstring & del_pair(char pair_symb);

        ///Insert escaped symbols into the string
        mstring & ins_escaped(const string &chrset, char escaped_symb);

        ///Delete escaped symbols from the string
        mstring & del_escaped(char escaped_symb);

        ///Add quote symbols into the string
        mstring & quote(const string &q_start, const string &q_end);

        //see mstring.cxx
        size_type next_token(size_type start, const string &sep,
                             const string &quote, char pair_chr = 0,
                             sep_flag flag = sep_multiple) const;

        //see mstring.cxx
        size_type token(string *dst, size_type start, const string &sep,
                        const string &quote, char pair_chr = 0,
                        sep_flag flag = sep_multiple) const;

        //see mstring.cxx
        unsigned split(mstr_vector *vec, const string &sep, const string &quote,
                       char pair_chr = 0, sep_flag flag = sep_multiple) const;

        //see mstring.cxx
        size_type brtok(string *dst, size_type *start, int *balance,
                        char br_open, char br_close,
                        const string &quote, char pair_chr = 0) const;

        //see mstring.cxx
        bool chkquote(const string &quote, char pair_chr = 0) const;
};
}







#endif
