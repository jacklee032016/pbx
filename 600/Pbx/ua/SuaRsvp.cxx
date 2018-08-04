/*
 * $Id: SuaRsvp.cxx,v 1.2 2007/03/02 04:19:29 lizhijie Exp $
 */

#include "global.h"
#include "SuaRsvp.hxx"
#include "rsvp_socks.h"
#include "rsvp.h"
#include "rapi_err.h"
#include "cpLog.h"


#ifndef MAX
#define MAX(a,b) ((a > b) ? (a) : (b))
#endif

SuaRsvp::SuaRsvp ()
        : session_id (0),
        ttl (0),
        resv_flag (0),
        rapi_fd (0)
{}

SuaRsvp::SuaRsvp( const SuaRsvp& src )
{
    session_id = src.session_id;
    ttl = src.ttl;
    resv_flag = src.resv_flag;
    rapi_fd = src.rapi_fd;
}

SuaRsvp&
SuaRsvp::operator=( const SuaRsvp& src )
{
    session_id = src.session_id;
    ttl = src.ttl;
    resv_flag = src.resv_flag;
    rapi_fd = src.rapi_fd;
    return *(this);
}

int
SuaRsvp::session( NetworkAddress* destAddr )
{
    cpDebug(LOG_DEBUG, "SuaRsvp::session");

    int rtn_code;

    proto_id = 17;  /* udp 17; tcp 6 */

    if (destAddr == NULL)
    {
        printf("SuaRsvp error: dest addr needed\n");
        return -1;
    }
    else
        destAddr->getSockAddr(dest_addr);

    session_id = rapi_session(&(dest_addr),
                              proto_id,
                              RAPI_USE_INTSERV,
                              (rapi_event_rtn_t)upcallHandler,
                              0,
                              &rtn_code);

    if (session_id == 0)
        printf("RAPI: %s err %d : %s\n", "rsvp_session()", rtn_code,
               rapi_errlist[rtn_code]);
    else
    {
        rapi_fd = rapi_getfd(session_id);
        printf("rapi_session => sid= %d, fd= %d\n", session_id, rapi_fd);
    }

    return rtn_code;

}

int
SuaRsvp::sender ( NetworkAddress* srcAddr )
{
    int rtn_code;

    ttl = 0;

    if (srcAddr == NULL)
    {
        printf("SuaRsvp error: src addr needed\n");
        return -1;
    }
    else
        srcAddr->getSockAddr(src_addr);


    rapi_tspec_t *tspec_ptr = &(snd_tspec);
    qos_tspecx_t *qos_tspec = &(tspec_ptr->tspecbody_qosx);
    qos_tspec->spec_type = QOS_TSPEC;
    qos_tspec->xtspec_r = 10000;
    qos_tspec->xtspec_b = 200;
    qos_tspec->xtspec_p = 10000;
    qos_tspec->xtspec_m = 200;
    qos_tspec->xtspec_M = 200;  /* default 65535 */
    tspec_ptr->len = sizeof(rapi_hdr_t) + sizeof(qos_tspecx_t);
    tspec_ptr->form = RAPI_TSPECTYPE_Simplified;

    rtn_code = rapi_sender(session_id,
                           0,  /* flag */
                           &(src_addr),  /* src addr */
                           NULL,  /* sender template */
                           &(snd_tspec),  /* sender tspec */
                           NULL,  /* sender adsepc */
                           NULL,   /* Policy */
                           ttl);

    if (rtn_code)
        printf("RAPI: %s err %d : %s\n", "rapi_sender()", rtn_code,
               rapi_errlist[rtn_code]);
    else
        printf("rapi_sender() OK\n");

    return rtn_code;

}


int
SuaRsvp::reserve(NetworkAddress* rcvAddr)
{
    cpDebug(LOG_DEBUG, "SuaRsvp::reserve");

    int rtn_code;
    struct sockaddr * rcv_sockaddr;

    if (rcvAddr == NULL)
        rcv_sockaddr = NULL;
    else
    {
        rcvAddr->getSockAddr(rcv_addr);
        rcv_sockaddr = &(rcv_addr);
    }


    style = RAPI_RSTYLE_FIXED;  /* RAPI_RSTYLE_WILDCARD = 1
                                      RAPI_RSTYLE_FIXED = 2
                                      RAPI_RSTYLE_SE = 3 */

    resv_flag = 0;  /* RAPI_REQ_CONFIRM */

    rapi_flowspec_t *flowspec_cl_ptr = &(flowspec_cl);
    qos_flowspecx_t *qos_flowspec_cl = &flowspec_cl_ptr->specbody_qosx;
    qos_flowspec_cl->spec_type = QOS_CNTR_LOAD;
    qos_flowspec_cl->xspec_r = 10000;
    qos_flowspec_cl->xspec_b = 200;
    qos_flowspec_cl->xspec_p = 10000;
    qos_flowspec_cl->xspec_m = 200;
    qos_flowspec_cl->xspec_M = 200;  /* 65535 */
    flowspec_cl_ptr->form = RAPI_FLOWSTYPE_Simplified;
    flowspec_cl_ptr->len = sizeof(rapi_flowspec_t);

    rapi_flowspec_t *flowspec_g_ptr = &(flowspec_g);
    qos_flowspecx_t *qos_flowspec_g = &flowspec_g_ptr->specbody_qosx;
    qos_flowspec_g->spec_type = QOS_GUARANTEEDX;
    qos_flowspec_g->xspec_R = 10000;
    qos_flowspec_g->xspec_S = 0;
    qos_flowspec_g->xspec_r = 10000;
    qos_flowspec_g->xspec_b = 200;
    qos_flowspec_g->xspec_p = 10000;
    qos_flowspec_g->xspec_m = 200;
    qos_flowspec_g->xspec_M = 200;  /* 65535 */
    flowspec_g_ptr->form = RAPI_FLOWSTYPE_Simplified;
    flowspec_g_ptr->len = sizeof(rapi_flowspec_t);

    filter_spec.form = RAPI_FILTERFORM_BASE;
    filter_spec.len = sizeof(rapi_hdr_t) + sizeof(rapi_filter_base_t);
    filter_spec.rapi_filt4 = *(struct sockaddr_in *) & src_addr;

    rtn_code = rapi_reserve(session_id,
                            resv_flag,
                            (struct sockaddr *) & rcv_sockaddr,
                            style,
                            NULL,
                            NULL,
                            1,
                            &(filter_spec),
                            1,
                            &(flowspec_cl));  /* flowspec_cl or flow_spec_g */

    if (rtn_code)
        printf("RAPI: %s err %d : %s\n", "rapi_reserve()", rtn_code,
               rapi_errlist[rtn_code]);

    resv_flag = 0;

    return rtn_code;

}

int
SuaRsvp::close()
{
    int rtn_code;

    rtn_code = rapi_release(session_id);

    if (rtn_code)
        printf("RAPI: %s err %d : %s\n", "rapi_release()", rtn_code,
               rapi_errlist[rtn_code]);

    printf("rapi_release(): sid= %d, fd= %d\n", session_id, rapi_fd );

    return rtn_code;

}

int
upcallHandler (
    rapi_sid_t sid,
    rapi_eventinfo_t event,
    int styleid,
    int errcode,
    int errval,
    struct sockaddr *errnode,
    u_char errflags,
    int filt_num,
    rapi_filter_t *filt_list,
    int flow_num,
    rapi_flowspec_t *flow_list,
    int adspec_num,
    rapi_adspec_t *adspec_list,
    void *myparm)
{
    rapi_filter_t *filtp = filt_list;
    rapi_flowspec_t *flowp = flow_list;
    rapi_adspec_t *adsp = adspec_list;
    int isPath = 1;
    char buff[128], out1[80];
    int T, i;
    extern char *rapi_rstyle_names[];

    //    for (T=0 ; T < MAX_T; T++)      /* Map sid into thread# T */
    //        if (Sid[T] == sid)
    //            break;

    //    sprintf(out1, "  Session= %.32s",
    //            net_sockaddr_print((struct sockaddr *) dest_addr));
    sprintf(out1, "  Session= %.32s", "sid");

    printf(
        "---------------------------------------------------------------\n");
    switch (event)
    {

        case RAPI_PATH_EVENT:
        printf("Path Event -- %s\n", out1);
        if (flow_num == 0)
            printf("    (No Path state)\n");
        break;

        case RAPI_RESV_EVENT:
        printf("T%d: Resv Event -- %s\n", T, out1);
        if (flow_num == 0)
            printf("    (No Resv state)\n");
        isPath = 0;
        break;

        case RAPI_RESV_ERROR:
        isPath = 0;
        case RAPI_PATH_ERROR:
        printf("sid=%d %s -- RSVP error: %s\n",
               sid, out1,
               (errcode == RSVP_Err_API_ERROR) ?
               rapi_errlist[errval] :
               rsvp_errlist[errcode]);
        if (event == RAPI_RESV_ERROR)
            printf("    Style=%s", rapi_rstyle_names[styleid]);
        printf("    Code=%d  Val=%d  Node= %s",
               errcode, errval, net_sockaddr_print(errnode));
        if (errflags&RAPI_ERRF_InPlace)
            printf(" *InPlace*");
        if (errflags&RAPI_ERRF_NotGuilty)
            printf(" *NotGuilt*");
        printf("\n");
        break;

        case RAPI_RESV_STATUS:
        isPath = 0;
        printf("Resv State -- %s:\n", out1);
        break;

        case RAPI_PATH_STATUS:
        printf("Path Stat -- %s:\n", out1);
        break;

        case RAPI_RESV_CONFIRM:
        isPath = 0;
        printf("Confirm Event -- %s:\n", out1);
        break;

        default:
        printf("!!?\n");
        break;
    }
    for (i = 0; i < MAX(filt_num, flow_num); i++)
    {
        if (i < filt_num)
        {
            static char FIbuff[256];
            rapi_fmt_filtspec(filtp, FIbuff, 256);
            printf("\t%s", FIbuff);
            filtp = (rapi_filter_t *)After_RAPIObj(FIbuff);
        }
        else
            printf("\t\t");
        if (i < flow_num)
        {
            if (isPath)
                rapi_fmt_tspec((rapi_tspec_t *)flowp,
                               buff, sizeof(buff));
            else
                rapi_fmt_flowspec(flowp, buff, sizeof(buff));
            printf("\t%s\n", buff);
            flowp = (rapi_flowspec_t *)After_RAPIObj(flowp);
        }
        else
            printf("\n");
        if (i < adspec_num)
        {
            rapi_fmt_adspec(adsp, buff, sizeof(buff));
            printf("\t\t%s\n", buff);
            adsp = (rapi_adspec_t *)After_RAPIObj(adsp);
        }

    }
    printf(
        "---------------------------------------------------------------\n");
    fflush(stdout);
    return 0;
}
