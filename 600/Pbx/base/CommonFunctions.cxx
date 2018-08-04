/*
 * $Id: CommonFunctions.cxx,v 1.3 2007/03/03 18:39:27 lizhijie Exp $
 */

#include "CommonFunctions.hxx"
#include "SipVia.hxx"

void 
Vocal::addSelfInVia(Sptr<SipCommand> cmd, 
		    int port /* default value*/, 
		    const Data& transport /*default value*/)
{
    cmd->flushViaList();
    SipVia via;
    via.setHost(theSystem.gethostAddress());
    via.setPort(port);
    via.setTransport(transport);
    cmd->setVia(via);
}

