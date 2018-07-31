#include "CommonFunctions.hxx"
#include "SipVia.hxx"
/*
* $Id: CommonFunctions.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

void Assist::addSelfInVia(Sptr<SipCommand> cmd,  int port /* default value*/, const Data& transport /*default value*/)
{
	cmd->flushViaList();

	SipVia via;
	//    via.setHost("192.168.3.172"); //theSystem.gethostAddress());
	via.setHost(theSystem.gethostAddress());
	via.setPort(port);
	via.setTransport(transport);
	cmd->setVia(via);
}

