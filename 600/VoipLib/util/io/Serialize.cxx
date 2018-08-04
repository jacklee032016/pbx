/*
 * $Id: Serialize.cxx,v 1.3 2007/03/02 06:25:54 lizhijie Exp $
 */


#include <iostream.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <cassert>

#include "Serialize.hxx"
#include "cpLog.h"


using namespace Vocal::IO;


char* Serialize::myScriptFileName = 0;
char* Serialize::myReadWriteMode = 0;
FILE* Serialize::myFilePtr = 0;
Serialize* Serialize::self = 0;


Serialize::Serialize()
{
    myScriptFileName = 0;
    myReadWriteMode = 0;
    myFilePtr = 0;
    self = 0;
}


Serialize& 
Serialize::singleton(const char* scriptFileName, const char* readWriteMode)
{
    Serialize::doCpLog();

    if (self != 0)          // singleton already created
    {
        if (scriptFileName == 0)  // reusing the same singleton
        {
             return *self; 
        }
        else
        {
            delSingleton();     // releasing the existing singleton
        }
    }

    // databasefilename and readwritemode must be present
    
    assert ( scriptFileName != 0);
    assert ( ( strncmp(readWriteMode, "w", 1) == 0  ) || ( strncmp( readWriteMode, "r", 1) == 0) );

    self = new Serialize();
    
    myScriptFileName = new char[strlen(scriptFileName)+1];
    strncpy( myScriptFileName, scriptFileName, strlen(scriptFileName)+1);

    myReadWriteMode = new char[strlen(readWriteMode)+1];
    strncpy( myReadWriteMode, readWriteMode, strlen(readWriteMode)+1);

    myFilePtr = popen(scriptFileName, readWriteMode);
    
    if (myFilePtr == 0)
        cpLog(LOG_ERR, "popen failed on File: %s .  Mode: %s\n", scriptFileName, readWriteMode);
    else
        cpLog(LOG_ERR, "popen successful on File: %s .  Mode: %s\n", scriptFileName, readWriteMode);

    assert(myFilePtr);


    Serialize::doCpLog();

    return *self;

}

    
void
Serialize::doCpLog() 
{
    cpDebug(LOG_DEBUG, "MyScript: %s . MyReadWriteMode: %s . MyFilePtr: %XX .\n",
			 (myScriptFileName ? myScriptFileName : "InvalidFileName"), 
			   (myReadWriteMode ? myReadWriteMode : "InvalidMode"), myFilePtr);

}
    

void Serialize::delSingleton()
{
	Serialize::doCpLog();

	if (self->validFile())
	{
#ifndef __AS_RELEASE__
		int retClose =  pclose(myFilePtr);
#else
		pclose(myFilePtr);
#endif
		cpDebug(LOG_DEBUG, "Closing File: %s .  pclose() ret: %d\n", myScriptFileName, retClose);
	}

	delete [] myScriptFileName;
	delete [] myReadWriteMode;
	delete self;

	myScriptFileName = 0;
	myReadWriteMode = 0;
	myFilePtr = 0;
	self = 0;

	Serialize::doCpLog();

}

bool
Serialize::validFile() const
{
    Serialize::doCpLog();

    if (myScriptFileName == 0)
    {
        cpDebug(LOG_DEBUG, "Invalid File Name");
        return false;
    }

    if (myFilePtr <= 0)
    {
        cpDebug(LOG_DEBUG, "File: %s Not opened or error in opening\n", (strlen(myScriptFileName) ? myScriptFileName : "InvalidFileName") );
        return false;
    }

    if (feof(myFilePtr))
    {
        cpDebug(LOG_DEBUG, "File: %s EOF\n", myScriptFileName);
        return false;
    }

    return true;
}

void 
Serialize::addWrite(const Data& key, const Data& value)
{

    if (!validFile()) return;

    cpDebug(LOG_DEBUG, "FileName: %s .  Adding... Key: %s . Value: %s .\n", myScriptFileName, key.logData(), value.logData());
  
 
    int valueLen = value.length();
    int cmdLen = strlen("ADD");
    int keyLen = key.length();
  
    char* temp = new char[valueLen+cmdLen+keyLen+3];

    memset (temp, 0, (valueLen+keyLen+cmdLen+3)); 
    strncat (temp, value.logData(), valueLen);
    strncat (temp, "\n",1);
    strncat (temp, "ADD" , cmdLen);
    strncat (temp, " " , 1);
    strncat (temp, key.logData(), keyLen);

    fprintf (myFilePtr, "%s\n", temp);
    fflush(myFilePtr);

    cpDebug(LOG_DEBUG, "Wrote(add): %s\n", temp);
    delete [] temp;
}

void
Serialize::delWrite(const Data& key)
{
   if (!validFile()) return ;

    cpDebug(LOG_DEBUG, "FileName: %s .  Deleting... Key: %s .\n", myScriptFileName, key.logData());

    int keyLen = key.length();
    int cmdLen = strlen("DEL");

    char* temp = new char[keyLen+cmdLen+2];
    memset (temp, 0, (keyLen+cmdLen+2)); 

    strncat (temp, "DEL", cmdLen);
    strncat (temp, " ", 1);
    strncat (temp, key.logData(), keyLen);

    fprintf (myFilePtr, "%s\n",temp);
    fflush(myFilePtr);

    cpDebug(LOG_DEBUG, "Wrote(del): %s .\n", temp);
    delete [] temp;
}


char*
Serialize::read(char* readBuffer, int numBytesToRead)
{
    if (!validFile()) return 0;

    assert (readBuffer);
    assert (numBytesToRead);
    
    
    char* readChar = fgets(readBuffer,numBytesToRead, myFilePtr);

    cpDebug(LOG_DEBUG, "Read: %s .\n", readBuffer);

    return readChar;
}

//End of File
