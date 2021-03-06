/*
* $Id: support.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "support.hxx"
#include <sys/types.h>

#ifndef WIN32
#include <regex.h>
#else
#define __STDC__ 1
extern "C" {
#include <regex.h>
}
#include <iostream>
#endif

#include "debug.h"
#include <set>
#include "cpLog.h"


/**********************************************************************
 
Support functions for the MGCP library.
 
**********************************************************************/


bool isIn(char ch, const string& charlist)
{
    string::size_type pos = charlist.find_first_of(ch);
    if (pos != string::npos)
    {
        return true;
    }
    else
    {
        return false;
    }
}


substring
paren_match(const string& inputText)
{
    // create a substring from inputText and call it
    substring subInputText(&inputText, 0, inputText.length());
    return paren_match(subInputText);
}


substring
paren_match(const substring& inputText)
{
    string::size_type position = 0, firstParenLocation = string::npos;
    bool foundFirstParen = false;
    int parenCount = 0;

    while (
        position < inputText.length()
        && !(foundFirstParen && (parenCount == 0))
    )
    {
        // scan for the first paren
        char currentChar = const_cast < substring& > (inputText)[position];
        if (currentChar == '(')
        {
            if (!foundFirstParen)
            {
                foundFirstParen = true;
                firstParenLocation = position;
            }
            parenCount++;
        }

        else if (currentChar == ')')
        {
            if (foundFirstParen)
            {
                parenCount--;
            }
        }
        position++;
    }

    if (foundFirstParen && (parenCount == 0))
    {
        // in this case, create a substring based on this one
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << ":" << inputText.str().substr(firstParenLocation + 1, (position - firstParenLocation - 2)) << ":\n"; );
        substring retVal(inputText, firstParenLocation + 1, position - firstParenLocation - 2);
        return retVal;
    }
    else
    {
        // in this case, it is an error
        cerr << "could not match the string" << endl;
        if (firstParenLocation != string::npos)
        {
            substring errVal(inputText, firstParenLocation, inputText.length() - firstParenLocation);
            cerr << "string: " << errVal << endl;
        }
        return substring();
    }
    return substring();
}


string
str_paren_match(const string& inputText)
{
    // create a substring from inputText and call it
    substring subInputText(&inputText, 0, inputText.length());
    string matched = (paren_match(subInputText)).str();
    return( matched );
}



sub_split_t
sub_split_paren_match(const string& inputText, const string& characters)
{
    string::size_type position = 0, oldPosition = 0;

    sub_split_t splitted;

    unsigned int length = inputText.length();

    while (position < length)
    {
        // the code must loop through the entire text to make sure it
        // matches parentheses

        int parenCount = 0;

        while ((position < length)
                && !(isIn(inputText[position], characters) && (parenCount == 0))
              )
        {
            char currentChar = inputText[position];

            if (currentChar == '(')
            {
                parenCount++;
            }

            if (currentChar == ')')
            {
                parenCount--;
            }
            position++;
        }

        if (position == string::npos)
            position = inputText.length();

        if (parenCount != 0)
        {
            // we reached the end without balanced parentheses -- mark as an
            // error and continue
            cerr << "this is an error" << endl;

        }

        ODEBUG(DEBUG_LOWLEVEL, 2, cout << inputText.substr(oldPosition, (position - oldPosition)); );
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << ":\n"; );
        (splitted).push_back(
            substring(
                &inputText, oldPosition, (position - oldPosition))
        );
        oldPosition = inputText.find_first_not_of(characters, position);
        if (oldPosition == string::npos)
            oldPosition = inputText.length();

        position = oldPosition;
    }

    ODEBUG(DEBUG_LOWLEVEL, 2, cout << "number of lines: " << splitted.size() << endl; );

    return splitted;
}


split_t
split_paren_match(const string& inputText, const string& characters)
{
    string::size_type position = 0, oldPosition = 0;

    split_t splitted;

    unsigned int length = inputText.length();

    while (position < length)
    {
        // the code must loop through the entire text to make sure it
        // matches parentheses

        int parenCount = 0;

        while ((position < length)
                && !(isIn(inputText[position], characters) && (parenCount == 0))
              )
        {
            char currentChar = inputText[position];

            if (currentChar == '(')
            {
                parenCount++;
            }

            if (currentChar == ')')
            {
                parenCount--;
            }
            position++;
        }

        if (position == string::npos)
            position = inputText.length();

        if (parenCount != 0)
        {
            // we reached the end without balanced parentheses -- mark as an
            // error and continue
            cerr << "this is an error" << endl;

        }

        ODEBUG(DEBUG_LOWLEVEL, 2, cout << inputText.substr(oldPosition, (position - oldPosition)); );
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << ":\n"; );
	
	if((position != oldPosition) && (oldPosition < position))
        {      
	    (splitted).push_back(
                inputText.substr( oldPosition, (position - oldPosition)));
	}

        oldPosition = inputText.find_first_not_of(characters, position);
        if (oldPosition == string::npos)
            oldPosition = inputText.length();

        position = oldPosition;
    }

    ODEBUG(DEBUG_LOWLEVEL, 2, cout << "number of lines: " << splitted.size() << endl; );

    return splitted;
}



sub_split_t sub_split(const string& inputText, const string& characters)
{
    string::size_type position = 0, oldPosition = 0;

    sub_split_t splitted;

    unsigned int length = inputText.length();

    while (position < length)
    {
        position = inputText.find_first_of(characters, oldPosition);
        // advance forward and eat all characters of type characters

        if (position == string::npos)
            position = inputText.length();

        ODEBUG(DEBUG_LOWLEVEL, 2, cout << inputText.substr(oldPosition, (position - oldPosition)); );
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << ":\n"; );
        (splitted).push_back(
            substring(
                &inputText, oldPosition, (position - oldPosition))
        );
        oldPosition = inputText.find_first_not_of(characters, position);
        if (oldPosition == string::npos)
            oldPosition = inputText.length();
        position = oldPosition;

    }
    ODEBUG(DEBUG_LOWLEVEL, 2, cout << "number of lines: " << splitted.size() << endl; );

    return splitted;
}


split_t split(const string& inputText, const string& characters)
{
    // this is the original split definition, which I think may be inefficient
    // I will instead try a more efficient way of doing things.

    string::size_type position = 0, oldPosition = 0;

    split_t splitted;

    unsigned int length = inputText.length();

    while (position < length)
    {
        position = inputText.find_first_of(characters, oldPosition);
        if (position == string::npos)
            position = inputText.length();
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << inputText.substr(oldPosition, (position - oldPosition)); );
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << ":\n"; );
        if((position != oldPosition) && (oldPosition < position))
        {      
            (splitted).push_back(
                inputText.substr(oldPosition, (position - oldPosition)));
        }
        oldPosition = position + 1;
    }
    return splitted;
}

vector < string > new_split(const string& inputText, const string& characters)
{
    vector < string > splitted;
#if 0
    // this is the original split definition, which I think may be inefficient
    // I will instead try a more efficient way of doing things.

    string::size_type position = 0, oldPosition = 0;

    vector < string > splitted;

    unsigned int length = inputText.length();

    while (position < length)
    {
        position = inputText.find_first_of(characters, oldPosition);
        if (position == string::npos)
            position = inputText.length();
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << inputText.substr(oldPosition, (position - oldPosition)); );
        ODEBUG(DEBUG_LOWLEVEL, 2, cout << ":\n"; );
        (splitted).push_back(
            inputText.substr(oldPosition, (position - oldPosition)));
        oldPosition = position + 1;
    }
    return splitted;

    // this is the new way

    // build a set

    set < char > charSet;

    string::size_type charPos = 0;
    string::size_type charLen = characters.length();
    while (charPos < charLen)
    {
        charSet.insert(characters[charPos]);
        charPos++;
    }


    string::size_type position = 0;
    string::size_type length = inputText.length();

    vector < string > splitted;

    bool inDividingChar = true;

    while (position < length)
    {
        if (!inDividingChar)
        {
            if (charSet.find(inputText[position]) != charSet.end())
            {
                inDividingChar = true;
            }
        }
        if (charSet.find(inputText[position]) == charSet.end())
        {
            if (inDividingChar)
            {
                splitted.push_back(string("", 20));
            }
            inDividingChar = false;
            splitted.back() += inputText[position];
        }
        position++;
    }
    return splitted;
#endif 
    return splitted;
}


void chop(string* input)
{
    string::iterator p = input->end();

    if (p != input->begin())
        --p;
    input->erase(p);
}

void chomp(string* input)
{
    size_t i;

    i = input->length() - 1;

    if (i >= 0)
    {
        if ( ((*input)[i] == '\n' ) ||
                ((*input)[i] == '\r'))
        {
            input->erase(i, 1);
        }
    }
}



string str2lower(string str)
{
    unsigned int i = 0;
    unsigned int size = str.size();

    while (i < size)
    {
        str[i] = tolower(str[i]);
        i++;
    }
    return str;
}

string c2lower_s(const char* cstr)
{
    string str;
    //    unsigned int i = 0;

    while (cstr && *cstr)
    {
        str += tolower(*cstr);
        cstr++;
    }
    return str;
}


int matchString(string a, string regex)
{
    regex_t re;
    int status;


    if ((status = regcomp( &re, regex.c_str(), REG_EXTENDED)) != 0)
        return (status);
    status = regexec( &re, a.c_str(), 0, NULL, 0);
    regfree(&re);
    return (status);
}


bool stringToInt ( const string& s, int *num)
{
    /// skipping spaces in the front
    //   string::size_type position = 0;
    //  position = s.find_first_not_of(string(" "), 0);

    int position = 0;
    int i, j;

    for (i = 0, j = s.length(); i < j; i++)
    {
        if (s[i] != ' ')
        {
            position = i;
            break;
        }
    }


    /// convert the string to integer
    char ch;
    int value = 0;
    for (i = position, j = s.length(); i < j; i++)
    {
        ch = s[i];
        if ( (ch >= '0') && (ch <= '9') )
            value = atoi(&ch) + value * 10;
        else if ( ch == ' ' )
            break;   /// a whole number is read
        else
            return (false);   /// illegal char, not a digit or space
    }


    *num = value;
    return (true);
}



bool getRange( const string& s, int* start, int* end, bool* isRange)
{
    /// initialize all possible returns
    *start = 0;
    *end = 0;
    *isRange = false;

    /// split the range into two integers, assuming the numbers
    /// are separeted by '-'.
    // deque<string> test_1_vector;
    //sub_split_t test_1_vector;
    split_t test_1_vector;

    // string myStr(s.str());

    //test_1_vector = sub_split(myStr, "-");
    test_1_vector = split( s, "-");

    /// read in the first integer if any
    if (test_1_vector.size() >= 1 )
    {
        if (!stringToInt( test_1_vector[0], start))  // if (true), a valid integer is read into start variable
            return (false);  // first number is not a valid integer
    }

    else
        return (false);   // no data in the input string at all

    /// read in the second integer if any
    if (test_1_vector.size() >= 2 )
    {
        if (stringToInt( test_1_vector[1], end))  // if (true), a valid second integer is read into end variable
        {
            if (*start <= *end )
                *isRange = true;   // got a valid range
            else
                return (false);   // range is not valid, larger number went first
        }

        else
            return (false);   // second number after '-' is not valid, ignore the range
    }


    return (true);  // success in getting one or two valid integers from string
}

//

Data convertToHex(const unsigned char* src, int len)
{
    Data data;

    unsigned char temp;

    //convert to hex.
    int i;
    //int j = 0;

    for (i = 0; i < len; i++)
    {
        temp = src[i];

        int hi = (temp & 0xf0) / 16;
        int low = (temp & 0xf);

        char buf[5];
        buf[0] = '\0';

	assert(hi >= 0);
	assert(hi < 16);
	assert(low >= 0);
	assert(low < 16);

        sprintf(buf, "%1.1x%1.1x", hi, low);
        data += buf;
    }

#if 0  //Change to 1 if needed for debugging
    cpLog(LOG_DEBUG, "convertToHex, value is: %s", data.logData());
#endif

    return data;
}



int convertToUnsigned(const Data& data, unsigned char* dest)
{
    int len = data.length();
    data.getData(reinterpret_cast<char*>(dest), len+1);

    dest[len] = '\0';
    //convert and return the length of dest.

    cpLog(LOG_DEBUG, "orig value string: %s", data.logData());

    cpLog(LOG_DEBUG, "orig value hex:    %s", convertToHex(dest, len).logData());
    cpLog(LOG_DEBUG, "convertToUnsigned, length is %d, value is : %s", len, dest);
    return len;
}
