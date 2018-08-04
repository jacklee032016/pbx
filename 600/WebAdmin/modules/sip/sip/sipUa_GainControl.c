/*
* $Id: sipUa_GainControl.c,v 1.3 2007/09/08 15:18:55 lizhijie Exp $
*/

#include "sipUa.h"
#include "_sipUa.h"
#include "aslib.h"

typedef	struct	_cgi_gain_t
{
	int		value;
	char		display[64];
}cgi_gain_t;


cgi_gain_t laws[] = 
{
	{A_LAW_CODE,		_("a Law")	},
	{U_LAW_CODE,		_("u Law")	},
	{-1,					""			}
};

cgi_gain_t	gains[] = 
{
		{20,		"26"	},
		{10,		"20"	},
		{5,		"14"	},
		{2,		"6"	},
		{1,		"0"	},
		{-1,		""	}
};

static llist  	gainsCfgs;

static char *__sipUa_gain_button(llist *cgiVars)
{
	char 	buf[8192*5];
	int 		length = 0;
	cgi_gain_t	*_gain;
	int 		key;

	length += CGI_SPRINTF(buf, length, "<form name=\"recordForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SIP_UA );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(cgiVars) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_SAVE);

	length += CGI_SPRINTF(buf,length, "<table border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");

	/* Code Law */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", gettext("G711 Law") );
	key = atoi(GET_VALUE(&gainsCfgs, SIPUA_G711_LAW) );
	
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' >\n", 
		SIPUA_G711_LAW );
	_gain = laws;
	while(_gain->value != -1)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			_gain->value, (key==_gain->value)?"selected":"", _gain->display );
		_gain++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>\n</TD></TR>\r\n");
	
	/* TX Gain */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", gettext("TX Gain") );
	key = atoi(GET_VALUE(&gainsCfgs, SIPUA_TX_GAIN) );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' >\n", 
		SIPUA_TX_GAIN );
	_gain = gains;
	while(_gain->value != -1)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			_gain->value, (key==_gain->value)?"selected":"", _gain->display );
		_gain++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>%s\n</TD></TR>\r\n", gettext("dB") );
	
	/* RX Gain */
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" ><strong>%s:</strong></TD>\r\n", gettext("RX Gain") );
	key = atoi(GET_VALUE(&gainsCfgs, SIPUA_RX_GAIN) );
	length += CGI_SPRINTF(buf, length, "\t<TD>\n\t<select name=\"%s\" style='width:90px;' >\n", 
		SIPUA_RX_GAIN );
	_gain = gains;
	while(_gain->value != -1)
	{
		length += CGI_SPRINTF(buf,length, "\t\t<option value=\"%d\" %s>%s</option>\n", 
			_gain->value, (key==_gain->value)?"selected":"", _gain->display );
		_gain++;
	}
	length += CGI_SPRINTF(buf,length, "\t</select>%s\n</TD></TR>\r\n", gettext("dB") );

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=4 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_SAVE) ) );

	length += CGI_SPRINTF(buf,length, "\t\t</TD></TR></FORM>\r\n");

	length += CGI_SPRINTF(buf,length, "<TR><TD><strong>%s</strong><br><strong>* '%s'</strong> %s. <br> %s. %s.\r\n</TD></TR>",
		gettext(CGI_STR_NOTE), gettext("Gain Control"),
		gettext("is implemented by Assist Software DSP"),
		gettext("When 0 dB is selected, Software DSP is disabled"),
		gettext("The larger of volume gain is, then more noise is added at same time") );


	return strdup(buf);

}


int	sipUa_gain_control(llist *cgiVariables)
{
	char		*action;

	list_create(&gainsCfgs);

	if (readconfig(SIP_UA_CONFIG, ITEM_TRI, NULL, &gainsCfgs) != 0)
	{
		return -1;
	}

	action = GET_VALUE(cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Gain Control"), "", __sipUa_gain_button(cgiVariables) );
		return 0;
	}
	else if(!strcasecmp(action, CGI_ACTION_SAVE)  )
	{
		list_replace_with_new_list(&gainsCfgs, cgiVariables );
		return sipUa_write_config_file(&gainsCfgs, GET_VALUE(cgiVariables,CGI_KEYWORD_OPERATOR) );
	}

	return 0;
}


