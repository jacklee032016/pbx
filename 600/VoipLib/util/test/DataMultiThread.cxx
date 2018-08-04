/*
* $Id: DataMultiThread.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include <unistd.h>
#include "Data.hxx"
#include "Verify.hxx"
#include "VThreadPool.hxx"

inline int randomInt(int max)
{
    return random() % max;
}

enum numberCases
{
    INITIALIZE_ALL,
    INITIALIZE_ONE,
    INITIALIZE_TWO,
    COPY_ONE_TWO,
    COPY_TWO_ONE,
    COMPARE_EQUAL,
    COMPARE_UNEQUAL,
    ASSIGN_C_STRING,
    APPEND_TWO,
    APPEND_C_STRING,
    NOACTION
};


inline char randomChar()
{
    return (32 + randomInt(126 - 32));
} 

inline bool operate(Data& data1, Data& data2)
{
    bool result = false;

    switch(randomInt(NOACTION))
    {
    case INITIALIZE_ALL:
	data1 = "";
	data2 = "";
	break;
    case INITIALIZE_ONE:
	data1 = "";
	break;
    case INITIALIZE_TWO:
	data2 = "";
	break;
    case COPY_ONE_TWO:
	data1 = data2;
	break;
    case COPY_TWO_ONE:
	data2 = data1;
	break;
    case COMPARE_EQUAL:
	result = data1 == data2;
	break;
    case COMPARE_UNEQUAL:
	result = data1 != data2;
	break;
    case ASSIGN_C_STRING:
    {
	int buflen = randomInt(10);
	char buf[buflen+1];
	for(int i = 0; i < buflen; i++)
	{
	    buf[i] = randomChar();
	}
	buf[buflen] = '\0';
	data1 = buf;
	break;
    }
    case APPEND_TWO:
    {
	data1 += data2;
	break;
    }
    case APPEND_C_STRING:
    {
	int buflen = randomInt(10);
	char buf[buflen+1];
	for(int i = 0; i < buflen; i++)
	{
	    buf[i] = randomChar();
	}
	buf[buflen] = '\0';
	data1 += buf;
	break;
    }
    case NOACTION:
    default:
	break;
    }
    return result;
}


Data one;
Data two;

void operateThread(void *)
{
    while (1)
    {
	operate(one, two);
	operate(two, one);
    }
}



int main()
{
    try
    {
        VThreadPool pool(5);


	VFUNC function = operateThread;

	
        VFunctor functor1(function, NULL);
        pool.addFunctor(functor1);

        VFunctor functor2(function, NULL);
        pool.addFunctor(functor2);

        VFunctor functor3(function, NULL);
        pool.addFunctor(functor3);

        VFunctor functor4(function, NULL);
        pool.addFunctor(functor4);

        VFunctor functor5(function, NULL);
        pool.addFunctor(functor5);


        while (true)
        {
            sleep(1);
        }
    }
    catch (VException& e)
    {
        cerr << "Caught exception:" << e.getDescription() << endl;
        exit( -1);
    }

    return 0;
}
