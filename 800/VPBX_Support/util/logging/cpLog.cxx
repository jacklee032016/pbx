/*
* $Id: cpLog.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <syslog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <cassert>

#include <string>
#include <map>

#include "cpLog.h"
#include "global.h"
#include "Mutex.hxx"
#include "VThread.hxx"
#include "Lock.hxx"
#include "support.hxx"
#if 0
	#include "../../sip/gua/channel.h"
	#include "../../sip/gua/LinAudioDevice.hxx"
#endif

using Assist::Threads::Mutex;
using Assist::Threads::Lock;
#if 0
 class LinAudioDevice;
#endif 
/* String sizes */
#define DATEBUF_SIZE 256
#define FILEBUF_SIZE 256
#define LOG_FNAME_MAX_SIZE 256

/* The trailing 0 is a nil pointer, for use by code that iterates over
the array and needs to know where to stop. */
static const char* priName[] =
    {
        "EMERG",
        "ALERT",
        "CRIT",
        "ERR",
        "WARNING",
        "NOTICE",
        "INFO",
        "DEBUG",
        "DEBUG_STACK",
        "DEBUG_OPER",
        "DEBUG_HB",
        0
    };

/* The trailing 0 is a nil pointer, for use by code that iterates over
the array and needs to know where to stop. */
static const char* priNameShort[] =
    {
        "EMRG",
        "ALRT",
        "CRIT",
        "ERR ",
        "WARN",
        "NOTC",
        "INFO",
        "DBUG",
        "DSTK",
        "DOP ",
        "DHB ",
        0
    };

static char cpLogFilename[LOG_FNAME_MAX_SIZE + 1] = "";
static FILE* cpLogFd = stderr;
static int numberOfBackupFilesToKeep = 6;

/**
The maximum size for a single log file.  After the file reaches this
size, it will automatically be archived by the file-rotation algorithm.
@see rotateFiles
*/
static int SIZE_PER_LOGFILE=6291456; /* in bytes, i.e. six megabytes */

/* 1 if we're using syslog, 0 if we're not */
int usingSyslog = 0;

int openTheLogFile();
void handleCriticalError (char* fmt, ...);
inline void rotateFilesIfNecessary();
void rotateFiles();

static Mutex cpLogMutex;

class CpLogPriority
{
    public:
        static int getPriority();
        static void setPriority(int pri);
        static void setPriorityThread(vthread_t threadId, int pri);
        static void clearPriorityThread(vthread_t threadId);

        static const char* getLabel();
        static void setLabel(const char* label);
        static void setLabelThread(vthread_t threadId, const char* label);
        static void clearLabelThread(vthread_t threadId);

    protected:
        CpLogPriority();

    private:
        static Mutex logMutex;
        static CpLogPriority* getInstance();
        static CpLogPriority* instance_;

        int logPriority;
        map < vthread_t, int > logPriorityMap;
        map < vthread_t, string > logLabelMap;
        string logLabel;

        /* This is a singleton class, so it makes no sense for it
        to have an assignment operator or a copy constructor.  Keep the
        compiler from auto-generating by declaring them as private
        methods, and then refusing to define them. */
        CpLogPriority (const CpLogPriority& x);
        CpLogPriority& operator= (const CpLogPriority& x);
};


CpLogPriority* CpLogPriority::instance_ = 0;
Mutex CpLogPriority::logMutex;

CpLogPriority::CpLogPriority()
        : logPriority(LOG_ERR),
        logLabel("")
{}



CpLogPriority* CpLogPriority::getInstance()
{
    if (!instance_)
    {
        instance_ = new CpLogPriority;
    }
    return instance_;
}


int CpLogPriority::getPriority()
{
    map < vthread_t, int > ::iterator i;

    Lock lock(logMutex);
    i = getInstance()->logPriorityMap.find(VThread::selfId());

    if (i != getInstance()->logPriorityMap.end())
    {
        // found it!
        return i->second;
    }
    else
    {
        // not found, use default
        return getInstance()->logPriority;
    }
}


void
CpLogPriority::setPriority(int pri)
{
    assert (pri >= 0 && pri <= LAST_PRIORITY);
    Lock lock(logMutex);
    getInstance()->logPriority = pri;
}


void
CpLogPriority::setPriorityThread(vthread_t threadId, int pri)
{
    assert (pri >= 0 && pri <= LAST_PRIORITY);
    Lock lock(logMutex);
    getInstance()->logPriorityMap[threadId] = pri;
}


void
CpLogPriority::clearPriorityThread(vthread_t threadId)
{
    map < vthread_t, int > ::iterator i;

    Lock lock(logMutex);
    i = getInstance()->logPriorityMap.find(threadId);

    if (i != getInstance()->logPriorityMap.end())
    {
        getInstance()->logPriorityMap.erase(i);
    }
}


const char* CpLogPriority::getLabel()
{
    map < vthread_t, string > ::iterator i;

    Lock lock(logMutex);
    i = getInstance()->logLabelMap.find(VThread::selfId());

    if (i != getInstance()->logLabelMap.end())
    {
        // found it!
        return i->second.c_str();
    }
    else
    {
        // not found, use default
        return getInstance()->logLabel.c_str();
    }
}


void
CpLogPriority::setLabel(const char* label)
{
    Lock lock(logMutex);
    getInstance()->logLabel = label;
}


void
CpLogPriority::setLabelThread(vthread_t threadId, const char* label)
{
    Lock lock(logMutex);
    getInstance()->logLabelMap[threadId] = label;
}


void
CpLogPriority::clearLabelThread(vthread_t threadId)
{
    map < vthread_t, string > ::iterator i;

    Lock lock(logMutex);
    i = getInstance()->logLabelMap.find(threadId);

    if (i != getInstance()->logLabelMap.end())
    {
        getInstance()->logLabelMap.erase(i);
    }
}


void cpLogOpenSyslog()
{
    #if !defined(WIN32)
    /* Use LOG_LOCAL7, which is reserved for non-system programs, to avoid
    gratuitously intermingling our logs with system logs.  LOG_CONS means
    to print messages to the system console if there is some error in sending
    them to syslog.  LOG_NDELAY means we will try to open syslog now, not
    after the first message is received. */
    openlog (CpLogPriority::getLabel(), LOG_CONS | LOG_NDELAY, LOG_LOCAL7);
    usingSyslog = 1;
    #endif // !defined(WIN32)
}


int
cpLogOpen (const char* filename)
{
    assert (strlen (filename) <= FILEBUF_SIZE);
    strncpy (cpLogFilename, filename, FILEBUF_SIZE);
    usingSyslog = 0;
    return openTheLogFile();
}


int
openTheLogFile()
{
    if (!(cpLogFd = fopen (cpLogFilename, "a+")))
    {
        handleCriticalError ("cpLog: Cannot open log file %s:  %s", cpLogFilename, strerror(errno));
        return 0;
    }
    return 1;
}


#ifndef __GNUC__
#ifdef cpLog
#undef cpLog
#endif
void
cpLog(int pri, const char* fmt, ...)
{
    if (pri < 0)
        return ;

    va_list ap;

    if (pri <= CpLogPriority::getPriority())
    {
        Lock lock(cpLogMutex);
        va_start(ap, fmt);
        vCpLog(pri, "", 0, fmt, ap);
        va_end(ap);
    }
}  /* cpLog  */

/////////////////
// ThienChange
/////////////////

void cpLog_old_version(int pri, const char* fmt, ...)
{
    if (pri < 0)
        return ;

    va_list ap;

    if (pri <= CpLogPriority::getPriority())
    {
        Lock lock(cpLogMutex);
        va_start(ap, fmt);
        vCpLog(pri, "", 0, fmt, ap);
        va_end(ap);
    }
}  /* cpLog  */

WrapLog::WrapLog(const char *fname, int lineno)
        : m_fname(fname), m_lineno(lineno) 
{
}

void WrapLog::operator() (int pri, const char *fmt, ...)
{
    if (pri < 0)
        return ;

    va_list ap;

    if (pri <= CpLogPriority::getPriority())
    {
	Lock lock(cpLogMutex);
        va_start(ap, fmt);
        vCpLog(pri, m_fname, m_lineno, fmt, ap);
        va_end(ap);
    }
}


#endif

void cpLog_impl_(int pri, const char* file, int line, const char* fmt, ...)
{
    va_list ap;

    if (pri <= CpLogPriority::getPriority())
    {
        // We need to protect access to vCpLog. The race we are really 
        // worried about is not the multiple writes to the same file
        // descriptor, since the underlying system libraries protect
        // against that. We are more worried about the race caused by file
        // rotation. If we aren't locked here, we could possible close the
        // cpLogFd during file rotation, then try to write to that file
        // from another thread. This is something that the underlying
        // system libraries cannot protect against.
        //
	Lock lock(cpLogMutex);
        va_start(ap, fmt);
        vCpLog(pri, file, line, fmt, ap);
        va_end(ap);
    }
}


void
vCpLog(int pri, const char* file, int line, const char* fmt, va_list ap)
{
    assert (pri >= 0 && pri <= LAST_PRIORITY);

    char datebuf [DATEBUF_SIZE];
    int datebufCharsRemaining;

    struct timeval tv;
    int result = gettimeofday (&tv, NULL);
    
    if (result == -1)
    {
        /* If we can't get the time of day, don't print a timestamp.
        (Under Unix, this will never happen:  gettimeofday can fail only
        if the timezone is invalid [which it can't be, since it is
        uninitialized] or if &tv or &tz are invalid pointers.) */

        datebuf [0] = '\0';
    }
    else
    {
        /* The tv_sec field represents the number of seconds passed since
        the Epoch, which is exactly the argument gettimeofday needs. */
        const time_t timeInSeconds = (time_t) tv.tv_sec;
        struct tm localT;
        strftime (datebuf,
                  DATEBUF_SIZE,
                  "%Y%m%d-%H%M%S", /* guaranteed to fit in 256 chars,
                                      hence don't check return code */
#if defined( __APPLE__ )
                  localtime (&timeInSeconds));
#else
                  localtime_r (&timeInSeconds, &localT));
#endif
    }

    char msbuf[5];
    /* Dividing (without remainder) by 1000 rounds the microseconds
    measure to the nearest millisecond. */
    sprintf(msbuf, ".%3.3ld", (tv.tv_usec / 1000));

    datebufCharsRemaining = DATEBUF_SIZE - strlen (datebuf);
    strncat (datebuf, msbuf, datebufCharsRemaining - 1);
    datebuf[DATEBUF_SIZE - 1] = '\0'; /* Just in case strncat truncated msbuf,
                                     thereby leaving its last character at
                                     the end, instead of a null terminator */

    if (usingSyslog)
    {
        #if !defined(WIN32)
        /* syslog does not recognize priorities conceptually lower (numerically
        greater) than LOG_DEBUG.  If our current priority is lower, "promote"
        it to LOG_DEBUG. */
        int coercedPriority = (pri <= LOG_DEBUG) ? pri : LOG_DEBUG;

	char buf[4097];

	vsnprintf(buf, 4096, fmt, ap);

        syslog (coercedPriority,
                "%s [%5.5ld] %s %s: %s:%d %s\n",
                datebuf,
                VThread::selfId(),
                priNameShort[pri],
                CpLogPriority::getLabel(),
                file,
                line,
		buf);
        #else
	printf("syslog not implemented for win32");
	assert(0);
        #endif // !defined(WIN32)
    }
    else
    {
        fprintf (cpLogFd,
                 "%s [%5.5ld] %s %s: %s:%d ",
                 datebuf,
                 VThread::selfId(),
                 priNameShort[pri],
                 CpLogPriority::getLabel(),
                 file,
                 line);
        vfprintf (cpLogFd, fmt, ap);
        fprintf (cpLogFd, "\n");
        fflush(cpLogFd);

        /* in case we just pushed the current file past the size limit... */
        rotateFilesIfNecessary();
    }
}


/* Handle a critical error in cpLog itself (an error so bad, by definition,
it prevents logging in the normal way).  Do this by reverting to using
standard error as the log "file" and immediately printing a warning about
the situation.*/
void
handleCriticalError (char* fmt, ...)
{
    cpLogFd = stderr;
    strcpy (cpLogFilename, "");

    fprintf (cpLogFd, "\nCRITICAL LOGGING ERROR:\n");
    
    va_list ap;
    va_start (ap, fmt);
    vfprintf (cpLogFd, fmt, ap);
    va_end (ap);

    fprintf (cpLogFd, "\ncpLog has reverted to logging to standard error...\n\n");
}


inline
void
rotateFilesIfNecessary()
{
    /* If we are logging to standard error, there are no files to rotate. */
    if (cpLogFd == stderr)
    {
        return;
    }

    /* If we are logging to syslog, log rotation is somebody else's problem
    (SEP); the log file name is outside of our knowledge, and the file itself
    may be outside of our permissions */
    if (usingSyslog)
    {
        return;
    }

    /* Test to see if the present log file has exceeded the maximum
       size.  (If it has, rotate it.) */

    struct stat fileInfo;
    
    if (stat (cpLogFilename, &fileInfo))
    {
        /* What?  We can't see the log file? */
        handleCriticalError ("cpLog could not stat its own current log file, %s:  %s", cpLogFilename, strerror (errno));
            return;
    }
    
    if (fileInfo.st_size >= SIZE_PER_LOGFILE)
    {
        rotateFiles();
    }
}


/* Move the file names, cascading down, so that logfile.1 is renamed
to logfile.2, logfile.2 is renamed to logfile.3, et cetera.  logfile.6,
if it exists, will be overwritten. */
void
rotateFiles()
{
    /* First double-check the log file size, to avoid a race condition.
    It is possible that, between the time rotateFiles was called and
    the present moment, some other thread has attempted to log a message
    (using vCpLog), noticed that fileInfo.st_size +. SIZE_PER_LOGFILE
    (in rotateFilesIfNecessary), and rotated the logs out from under us.
    */

    struct stat fileInfo;
    if (stat (cpLogFilename, &fileInfo) != 0)
    {
        handleCriticalError ("rotateFiles could not stat the log file (%s)", cpLogFilename);
    }
    if (fileInfo.st_size < SIZE_PER_LOGFILE)
    {
        /* The race condition occurred; our files have already been
        moved for us. */
        return;
    }

    /* Close the current log file */
    if( fclose (cpLogFd) )
    {
        handleCriticalError ("Could not close the log file:  %s", strerror (errno));
    }

    /* Make room for the new log file */
    for( int i = numberOfBackupFilesToKeep - 1; i >= 0; i-- )
    {
        string oldFilename( cpLogFilename );
        if( i > 0 )
        {
            oldFilename += "." + itos( i );
        }
        const char* oldFilename_c_str = oldFilename.c_str();
        if (stat (oldFilename_c_str, &fileInfo) == 0) /* if the file _does_ exist... */
        {
            string newFilename( cpLogFilename );
            newFilename += "." + itos( i+1 );

            const char* newFilename_c_str = newFilename.c_str();
            if (rename (oldFilename_c_str, newFilename_c_str) != 0) /* If rename() fails... */
            {
                handleCriticalError ("cpLog could not rename %s to %s:  %s", oldFilename_c_str, newFilename_c_str, strerror (errno));
                return;
            }
        }
        /* The only reason the file should be un-stat-able is that it
        does not exist.  That is a legitimate condition, since rotation may
        not yet have created a file with that number (i).  Any other failure
        is an error. */
        else if (errno != ENOENT)
        {
            handleCriticalError ("cpLog could not stat %s:  %s", oldFilename_c_str, strerror (errno));
            return;
        }
    }

    /* Open the log file for writing once more.  (The current log file will
    always have the name stored in cpLogFilename, without a numeric extension.)*/
    openTheLogFile();
}


void
cpLogSetPriority (int pri)
{
    CpLogPriority::setPriority(pri);
}


int
cpLogGetPriority ()
{
    return CpLogPriority::getPriority();
}


void
cpLogSetPriorityThread (vthread_t thread_id, int pri)
{
    if (pri < 0)
        return ;
    CpLogPriority::setPriorityThread(thread_id, pri);
}


void
cpLogClearPriorityThread (vthread_t thread_id)
{
    CpLogPriority::clearPriorityThread(thread_id);
}


void
cpLogSetLabel (const char* label)
{
    CpLogPriority::setLabel(label);
}

void
cpLogSetLabelThread (vthread_t thread_id, const char* label)
{
    CpLogPriority::setLabelThread(thread_id, label);
}


void
cpLogClearLabelThread (vthread_t thread_id)
{
    CpLogPriority::clearLabelThread(thread_id);
}


void
cpLogShow (void)
{
    fprintf (stderr, "\tLabel    : %s\n", CpLogPriority::getLabel());
    fprintf (stderr, "\tPriority : %s\n", priName[CpLogPriority::getPriority()]);
    fprintf (stderr, "\tFile     : %s (cpLogFd = %d)\n", cpLogFilename, fileno(cpLogFd));
}


int
cpLogStrToPriority(const char* priority)
{
    string pri = priority;

    if (pri.find("LOG_", 0) == 0)
    {
        pri.erase(0, 4);
    }

    int i = 0;
    while (priName[i] != 0)
    {
        if (pri == priName[i])
        {
            return i;
        }
        i++;
    }
    return -1;  // invalid
}

void cpLogSetFileSize (const int size)
{
    SIZE_PER_LOGFILE = size;
}
 
void cpLogSetNumOfBackupFiles (const int num)
{
    numberOfBackupFilesToKeep = num;
}


const char*
cpLogPriorityToStr(int priority)
{
    int priorityCount = 0;
    while (priName[priorityCount] != 0)
    {
        priorityCount++;
    }

    if ((priority >= 0) && (priority < priorityCount))
    {
        return priName[priority];
    }
    else
    {
        return 0;
    }
}


/* this function is not called, it is just compiled to make sure that
   the cpLog Macro expansion works */
void
testCpLogMacroExpansion()
{
    if (true)
        cpLog(LOG_DEBUG, "this is a test");
    else
        cpLog(LOG_DEBUG, "and a second");
}
