

#include "_video.h"


int main(int argc, char *argv[])
{


	int 		t,i,m=0;
	char 	temp[32];
	char		key[32];
	char		value[8];
	char		*p;
	printf("<HTML><HEAD>	\r\n<META NAME=\"description\" CONTENT=\"SONIX IPCAM\">\r\n<title>SONiX Technology, Inc.VideoView</title>\r\n");	
	printf("<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"std.css\"></HEAD>\r\n");

	
	printf("<BODY marginheight=\"0\" marginwidth=\"0\" leftmargin=\"0\" topmargin=\"0\" ><CENTER>\r\n");
	
	printf("\r\n</CENTER></BODY>\r\n</HTML>\r\n");	
	
	
	sprintf(temp,"%s",QUERY_STRING);

	

	p=strstr(temp,"&");
	m=p-temp;

	i=0;
	while(i<m)
		{
			key[i]=temp[i];
			i++;
		
		}
	i=0;
	t=m+1;
	while(t<strlen(temp))
		{
			value[i]=temp[t];
			i++;
			t++;
		}
	

	cgidebug("value:key= %s:%s\n",value,key);
	cgitrace;
	
	if(!strcasecmp(key,"modeindex"))
		Cam_mode_Set(value);
	else
		effective_activeCtrl(key,value);
	
	return 0;

}



  int effective_activeCtrl(char *key,char *value )
{
	FILE 	*fp;

	char		cmd[64];


	if((fp=fopen("/proc/sonix/grab/FIC8120/ctrl","w"))==NULL)
		{	
			return	-1;
		}
	sprintf(cmd,"%s %s\r\n",key,value);

	fputs(cmd,fp);
	fclose(fp);
	system("cp /proc/sonix/grab/FIC8120/ctrl  /etc/InitialSnrActive.cfg ");

	return 0;
}


int Cam_mode_Set(char *value)
{
	cgidebug("cam set !!!!");
	cgitrace;
	int index;
	index=atoi(value);
	switch(index)
		{
			case 0:	break;
			case 1:	break;
			case 2:	break;
			case 3:	break;
			default:	break;
		}
	return 0;
}
int effect_modedata(char *B_value,char *C_value,char *H_value,char *Sa_value,char *Sh_value,char *G_value)
{
	effective_activeCtrl("Brightness",B_value);
	effective_activeCtrl("Contrast",C_value);
	effective_activeCtrl("Hue",H_value);
	effective_activeCtrl("Saturation",Sa_value);
	effective_activeCtrl("Sharpness",Sh_value);
	effective_activeCtrl("Gray",G_value);
	return 0;
}
  

