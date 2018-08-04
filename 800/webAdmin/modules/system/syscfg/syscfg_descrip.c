/*
* $Id: syscfg_hostname.c,v 1.3 2007/09/06 09:19:58 lizhijie Exp $
*/

#include "sysconf.h"
#define	CGI_CAMERA_ADDRESS			"address"
#define CGI_CAMERA_ID					"cameraID"
#define CGI_CAMERA_RESPONSIBLE				"responsible"

static char *get_cameraIP()
{
	FILE * fp;
	char		*ret = NULL;
	char line[1024];
	char hostname[128];
	char fqdn[128];
	char ip[20];
	if( (fp=fopen(HOSTNAME_CONF,"r") )==NULL)
	{
		cgidebug("open %s file error\n" , HOSTNAME_CONF);
		return strdup("");
	}

	while( (ret=fgets(line,sizeof(line)-1,fp) )!=NULL)
	{
		if( (line[0]=='#') || (line[0]=='\0')|| (line[0]=='\r') || (line[0]=='\n') )
			continue;
		memset(hostname, 0, sizeof(hostname));
		memset(ip, 0, sizeof(ip));
		memset(fqdn, 0, sizeof(fqdn));
		sscanf(line,"%s %s %s",ip, hostname, fqdn );
		if( strstr(hostname,"localhost")  || !strcmp(ip,"127.0.0.1") )
			continue ;
		else
			break;
		
		memset(line,0,sizeof(line));
	}
	
	fclose(fp);

	return strdup(ip);
	
}

static char *__cgi_output_descrip()
{
	FILE * fp;
	char		*ret = NULL;
	char line[1024];
	char address[1024];
	char responsible[1024];
	char cameraNO[16];
	char msg[4096];
	int length = 0;
	int i = 0,j = 0;
	int line_no = 0;
	if( (fp=fopen(CAMERA_DECRIP_CONF,"r") )==NULL)
	{
		cgidebug("open %s file error\n" , CAMERA_DECRIP_CONF);
		return strdup("");
	}
	
	memset(line,0,sizeof(line));
#if 0
	length += sprintf(msg+length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td>",
		gettext("Hostname"), gettext("FQDN"), gettext(CGI_STR_IP_ADDRESS) );
#else
	//length += CGI_SPRINTF(msg, length, "<tr><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td><td align=\"center\" bgcolor=\"#cccccc\"><strong>%s</strong></td>",
	//	gettext("Address"), gettext("Responsible"), gettext(CGI_STR_IP_ADDRESS) );
#endif

	while( (ret=fgets(line,sizeof(line)-1,fp) )!=NULL)
	{
		if( (line[0]=='#') || (line[0]=='\0')|| (line[0]=='\r') || (line[0]=='\n') )
			continue;
		
		memset(address, 0, sizeof(address));
		//memset(ip, 0, sizeof(ip));
		memset(responsible, 0, sizeof(responsible));
		for(i = 0;i < 1024; i++)
		{
			if(line[i] == '=')
			{
				i++;
				break;
			}
		}
		if(strstr(line,"address"))
		{
			for(;i < 1024 && line[i] != '\n' && line[i] != '\0' && line[i] != '\r';i++,j++)
			{
				address[j] = line[i];
			}
			address[++j] = '\0';
		}
		else if(strstr(line,"cameraID"))
		{
			for(;i < 16 && line[i] != '\n' && line[i] != '\0' && line[i] != '\r';i++,j++)
			{
				cameraNO[j] = line[i];
			}
			cameraNO[j] = '\0';
		}
		else
		{
			for(;i < 1024 && line[i] != '\n' && line[i] != '\0' && line[i] != '\r';i++,j++)
			{
				responsible[j] = line[i];
			}
			responsible[++j] = '\0';
		}
		i = 0;
		j = 0;
		
		
#if 0		
		length += sprintf(msg+length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",hostname);
//		printf("<td align=\"center\"><font size=2>%s&nbsp </font></td>" ,fqdn);
		length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s.%s&nbsp </font></td>" , hostname, cgi_domain_name_read() );
		length += sprintf(msg+length, "<td align=\"center\"><font size=2>%s&nbsp </font></td></tr>" ,ip); 
#else
		if(0 == line_no)
		{
			//length += CGI_SPRINTF(msg, length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",address);
			//length += CGI_SPRINTF(msg,length, "<tr>\n\t<td><strong>%s<p></strong></TD></TR>\n", gettext("Camera Description") );
	//fprintf(stderr,"************Ipcamera cameraNO is '%s' \n", cameraNO);

		length += sprintf(msg+length, "<TR>\n\t<TD align=\"center\">%s</TD>\n\t<TD align=\"center\">%s</TD>\n</TR>\n", 
			"摄像头编号",cameraNO);
		line_no++;

		}
		else if(1 == line_no)
		{
			//length += CGI_SPRINTF(msg, length, "<tr><td align=\"center\"><font size=2>%s&nbsp </font></td>",address);
			//length += CGI_SPRINTF(msg,length, "<tr>\n\t<td><strong>%s<p></strong></TD></TR>\n", gettext("Camera Description") );
	//fprintf(stderr,"************Ipcamera address is '%s' \n", address);

		length += sprintf(msg+length, "<TR>\n\t<TD align=\"center\">%s</TD>\n\t<TD align=\"center\">%s</TD>\n</TR>\n", 
			"摄像头地址",address);
		line_no++;

		}
		else
		{
			length += sprintf(msg+length, "<TR>\n\t<TD align=\"center\">%s</TD>\n\t<TD align=\"center\">%s</TD>\n</TR>\n", 
			"负责人",responsible);
			length += sprintf(msg+length, "<TR>\n\t<TD align=\"center\">%s</TD>\n\t<TD align=\"center\">%s</TD>\n</TR>\n", 
			"IP地址",get_cameraIP());
			
		}
#endif
		memset(line,0,sizeof(line));
	}
	
	fclose(fp);

	return strdup(msg);
	
}

static char *__cgi_host_add_buttons()
{
	char buf[4096*2];
	int 	length = 0;

	length += CGI_SPRINTF(buf, length, "<form name=\"cameraDescripForm\" method=\"post\" action=\"%s\">\n", WEB_URL_SYSCONFIG );
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, CGI_SYSCFG_OP_DESCRIPTION);
	length += CGI_SPRINTF(buf, length, "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_MODIFY );

	length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		"摄像头编号" );

	length += CGI_SPRINTF(buf, length, "<TD  align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n",
		CGI_CAMERA_ID);
	
	length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		"摄像头地址" );
	length += CGI_SPRINTF(buf, length, "<TD  align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n",
		CGI_CAMERA_ADDRESS);

	length += CGI_SPRINTF(buf, length, "<TR><TD  align=\"right\"><strong>%s</strong></TD>", 
		"负责人" );
	length += CGI_SPRINTF(buf, length, "<TD  align=\"left\"><input name=\"%s\" type=\"text\" size=\"15\" value=\"\" maxlength=\"50\"></TD></TR>\r\n",
		CGI_CAMERA_RESPONSIBLE);
	
	length += CGI_SPRINTF(buf, length,  "<TR><TD  align=\"center\"><input type=\"submit\" name=\"WriteToConf\" value=\"%s\" class=\"button\"></TD>\
		<TD  align=\"center\"><input type=\"reset\" name=\"WriteToConf\" value=\"%s\" class=\"button\"></TD></TR>\r\n", \
		"修改","重置");
	
	return strdup(buf);
}

int cgi_descrip_info_page( char *header, char *infomsg, char *othermsg)
{
	printf("<HTML><HEAD>	<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=gb2312\"><TITLE></TITLE>\r\n");
	printf("<META HTTP-EQUIV=\"Expires\" CONTENT=\"now\"><META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\r\n"); 
	printf("<SCRIPT SRC=\"/include/help.js\"></SCRIPT>\r\n");
	printf("</HEAD><LINK href=\"/nb.css\" rel=stylesheet type=text/css></HEAD>\r\n<BODY><CENTER>\r\n");
	printf("\t<TABLE border=0 cellPadding=0 cellSpacing=0 width=502><TBODY>\r\n");
	/* header line */
	printf("\t\t<TR><TD class=title width=7><IMG height=24 src=\"/arc.gif\" width=7></TD><TD align=left class=title vAlign=center width=495>%s</TD></TR>\r\n", header);
	printf("\t\t<TR><TD colSpan=2>\r\n");
	printf("\t\t\t<TABLE border=0 cellPadding=0 cellSpacing=0  width=502><TBODY>\r\n");
	printf("\t\t\t\t<TR><TD class=vline rowSpan=15><BR></TD><TD width=500>\r\n");
#if 1	
	printf("\t\t\t\t\t<TABLE align=center border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\"  class=space width=400>\r\n");
#else/* lzj , 2007.05.31 */
	printf("\t\t\t\t\t<TABLE align=center border=\"1\" align=\"center\" cellpadding=\"1\" cellspacing=\"1\"  class=space \"85%%\">\r\n");
#endif
	printf("%s\r\n", infomsg);
	printf("\t\t\t\t\t</TABLE>");
	
	if(othermsg)
	{
		printf("\r\n\t\t\t\t\t<TABLE align=center border=\"0\" align=\"center\" cellpadding=\"0\" cellspacing=\"0\"  class=space width=400>\r\n");
		printf("%s\r\n", othermsg );
		printf("\t\t\t\t\t</TABLE>\r\n\t\t\t\t");
	}
	
	printf("\r\n\t\t\t\t</TD><TD class=vline rowSpan=15><BR></TD></TR>\r\n\t\t\t\t<TR><TD class=hline></TD></TR>\r\n");
	printf("\r\n\t\t\t</TBODY></TABLE>\r\n");
	printf("\r\n\t\t</TD></TR>\r\n\t</TBODY></TABLE>\r\n</CENTER></BODY>\r\n</HTML>\r\n");

	return 0;
}
int sysconfig_descrip(SYSCFG_INFO *info)
{

	//char *host=NULL;
	//char *domain = NULL;
	//char cmd[128];
	char *cameraID = NULL;
	char *cameraAddress = NULL;
	char *cameraInCharge = NULL;
	char *action = NULL;

	action = GET_CGI_ACTION(&info->cgiVariables );
	if(action!= NULL && !strcasecmp(action, CGI_ACTION_MODIFY) )
	{
		cameraID = GET_VALUE(&info->cgiVariables,CGI_CAMERA_ID);
		if(!cameraID ||strlen(cameraID) == 0 )
			return cgi_error_page(NULL, gettext("Not Found"), gettext("Field of hostname is not found") );
		cameraAddress = GET_VALUE(&info->cgiVariables,CGI_CAMERA_ADDRESS);
		if(!cameraAddress||strlen(cameraAddress) == 0 )
			return cgi_error_page(NULL, gettext("Not Found"), gettext("Field of hostname is not found") );
		cameraInCharge = GET_VALUE(&info->cgiVariables,CGI_CAMERA_RESPONSIBLE);
		if(!cameraInCharge||strlen(cameraInCharge) == 0 )
			return cgi_error_page(NULL, gettext("Not Found"), gettext("Field of hostname is not found") );
		
		FILE *fp;
	
		if(( fp = fopen(CAMERA_DECRIP_CONF,"w"))==NULL)
		{
			cgidebug("Open file %s failure!", CAMERA_DECRIP_CONF );
			return -1;
		}
	
		fprintf(fp, "#Description for the IP Camera\n\n");
		fprintf(fp, "cameraID=%s\n" , cameraID);
		fprintf(fp, "address=%s\n" , cameraAddress);
		fprintf(fp, "responsible=%s\n" , cameraInCharge);
		fclose(fp);
	}

	cgi_descrip_info_page("摄像头描述信息", __cgi_output_descrip(), __cgi_host_add_buttons() );
	
	return 0;
	
}
