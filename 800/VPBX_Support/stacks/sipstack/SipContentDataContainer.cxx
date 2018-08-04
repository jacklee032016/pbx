/*
* $Log: SipContentDataContainer.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipContentDataContainer.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include "SipContentDataContainer.hxx"
#include "SipOsp.hxx"
#include "SipIsup.hxx"
#include "SipSdp.hxx"
#include "SipHeader.hxx"
#include "SipUnknownContentData.hxx"
#include "SipContentData.hxx"

using namespace Assist;

SipContentDataContainer::SipContentDataContainer()
        : contentType(new SipContentType),
	  contentLength(new SipContentLength),
	  contentDisposition(new SipContentDisposition),
	  mimeDataList(),
	  mimeData(),
	  isParsed(false),
	  actualBoundary()
{
    assert(mimeDataList.begin() == mimeDataList.end());
}


SipContentDataContainer::~SipContentDataContainer()
{
}


SipContentDataContainer::SipContentDataContainer( const SipContentDataContainer& rhs )
{
    copyRhs2This( rhs );
}


SipContentDataContainer&
SipContentDataContainer::operator=( const SipContentDataContainer& rhs )
{
    if ( &rhs != this )
    {
        copyRhs2This( rhs );
    }
    return *this;
}


void
SipContentDataContainer::copyRhs2This( const SipContentDataContainer& rhs )
{
    if( rhs.contentType != 0 )
    {
        contentType = new SipContentType( *rhs.contentType );
    }
    if( rhs.contentLength != 0 )
    {
        contentLength = new SipContentLength( *rhs.contentLength );
    }
    if( rhs.contentDisposition != 0 )
    {
        contentDisposition = new SipContentDisposition( *rhs.contentDisposition );
    }

    cpDebug(LOG_DEBUG_STACK, "rhs mimeDataList size is %d",
                            rhs.mimeDataList.size() );
    mimeDataList.clear();
    for( vector< Sptr <SipContentData> >::const_iterator i = rhs.mimeDataList.begin();
         i != rhs.mimeDataList.end();
         ++i )
    {
        //TODO?: assert( *i != 0 ); and remove else part
        if( 0 != i->getPtr() )
        {
            mimeDataList.push_back( (*i)->duplicate() );
        }
        else
        {
            cpLog( LOG_ERR, "Null entry in mimeDataList" );
            // mimeDataList.push_back( 0 );
        }
    }

    mimeData = rhs.mimeData;
    isParsed = rhs.isParsed;
    actualBoundary = rhs.actualBoundary;
}


void
SipContentDataContainer::encode(Data* data) const
{
    
    Data tempData;
    
    if ( (isParsed) || (mimeDataList.size() > 0) )
    {
	Data parsedData;

	// call encode of SipContentData list
	vector< Sptr <SipContentData> >::const_iterator iter; 
    
	int len = 0;
    
	int numOfContentData = mimeDataList.size();
	if (numOfContentData > 1)
	{
	    iter = mimeDataList.begin();
 
	    int singleMimeLen;
    
	    while (iter != mimeDataList.end())
	    {
		singleMimeLen = 0;
    
		if  ((*iter) != 0)
		{
		    //add the boundary.
		    parsedData+= "--";
                    parsedData+= actualBoundary;
		    parsedData+= CRLF;
		    //this is a SipContentData item. call encode functions of SipContentData.
		    parsedData+= (*iter)->encodeHeaders();
		    parsedData+= CRLF;
		    parsedData+= (*iter)->encodeBody(singleMimeLen);
    
		    len+= singleMimeLen;
		    
		}
		++iter;
                                            
	    }
	    if (mimeDataList.size() > 1)
	    {
		//end with the boundary.
		
		parsedData+= "--";
		parsedData+= actualBoundary;
		parsedData+= CRLF;
	    }
	}
	else
	{
	    if(!mimeDataList.empty() && (mimeDataList[0] != 0))
	    {
		parsedData+= mimeDataList[0]->encodeBody(len);
	    }
	}
	tempData = parsedData;
	assert(contentLength != 0);

	//this needs to be called by the SipMsg, while encoding content length.
	contentLength->setLength(len); 
    }
    else
    {
	tempData = mimeData;
    }
    if (tempData.length())
    {
	//there is mime defined here.
	(*data) += CRLF;
	(*data) += tempData;
    }
    else
    {
	(*data) += CRLF;
    }
}
   
    
void
SipContentDataContainer::decode(const Data& data, 
				Sptr <SipContentLength> contLength, 
				Sptr <SipContentType> contType, 
				Sptr <SipContentDisposition> contDisp)
{
    contentLength = contLength;
    contentType = contType;
    contentDisposition = contDisp;
    
    //maintain string here.
    mimeData = data;
}
    
    
bool
SipContentDataContainer::parse() 
{
    bool success = false;
    //construct the SipContentData objects here.
    // xxx fixme
    if (contentType != 0)
    {
        // this is a multipart MIME (RFC 2046)
	if(contentType->getType() == "multipart") 
	{
	    //this is a multi-MIME.
	    Data boundary = contentType->getAttributeValue("boundary");
	    if (boundary.length())
	    {
		actualBoundary = "--";
		actualBoundary += boundary;

		success = parseMultiMime(actualBoundary);
	    }
	}
	else
	{
	    success = parseSingleMime(mimeData, 
                                      contentLength, 
                                      contentType, 
                                      contentDisposition);
	}
    }

    if(!success)
    {
	cpDebug(LOG_DEBUG, "failed to parse MIME data when asked to");
    }

    isParsed = true;
    
    return success;
}
    
    
bool
SipContentDataContainer::parseSingleMime(Data singleMimeData, 
					 Sptr<SipContentLength> len, 
					 Sptr<SipContentType> type, 
					 Sptr<SipContentDisposition> disp)
{
    cpDebug(LOG_DEBUG_STACK, "parseSingleMime %s", singleMimeData.logData() );
    Data subType;
    //get contentType. Defaults to text/plain.
    if (type != 0)
    {
	subType = type->getSubType();
    }
    else
    {
	subType = "plain";
    }
    Sptr <SipContentData> contentData;
    
    //parse singleMimeData, and add into ContentData object.
    if ( isEqualNoCase(subType, "SDP") )
    {
	contentData  = new SipSdp(singleMimeData);
    }
    else if (isEqualNoCase(subType, "ISUP") )
    {
	//this is ISUP
	contentData = new SipIsup(singleMimeData);
    }
    else if (isEqualNoCase(subType, "OSP") )
    {
	//this is OSP token
	contentData = new SipOsp(singleMimeData);
    }
    else if (singleMimeData.length() > 0) // if length is 0, ignore
    {
        contentData = new SipUnknownContentData(singleMimeData);
    }

    if(contentData != 0)
    {
	if (len != 0)
	{
	    contentData->setContentLength(len);
	}
	if (type != 0)
	{
	    contentData->setContentType(type);
	}
	if (disp != 0)
	{
	    contentData->setContentDisposition(disp);
	}
	//set this into the map.

	mimeDataList.push_back(contentData);
    }
    else
    {
	// xxx this is not an error if there is a null-bodied object
	// passed here ?
    }
    
    return true;  //need to find out error condition here.
}
    
    
bool
SipContentDataContainer::parseMultiMime(Data boundary)
{
    bool success = true;
    
    bool matchFail = true;
    Data mimeBoundary = mimeData.getLine(&matchFail);
    if ( (mimeBoundary.operator!=(boundary)) || ( matchFail ) )
    {
	success = false;
	return success;
    }
    else  //continue
    {
	while (mimeData.length())
        {
	    //get the string between this and the next boundary line.
	    Data singleMimeData;
            
	    //form the line that we need to search for.
	    Data boundaryLine = boundary;
	    boundaryLine+= CRLF;
            LocalScopeAllocator lo; 
	    int found = mimeData.match(boundaryLine.getData(lo), &singleMimeData, true);
    
	    if ( (found == NOT_FOUND) || (found == FIRST) )
	    {
                // this is the epilogue boundary, which is mandatory
		success = false; 
		break;  //break out of while loop.
	    }
	    // else continue
	    {
	        //get headers if any in the MIME section
		bool matchFailHeaders = true;
    
		Data nextLine = singleMimeData.getLine(&matchFailHeaders);
    
		Sptr <SipContentType> singleMimeContentType;
		Sptr <SipContentLength> singleMimeContentLength;
		Sptr <SipContentDisposition> singleMimeContentDisposition;
		bool dummyFlag;
    
		while  ( (nextLine.length()) && (!matchFailHeaders) )
		{
		    
		    Data headerValue = nextLine;
		    // decode this line of the message
		    Data headerName = headerValue.parse(": \t", &dummyFlag);
		    //we dont check for match failing, since these fields are not mandatory here.
		    headerName.lowercase();
		    
		    SipHeaderType  headerType = headerTypeDecode(headerName);

		    switch(headerType)
		    {
			case SIP_CONTENT_TYPE_HDR:
			{
			    singleMimeContentType 
                                = new SipContentType(headerValue);
			}
			break;
			case SIP_CONTENT_LENGTH_HDR:
			{
			    singleMimeContentLength 
                                = new SipContentLength(headerValue);
			}
			break;
			case SIP_CONTENT_DISPOSITION_HDR:
			{
			    singleMimeContentDisposition 
                                = new SipContentDisposition(headerValue);
			}
			break;
			default:
			{
			}
			break;
		    } //end switch
		    //get the next line.
                    nextLine = singleMimeData.getLine(&matchFailHeaders);
		} //end while headers found
		//CRLF found now.
		//the remaining is content data.
		success 
                    = success && parseSingleMime(singleMimeData, 
                                                 singleMimeContentLength, 
                                                 singleMimeContentType, 
                                                 singleMimeContentDisposition);
	    }//else
	}// while mimeData.length()
	//parse based on boundary,
        //if you get contentlength, type, or disposition,
        //set in SipCOntentData.
    }//else found boundary.
    return success;
}
   

int
SipContentDataContainer::getNumContentData() 
{
    if (!isParsed)
    {
        //parse here
	parse();
    }
    return static_cast<int>(mimeDataList.size());
}


void
SipContentDataContainer::setNumContentData(int index)
{
    // precondition
    assert(index >= 0);

    if (!isParsed)
    {
        parse();
    }

    while(index < static_cast<int>(mimeDataList.size()) )
    {
        // reduce the number by trimming from the end
        mimeDataList.pop_back();
    }

    // postcondition
    assert(index >= static_cast<int>(mimeDataList.size()) );
}


Sptr <SipContentData>
SipContentDataContainer::getContentData(int index)
{
    if (!isParsed)
    {
        //parse here
	parse();
    }
    
    //return from list
    if (index >=  static_cast<int>(mimeDataList.size() ) || 
        (mimeDataList.size() == 0))
    {
	return 0;
    }
    else if (index < 0)
    {
	return mimeDataList[mimeDataList.size()-1];
    }
    else
    {
	return mimeDataList[index];
    }
}
    

// xxx there are two known bugs in this function:

// 1.  from an API perspective, this should have more consistent
// behavior if you pass an index that's out of range.  Either 

//     (a) you need to know what the range is, and you should assert
//     if you try to set something out of range

//     (b) you can use a big index, and it will either expand w/ null
//     items or append

// either is better than the current behavior (silently fail, unless
// the index is 0, in which case insert).

// 2.  the boundaries are not uniquely generated.  they need to be to
// work correctly.



void
SipContentDataContainer::setContentData(Sptr<SipContentData> contData, 
					int index)
{
    assert( contData != 0 );
    cpDebug(LOG_DEBUG_STACK, "setContentData %d: %s",
                            index,
                            contData->encodeHeaders().logData() );
    int bodyLength;
    cpDebug(LOG_DEBUG_STACK, "\n%s",
                            contData->encodeBody(bodyLength).logData() );

    cpDebug(LOG_DEBUG_STACK, "mimeDataList size before parse is %d", mimeDataList.size() );

    //if the existing mimeData is not parsed yet, parse now.
    if  (!isParsed)
    {
	parse();
    }
    //set at index.
    int size = mimeDataList.size();
    
    cpDebug(LOG_DEBUG_STACK, "mimeDataList size after parse is %d", size );
    if ( (index >= -1) && (index <= size) )
    {
        if (index == -1)
        {
            mimeDataList.push_back(contData);
        }
        else if ( index <= size )
        {
            vector< Sptr <SipContentData> >::iterator iter;
	    
            iter = mimeDataList.begin();
	    
            iter+=index;
            //insert it at index.
            mimeDataList.insert(iter, contData);      
	}
    }
    if ( (mimeDataList.size() == 2) && ( !actualBoundary.length()) ) //more than one item in mime list
    {
	//give a boundary if none exists before.
	actualBoundary = "unique-boundary-1"; // xxx need to generate this some how.
    }
}
    
    
void
SipContentDataContainer::removeContentData(int index)
{
    if (!isParsed)
    {
	parse();
    }
    if (index == -1)
    {
	mimeDataList.pop_back();
    }
    else if ( index <= static_cast<int>(mimeDataList.size())  && (index > -1) )
    {
	vector<Sptr <SipContentData> >::iterator iter;
 
        iter = mimeDataList.begin();
        iter+=index;
        if(iter != mimeDataList.end())
        {
            mimeDataList.erase(iter);
        }
    }

    if (mimeDataList.size() == 0)
    {          
	mimeDataList.clear();
    }

    if (mimeDataList.size() ==1)
    {
	//set boundary to ""
	actualBoundary = "";
    }
}         

    
Sptr <SipContentType>
SipContentDataContainer::getContentType()
{
    if (isParsed)
    {
        if(actualBoundary.length())
        {
            contentType->setTokenDetails("boundary", actualBoundary);
        }
    
	if ( (actualBoundary.length()) && 
	     (contentType->getType() != "multipart"))
	{
	    //set the content type.
	    contentType->setType("multipart");
	    contentType->setSubType("mixed");
	   
	}
#if 0
	else if ( (!actualBoundary.length()) &&
		  (contentType->getType() != "multipart"))
#endif
	else if ( (!actualBoundary.length()) )
	{
	    //get the type of object.
            if(mimeDataList.empty())
            {
                // there is nothing here -- so there is no contenttype
                contentType->setType("");
                contentType->setSubType("");
            }
            else
            {
                contentType->setType(mimeDataList[0]->getContentType()->getType());
                contentType->setSubType(mimeDataList[0]->getContentType()->getSubType());
            }
            contentType->clearToken("boundary");
	}
    }
    return contentType;
}

    
void SipContentDataContainer::setContentType(Sptr <SipContentType> content)
{
    contentType = content;
}

    
Sptr <SipContentLength> SipContentDataContainer::getContentLength()
{
    return contentLength;
}

    
void SipContentDataContainer::setContentLength(Sptr <SipContentLength>  content)
{
    contentLength = content;
}

    
void 
SipContentDataContainer::setContentDisposition(Sptr <SipContentDisposition> disp)
{
    contentDisposition = disp;
}
    
    
Sptr <SipContentDisposition> 
SipContentDataContainer::getContentDisposition()
{
    return contentDisposition;
}
    
bool
SipContentDataContainer::operator ==(const SipContentDataContainer& src)
{
    if (  (!src.isParsed) && (!isParsed) )
    {
	return (src.mimeData == mimeData);
    }
    else if ( ( src.isParsed ) && (!isParsed) )
    {
	//encode src
	Data srcEncodedBody;
	src.encode(&srcEncodedBody);
    
	//should we maintain a flag again, to say that the mimeData is 
	//here

	return (srcEncodedBody == mimeData);
    }
    else if ( (!src.isParsed) && (isParsed) )
    {
	//encode this.
	Data thisEncodedBody;
	encode(&thisEncodedBody);
    
	return (src.mimeData == thisEncodedBody);
    }
    else //both are parsed, run through list and compare.
    {
        SipContentDataContainer cData(src);
	int srcContentDatasize = cData.getNumContentData();
 
	if ( srcContentDatasize != getNumContentData())
	{
	    return false;
	}
	else
	{
	    //encode both and compare.
	    //optionally, we could also go over each in the list and
	    //call the == operator on that.
    
	    //encode this.
	    Data thisEncodedBody;
	    encode(&thisEncodedBody);
    
	    //encode src
	    Data srcEncodedBody;
	    src.encode(&srcEncodedBody);
    
	    return (thisEncodedBody == srcEncodedBody);
	}
    }         
}


void SipContentDataContainer::forceParse()
{
    parse();
}

