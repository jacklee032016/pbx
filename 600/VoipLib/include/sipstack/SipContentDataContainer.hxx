#ifndef SipContentDataContainer_HXX
#define SipContentDataContainer_HXX
/*
 * $Id: SipContentDataContainer.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Data.hxx"
#include "SipContentType.hxx"
#include "SipContentLength.hxx"
#include "SipContentDisposition.hxx"
#include "SipContentData.hxx"
#include <vector>

namespace Vocal
{

/// data container for ContentLength header
class SipContentDataContainer
{
    public:
        /// Create one with default values
        SipContentDataContainer();
        ///
        void encode(Data*) const;
        ///
        void decode(const Data&, Sptr<SipContentLength>, 
		    Sptr<SipContentType>, 
		    Sptr<SipContentDisposition> );
        ///
        virtual ~SipContentDataContainer();
        ///
        SipContentDataContainer( const SipContentDataContainer& rhs );
        ///
        SipContentDataContainer& operator=( const SipContentDataContainer& rhs );
        ///
        void setContentLength(Sptr <SipContentLength>);
        ///
        Sptr <SipContentLength>  getContentLength();
        /// 
        void setContentType(Sptr <SipContentType>);
        ///
        Sptr <SipContentType> getContentType();
        ///
        void setContentDisposition(Sptr <SipContentDisposition>);
        ///
        Sptr <SipContentDisposition> getContentDisposition();
        ///
        void setContentData(Sptr<SipContentData> contData, int index=-1);
        ///
        Sptr <SipContentData> getContentData(int index=-1);
        ///
        void removeContentData(int index=-1);
        ///
        int getNumContentData();
        ///
        void setNumContentData(int index);
        ///
        bool operator==(const SipContentDataContainer& );
        /// force all headers to be parsed
        void forceParse();
    
    private:

        void copyRhs2This( const SipContentDataContainer& rhs );

        Sptr <SipContentType>  contentType;
        Sptr <SipContentLength> contentLength;
        Sptr <SipContentDisposition> contentDisposition;
    
    
        ///
        vector< Sptr <SipContentData> > mimeDataList;
   
        bool parse() ;
    
        bool parseMultiMime(Data boundary);
    
        bool parseSingleMime(Data singleMimeData, Sptr<SipContentLength> len, 
			     Sptr<SipContentType> type, 
			     Sptr<SipContentDisposition> disp);

        Data mimeData;
        bool isParsed;
        Data actualBoundary;
};
 
} // namespace Vocal

#endif
