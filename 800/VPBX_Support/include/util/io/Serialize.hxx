#ifndef SERIALIZE_HXX_
#define SERIALIZE_HXX_
/*
* $Id: Serialize.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Data.hxx"


/** Infrastructure common to ASSIST.
 */
namespace Assist 
{



/** Infrastructure in ASSIST related to Input/Output.<br><br>
 */
namespace IO
{


//uncomment the following line, to enable serialize 
// #define SERIALIZE       0 


/** usage 1:
 *
 *  Serialize::singleton("script_one.py w/ command line arguments", "w")
 *
 *  Serialize::singleton().addWrite("key", "value")
 *  Serialize::singleton().delWrite("value");
 *
 *  usage 2 (changing the script):
 *
 *  Serialize::singleton("script_one1.py", "w")
 *
 *  Serialize::singleton().addWrite("key", "value")
 *  Serialize::singleton().delWrite("value");
 *
 *  Serialize::singleton("script_two.py", "w")
 *  Serialize::singleton().addWrite("key", "value")
 *  Serialize::singleton().delWrite("value")
 *
 *  usage 3:
 *
 *  Serialize::singleton("script3.py w/ command line arguments", "r")
 *  char* actualBytesRead =  Serialize::singleton().read(readBuffer, numBytesToRead)
 *
 */



class SerializeFriend { };

class Serialize
{
    private:

    /**
     * name of the script file. Used in popen
     */
    static char*       myScriptFileName;
    /**
     *  readwritemode == "w" or "r" only. Used in popen
     */
    static char*       myReadWriteMode;
    /**
     */

    /**
     * fileptr returned by popen
     */
    static FILE*       myFilePtr;


    static Serialize*  self;

    /**
     */
    Serialize();

    /** to prevent compiler warning for having private destructor
     */
    friend class SerializeFriend;
    /**
     */
    virtual ~Serialize() {};
    /**
     */
    Serialize( Serialize &){};
    /**
     */
    Serialize& operator=( Serialize &);
    /**
     */
    bool validFile() const;

    /**
     */
    static void  doCpLog();

    public:

    /**
     */
    static Serialize& singleton( const char* scriptFileName = 0, const char* readWriteMode = "w" );

    /**
     */
    void addWrite( const Data& key, const Data& value );
    /**
     */
    void delWrite( const Data& key );
    /**
     */
    char* read(char* readBuffer, int numBytesToRead);

    /**
     */
    static void delSingleton();
};


} // namespace Transport
} // namespace Assist




#endif // !defined(SERIALIZE_HXX_)

//End of File
