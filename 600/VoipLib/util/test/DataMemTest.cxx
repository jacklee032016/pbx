/*
* $Id: DataMemTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Data.hxx"

class XY
{
    public:
        XY();
        void parse();
    private:
        Data s;
};


XY::XY()
        : s("user@host.com:5000")
{}


void
XY::parse()
{
    Data x;
    int ret = s.match(":", &x, true);
}

void sub(const char* s)
{
    XY asdf;

    XY* x = new XY;
    x->parse();
    delete x ;
    //    string foo;

    //    foo = "helloasdifhasdfhoiasdhfoi hqwio hqiwth qphtpqihthqewiht";

    //    string baz = s;

    //    string bar = foo.substr(0, 25);
}


int main()
{
    for (int i = 0; i < 100000; i++)
    {
        sub("hello, world");
        //Data x = "helloasdifhasdfhoiasdhfoi hqwio hqiwth qphtpqihthqewiht";
        //	Data x = "hello";
    }
    //	Data z = "helloasdifhasdfhoiasdhfoi hqwio hqiwth qphtpqihthqewiht";
    //    Data y = "hi there";



    return 0;
}
