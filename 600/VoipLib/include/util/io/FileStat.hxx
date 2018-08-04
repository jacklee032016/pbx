#ifndef FILESTAT_HXX_
#define FILESTAT_HXX_
/*
 * $Id: FileStat.hxx,v 1.3 2007/03/03 18:38:55 lizhijie Exp $
 */


#include <sys/stat.h>
#include "Data.hxx"

/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to Input/Output.<br><br>
 */
namespace IO
{


/** A simple stat operation wrapper
 */
class FileStat
{
    public:

	FileStat(const char* filename);
	FileStat(const Data& filename);
	virtual ~FileStat();

	/// redo the stat operation
	void reStat();

	time_t lastModified() const;

	bool isFile() const;

        bool statOK() const;

    protected:
	struct stat myStat;
	Data myFilename;
        bool isValid;

    private:
};


} // namespace Transport
} // namespace Vocal

#endif

