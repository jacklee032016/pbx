/*
* $Id: IsdnParserMode.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IsdnParserMode.hxx"

using namespace Assist;

IsdnParserMode * IsdnParserMode::instance_ = 0;

IsdnParserMode::IsdnParserMode()
{
	sipParserMode_ = true;
#if 0
	if (getenv("SIP_PARSER_STRICT_MODE"))
	{
		sipParserMode_ = true;
	}
	else
	{
		sipParserMode_ = false;
	}
#endif
}

bool IsdnParserMode::isdnParserMode()
{
	if (!instance_)
	{
		instance_ = new IsdnParserMode;
	}
	return instance_->sipParserMode_;
}

IsdnParserMode& IsdnParserMode::instance() 
{
	if (!instance_)
	{
		instance_ = new IsdnParserMode;
	}
	return *instance_;
}

void IsdnParserMode::setStrictMode(bool val)
{ 
	sipParserMode_ = val; 
}     

