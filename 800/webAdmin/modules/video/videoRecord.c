/*
* $Id: videoRecord.c,v 1.4 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "_video.h"
static char	indextable[35][256];
//#define	CAM_RECORD_INDEX		"index"
#if 0    
static char *_get_avi_files(char *dirName)
{
	struct dirent	**namelist;
	struct stat 	fstats;
	char			filename[256];
	int n, i = 0;
	char		buf[4096*5];
	int		length = 0;
	
	n = scandir( dirName, &namelist, NULL, alphasort);
	while(n--)
	{
		if( !strcasecmp(namelist[n]->d_name, ".") ||!strcasecmp(namelist[n]->d_name, "..") )
			continue;
		sprintf(filename, "%s/%s", dirName, namelist[n]->d_name );
		stat(filename, &fstats);
		
		length += CGI_SPRINTF(buf,length, "<TR><TD >%s</TD><TD >%ld KB</TD></TR>\r\n", 
			namelist[n]->d_name, KB(fstats.st_size) );
		
		free(namelist[n]);
		i++;
	}
		
	free(namelist);
	return strdup(buf);
}
#endif

static char *_get_avi_files(char *dirName) 
{
	struct dirent	**namelist;
	struct stat 	fstats;
	char			filename[256];
	int 			n, i ,t= 0;
	char			buf[4096*5];
	char			timebuf[256];
	int			length = 0;
	
	n = scandir( dirName, &namelist, NULL, alphasort);
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=3>%s</TD><TD >%s</TD><TD colspan=2>%s</TD></TR>\r\n", 
			gettext("filename"),gettext("filesize"),gettext("modifytime"));
	while(n--)
	{
		if( !strcasecmp(namelist[n]->d_name, ".") ||!strcasecmp(namelist[n]->d_name, "..") )
			continue;
		sprintf(filename, "%s/%s", dirName, namelist[n]->d_name );
		stat(filename, &fstats);


		struct tm *tm_temp;
		tm_temp=localtime(&fstats.st_mtime);
		strftime(timebuf,256,"%F:%H:%M:%S",tm_temp);
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=3><a href=\"/record/avis/%s\">%s</a></TD><TD >%ld KB</TD><TD colspan=3>%s</TD>", 
			namelist[n]->d_name,namelist[n]->d_name,  KB(fstats.st_size),timebuf);
		
		sprintf(indextable[t],"%s",filename);

		
		char target[128];
		sprintf(target, "%s?%s=%s&%s=%s&Index=%d", WEB_URL_VIDEO,CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_RECORD, CGI_ACTION, CGI_ACTION_DEL,t);

		length += CGI_SPRINTF(buf,length, "<TD>%s</TD></TR>\r\n", cgi_button(gettext(CGI_STR_DELETE), target) );	
		t++;
		free(namelist[n]);
		i++;
	}
		
	free(namelist);
	return strdup(buf);
}

static char *_video_record_status_page(VIDEO_INFO *info)
{
	char 	buf[8192*20];
	char		buftemp[8192];
	char 	target[128];
	int 		length = 0;
	
#if 0
	int		i;
	char	target[512];
	homeDir = GET_VALUE(&info->globalCfgs, VIDEO_KEYWORD_CAMERA_VIDEO_DATA_DIR);
	sprintf(key, "%s/cam%d", homeDir, i);
	length += CGI_SPRINTF(buf,length, "%s\r\n", _get_avi_files(key) );
#endif
	length += CGI_SPRINTF(buf,length, "<TR><TD >%s</TD></TR>\r\n",gettext("Record history"));
	length += CGI_SPRINTF(buf,length, "%s\r\n", _get_avi_files("/tmp/record/avis") );//
	length += CGI_SPRINTF(buf, length, "<form name=\"videoRecord\" method=\"post\" action=\"%s\">\n", WEB_URL_VIDEO );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_KEYWORD_OPERATOR, GET_CGI_OP(&info->cgiVariables) );
	length += CGI_SPRINTF(buf, length,  "<input type=\"hidden\" name=\"%s\" value=\"%s\">\n", CGI_ACTION, CGI_ACTION_ACTIVATE );

	sprintf(buftemp,"%s",autorecord_plan_history_page(info));
	length += CGI_SPRINTF(buf, length, "%s",buftemp);
	


	
	length += CGI_SPRINTF(buf,length, "<TR><TD >%s</TD></TR>\r\n",gettext("Record config"));	


	length += CGI_SPRINTF(buf,length, "<TR><TD >%s</TD>",gettext("Start:"));
	length += CGI_SPRINTF(buf,length, "<TD><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(hour,'%s')\"  type=\"text\" size=\"6\" value=\"%s\" maxlength=\"8\"></TD>", 
		"hour",gettext("hour can not be NULL"), "");
	length += CGI_SPRINTF(buf,length, "<TD >%s</TD>",gettext("HOUR"));
	length += CGI_SPRINTF(buf,length, "<TD><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(minutes,'%s')\"  type=\"text\" size=\"6\" value=\"%s\" maxlength=\"8\"></TD>", 
		"minutes",gettext("minutes can not be NULL"), "");
	length += CGI_SPRINTF(buf,length, "<TD>%s</TD>",gettext("MIN"));

	length += CGI_SPRINTF(buf,length, "<TD ><select name=\"dayselect\" ><option value=\"1\" >%s</option><option value=\"2\" >%s</option><option value=\"3\" >%s</option><option value=\"4\" >%s</option>","Mon","Tue","Wed","Thur");
	length += CGI_SPRINTF(buf,length, "<option value=\"5\" >%s</option><option value=\"6\" >%s</option><option value=\"7\" >%s</option></select></TD></TR>\r\n","Fri","Sat","Sun");	


	length += CGI_SPRINTF(buf,length, "<TD >%s</TD>",gettext("Timelength"));
	length += CGI_SPRINTF(buf,length, "<TD><input name=\"%s\" onKeyPress=\"\" onChange=\"checkIsNotNULL(time,'%s')\"  type=\"text\" size=\"6\" value=\"%s\" maxlength=\"8\"></TD>", 
		"time",gettext("timelength can not be NULL"), "");
	length += CGI_SPRINTF(buf,length, "<TD >%s</TD></TR>\r\n",gettext("MIN"));	
	
	
	
	length += CGI_SPRINTF(buf,length, "<TD >%s</TD>\r\n",cgi_submit_button( gettext("record") ) );
	sprintf(target, "%s?%s=%s&%s=%s", WEB_URL_VIDEO,CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_RECORD, CGI_ACTION, "stop");
	
		
	length += CGI_SPRINTF(buf,length, "<TD colspan=2 align=\"right\">%s</TD></TR></FORM>\r\n" , cgi_help_button(CGI_HELP_MSG_MEDIA_CONFIG) );


	return strdup(buf);
}


char *autorecord_plan_history_page(VIDEO_INFO *info)
{	
	char 	buf[8192];
	char		hour[4];
	char		min[4];
	char		timelength[8];
	char	     	valuetemp[32];
	char		week[8];
	int 		length = 0;
	int		t=0;
	node* 	current;
	llist*		list=&info->recordCfgs;	
	
	length += CGI_SPRINTF(buf,length, "<TR><TD colspan=3>%s</TD></TR>\r\n",gettext("Record config history"));	
	current = list->head;
	while (current != 0) 
	{
		if(!strcasecmp(current->entry.name,"type"))
		{
			current = current->next;
			continue;
		}
		sprintf(valuetemp,"%s",current->entry.value);
		hour[0]=valuetemp[0];
		hour[1]=valuetemp[1];
		min[0]=valuetemp[2];
		min[1]=valuetemp[3];
	
		sprintf(timelength,"%s",valuetemp+6);
		switch(atoi(&valuetemp[4]))
		{
			case 1:	sprintf(week,"%s","Mon");	break;
			case 2:	sprintf(week,"%s","Tue");	break;
			case 3:	sprintf(week,"%s","Wed");	break;
			case 4:	sprintf(week,"%s","Thur");	break;
			case 5:	sprintf(week,"%s","Fri");	break;
			case 6:	sprintf(week,"%s","Sat");	break;
			case 7:	sprintf(week,"%s","Sun");	break;
		}
		
		length += CGI_SPRINTF(buf,length, "<TR><TD colspan=3>%s%s\t%s\t%s%s%s</TD><TD colspan=3>%s%s%s</TD>\r\n",gettext("starttime:"),week,hour,gettext("Hour"),min,gettext("Min"),gettext("timelength:"),timelength,gettext("minutes"));
		char target[128];
		sprintf(target, "%s?%s=%s&%s=%s&index=%d", WEB_URL_VIDEO,CGI_KEYWORD_OPERATOR, CGI_VIDEO_OP_RECORD, CGI_ACTION, "taskdelete",(t+1));
		length += sprintf(buf+length, "<TD></TD><TD>%s</TD></TR>\r\n", cgi_button(gettext(CGI_STR_DELETE), target) );	
		t++;
		current = current->next;		
	}
	
	return strdup(buf);

}
#if 0
int	videoRecordStatus(VIDEO_INFO *info)
{
	char target[256];
	char *action;

	action = GET_CGI_ACTION( &info->cgiVariables);
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext(CGI_STR_RECORD), gettext("IP Camera Record"), _video_record_status_page(info));
		return 0;
	}	

	videoActionButton(info, CMN_CMD2SERVER_VIDEO_RECORD );

	CGI_SPRINTF(target, 0, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"=%s", GET_CGI_OP(&info->cgiVariables) );

	/* 10 seconds is recorded on media server */
	cgi_refresh_page(12, target, gettext(CGI_STR_PLEASE_WAIT), gettext("Media is recording.....") );

	return 0;
}
#endif


int	videoRecordStatus(VIDEO_INFO *info)
{
	char 	target[256];
	char 	*action;
	char 	*index;
	char		*taskindex;	
	int 		indextemp;
	int 		taskindextemp;
	int		returnstatus;

	action = GET_CGI_ACTION( &info->cgiVariables);
	cgidebug("the action is %s\n",action);

	CGI_SPRINTF(target, 0, WEB_URL_VIDEO"?"CGI_KEYWORD_OPERATOR"=%s", GET_CGI_OP(&info->cgiVariables) );
	cgi_info_page(gettext(CGI_STR_RECORD), " ", _video_record_status_page(info));	

	if(action || strlen(action) )
		{
		if(!strcasecmp(action,CGI_ACTION_ACTIVATE))
			{					
				returnstatus=check_cfgdata(info);
				if(returnstatus)
					{							
						switch(returnstatus)
							{
								case 1:	status_page_adding_deleting(target,2,gettext("HOUR  CAN  NOT  BE  NULL!!!"));						break;
								case 2:	status_page_adding_deleting(target,2,gettext("MINUTES  CAN  NOT  BE  NULL!!!"));					break;
								case 3:	status_page_adding_deleting(target,2,gettext("TIMELENGTH  CAN  NOT  BE  NULL!!!"));				break;
								case 4:	status_page_adding_deleting(target,2,gettext("WRONG     INPUT ,  PLEASE CHECK AND RETRY!!!"));	break;
								case 5:	status_page_adding_deleting(target,2,gettext("TIMELENGTH  RANGE WRONG!!!"));					break;
								case 6:	status_page_adding_deleting(target,2,gettext("PLAN CONFLICT , PLEASE CHECK AND RETRY!!!"));		break;
								default:  break;
							}
						return 0;
							
					}
				effective_record_cfgfile(info);
				effective_crontables(info);
				system("killall -9 crond");
				system("crond -b");			
				status_page_adding_deleting(target,1,gettext("configuring ,please wait......"));		
				 return 0;
			}
	if(!strcasecmp(action,CGI_ACTION_DEL))
		{
			char		cmd[128];
			index=GET_VALUE(&info->cgiVariables, "Index");
			indextemp=atoi(index);
			sprintf(cmd,"rm %s",indextable[indextemp]);
			system(cmd);	
			status_page_adding_deleting(target,1,gettext("deleting ,please wait...."));
			return 0;				
		}	
	if(!strcasecmp(action,"taskdelete"))
		{
			taskindex=GET_VALUE(&info->cgiVariables, "index");
			taskindextemp=atoi(taskindex);
			list_delete_by_index(&info->recordCfgs,"task",taskindextemp);
			writeconfig(RECORD_CONFIG_FILE, ITEM_DBL, NULL, &info->recordCfgs);
			remove_task_crontables(taskindextemp);
			status_page_adding_deleting(target,1,gettext("deleting ,please wait...."));		
			return 0;				
		}	
	}

	return 0;
}


 int  check_cfgdata(VIDEO_INFO *info)
{
	
	char 	exist_hour[2];
	char		exist_min[2];
	int 		exist_week;
	char		exist_timelength[8];
	int		new_hour;
	int		new_min;
	int		new_week;
	int		new_timelength;
	int   		add;
	int		residue;
	char 	temp[12];
	node* 	current;
	llist*		list=&info->recordCfgs;

	if(strlen(GET_VALUE(&info->cgiVariables, "hour"))==0)							
		return 1;						
	if(strlen(GET_VALUE(&info->cgiVariables, "minutes"))==0)							
		return 2;							
	if(strlen(GET_VALUE(&info->cgiVariables, "time"))==0)
		return 3;					
	if(atoi(GET_VALUE(&info->cgiVariables, "hour"))<0||atoi(GET_VALUE(&info->cgiVariables, "hour"))>23||atoi(GET_VALUE(&info->cgiVariables, "minutes"))>59||atoi(GET_VALUE(&info->cgiVariables, "minutes"))<0)				
		return 4;
	if(atoi(GET_VALUE(&info->cgiVariables, "time"))<=0||atoi(GET_VALUE(&info->cgiVariables, "time"))>1440)
		return 5;

	
	new_hour=atoi(GET_VALUE(&info->cgiVariables, "hour"));
	new_min=atoi(GET_VALUE(&info->cgiVariables, "minutes"));
	new_timelength=atoi(GET_VALUE(&info->cgiVariables, "time"));
	new_week=atoi(GET_VALUE(&info->cgiVariables, "dayselect"));
	current = list->head;
	while (current != 0) 
	{	   
			if(!strcasecmp(current->entry.name,"type"))
				{
					current = current->next;
					continue;
				}
			sprintf(temp,"%s",current->entry.value);
			sprintf(exist_timelength,"%s",temp+6);
			
			exist_hour[0]=temp[0];
			exist_hour[1]=temp[1];
			exist_min[0]=temp[2];
			exist_min[1]=temp[3];
			exist_week=atoi(&temp[4]);
			if(atoi(&temp[4])==new_week)
				{
					if(atoi(exist_hour)==new_hour)
						{
							if(atoi(exist_min)>new_min)
								{	

									if((new_min+new_timelength)>=atoi(exist_min))
										return 6;
										current = current->next;
										continue;
								}
							if(atoi(exist_min)<new_min)
								{

									if((atoi(exist_min)+atoi(exist_timelength))>=new_min)
										return 6;

										current = current->next;
										continue;
								}

								return 6;							
						}
					if(atoi(exist_hour)<new_hour)
						{
							add=(atoi(exist_min)+atoi(exist_timelength))/60;
							residue=(atoi(exist_min)+atoi(exist_timelength))%60;
							if((atoi(exist_hour)+add)>new_hour)
								return 6;
							if((atoi(exist_hour)+add)==new_hour)
								{
									if(residue>=new_min)
										return 6;
									else
										current = current->next;
										continue;
								}
								current = current->next;
								continue;
							
						}
					add=(new_min+new_timelength)/60;					
					residue=(new_min+new_timelength)%60;
					if((new_hour+add)>atoi(exist_hour))
						return 6;
					if((new_hour+add)==atoi(exist_hour))
						{
							if(residue>=atoi(exist_min))
								return 6;
							else
							current = current->next;
							continue;
						}
					current = current->next;
					continue;
					
				}
			current = current->next;
			continue;
		
	}

	return 0;	
}


int remove_task_crontables(int taskindex)
{	
	FILE 	*fp;
	char 	*rep;
	char  	buf[128];
	char	  	temp[22][128];
	int 		i=1;
	int		t;
	
	if((fp=fopen(CRONTASKFILE,"r"))==NULL)
	{	
		return	-1;
	}
	
	while((rep=fgets(buf, 128, fp)) != NULL)
	{	
		memset(temp[i], 0, sizeof(temp[i]));
		sprintf(temp[i],"%s",rep);
		i++;
	}
	fclose(fp);
	t=i;
	i=1;

	
	if((fp=fopen(CRONTASKFILE,"w"))==NULL)
	{	
		return	-1;
	}
	
	while(t>1)
	{
		if(i==(taskindex))
		{
			i++;
			t--;
			continue;
		}
		fputs(temp[i],fp);
		i++;
		t--;
	}
	fclose(fp);	

	return 0;

}


int  effective_record_cfgfile(VIDEO_INFO *info)
{
	llist*		list=&info->recordCfgs;		
	char		nodevalue[64];	
	char		recordstarttime[64];
	char		*timevalue;
	char		hour[8];
	char		min[8];

	if(strlen(GET_VALUE(&info->cgiVariables, "minutes"))==1)
		{
			sprintf(min,"0%s",GET_VALUE(&info->cgiVariables, "minutes"));
		}
	else
		sprintf(min,"%s",GET_VALUE(&info->cgiVariables, "minutes"));
	if(strlen(GET_VALUE(&info->cgiVariables, "hour"))==1)
		{
			sprintf(hour,"0%s",GET_VALUE(&info->cgiVariables, "hour"));
		}
	else
		sprintf(hour,"%s",GET_VALUE(&info->cgiVariables, "hour"));				
	timevalue=GET_VALUE(&info->cgiVariables, "time");						
	 sprintf(recordstarttime, "%s%s%s",hour,min,GET_VALUE(&info->cgiVariables, "dayselect"));
	
	sprintf(nodevalue,"%s:%s",recordstarttime,timevalue);
	list_append_raw(list, "task", nodevalue, NULL, NULL);
	if(writeconfig(RECORD_CONFIG_FILE, ITEM_DBL, NULL, &info->recordCfgs) )
		{
			return -1;
		}
	
	return 0;
}


int  effective_crontables(VIDEO_INFO *info)
{
	FILE 	*fp;
	int 		t,i=1;
	char 	*rep;
	char		*min;
	char		*hour;
	char		*week;
	char 	buf[128];
	char	  	temp[22][128];

	
	if((fp=fopen(CRONTASKFILE,"r"))==NULL)
		{	
			return	-1;
		}
	while((rep=fgets(buf, 128, fp)) != NULL)
		{			
			sprintf(temp[i],"%s",rep);
			i++;
		}
	fclose(fp);
	t=i;
	i=1;		
		
	hour=GET_VALUE(&info->cgiVariables, "hour");
	min=GET_VALUE(&info->cgiVariables, "minutes");
	week=GET_VALUE(&info->cgiVariables, "dayselect");
	memset(temp[t], 0, sizeof(temp[t]));
	sprintf(temp[t],"%s %s * * %s record.sh\n",min,hour,week);
	if((fp=fopen(CRONTASKFILE,"w"))==NULL)
		{	
			return	-1;
		}
	while(t>0)
		{	
			fputs(temp[i],fp);
			i++;
			t--;
		}
	fclose(fp);
	
	return 0;			
}


int status_page_adding_deleting(char *target,int delay,char *msg)
{
	printf("<HTML><HEAD>	<META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=utf-8\"><TITLE></TITLE>\r\n");
	printf("<META HTTP-EQUIV=\"Expires\" CONTENT=\"now\"><META HTTP-EQUIV=\"Pragma\" CONTENT=\"no-cache\">\r\n"); 
	printf("<SCRIPT SRC=\"/include/help.js\"></SCRIPT>\r\n");
	printf("<META HTTP-EQUIV=\"Refresh\" CONTENT=\"%d;URL=%s\">\r\n", delay, target);
	printf("</HEAD><BODY><CENTER>\r\n");
	printf("<TBODY> <TR> <TD><p><strong>%s</strong></p></TD></TR></TBODY></TABLE><TD class=vline rowSpan=30><BR></TD></TR><TR>   <TD class=hline></TD></TR> </TBODY></TABLE></TD></TR></TBODY></TABLE>\r\n", msg);
	printf("</CENTER></BODY></HTML>\r\n");
	return 0;
}

