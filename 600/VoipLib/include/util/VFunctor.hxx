#ifndef VFUNCTOR_HXX_
#define VFUNCTOR_HXX_
/*
 * $Id: VFunctor.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


typedef void (*VFUNC)(void*);

class VFunctor
{
    public:
	/// constructor.  Takes a VFUNC and the arguments to that VFUNC
        VFunctor(VFUNC func, void* args)
	    : func_(func),
	      args_(args)
        {
        }

	/// to the work specified by this functor
        void doWork()
        {
            (func_)(args_);
        }

	/// copy constructor
        VFunctor(const VFunctor& other)
	    : func_(other.func_),
	      args_(other.args_)
        {
        }

	/// assignment operator
        VFunctor& operator=(const VFunctor& other)
        {
            if (this != &other)
            {
                func_ = other.func_;
                args_ = other.args_;
            }
            return (*this);
        }

    private:
        VFUNC func_;
        void* args_;
};

#endif
