#ifndef SUPPORT_HXX_
#define SUPPORT_HXX_
/*
* $Id: support.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include "global.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <deque>
#include <string>
#include "substring.h"
#include "Data.hxx"

/***********************************************************************
 
  Utility and support functions for the stack.
 
 **********************************************************************/

#ifndef __sparc

#if !(defined(__FreeBSD__) || defined(__APPLE__))
typedef deque < substring > sub_split_t;
typedef deque < string > split_t;
#else
// Apple OSX
typedef vector < substring > sub_split_t;
typedef vector < string > split_t;
#endif

#else
typedef vector < substring > sub_split_t;
typedef vector < string > split_t;
#endif


/** paren_match will return the data that is in the first matched
    set of parentheses.
 
    e.g. if the text is like this:
 
    foo(la, la, la)
 
    it will return a substring pointing to "la, la, la" (no quotes,
    obviously)
 
    or, if it is
 
    (a, b(d, e(g, h), f), c)
 
    it will return "a, b(d, e(g, h), f), c" .
 
    paren_match will work both on strings and substrings.
 
*/
substring
paren_match(const string& inputText);


/// same as the one for strings, but for substrings
substring
paren_match(const substring& inputText);

/// the same as above, but it returns a string, not a substring
string
str_paren_match(const string& inputText);


/** splits a string into a vector of substrings while first matching
   parentheses.
 
   This function will NOT match the characters when they are inside
   brackets.  At the moment, this is specialized to only match "()" as
   brackets, although this may change in the future.
 
   sub_split_paren_patch does not copy the strings itself.  Instead,
   it substrings, which are pointers into the original string and
   length counts.
 
   The substring.h file contains the useful information about what a
   substring is, and what you can do with one.
 
   Clearly, this was influenced by perl.  
*/
sub_split_t
sub_split_paren_match(const string& inputText, const string& characters);


/** splits a string into a vector of strings while first matching
   parentheses.
 
   This function will NOT match the characters when they are inside
   brackets.  At the moment, this is specialized to only match "()" as
   brackets, although this may change in the future.
 
   split_paren_patch copies the strings itself.  
 
   Clearly, this was influenced by perl.  */
split_t
split_paren_match( const string& inputText, const string& characters);


/**
   splits a string into a vector of substrings. 
 
   This version is more efficient than split() because it does not
   copy the strings itself.  Instead, it uses substrings, which are
   pointers into the original string and length counts.
 
   The substring.h file contains the useful information about what a
   substring is, and what you can do with one.
 
   Clearly, this was influenced by perl.  */
sub_split_t
sub_split(const string& inputText, const string& characters);


/**
   splits a string into a vector of substrings. 
 
   This is inefficient because copying is generally involved here.  A
   more efficient implementation which uses pointers into the original
   string (and length counts) may replace this in a future version.
 
   Clearly, this was influenced by perl.
*/
split_t
split(const string& inputText, const string& characters);


/// slower, new split code
vector < string >
new_split(const string& inputText, const string& characters);


/**
   removes the trailing character from a string.
 
   Clearly, this was influenced by perl.
*/
void
chop(string* input);


/**
    removes the trailing character from a string if it is \r or \n.
 
    Clearly, this was influenced by perl.
*/
void
chomp(string* input);


/**
   convert an int to a string
*/
inline string itos(unsigned int i)
{
    char buf[32];
    sprintf(buf, "%d", i);
    return buf;
}


/// convert an int to a base 16 (hex) string
inline string itohexs(unsigned int i)
{
    char buf[32];
    sprintf(buf, "%x", i);
    return buf;
}


/**
   returns a string converted to lowercase.
*/
string
str2lower(string str);


/**
   returns a string converted to lowercase.
*/
string
c2lower_s(const char* cstr);


/**
   check to see if a string is all legal digit characters
*/

int
matchString(string a, string regex);


/**
    get the integer from the string s, e.g. "   2598   ", it may contain
    arbituary spaces in the front or end, but not in between digits
    otherwise, the first whole integer is read and returned,
    e.g. "  235   67" is returned as 235.
    if true is returned, the num variable contains a valid integer for use
*/
bool stringToInt ( const string& s, int *num);


/**
    Assume the string s contains an integer or a number
    range between two integers, e.g. 20 or 23-45.
    Returns: If string s contains only a single integer, it is
    returned in start variable and isRange is set to false, and the function
    in whole returns true.
    If string s contains a range, start variable will contain
    the starting number, and end variable will contain the ending
    number, and isRange is set to true if the range is valid, i.e. start < end.
    the function in whole returns true.
    Any other case, the function returns false.
*/
bool getRange( const string& s, int* start, int* end, bool* isRange);


/// convert data to an unsigned char array
int convertToUnsigned(const Data& data, unsigned char* dest);


/// convert Hex-encoded ASCII in src to a Data containing the appropriate data
Data convertToHex(const unsigned char* src, int len);

#endif
