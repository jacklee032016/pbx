/*
 * $Id: SipCommand.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include <cstdlib>
#include <vector>
#include <cstring>

#include "BaseUrl.hxx"
#include "SipAllow.hxx"
#include "SipAlso.hxx"
#include "SipAuthorization.hxx"
#include "SipBasic.hxx"
#include "SipCommand.hxx"
#include "SipContact.hxx"
#include "SipContentDisposition.hxx"
#include "SipDigest.hxx"
#include "SipHide.hxx"
#include "SipMaxForwards.hxx"
#include "SipOrganization.hxx"
#include "SipOsp.hxx"
#include "SipOspAuthorization.hxx"
#include "SipPriority.hxx"
#include "SipProxyAuthorization.hxx"
#include "SipProxyRequire.hxx"
#include "SipRequire.hxx"
#include "SipResponseKey.hxx"
#include "SipRecordRoute.hxx"
#include "SipRoute.hxx"
#include "SipSubject.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SipSupported.hxx"
#include "SipTransferTo.hxx"
#include "SipUserAgent.hxx"
#include "SipVia.hxx"
#include "StatusMsg.hxx"
#include "SystemInfo.hxx"
#include "digest.hxx"
#include "vmd5.h"

#include "RandomHex.hxx"

using namespace Vocal;


SipCommand::SipCommand()
    : 
    SipMsg(),
    myRequestLine()
{
}


SipCommand::SipCommand(const SipCommand& src)
    : 
    SipMsg(src),
    myRequestLine(src.myRequestLine)
{
}


SipCommand::SipCommand(const SipCommand& src,
                       const SipVia& via,
                       const SipCSeq& cseq)
    : 
    SipMsg(),
    myRequestLine()
{
    // src should be a message that this UAS received
    myVersion = src.myVersion;
    // reverse To and From 
    SipFrom from(src.getTo());
    setFrom(from);
    SipTo to(src.getFrom());
    setTo(to);
    copyHeader(src, SIP_CALLID_HDR);
    setCSeq(cseq);
    SipMaxForwards smf;
    setMaxForwards(smf);
    setVia(via,0);

    if (src.getNumRoute() == 0)
    {
        if (src.getNumContact() == 1)
        {
            myRequestLine.setUrl(src.getContact(0).getUrl());

            SipRecordRouteList rrList = src.getrecordrouteList();
            SipRecordRouteList::iterator iter = rrList.begin();

            // Copy the recordRoute list and form a route list
            while ( iter != rrList.end() )
            {
                SipRoute* route = new SipRoute(**iter);
                routePushBack(*route);
                iter++;
            }
        }
    }
}

SipCommand::SipCommand(const StatusMsg& status)
    : SipMsg(),
      myRequestLine()
{
    myVersion = DEFAULT_VERSION;
    copyHeader(status, SIP_FROM_HDR);
    copyHeader(status, SIP_TO_HDR);
    copyHeader(status, SIP_CALLID_HDR);
    copyHeader(status, SIP_CSEQ_HDR);
    incrementCSeq(); // client can overwrite if this is incorrect
    SipMaxForwards smf;
    setMaxForwards(smf);
    //setVia(via,0);


    if(status.getNumContact() >= 1)
    {
        myRequestLine.setUrl(status.getContact(0).getUrl());
    }
    
    // why do this for only range 180 <-> 300? !jf!  

    // because it deals with the route list -- only 18x and 2xx can
    // have the record-route set, so only those can be acked w/
    // record-route

    if (status.getStatusLine().getStatusCode() >= 180 && 
        status.getStatusLine().getStatusCode() < 300)
    {
        
        SipRecordRouteList rrList = status.getrecordrouteList();
        SipRecordRouteList::iterator iter = rrList.begin();

        // Reverse the recordRoute list and form a route list
        // Record-Route (A,B,C) -> Route (C,B,A)
        while ( iter != rrList.end() )
        {
            SipRoute* route = new SipRoute(**iter);
            routePushFront(*route);
            iter++;
        }
    }
    // record-route to route
    // let the application set the via
}


SipCommand::~SipCommand()
{
}


SipCommand& 
SipCommand::operator =(const SipCommand& src)
{
    if (&src != this)
    {
        SipMsg::operator=(src);
        myRequestLine = src.myRequestLine;
    }
    return (*this);
}

bool 
SipCommand::operator ==(const SipCommand& src) const
{
    bool equal = (SipMsg::operator==(src) && 
		  (myRequestLine == src.myRequestLine));
    cpDebug(LOG_DEBUG_STACK, "SipCommand::operator== returns %s", 
	  equal ? "true" : "false");
    return equal;
}


bool
SipCommand::parseStartLine(const Data& line1)
{
    Data header;
    Data line2=line1;
    Data fg;
    line2.match(LF,&fg,true);
    int  fieldvalue = line2.match(SP, &header, false);
    if (fieldvalue == FOUND)
    {
        line2.removeSpaces();
        try
        {
            myRequestLine.decode(line2);
        }
        catch(SipRequestLineParserException&)
        {
            // xxx if this is bad, need to indicate failure
            return true;
        }
    }
    return false;
}


bool
SipCommand::decode(const Data& inputData)
{
    // new, revised decode

    // get the first line, and parse

    Data rawMsg = inputData;

    bool noMatch = false;
    Data line = rawMsg.getLine(&noMatch);

    bool requestLineError = parseStartLine(line);

    bool headerError = false;
    // pass the rest up to SipMsg::parse()
    try
    {
        headerError = SipMsg::parse(rawMsg);
    }
    catch(VException& e)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg, since: %s", 
              e.getDescription().c_str());
        throw SipParserException(e.getDescription(),
                                 __FILE__,
                                 __LINE__);
    }
    catch(...)
    {
        cpLog(LOG_ERR, "Failed to parse the SIP msg for unknown reason");
        throw SipParserException("unknown reason",
                                 __FILE__,
                                 __LINE__);
    }

    return requestLineError || headerError;
}


const SipRequestLine&
SipCommand::getRequestLine() const
{
    return myRequestLine;
}


SipRequestLine&
SipCommand::getMutableRequestLine() 
{
    return myRequestLine;
}


void
SipCommand::setRequestLine(const SipRequestLine& req)
{
    myRequestLine = req;
}


void
SipCommand::setViaBranch( int i )
{
    int numvia = getNumVia();
    if (numvia == 0)
    {
        throw SipCommandException(COMMAND_VIA_NOT_PRESENT);
    }
    if ( (i < 0) || (i >= numvia) )
    {
        throw SipCommandException(COMMAND_INDEX_OUT_OF_BOUND);
    }
    SipVia via = getVia(i);

    Data token(i);
    via.setBranch(token);
}


int SipCommand::getViaBranch()
{
    int numvia = getNumVia();
    if (numvia == 0)
    {
        throw SipCommandException(COMMAND_VIA_NOT_PRESENT);
    }
    else
    {
        SipVia via = getVia(0);
        Data token;
        token = via.getBranch();
        LocalScopeAllocator lo;
        int value = strtol(token.getData(lo), (char**)NULL, 10);
        return value;
    }
}


/*--------------------------- SipAuthorization ---------------------------*/


const SipAuthorization&
 SipCommand::getAuthorization() const
{
    Sptr<SipAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_AUTHORIZATION_HDR);
    return *x;
}


void SipCommand::setAuthorization( const SipAuthorization& author)
{
    Sptr<SipAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_AUTHORIZATION_HDR);
    *x = author;
}
    

void SipCommand::setAuthorization( const Data& author)
{
    Sptr<SipAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_AUTHORIZATION_HDR);
    x->decode(author);
}


/*--------------------------- SipHide ---------------------------*/


const SipHide& SipCommand::getHide() const
{
    Sptr<SipHide> x;
    myHeaderList.getParsedHeader(x, SIP_HIDE_HDR);
    return *x;
}


void SipCommand::setHide( const SipHide& newhide)
{
    Sptr<SipHide> x;
    myHeaderList.getParsedHeader(x, SIP_HIDE_HDR); 
    *x = newhide;
}
    
    
void SipCommand::setHide( const Data& newhide)
{
    Sptr<SipHide> x;
    myHeaderList.getParsedHeader(x, SIP_HIDE_HDR); 
    x->decode(newhide);
}    


/*--------------------------- SipAlso ---------------------------*/


const SipAlso& SipCommand::getAlso() const
{
    Sptr<SipAlso> x;
    myHeaderList.getParsedHeader(x, SIP_ALSO_HDR);
    return *x;
}


void SipCommand::setAlso( const SipAlso& newalso)
{
    Sptr<SipAlso> x;
    myHeaderList.getParsedHeader(x, SIP_ALSO_HDR);
    *x = newalso;
}


/*--------------------------- SipContentDisposition ---------------------------*/


const SipContentDisposition& SipCommand::getContentDisposition() const
{
    Sptr<SipContentDisposition> x;
    myHeaderList.getParsedHeader(x, SIP_CONTENT_DISPOSITION_HDR);
    return *x;
}

    
void SipCommand::setContentDisposition( const SipContentDisposition& newa)
{
    Sptr<SipContentDisposition> x;
    myHeaderList.getParsedHeader(x, SIP_CONTENT_DISPOSITION_HDR);
    *x = newa;
}
    
    
void SipCommand::setContentDisposition( const Data& newvalue)
{
    Sptr<SipContentDisposition> x;
    myHeaderList.getParsedHeader(x, SIP_CONTENT_DISPOSITION_HDR);
    x->decode(newvalue);
}


/*------------------------------ SipMaxForwards ------------------------------*/

    
const SipMaxForwards& SipCommand::getMaxForwards() const
{
    Sptr<SipMaxForwards> x;
    myHeaderList.getParsedHeader(x, SIP_MAX_FORWARDS_HDR);
    return *x;
}


void SipCommand::setMaxForwards( const SipMaxForwards& newmaxForwards)
{
    Sptr<SipMaxForwards> x;
    myHeaderList.getParsedHeader(x, SIP_MAX_FORWARDS_HDR);
    *x = newmaxForwards;
}
    
    
void SipCommand::setMaxForwards( const Data& newmaxForwards)
{
    Sptr<SipMaxForwards> x;
    myHeaderList.getParsedHeader(x, SIP_MAX_FORWARDS_HDR);
    x->decode(newmaxForwards);
}


bool SipCommand::checkMaxForwards()
{
    return getMaxForwards().get() >= 0;
}

bool
SipCommand::decrementMaxForwards()
{
    Sptr<SipMaxForwards> maxForwards;
    myHeaderList.getParsedHeader(maxForwards, SIP_MAX_FORWARDS_HDR);
    
    return maxForwards->decrement() != -1;
}


/*--------------------------- SipOrganization ---------------------------*/


const SipOrganization& SipCommand::getOrganization() const
{
    Sptr<SipOrganization> x;
    myHeaderList.getParsedHeader(x, SIP_ORGANIZATION_HDR);
    return *x;
}

void SipCommand::setOrganization( const SipOrganization& neworganization)
{
    Sptr<SipOrganization> x;
    myHeaderList.getParsedHeader(x, SIP_ORGANIZATION_HDR);
    *x =  neworganization;
}
    
    
void SipCommand::setOrganization( const Data& neworganization)
{
    Sptr<SipOrganization> x;
    myHeaderList.getParsedHeader(x, SIP_ORGANIZATION_HDR);
    x->decode(neworganization);
}


/*--------------------------- SipPriority ---------------------------*/


const SipPriority& SipCommand::getPriority() const
{
    Sptr<SipPriority> x;
    myHeaderList.getParsedHeader(x, SIP_PRIORITY_HDR);
    return *x;
}


void SipCommand::setPriority( const SipPriority& newpriority)
{
    Sptr<SipPriority> x;
    myHeaderList.getParsedHeader(x, SIP_PRIORITY_HDR);
    *x = newpriority;
}
    
    
void SipCommand::setPriority( const Data& newpriority)
{
    Sptr<SipPriority> x;
    myHeaderList.getParsedHeader(x, SIP_PRIORITY_HDR);
    x->decode(newpriority);
}


/*--------------------------- SipProxyAuthorization ---------------------------*/


const SipProxyAuthorization& 
SipCommand::getProxyAuthorization() const
{
    Sptr<SipProxyAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_AUTHORIZATION_HDR);
    return *x;
}


void
SipCommand::setProxyAuthorization( const SipProxyAuthorization& newproxyAuth)
{
    Sptr<SipProxyAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_AUTHORIZATION_HDR);
    *x = newproxyAuth;
}
    
    
void
SipCommand::setProxyAuthorization( const Data& newproxyAuth)
{
    Sptr<SipProxyAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_AUTHORIZATION_HDR);
    x->decode(newproxyAuth);
}
    

int 
SipCommand::getNumProxyRequire() const
{
    return myHeaderList.getNumHeaders(SIP_PROXY_REQUIRE_HDR);
}


const SipProxyRequire& 
SipCommand::getProxyRequire( int i /*Defalut Value*/) const
{
    Sptr<SipProxyRequire> x;
    myHeaderList.getParsedHeader(x, SIP_PROXY_REQUIRE_HDR, i);
    return *x;
}


void 
SipCommand::setProxyRequire(SipProxyRequire item, int index /*Default Value*/)
{
    Sptr<SipHeader> x = new SipProxyRequire(item);
    myHeaderList.appendHeader(SIP_PROXY_REQUIRE_HDR, x, index);
}
    
    
void 
SipCommand::setProxyRequire(const Data&item, int index /*Default Value*/)
{
    Sptr<SipHeader> x = new SipProxyRequire(item);
    myHeaderList.appendHeader(SIP_PROXY_REQUIRE_HDR, x, index);
}


void 
SipCommand::setNumProxyRequire(int index)
{
    myHeaderList.setNumHeaders(SIP_PROXY_REQUIRE_HDR, index);
}


//------------------------------ SipResponseKey ------------------------------


const SipResponseKey& 
SipCommand::getResponseKey() const
{
    Sptr<SipResponseKey> x;
    myHeaderList.getParsedHeader(x, SIP_RESPONSE_KEY_HDR);
    return *x;
}


void 
SipCommand::setResponseKey( const SipResponseKey& newResponseKey)
{
    Sptr<SipResponseKey> x;
    myHeaderList.getParsedHeader(x, SIP_RESPONSE_KEY_HDR);
    *x = newResponseKey;
}


/*------------------------------ SipRequire ----------------------------------*/


int 
SipCommand::getNumRequire() const
{
    return myHeaderList.getNumHeaders(SIP_REQUIRE_HDR);

}


const SipRequire& 
SipCommand::getRequire( int i /*Default Value*/) const
{
    Sptr<SipRequire> x;
    myHeaderList.getParsedHeader(x, SIP_REQUIRE_HDR, i);
    return *x;
}


void 
SipCommand::setRequire(const SipRequire& item, int index /*default value*/)
{
    Sptr<SipHeader> x = new SipRequire(item);
    myHeaderList.appendHeader(SIP_REQUIRE_HDR, x, index);
}
    
    
void 
SipCommand::setRequire(const Data& item, int index /*default value*/)
{
    Sptr<SipHeader> x = new SipRequire(item);
    myHeaderList.appendHeader(SIP_REQUIRE_HDR, x, index);
}


void 
SipCommand::setNumRequire(int index)
{
    myHeaderList.setNumHeaders(SIP_REQUIRE_HDR, index);
}


/*--------------------------------- SipRoute -------------------------------*/


void 
SipCommand::flushrouteList()
{
    myHeaderList.setNumHeaders(SIP_ROUTE_HDR, 0);
}


const vector < Sptr<SipRoute> > 
SipCommand::getRouteList() const
{
    vector < Sptr<SipRoute> > routes;
    int cnt = myHeaderList.getNumHeaders(SIP_ROUTE_HDR);
    for (int i=0; i < cnt; i++)
    {
        Sptr<SipRoute> x;
        myHeaderList.getParsedHeader(x, SIP_ROUTE_HDR, i);
        routes.push_back(x);
    }
    return routes;
}

void 
SipCommand::setRouteList(const vector < SipRoute* > sipRouteList)
{
    flushrouteList();
    vector < SipRoute* > ::const_iterator iter = sipRouteList.begin();
    while ( iter != sipRouteList.end() )
    {
	myHeaderList.appendHeader(SIP_ROUTE_HDR, new SipRoute( *(*iter) ));
        iter++;
    }
}

void 
SipCommand::setRouteList(const vector < Sptr<SipRoute> > sipRouteList)
{
    flushrouteList();
    vector < Sptr<SipRoute> > ::const_iterator iter = sipRouteList.begin();
    while ( iter != sipRouteList.end() )
    {
	myHeaderList.appendHeader(SIP_ROUTE_HDR, *iter);
        iter++;
    }
}


int 
SipCommand::getNumRoute() const
{
    return myHeaderList.getNumHeaders(SIP_ROUTE_HDR);
}


const SipRoute& 
SipCommand::getRoute( int i /*Default Values*/) const
{
    Sptr<SipRoute> x;
    myHeaderList.getParsedHeader(x, SIP_ROUTE_HDR, i);
    return *x;
}


void 
SipCommand::setRoute(const SipRoute& item, int index /*Default values*/)
{
    Sptr<SipHeader> x = new SipRoute(item);
    myHeaderList.appendHeader(SIP_ROUTE_HDR, x, index);
}
    
    
void 
SipCommand::setRoute(const Data&item, int index /*Default values*/)
{
    Sptr<SipHeader> x = new SipRoute(item);
    myHeaderList.appendHeader(SIP_ROUTE_HDR, x, index);
}
    

void 
SipCommand::removeRoute(int index /*Default value*/)
{
    myHeaderList.removeHeader(SIP_ROUTE_HDR, index);
}


void 
SipCommand::setNumRoute(int index)
{
    myHeaderList.setNumHeaders(SIP_ROUTE_HDR, index);
}

void 
SipCommand::routePushFront(const SipRoute& item)
{
    int n = getNumRoute();
    setRoute(item, 0);
    assert (getNumRoute() == n+1);
}

void 
SipCommand::routePushBack(const SipRoute& item)
{
    int n = getNumRoute();
    setRoute(item, -1);
    assert (getNumRoute() == n+1);
}

void 
SipCommand::routePopFront()
{
    assert(!routeEmpty());
    int n = getNumRoute();
    removeRoute(0);
    assert (getNumRoute() == n-1);
}

void 
SipCommand::routePopBack()
{
    assert(!routeEmpty());
    int n = getNumRoute();
    removeRoute(-1);
    assert (getNumRoute() == n-1);
}

const SipRoute&
SipCommand::routeFront() const
{
    assert(!routeEmpty());
    return getRoute(0);
}

const SipRoute& 
SipCommand::routeBack() const
{
    assert(!routeEmpty());
    return getRoute(-1);
}

bool
SipCommand::routeEmpty() const
{
    return getNumRoute() == 0;
}


/*--------------------------------- SipSubject -------------------------------*/


const SipSubject& 
SipCommand::getSubject() const
{
    Sptr<SipSubject> x;
    myHeaderList.getParsedHeader(x, SIP_SUBJECT_HDR);
    return *x;
}
    

void 
SipCommand::setSubject( const SipSubject& newsubject)
{
    Sptr<SipSubject> x;
    myHeaderList.getParsedHeader(x, SIP_SUBJECT_HDR);
    *x = newsubject;
}
    
    
void 
SipCommand::setSubject( const Data& newsubject)
{
    Sptr<SipSubject> x;
    myHeaderList.getParsedHeader(x, SIP_SUBJECT_HDR);
    x->decode(newsubject);
}


/*----------------------------- SipOspAuthorization ----------------------------*/


const SipOspAuthorization& 
SipCommand::getSipOspAuthorization() const
{
    Sptr<SipOspAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_OSP_AUTHORIZATION_HDR);
    return *x;
}


void 
SipCommand::setSipOspAuthorization( const SipOspAuthorization& newosp)
{
    Sptr<SipOspAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_OSP_AUTHORIZATION_HDR);
    *x = newosp;
}

    
void 
SipCommand::setSipOspAuthorization( const Data& data)
{
    Sptr<SipOspAuthorization> x;
    myHeaderList.getParsedHeader(x, SIP_OSP_AUTHORIZATION_HDR);
    x->decode(data);
}


/*--------------------------------- SipAuthBasic -------------------------------*/

void 
SipCommand::setAuthBasic(const Data& data, const Data& pwd)
{
    Data cookie;

    if (pwd.length())
    {
        //form the basic cookie., treating data as username, and pwd as password
        SipBasic basic;
        cookie = basic.formSIPBasic(data, pwd);
    }
    else
    {
        cookie = data;
    }

    //get proxyAuthorization and set the basic cookie.
    Sptr<SipAuthorization> authorization;
    myHeaderList.getParsedHeader(authorization, SIP_AUTHORIZATION_HDR);

    authorization->setAuthScheme(AUTH_BASIC);
    authorization->setBasicCookie(cookie);
}


/*--------------------------------- SipAuthBasic -------------------------------*/

void 
SipCommand::setProxyAuth(const Data& data, const Data& pwd)
{
    Data cookie;

    if (pwd.length())
    {
        //form the basic cookie., treating data as username, and pwd as password
        SipBasic basic;
        cookie = basic.formSIPBasic(data, pwd);
    }
    else
    {
        cookie = data;
    }

    //get proxyAuthorization and set the basic cookie.
    Sptr<SipProxyAuthorization> proxyAuth;
    myHeaderList.getParsedHeader(proxyAuth, SIP_PROXY_AUTHORIZATION_HDR);

    proxyAuth->setAuthScheme(AUTH_BASIC);
    proxyAuth->setBasicCookie(cookie);
}


bool 
SipCommand::checkProxyAuth(const Data& data, const Data& pwd)
{
    Sptr<SipProxyAuthorization> proxyAuth;
    myHeaderList.getParsedHeader(proxyAuth, SIP_PROXY_AUTHORIZATION_HDR);
    Data cookie;

    if (proxyAuth->getAuthScheme() != AUTH_BASIC)
    {
        cpDebug(LOG_DEBUG_STACK, "SCHEME IS NOT BASIC");
        return false;
    }

    if (pwd.length())
    {
        SipBasic basic;
        cookie = basic.formSIPBasic(data, pwd);
    }
    else
    {
        cookie = data;
    }

    //get cookie from proxy auth and compare
    const Data& recvdcookie = proxyAuth->getBasicCookie();
    if (recvdcookie == cookie)
    {
        cpDebug(LOG_DEBUG_STACK, " basic authentication successful");
        return true;
    }

    cpDebug(LOG_DEBUG_STACK, " basic authentication failed");
    return false;
}

bool 
SipCommand::checkAuthBasic(const Data& data, const Data& pwd)
{
    Sptr<SipAuthorization> authorization;
    myHeaderList.getParsedHeader(authorization, SIP_AUTHORIZATION_HDR);

    Data cookie;
    Data authScheme = authorization->getAuthScheme();

    if ( isEqualNoCase(authScheme, AUTH_BASIC ) == false )
    {
        cpDebug(LOG_DEBUG_STACK, "Tag %s scheme is not Basic", authScheme.logData() );
        return false;
    }
    if (pwd.length())
    {
        SipBasic basic;
        cookie = basic.formSIPBasic(data, pwd);
    }
    else
    {
        cookie = data;
    }
    //get cookie from proxyAuth, and compare
    Data recvdcookie = authorization->getBasicCookie();
    if (recvdcookie == cookie)
    {
        cpDebug(LOG_DEBUG_STACK, "Basic authentication successful");
        return true;
    }
    // -ctam
    cpDebug(LOG_DEBUG_STACK, "cookie (msg) = %s", cookie.logData() );
    cpDebug(LOG_DEBUG_STACK, "data   (ms)  = %s", data.logData() );
    cpDebug(LOG_DEBUG_STACK, "Basic authentication failed");
    return false;
}


void
SipCommand::setProxyAuthDigest(const Data& nonce, const Data& user,
                               const Data& pwd, const Data& method,
                               const Data& realm, const Data& requestURI,
                               const Data& qop, const Data& cnonce,
                               const Data& alg, const Data& noncecount,
                               const Data& opaque)
{
    Sptr<SipProxyAuthorization> authorization;
    myHeaderList.getParsedHeader(authorization, SIP_PROXY_AUTHORIZATION_HDR);

    SipDigest sipDigest;

    Data response = sipDigest.form_SIPdigest(nonce, user, pwd, method,
                    requestURI, realm, qop, cnonce, alg, noncecount);

    cpDebug(LOG_DEBUG_STACK, "setAuthDigest::Response = %s\n", 
	  response.logData());
    
    //set this as response in authorization.    
    authorization->setAuthScheme(AUTH_DIGEST);

    if(user != "")
    {
        authorization->setTokenDetails("username", user); //1
    }
    if(realm != "")
    {
        authorization->setTokenDetails("realm", realm);  //2
    }
    if(nonce != "")
    {
        authorization->setTokenDetails("nonce", nonce);  //3
    }
    if(response != "")
    {
        authorization->setTokenDetails("response", response); //4
    }
    if(qop != "")
    {
        authorization->setTokenDetails("qop", qop); //5
    }
    if(requestURI != "")
    {
        authorization->setTokenDetails("uri", requestURI); //6
    }
    if(cnonce != "")
    {
        authorization->setTokenDetails("cnonce", cnonce); //7
    }
    if(noncecount != "")
    {
        authorization->setTokenDetails("nc", noncecount); //8
    }
    if(opaque != "")
    {
        authorization->setTokenDetails("opaque", opaque); //9
    }
    if(alg != "")
    {
        authorization->setTokenDetails("algorithm", alg); // 10
    }
}


void
SipCommand::setAuthDigest(const Data& nonce, const Data& user,
                          const Data& pwd, const Data& method,
                          const Data& realm, const Data& requestURI,
                          const Data& qop, const Data& cnonce,
                          const Data& alg, const Data& noncecount,
                          const Data& opaque)
{
    Sptr<SipAuthorization> authorization;
    myHeaderList.getParsedHeader(authorization, SIP_AUTHORIZATION_HDR);

    SipDigest sipDigest;

    Data response = sipDigest.form_SIPdigest(nonce, user, pwd, method,
                    requestURI, realm, qop, cnonce, alg, noncecount);

    cpDebug(LOG_DEBUG_STACK, "setAuthDigest::Response = %s\n", 
	  response.logData());
    
    //set this as response in authorization.    
    authorization->setAuthScheme(AUTH_DIGEST);
    
    if(user != "")
    {
        authorization->setTokenDetails("username", user); //1
    }
    if(realm != "")
    {
        authorization->setTokenDetails("realm", realm);  //2
    }
    if(nonce != "")
    {
        authorization->setTokenDetails("nonce", nonce);  //3
    }
    if(response != "")
    {
        authorization->setTokenDetails("response", response); //4
    }
    if(qop != "")
    {
        authorization->setTokenDetails("qop", qop); //5
    }
    if(requestURI != "")
    {
        authorization->setTokenDetails("uri", requestURI); //6
    }
    if(cnonce != "")
    {
        authorization->setTokenDetails("cnonce", cnonce); //7
    }
    if(noncecount != "")
    {
        authorization->setTokenDetails("nc", noncecount); //8
    }
    if(opaque != "")
    {
        authorization->setTokenDetails("opaque", opaque); //9
    }
    if(alg != "")
    {
        authorization->setTokenDetails("algorithm", alg); // 10
    }
}


bool
SipCommand::checkAuthDigest(const Data& nonce, 
                            const Data& user,
                            const Data& pwd, 
                            const Data& realm,
                            const Data& requestURI,
                            const bool authReg /* default value */)
{
    Sptr<SipAuthorization> authorization;
    myHeaderList.getParsedHeader(authorization, SIP_AUTHORIZATION_HDR);

    Sptr<SipProxyAuthorization> proxyAuth;
    myHeaderList.getParsedHeader(proxyAuth, SIP_PROXY_AUTHORIZATION_HDR);

    Data authScheme;

    if (authReg)
    {
        cpDebug(LOG_DEBUG_STACK, 
               "using Authorization: header for digest check");
        authScheme = authorization->getAuthScheme();
    }
    else
    {
        cpDebug(LOG_DEBUG_STACK, 
               "using Proxy-Authorization: header for digest check");
        authScheme = proxyAuth->getAuthScheme();
    }
    
    Data method;

    method = myRequestLine.getMethod();

    // xxx need to test this
    if ( isEqualNoCase( authScheme, AUTH_DIGEST ) == false   )
    {
        cpDebug(LOG_DEBUG_STACK, "AUTH_DIGEST: %s", AUTH_DIGEST.logData());
        cpDebug(LOG_DEBUG_STACK, "authScheme: %s", authScheme.logData());
        cpDebug(LOG_DEBUG_STACK, "Tag %s Not a DIGEST scheme", authScheme.logData() );
        return false;
    }
    else
    {
        Data checksum;
        Data algorithm;
        Data msgNonce;

        if (authReg)
        {
            checksum  = authorization->getTokenValue("response");
            algorithm = authorization->getTokenValue("algorithm");
            msgNonce  = authorization->getTokenValue("nonce");
        }
        else
        {
            checksum  = proxyAuth->getTokenValue("response");
            algorithm = proxyAuth->getTokenValue("algorithm");
            msgNonce  = proxyAuth->getTokenValue("nonce");
        }

        if(!isEqualNoCase(nonce, msgNonce))
        {
            cpDebug(LOG_DEBUG_STACK, "passed nonce != msg nonce");
            cpDebug(LOG_DEBUG_STACK, "passed nonce == %s", nonce.logData());
            cpDebug(LOG_DEBUG_STACK, "msg nonce    == %s", msgNonce.logData());
            return false;
        }

        if (checksum.length() == 0)
        {
            cpDebug(LOG_DEBUG_STACK, "zero length digest data");
            return false;
        }

        cpDebug(LOG_DEBUG_STACK, "Calling form_SIPdigest with:" );
        cpDebug(LOG_DEBUG_STACK, "  nonce    = %s", nonce.logData() );
        cpDebug(LOG_DEBUG_STACK, "  user     = %s", user.logData() );
        cpDebug(LOG_DEBUG_STACK, "  pswd     = %s", pwd.logData() );
        cpDebug(LOG_DEBUG_STACK, "  method   = %s", method.logData() );
        cpDebug(LOG_DEBUG_STACK, "  uri      = %s", requestURI.logData());
        cpDebug(LOG_DEBUG_STACK, "  realm    = %s", realm.logData());
        cpDebug(LOG_DEBUG_STACK, "  algorithm= %s", algorithm.logData());

        SipDigest sipDigest;

        Data response = sipDigest.form_SIPdigest(nonce, 
                                                 user, 
                                                 pwd, 
                                                 method,
                                                 requestURI, 
                                                 realm, 
                                                 Data(""),
                                                 Data(""), 
                                                 algorithm,
                                                 Data(""));

        cpDebug(LOG_DEBUG_STACK, "Message digest    == %s", 
              checksum.logData());
        cpDebug(LOG_DEBUG_STACK, "Calculated digest == %s\n", 
              response.logData());

        if (checksum == response)
        {
            cpDebug(LOG_DEBUG_STACK, " Digest authentication successful");
            return true;
        }
        else
        {
            cpDebug(LOG_DEBUG_STACK, " Digest authentication failed");
            return false;
        }
    }
    return false;
}


Data 
SipCommand::computeBranch(Data hashBranch /* default value */) const
{
    Sptr <BaseUrl> toBaseUrl = getTo().getUrl();
    if (toBaseUrl.getPtr() != 0)
    {
	if (toBaseUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(toBaseUrl);
	    hashBranch += sipUrl->getNameAddr();
	}
    }
    
    Sptr <BaseUrl> fromBaseUrl = getFrom().getUrl();
    if (fromBaseUrl.getPtr() != 0)
    {
	if (fromBaseUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(fromBaseUrl);
    
	    hashBranch+= sipUrl->getNameAddr();
	}
    }
    hashBranch+= getCallId().encode();
    
    Sptr <BaseUrl> reqBaseUrl = getRequestLine().getUrl();
    if (reqBaseUrl.getPtr() != 0)
    {
	if (reqBaseUrl->getType() == SIP_URL)
	{
	    Sptr <SipUrl> sipUrl;
	    sipUrl.dynamicCast(reqBaseUrl);
	    hashBranch+= sipUrl->getNameAddr();
	}
    }
    hashBranch+= getCSeq().getCSeqData();
    
    unsigned len = (unsigned int)(hashBranch.length());
    
    //form MD5 out of hashBranch, and return.
    MD5Context ctx;
    MD5Init(&ctx);
    LocalScopeAllocator lo; 
    MD5Update(&ctx, (unsigned char*)(hashBranch.getData(lo)), len);
    
    
    unsigned char hashDigest[16];
    
    MD5Final(hashDigest, &ctx);
    
    hashBranch = convertToHex(hashDigest, 16);
    
    return hashBranch;
}    

Data
SipCommand::computeUniqueBranch()
{
    return RandomHex::get(4);
}

Data
SipCommand::computeProxyBranch() const
{
    Data result("z9hG4bK"); // magic cookie per rfc2543bis-09    
    result += computeStatelessProxyBranch();
    result += ".";
    result += computeUniqueBranch();
    return result;
}

Data
SipCommand::computeStatelessProxyBranch() const
{
    return computeBranch();
}

bool
SipCommand::detectLoop() const
{
    SipViaList viaList = getViaList();
    Data checkBranch = computeBranch();

    for( SipViaList::iterator i = viaList.begin();i != viaList.end(); i++ )
    {
        Data pb;
        if (((*i)->getBranch().match(".", &pb) == FOUND) && 
            ( pb == checkBranch) )
        {
            return true;
        }
    }

    return false;
}

Data 
SipCommand::computeBranch2() const
{
    return computeBranch(Data(theSystem.gethostAddress()) 
                         + Data(theSystem.getSystemPort()));
}

Data
SipCommand::encode() const
{
    Data data;

    data = myRequestLine.encode();
    encodeBody(&data);

    return data;
}

bool 
SipCommand::toBeRetransmitted() const
{
    if(getType() != SIP_ACK) return true;
    return false;
}

// see section 16.6 and 16.7 of bis09 draft - !jf!
Sptr<SipUrl>
SipCommand::postProcessRouteAndGetNextHop() 
{
    Sptr<SipUrl> url;
    Sptr<BaseUrl> burl;
#if 1
    burl = getRequestLine().getUrl();
#else     // loose routing not working !jf!
    if (routeEmpty()) // send to value in req-uri
    {
        burl = getRequestLine().getUrl();
    }
    else if (!routeFront().getUrl()->isLooseRouterPresent())
    {
        SipRoute route;
        route.setUrl(getRequestLine().getUrl());
        routePushBack(route);
        burl = routeFront().getUrl();
        getMutableRequestLine().setUrl(burl);
        routePopFront();
    }
    else   // send it to first entry in route if it exists
    {
        burl = routeFront().getUrl();
    }
#endif
    url.dynamicCast(burl);        
    return url;
}

 int
 SipCommand::isLocaladdr(const char *addr)
{
    char hostname[20];
    struct hostent *host;
	
    gethostname(hostname, 20);
    
    host = gethostbyname(hostname);
    char * p = inet_ntoa(*(struct in_addr*) host->h_addr);
    printf("hostname %s addr %s\n",hostname,p);
    if(!strcmp(addr,p))
    {
   	printf( "addr same");
	return 1;
    }
	return 0;
}
