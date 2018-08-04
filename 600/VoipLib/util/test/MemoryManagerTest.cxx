/*
* $Id: MemoryManagerTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "MemoryManager.hxx"
#include "Verify.hxx"
#include "Data.hxx"
#include <new>

void  testAlloc()
{

    char* p = (char*)(MemoryManager::mynew(100));
    MemoryManager::displayAllocList();
    char* q = (char*)(MemoryManager::mynew(300));
    MemoryManager::displayAllocList();
    char* r = (char*)(MemoryManager::mynew(400));
    MemoryManager::displayAllocList();
    char* s = (char*)(MemoryManager::mynew(500));
    MemoryManager::displayAllocList();
    char* t = (char*)(MemoryManager::mynew(600));
    MemoryManager::displayAllocList();
    char* u = (char*)(MemoryManager::mynew(700));
    MemoryManager::displayAllocList();
    char* v = (char*)(MemoryManager::mynew(800));
    MemoryManager::displayAllocList();
    char* w = (char*)(MemoryManager::mynew(900));
    MemoryManager::displayAllocList();
    char* x = (char*)(MemoryManager::mynew(1000));
    MemoryManager::displayAllocList();
    char* y = (char*)(MemoryManager::mynew(1100));
    MemoryManager::displayAllocList();
    char* z = (char*)(MemoryManager::mynew(1200));
    MemoryManager::displayAllocList();
    char* a = (char*)(MemoryManager::mynew(1300));
    MemoryManager::displayAllocList();
    char* b = (char*)(MemoryManager::mynew(1400));
    MemoryManager::displayAllocList();
    char* c = (char*)(MemoryManager::mynew(1500));
    MemoryManager::displayAllocList();

    
    strcpy(p, "lmnopqrstuvwxyz");
    strcpy(q, "lmnopqrstuvwxyz");
    strcpy(r, "lmnopqrstuvwxyz");
    strcpy(s, "lmnopqrstuvwxyz");
    strcpy(t, "lmnopqrstuvwxyz");
    strcpy(u, "lmnopqrstuvwxyz");
    strcpy(v, "lmnopqrstuvwxyz");
    strcpy(w, "lmnopqrstuvwxyz");
    strcpy(x, "lmnopqrstuvwxyz");
    strcpy(y, "lmnopqrstuvwxyz");
    strcpy(z, "lmnopqrstuvwxyz");
    strcpy(a, "lmnopqrstuvwxyz");
    strcpy(b, "lmnopqrstuvwxyz");
    strcpy(c, "lmnopqrstuvwxyz");


    cout << " p is: " << p << endl;
    cout << " q is: " << q << endl;
    cout << " r is: " << r << endl;
    cout << " s is: " << s << endl;
    cout << " t is: " << t << endl;
    cout << " u is: " << u << endl;
    cout << " v is: " << v << endl;
    cout << " w is: " << w << endl;
    cout << " x is: " << x << endl;
    cout << " y is: " << y << endl;
    cout << " z is: " << z << endl;
    cout << " a is: " << a << endl;
    cout << " b is: " << b << endl;
    cout << " c is: " << c <<  endl;

    MemoryManager::displayAllocList();

    MemoryManager::mydelete(p);

    MemoryManager::mydelete(q);

    MemoryManager::mydelete(r);

    MemoryManager::mydelete(s);

    MemoryManager::mydelete(t);

    MemoryManager::mydelete(u);

    MemoryManager::mydelete(v);

    MemoryManager::mydelete(w);

    MemoryManager::mydelete(x);

    MemoryManager::mydelete(y);

    MemoryManager::mydelete(z);

    MemoryManager::mydelete(a);

    MemoryManager::mydelete(b);

    MemoryManager::mydelete(c);

    MemoryManager::displayAllocList();
    //allocate again. this time, this should be allocating from free list.
    (char*)(MemoryManager::mynew(100));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList();
    (char*)(MemoryManager::mynew(300));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList();
    (char*)(MemoryManager::mynew(400));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList();
    (char*)(MemoryManager::mynew(500));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList();
    
    (char*)(MemoryManager::mynew(1000));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList();
    
    (char*)(MemoryManager::mynew(5000));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList();
    
    (char*)(MemoryManager::mynew(50000));
    MemoryManager::displayAllocList();
    MemoryManager::displayFreeList(); 
    
    
    char* systemPtr = new char(1000);
    MemoryManager::mydelete(systemPtr);

    
    
    
    test_verify(1); // do some valid tests later
}

    
int main()
{

    testAlloc();
    return test_return_code(1);
}
