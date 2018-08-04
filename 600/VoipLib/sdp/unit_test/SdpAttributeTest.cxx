/*
* $Id: SdpAttributeTest.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include "Verify.hxx"
#include "SdpAttributes.hxx"

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

    return test_return_code(15);
}
