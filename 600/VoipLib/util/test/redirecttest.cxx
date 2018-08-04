/*
* $Id: redirecttest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "SipRedirect.hxx"
#include "InviteMsg.hxx"
#include "Sptr.hxx"

main()
{
    SipRedirect x;
    SipRedirect y;

    Sptr < InviteMsg > invite = new InviteMsg();

    // add the sipredirect message
    // after adding the  counter, limit, and token
    //test scenarios
    // encode invite
    // encode status
    // make the status from the invite
    x.setLimit(1);
    x.setCounter(2);
    Data xa = "sssss";
    Data ya = "yyyyy";
    Data xa1 = "sssssaaaaa";
    Data ya1 = "yyyyymmmmm";
    y.setLimit(2);
    y.setReason(1);
    y.setCounter(50);
    x.setTokenDetails(xa, ya);
    x.setTokenDetails(xa1, ya1);
    invite->setRedirect(x);
    invite->setRedirect(y);


    cout << invite->encode().getData() << endl;
    Data data = invite->encode();
    Sptr < InviteMsg > invite1 = new InviteMsg( data );

    cout << invite1->encode().getData();

    //InviteMsg inmsg = *invite1;

    Sptr < StatusMsg > status = new StatusMsg(*invite1, 302);

    cout << status->encode().getData();


    status = new StatusMsg(*invite1, 302);

    cout << status->encode().getData();

    Sptr < StatusMsg > status1 = status;

    cout << status1->encode().getData();
    //  invite = new InviteMsg(data);

    //cout << invite->encode().getData();







}
