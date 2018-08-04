/*
* $Id: perfDataFunctionsTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Data.hxx"
#include "CWBuffer.hxx"
#include "DurationStatistic.hxx"
#include "PerformanceDb.hxx"
   
using Vocal::Statistics::DurationStatistic;
using Vocal::Statistics::StatisticsDb;

    
#define NONE 0
#define MATCH 1
#define MATCHREPLACE 11
#define EQUALOPERATOR 2
#define REMOVESPACES 3
#define LENGTH 4
#define EMPTYCONSTRUCTOR 5
#define TRUNCATE 6
#define NOTEQUAL 7
#define REPLACE 8
#define REMOVELWS 9
#define EXPAND 10       

//testing functions of Data, which gets called very frequently.
//initially, considering all functions, called by SipVia, decoding.
    
int main( int argc, char* argv[] )
{
    if ( argc < 2 )
    {
        cerr << "Usage: "
        << argv[0]
	<< " <#timesToRunTest>"
	<< " <FunctionToTest: Enter 1 - match, 2 - operator =, 3 - removeSpaces, 4 - length, 5 - empty Constructor, 6 - Truncate , 7 - operator !=> 8 - Replace , 9 - RemoveLWS, 10 - Expand , 11- MatchReplace" << endl;
        exit( -1 );
    }
    
    int numTimes = 1;
    
    int functionSelect = NONE;
    
    Data line(" SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.1 ");
    
    StatisticsDb db;
    
    DurationStatistic stat(db, Data("perfDataFunctionsTest"));
    
    float sum = 0;
    
    if (argc >= 2)
    {
	numTimes = atoi(argv[1]);
    }
    
    if (argc >= 3)
    {
        functionSelect = atoi(argv[2]); 
    }
    
    stat.start();
    for (int i = 0; i < numTimes; i++)
    {
	switch(functionSelect)
	{
	    case MATCH:
	    {
		Data viadata;
	        line.match("/", &viadata, true);
		//cout << "line after replace is: " << line.getData() << endl;
	    }
	    break;
            case MATCHREPLACE:
            {
                Data viadata;
                Data repl("ABCD");
                line.match("/", &viadata, true, repl);
                //        cout << "line after replace is: " << line.getData() << endl;
            }      
            break;
            case REMOVELWS:
            {
                line.removeLWS();
                //        cout << "line after replace is: " << line.getData() << endl;
            }     
	    case EQUALOPERATOR:
	    {
		Data line2;
    
	        line2 = line;
	    }
	    break;
	    case REMOVESPACES:
	    {
	        line.removeSpaces();
	    }
	    break;
	    case LENGTH:
	    {
	        line.length();
	    }
	    break;
	    case EMPTYCONSTRUCTOR:
	    {
	        Data emptyConstructor;
	    }
	    break;
	    case TRUNCATE:
	    {
		CWBuffer buff("abcdefgh", 8);
		buff.truncate(4, 0);
	    }
	    break;
	    case NOTEQUAL:
	    {
		Data line2("defghi");
		line != line2;
	    }
	    break;
            case REPLACE:
            {
                Data repLine("ABCDE");
//                line.replace(3, 4, repLine);
                //cout << "after replace: " << line.getData() << endl;
            }
           break;  
           case EXPAND:
            {
#if 0
                Data via2("SIP/2.0/UDP 172.19.175.150:5060;branch=78377a23f9fb8014efae61c726ec6e77.1");
                line+= ",";
                line+= via2;
                line.expand(Data line, Data(",") , Data("\r\n"), Data("\r\n"));
#endif
            }
            break;
	}
    }

    stat.stop();
    stat.record();
    sum+= stat.average();
    
    cout << "average ticks : " << sum/numTimes << endl;
    
    return 0;
}
