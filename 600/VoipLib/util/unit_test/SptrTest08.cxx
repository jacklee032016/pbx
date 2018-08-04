/*
 * $Id: SptrTest08.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "Sptr.hxx"
#include "Verify.hxx"

// test 2 -- this is construction of dual "parallel" Sptrs that would
// happen using the assignment operator

int a = 0;

class Test
{
    public:
        Test()
        {
	    a++;
        }
        ~Test()
        {
	    a--;
        }
};


int main()
{
    {
         Sptr<Test> a1 = new Test;
         Sptr<Test> s2 = new Test;
         Sptr<Test> c = new Test;
         Sptr<Test> d =  new Test;
         Sptr<Test> e = new Test;
         Sptr<Test> f = new Test;
         Sptr<Test> g = new Test;
         Sptr<Test> h = new Test;
         Sptr<Test> i = new Test;
	test_verify(a == 9);
    }
    test_verify(a == 0);
    return test_return_code(2);
}
