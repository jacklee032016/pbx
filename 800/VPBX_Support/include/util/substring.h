#ifndef SUBSTRING_H_
#define SUBSTRING_H_
/*
* $Id: substring.h,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <string>

class substring
{
    public:
        substring()
        :
        myString(0),
        start(string::npos),
        myLen(0)
        {}


        substring(const string& str)
        :
        myString(const_cast < string* > (&str)),
        start(0),
        myLen(str.length())
        {}



        substring(const string* str, string::size_type i, string::size_type n)
        :
        myString(const_cast < string* > (str)),
        start(i),
        myLen(n)
        {}


        substring(const substring& x)
        :
        myString(x.myString),
        start(x.start),
        myLen(x.myLen)
        {}


        substring(const substring& x, string::size_type i, string::size_type n)
        :
        myString(x.myString),
        start(x.start + i),
        myLen(n)
        {}


        substring& operator= (const substring& x)
        {
            this->myString = x.myString;
            this->start = x.start;
            this->myLen = x.myLen;
            return *this;
        }

        bool operator== (const string& x) const
        {
            if (!myString)
            {
                return false;
            }
            else
            {
                return myString->substr(start, myLen) == x;
            }
        }

        bool operator== (const char* x) const
        {
            if (!myString)
            {
                return false;
            }
            else
            {
                return myString->substr(start, myLen) == x;
            }
        }

        string str() const
        {
            return myString->substr(start, myLen);
        }

        friend ostream& operator<<(ostream& s, const substring& x)
        {
            return s << x.str();
        }

        char operator[](string::size_type i)
        {
            return (*myString)[i + start];
        }

        char operator[](string::size_type i) const
        {
            return (*myString)[i + start];
        }
#ifndef WIN32
        const char* c_str(void) const
        {
            return myString->substr(start, myLen).c_str();
        }
#endif
        string::size_type length() const
        {
            return myLen;
        }

        string::size_type getStart() const
        {
            return start;
        }

    private:
        string* myString;
        string::size_type start;
        string::size_type myLen;
};

#endif
