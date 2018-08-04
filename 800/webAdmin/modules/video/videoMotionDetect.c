/*
* $Id$
*/

#include "_video.h"



static char *__video_motion_status(VIDEO_INFO *info )
{
	char 	buf[8192*5];
	int 		length = 0;
	int		isTrue = 0;
	
	length += CGI_SPRINTF(buf, length, "<form name=\"motion\" method=\"post\" action=\"%s\">\n", WEB_URL_VIDEO );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );

/* Sensitity Config */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Sensitivity Configuration") );

	isTrue = (strcasecmp(GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MOTION_ENABLE), "1")==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD>", gettext("Enabled or Disabled Motion Detection"), gettext("Motion Detection") );
	length += CGI_SPRINTF(buf,length, "<TD width=\"50%%\"><input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s</TD></TR>\r\n", 
		VIDEO_KEYWORD_MOTION_ENABLE, 1, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), VIDEO_KEYWORD_MOTION_ENABLE, 0, (isTrue==0)?"checked":"", gettext(CGI_STR_OFF) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Sensitivity is value from 0~xx"), gettext("Sensitivity"), VIDEO_KEYWORD_SENSITIVITY, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_SENSITIVITY) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Threshold is value from 0~xx"), gettext("Threshold"), VIDEO_KEYWORD_THRESHOLD, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_THRESHOLD) );

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );

/* Mail Config */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += sprintf(buf+length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", 
		gettext("Mail Configuration") );

	isTrue = (strcasecmp(GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MAIL_ENABLE), "1")==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD>", gettext("Enabled or Disabled Mail Alert"), gettext("Mail Alert") );
	length += CGI_SPRINTF(buf,length, "<TD width=\"50%%\"><input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s</TD></TR>\r\n", 
		VIDEO_KEYWORD_MAIL_ENABLE, 1, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), VIDEO_KEYWORD_MAIL_ENABLE, 0, (isTrue==0)?"checked":"", gettext(CGI_STR_OFF) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("no more than 20 words"), gettext("Mail Subject"), VIDEO_KEYWORD_MAIL_SUBJECT, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MAIL_SUBJECT) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Mail Address, such as abcd@efgh.com"), gettext("Mail Recipients"), VIDEO_KEYWORD_MAIL_RECIPIENTS, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MAIL_RECIPIENTS) );
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("IP Address or FQDN of Mail Server"), gettext("Mail Server"), VIDEO_KEYWORD_MAIL_SERVER, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MAIL_SERVER) );

	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(UserName,'%s')\"  type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Account on this mail server"), CGI_STR_USER_NAME, VIDEO_KEYWORD_MAIL_USER,  gettext("User Name can not be NULL") , GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MAIL_USER) );
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s</TD>", gettext("Pass word for this mail account"), gettext(CGI_STR_PASSWORD) );
	length += CGI_SPRINTF(buf, length, "<TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Password,'%s')\"  type=\"password\" size=\"12\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n",
		VIDEO_KEYWORD_MAIL_PASSWD, gettext("Password can not be NULL"), GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_MAIL_USER)  );

	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );


/* FTP Config */
	length += CGI_SPRINTF(buf,length, "<table width=\"85%%\" border=\"1\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\" bgcolor=\"#FFFFFF\" >\n");
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"center\" colspan=2 bgcolor=\"#cccccc\"><strong>%s</strong></TD></TR>", gettext("FTP Configuration") );

	isTrue = (strcasecmp(GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_FTP_ENABLE), "1")==0)?1:0;
	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD>", gettext("Enabled or Disabled FTP Alert"), gettext("FTP Alert") );
	length += CGI_SPRINTF(buf,length, "<TD width=\"50%%\"><input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s<input name=\"%s\" type=\"Radio\" value=\"%d\" %s>%s</TD></TR>\r\n", 
		VIDEO_KEYWORD_FTP_ENABLE, 1, (isTrue==1)?"checked":"", gettext(CGI_STR_ON), VIDEO_KEYWORD_FTP_ENABLE, 0, (isTrue==0)?"checked":"", gettext(CGI_STR_OFF) );

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("IP Address or FQDN of FTP Server"), gettext("FTP Server"), VIDEO_KEYWORD_FTP_SERVER, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_FTP_SERVER) );

	length += CGI_SPRINTF(buf, length, "<TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(UserName,'%s')\"  type=\"text\" size=\"10\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("FTP Account name"), CGI_STR_USER_NAME, VIDEO_KEYWORD_FTP_USER,  gettext("User Name can not be NULL"), GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_FTP_USER)  );
	length += CGI_SPRINTF(buf, length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s</TD>", gettext("Pass word for this FTP account"), gettext(CGI_STR_PASSWORD) );
	length += CGI_SPRINTF(buf, length, "<TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(Password,'%s')\"  type=\"password\" size=\"12\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n",
		VIDEO_KEYWORD_FTP_PASSWD, gettext("Password can not be NULL"), GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_FTP_PASSWD));

	length += CGI_SPRINTF(buf,length, "<TR><TD align=\"right\" title=\"%s\" width=\"50%%\">%s:</TD><TD width=\"50%%\"><input name=\"%s\" onKeyPress=\"checkNumber(this)\" type=\"text\" size=\"20\" value=\"%s\" maxlength=\"50\"></TD></TR>\r\n", 
		gettext("Path name in remote FTP server"), gettext("Remote Path"), VIDEO_KEYWORD_FTP_REMOTE_PATH, GET_VALUE(&info->motionCfgs, VIDEO_KEYWORD_FTP_REMOTE_PATH) );
	length += CGI_SPRINTF(buf,length, "</TABLE>\r\n" );


	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=2 align=\"center\">%s\r\n",cgi_submit_button( gettext(CGI_STR_ACTIVATE) ) );
	length += CGI_SPRINTF(buf,length, "\t\t%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MOTION_DETECT) );

	return strdup(buf);
}


int	videoMotionForm(VIDEO_INFO *info )
{
	char *action;

	action = GET_CGI_ACTION(&info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Media Configuration"), gettext("Basic parameters of Motion Detection are set here"), __video_motion_status(info) );
		return 0;
	}	
	else if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
	{
		return cgi_camera_save_config(info );
	}
	else
	{
		cgi_invalidate_page();
	}
	
	return 0;
}

