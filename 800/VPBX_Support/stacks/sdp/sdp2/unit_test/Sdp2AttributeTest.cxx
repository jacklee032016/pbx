/*
* $Log: Sdp2AttributeTest.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: Sdp2AttributeTest.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Verify.hxx"
#include "Sdp2Attributes.hxx"

using Assist::SDP::SdpAttributes;
using Assist::SDP::SdpRtpMapAttribute;
using Assist::SDP::ValueAttribute;
using Assist::SDP::MediaAttributes;

int main()
{
    {
	SdpAttributes x;
	
	x.addValueAttribute(new ValueAttribute);
	x.addValueAttribute(new ValueAttribute);
	x.addValueAttribute(new ValueAttribute);
	
	SdpAttributes y;
	y = x;
	SdpAttributes z(x);
	
	vector<ValueAttribute*>::iterator i = x.getValueAttributes()->begin();
	vector<ValueAttribute*>::iterator j = y.getValueAttributes()->begin();
	vector<ValueAttribute*>::iterator k = z.getValueAttributes()->begin();
	
	while((i != x.getValueAttributes()->end()) && 
	      (j != y.getValueAttributes()->end()) &&
	      (k != z.getValueAttributes()->end())
	    )
	{
	    test_verify(*i != *j);
	    test_verify(*i != *k);
	    test_verify(strcmp((*i)->getAttribute(), (*j)->getAttribute()) == 0);
	    test_verify(strcmp((*i)->getValue(), (*j)->getValue()) == 0);
	    ++i;
	    ++j;
	    ++k;
	}
    }

    {
	MediaAttributes x;
	
	x.addmap(new SdpRtpMapAttribute);
	x.addmap(new SdpRtpMapAttribute);
	x.addmap(new SdpRtpMapAttribute);
	
	MediaAttributes y;
	y = x;
	
	vector<SdpRtpMapAttribute*>::iterator i = x.getmap()->begin();
	vector<SdpRtpMapAttribute*>::iterator j = y.getmap()->begin();
	
	while((i != x.getmap()->end()) && 
	      (j != y.getmap()->end())
	    )
	{
	    test_verify(*i != *j);
	    ++i;
	    ++j;
	}
    }

    {
	SdpAttributes x;
	Data y("recvonly");
	x.setAttribute(y);

	test_verify(x.getrecvonly());
    }

    {
	SdpAttributes x;
	Data y("sendonly");
	x.setAttribute(y);

	test_verify(x.getsendonly());
    }

    {
	SdpAttributes x;
	Data y("sendrecv");
	x.setAttribute(y);

	test_verify(x.getsendrecv());
    }

    {
	SdpAttributes x;
	Data y("sendrecv:1");
	x.setAttribute(y);

	vector < ValueAttribute* > * va = x.getValueAttributes();
	test_verify(va->begin() != va->end());
	assert(va->begin() != va->end());

	test_verify(Data((*va->begin())->getAttribute()) == "sendrecv");
	test_verify(Data((*va->begin())->getValue()) == "1");
    }


    return test_return_code(21);
}
