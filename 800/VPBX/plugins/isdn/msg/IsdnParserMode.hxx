#ifndef __ISDN_PARSER_MODE_HXX__
#define __ISDN_PARSER_MODE_HXX__
/*
* $Id: IsdnParserMode.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "Data.hxx"

namespace Assist
{
class IsdnParserMode
{
	public:
		static bool isdnParserMode();

		static IsdnParserMode& instance();

		void setStrictMode(bool val);
      
	private:
		bool 						sipParserMode_;
		static IsdnParserMode		*instance_;

		IsdnParserMode();

};

}
#endif

