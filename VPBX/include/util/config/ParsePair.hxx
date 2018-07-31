#if !defined(ASSIST_PARSEPAIR_HXX)
#define ASSIST_PARSEPAIR_HXX
/*
* $Id: ParsePair.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "AssistCommon.hxx"
#include "NonCopyable.hxx"
#include "NameValue.hxx"
#if defined(ASSIST_USE_XML)
#ifdef WIN32
#include "libxml/parser.h"
#else
#include "parser.h"
#endif
#else
struct xmlDocPtr;
#endif // ASSIST_USE_XML
#include <string>
#include <iostream>


/** Vovida Open Communication Application Library.<br><br>
 */
namespace Assist 
{


/** 
 */
namespace Configuration
{


using std::string;
using std::istream;


class ParsePair : Assist::NonCopyable
{
    public:

        ParsePair();
        
        virtual ~ParsePair();
        
        void                    tagName(const string &);
        
        enum Type
        {
            CFG_FILE,
            CFG_STRING,
            CFG_XML_FILE,
            CFG_XML_STRING
        };
        
        /** If type is CFG_FILE, the file will be opened and parsed.
         *  If the file extension is ".xml", it will be parsed as an
         *  XML file. Otherwise is will be parse as name value pairs.
         *  The name and value must be on a single line. The first
         *  word is parsed as the name, and the rest as the value.
         *  You may have one or more separation characters between the
         *  name and value. The separation characters are ":=;,\\/ \t".
         *  You may also include single line comments in the file. A 
         *  comment starts with one of the comment characters "#!".
         *  A line with a name value pair may have a comment on following
         *  the value.<br><br>
         *
         *  If type is CFG_STRING, the string will be parse similar to
         *  a text file.<br><br>
         *
         *  If type is CFG_XML_FILE, the file will be opened and parsed
         *  as an XML file. The general format is:
         *  <pre>
         *  &lt;myTagName&rt;
         *      &lt;name&rt;value&lt;/name&rt;  &lt;!- name value pair -&rt;
         *      &lt;name/&rt;             &lt;!- no value
         *      ...
         *  &lt;myTagName&rt;
         *  </pre><br><br>
         *
         *  If type is CFG_XML_STRING, the string will be parsed as XML.
         *  <br><br>
         *
         *  Calling parse multiple times will add entries to the name 
         *  value map.<br><br>
         */
        ReturnCode              parse(Type, const string &);


        /** Returns the name value map that is populated as a result of
         *  calling parse.
         */
        const NameValueMap &    pairs() const;
        
    private:

        ReturnCode          parseFile();
        ReturnCode          parseStream(istream &);

        ReturnCode          parseXMLFile();
        ReturnCode          parseXMLString(const string &);
        ReturnCode          parseXML(xmlDocPtr &);

        string              myTagName;
                
        string              myFileName;
        NameValueMap        myPairs;
};


} // namespace Configuration
} // namespace Assist


#endif // !defined(ASSIST_PARSEPAIR_HXX)
