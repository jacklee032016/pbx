#ifndef SPTR_REFCOUNT_HXX_
#define SPTR_REFCOUNT_HXX_
/*
* $Id: SptrRefCount.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/
#include <assert.h>
#include "VMutex.h"
#include "debug.h"
#include "CountSemaphore.hxx"
#include "FastMutex.hxx"


#define SPTR_DEBUG 1
#define SPTR_THREADSAFE 1
#define SPTR_TRACEBACK 1

#ifdef CODE_OPTIMIZE
// turn off debugging and tracebacks in optimized code

#undef SPTR_DEBUG
#undef SPTR_TRACEBACK
#define SPTR_DEBUG 0
#define SPTR_TRACEBACK 0

#endif

#ifdef WIN32
#define WORKAROUND_VISUAL_C
#endif


#if SPTR_DEBUG

#include <map>

/** assists in debugging Sptrs (both use and implementation), by
 * constructing a map of currently active Sptrs.  A singleton. */
class SptrDebugger
{
    private:
        /// default constructor
        SptrDebugger();
        /// used to get an instance.  If no instance exists, create one
        static SptrDebugger* getInstance();

        typedef map < void*, bool > PtrMap;

        static SptrDebugger* impl_;

        PtrMap ptrsInUse;

        VMutex mutex;

    public:
        /// add a new pointer when it is new.
        static void newPtr(void* ptr);
        /// remove an pointer after it has been deleted.
        static void deletePtr(void* ptr);
};

// these functions are used as debug breakpoints if SPTR_TRACEBACK
// is set to 1

void sptrDebugMarkTraceback(void* sptr, void* ptr);
void sptrDebugClearTraceback(void* sptr, void* ptr);
void sptrDebugDumpTraceback(char* filename);

#endif

#if SPTR_THREADSAFE
#define SPTR_LOCK(x)   (x).lock()
#define SPTR_UNLOCK(x) (x).unlock()
#else
#define SPTR_LOCK(x);
#define SPTR_UNLOCK(x);
#endif

/** Template simulates a "smart" pointer which deletes the item it is
    pointing to when no more references to the item exist.  Warning:
    circular references will produce memory leaks. 
    
    <P>Note that only one Sptr should be constructed from the original
    ptr -- Sptr will free twice (and cause havoc) if it is misused like so:
    <P>
    <B> WRONG: </B>
    <PRE>
        T&#42; obj = new T();
        Sptr&lt;T> p;
        Sptr&lt;T> q;
        p = obj;
        q = obj;  
    </PRE>
    <p>
    <I>now both p and q think they are the only ones who will free the 
    memory, so you will get an error.</i>
    </p>
    <p><B>CORRECT:</b></p>
    <PRE> 
       T&#42; obj = new T();
       Sptr&lt;T> p;
       Sptr&lt;T> q;
       p = obj; 
       q = p;   // this keeps things OK.
    </PRE>

    <p>
    When all Sptr&lt;T> objects pointing to the object go out of scope, or are
    assigned to point to some other object, the object is deleted.
    </p>
    <p>
    <b>Examples</b>
    </p>
    <p>
    <PRE>
       Sptr &lt;Data> object = new Data;
       &#42;object = "string1";
       cout << &#42;object << endl;  // prints string1
       
       {
         Sptr &lt;Data> o2 = new Data("string2");
         object = o2;
	 
         // at this point, the string1 object has no references
         // because object points at string2 rather than string1, so
         // string1 is deleted.
	 
         // the string2 object has 2 references to it: object and o2.

         Sptr &lt;Data> o3 = new Data("string3");
         
         // at this point, o3 points to string3.
       }
       
       // at this point, the second string "string2" has only 1
       // pointer to it since o2 was destroyed (it was local to the
       // above section) since the reference count is 1 (object points
       // to it) it is left alone.
       
       // however, since o3 was destoyed, "string3"'s reference count
       // goes to 0 and it is destroyed.
       
       LocalScopeAllocator lo;
       printf("%s\n", object->getData(lo)); // print the object

    </PRE>
    </p>
*/
template < class T >
class Sptr
{
// we need to work around Visual C++'s limitations. this
// is needed so that we can grab these bits directly
#if defined(WORKAROUND_VISUAL_C)
public:
#else
private:
#endif

        mutable  T* ptr;
        mutable CountSemaphore* count;
#if defined (ASSIST_FASTMUTEX_AVAILABLE)
    mutable FastMutex mutex;
#else
    mutable VMutex mutex;
#endif

        /// increment the reference count.
        void increment()
        {
            if (ptr)
            {
                if (!count)
                {
                    count = new CountSemaphore();
                }

                count->increment();
            }
#if SPTR_TRACEBACK
            sptrDebugMarkTraceback((void*) this, (void*) ptr);
#endif

        }

        /// decrement the reference count
        void decrement()
        {
            if (ptr && count)
            {
#if SPTR_TRACEBACK
                sptrDebugClearTraceback((void*) this, (void*) ptr);
#endif
                if(count->decrement())
                {
#if SPTR_DEBUG
#if 0
                    cerr << "deleting " << ptr << endl;
                    breakpoint();
#endif
                    SptrDebugger::deletePtr((void*) ptr);
#endif
                    delete ptr;
                    delete count;
                }
            }
            ptr = 0;
            count = 0;
        }

    public:
    // we will skip these since they don't work under Visual C++
#if !defined(WORKAROUND_VISUAL_C)
#if defined (__SUNPRO_CC) || defined (__INTEL_COMPILER)
        /** conversion operator converts pointers of this class to
         * class Sptr< T2 >., where T2 is a different base class.  This
         * is most often used when attempting to call a method of the
         * base class through a derived class pointer.
         */
        template < class T2 >
        operator Sptr<T2 > () 
        {
            return Sptr < T2 > (ptr, count);
        }
#endif

        /** conversion operator converts pointers of this const class
         * to class const Sptr< T2 >., where T2 is a different base
         * class.  This is most often used when attempting to call a
         * const method of the base class through a derived class
         * pointer.  This is a workaround for SUNPRO .
         */
        template < class T2 >
        operator const Sptr<T2 > () const
        {
            return Sptr < T2 > (ptr, count);
        }
#endif

        /// default constructor.  points to NULL.
        Sptr() : ptr(0), count(0)
        {}
        ;

        /** constructor used most often as the constructor from a
         * plain pointer.  Do not use this to convert a single pointer
         * to a smart pointer multiple times -- this will result in an
         * error (see class introduction for details).
         */
        Sptr(T* original, CountSemaphore* myCount = 0)
        : ptr(original), count(myCount)
        {
            if (ptr)
            {
#if SPTR_DEBUG
                if (!myCount)
                {
                    SptrDebugger::newPtr((void*) ptr);  // void* takes away warning
                }

#endif
                increment();
            }
        };

#if defined(WORKAROUND_VISUAL_C)
        /** conversion constructor -- this converts Sptrs of one type to
         *  another, but is ONLY used in working around the missing
         *  template conversion operators in Visual C++.
         */ 
        template <class T2>
        Sptr(const Sptr<T2>& x)
        : ptr(x.ptr), count(x.count)
        {
            increment();
        };
#endif

        /** copy constructor
         */
        Sptr(const Sptr& x)
        : ptr(x.ptr), count(x.count)
        {
            increment();
        };

        /// destructor
        ~Sptr()
        {
            {
                SPTR_LOCK(*this);
                decrement();
                SPTR_UNLOCK(*this);
            }
        }

        /// dereference operator
        T& operator*() const
        {
            assert(ptr);
            return *ptr;
        }

        /// ! operator .  Returns true if ptr == 0, false otherwise.
        int operator!() const
        {
            if (ptr)
            {
                return (ptr == 0);
            }
            else
                return true;
        }

        /// pointer operator.
        T* operator->() const
        {
            return ptr;
        }

        /** dynamicCast works similarly to the actual dynamic_cast()
         * operator.
	 <P>
	   <B>EXAMPLE</B>
         
         <PRE>
             class A {
               ...
             };
             class B : public A {
               ...
             };
             class C {
               ...
             };
             ...
             int main()
             {
                 Sptr< A > a;
                 Sptr< B > b;
                 Sptr< C > c;

                 a = new B;

                 b.dynamicCast(a);
                 // now, b points to the same thing as a

                 c.dynamicCast(a);
                 // now, c is the NULL pointer.
             }
         </PRE>
         */
        template < class T2 > Sptr& dynamicCast(const Sptr < T2 > & x)
        {
            if (ptr == x.getPtr()) return *this;
            SPTR_LOCK(x);
            SPTR_LOCK(*this);
            decrement();
            if(T* p = dynamic_cast < T* > (x.getPtr()))
            {
                count = x.getCount();
                ptr = p;
                increment();
            }
            SPTR_UNLOCK(*this);
            SPTR_UNLOCK(x);
            return *this;
        }
        /** assignment operator -- this is most often used to assign
         * from a smart pointer to a derived type to a smart pointer
         * of the base type.
         */
        template < class T2 >
        Sptr& operator=(const Sptr < T2 > & x)
        {
            if (ptr == x.getPtr()) return * this;
            SPTR_LOCK(x);
            SPTR_LOCK(*this);
            decrement();
            ptr = x.getPtr();
            count = x.getCount();
            increment();
            SPTR_UNLOCK(x);
            SPTR_UNLOCK(*this);
            return *this;
        }


        /** assignment operator from plain pointer.  Do not use this
         * to convert a single pointer to a smart pointer multiple
         * times -- this will result in an error (see class
         * introduction for details).
         */
        Sptr& operator=(T* original)
        {
            if (ptr == original) return * this;
            SPTR_LOCK(*this);
            decrement();
#if SPTR_DEBUG
            SptrDebugger::newPtr((void*) original);
#endif
            ptr = original;
            increment();
            SPTR_UNLOCK(*this);
            return *this;
        };

        /// assignment operator
        Sptr& operator=(const Sptr& x)
        {
            if (ptr == x.ptr) return * this;
            SPTR_LOCK(x);
            SPTR_LOCK(*this);
            decrement();
            ptr = x.ptr;
            count = x.count;
            increment();
            SPTR_UNLOCK(*this);
            SPTR_UNLOCK(x);
            return *this;
        }

        /// compare whether a pointer and a smart pointer point to different things
        friend bool operator!=(const void* y, const Sptr& x)
        {
            if (x.ptr != y)
                return true;
            else
                return false;
        }

        /// compare whether a smart pointer and a pointer point to different things
        friend bool operator!=(const Sptr& x, const void* y)
        {
            return (y != x);
        }

        /// compare whether a pointer and a smart pointer point to the same thing
        friend bool operator==(const void* y, const Sptr& x)
        {
            if (x.ptr == y)
                return true;
            else
                return false;
        }

        /// compare whether a smart pointer and a pointer point to the same thing
        friend bool operator==(const Sptr& x, const void* y)
        {
            return (y == x);
        }

        /// compare whether two smart pointers point to the same thing
        bool operator==(const Sptr& x) const
        {
            if (x.ptr == ptr)
                return true;
            else
                return false;
        }

        /// compare whether two smart pointers point to the same thing
        bool operator!=(const Sptr& x) const
        {
            return !(operator==(x));
        }

        /**
           this interface is here because it may sometimes be
           necessary.  DO NOT USE unless you must use it.
           get the actual mutex of the smart pointer.
        */
        VMutex* getMutex() const
        {
            return 0;
        }

        /**
           this interface is here because it may sometimes be
           necessary.  DO NOT USE unless you must use it.
           get the value of the reference count of the smart pointer.
        */
        CountSemaphore* getCount() const
        {
            return count;
        }

        /**
           this interface is here because it may sometimes be
           necessary.  DO NOT USE unless you must use it.
           get the pointer to which the smart pointer points.
        */
        T* getPtr() const
        {
            return ptr;
        }

#if SPTR_THREADSAFE

        void lock() const
        {
            mutex.lock();
        }
        void unlock() const
        {
            mutex.unlock();
        }
#endif


};

#endif
