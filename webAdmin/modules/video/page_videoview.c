
#include "_video.h"
#include "ctype.h"

int main(int argc, char *argv[])
{


#if 1	

	printf("<HTML><HEAD>	\r\n<META NAME=\"description\" CONTENT=\"LCS 3097\">\r\n<title>SONiX Technology, Inc.VideoView</title>\r\n");	
	printf("<LINK REL=\"stylesheet\" TYPE=\"text/css\" HREF=\"std.css\"></HEAD>\r\n");
	printf("<style type=\"text/css\" media=\"all\" title=\"Default\">\r\n");
	printf("\r\n@import \"/include/main.css\";");
	printf("\r\n@import \"/include/slider.css\";");
	printf("</style>\r\n");
	printf("<script language=\"JavaScript\" type=\"text/javascript\" src=\"/include/slider_extras.js\"></script>\r\n");
	
	
	printf("<BODY marginheight=\"20\" marginwidth=\"0\" leftmargin=\"0\" topmargin=\"0\" bgcolor=\"#000000\" onLoad=\"init();\" onbeforeUnload=\"if(run_flag==true) doStop();\"><CENTER>\r\n");
	printf("<form name=\"viewvideo\" method=\"post\"\r\n");
	printf("<DIV align=\"center\" style=\"padding-bottom:0px\">\r\n");
	printf("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"900\">\r\n");
	printf("<tr><td colspan=\"1\" bgcolor=\"#6766CC\" nowrap><img border=\"0\" src=\"/Linksys_Blue_Logo.gif\" align=\"middle\"></td>\r\n");
	printf("<td bgcolor=\"#6766CC\" align=\"right\" valign=\"top\"><span class=\"model\" style=\"position:relative;top:10px;right:15px\"></span></td></tr>\r\n");
	printf("</table>\r\n</DIV>\r\n");



	
	printf("<DIV align=\"center\">\r\n");
	printf("<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" width=\"900\">\r\n");
	printf("<tr><td><img src=\"/top_bar.jpg\" width=\"900\" height=\"10\" alt=\"\" border=\"0\"></td>	</tr>\r\n");
	printf("<tr><td bgcolor=\"#000000\" valign=\"middle\" ID=\"vidDIV\">\r\n");
		printf("<table border=0 cellspacing=0 cellpadding=0>\r\n");


		
			printf("<tr><td  WIDTH=\"161\"  bgcolor=\"#6666CC\" valign=\"top\">\r\n");
				printf("<table WIDTH=\"161\"  border=\"0\" cellspacing=\"0\" cellpadding=\"0\" bgcolor=\"#6666CC\">\r\n");
				
					printf("<tr><td class=\"pannel\" height=\"15\" valign=\"bottom\">Image Resolution</td></tr>\r\n");				
					printf("<tr><td class=\"pannel\" height=\"26\" valign=\"top\">\r\n");
					printf("<select name=\"resolution\" size=\"1\" style=\"width:161px\" onChange=\"setOCXSize(this.value);\">\r\n<option value=\"640\">640*480</option>\r\n<option value=\"320\">320*240</option>\r\n<option value=\"160\">160*128</option>\r\n</select>\r\n");
					printf("</td></tr>\r\n");
				
					printf("<tr><td class=\"cont\" height=\"80\" valign=\"top\">\r\n");	

			
						printf("<table border=0 cellspacing=0 cellpadding=0 class=\"zoom\">\r\n");	
							printf("<tr>\r\n");
								printf("<td ><img border=\"0\" title=\"Zoom Camera\" name=\"zoom\" src=\"/zoom_up.gif\" onMouseUp=\"press_zoom();\"/></td>\r\n");
								printf("<td ><img border=\"0\" title=\"Snap Shot\" name=\"snap\" src=\"/snap_up.gif\"  /></td>\r\n");
								printf("<td ><img border=\"0\" title=\"Flip Video\" name=\"flip\" src=\"/flip_up.gif\" onMouseUp=\"press_flip();\"/></td>\r\n");
								printf("<td ><img border=\"0\" title=\"Mirror Video\" name=\"mir\" src=\"/mir_up.gif\" onMouseUp=\"press_mir();\"/></td>\r\n");
							printf("</tr>\r\n");	
							printf("<tr><td ><img title=\"On-image PT Control\" name=\"dir\" src=\"/dir_up.gif\" / ></td>\r\n");
								printf("<td ><img  title=\"Camera Auto Pan\" name=\"upp\" src=\"/upp_up.gif\" / ></td>\r\n");
								printf("<td ><img title=\"Motion Detection\" name=\"md\" src=\"/md_up.gif\"  / ></td>\r\n");
							printf("</tr>\r\n");
						printf("</table>\r\n");
						

					printf("<tr><td><img src=\"/bottom_bar.jpg\" width=\"161\" height=\"5\" alt=\"\" border=\"0\"></td></tr>\r\n");	
	
					printf("<tr><td class=\"sound\">\r\n");					
						printf("<table border=0 cellspacing=0 cellpadding=0>\r\n");
							printf("<tr><td  WIDTH=\"50\" height=\"30\" align=\"center\"><img title=\"Sound On/Off\" name=\"sound\" src=\"/muteon_over.gif\" onMouseUp=\"pressMuteButton();\" ></td>\r\n");
								printf("<td WIDTH=\"80\" height=\"30\">\r\n<DIV  name=\"test\" id=\"sliderDemo1\" style=\"height:20px;\"> </DIV>\r\n</td>\r\n");
								printf("<td>\r\n<DIV name=\"testtest\" id=\"sliderDetail1\" style=\"font-weight:bolder;\"></DIV>\r\n</td>\r\n");	
							printf("</tr>\r\n");
						printf("</table>\r\n");
					printf("</td></tr>\r\n");
			
					printf("<tr><td class=\"control\"><DIV class=\"padbox\" id=\"padbox\" style=\"display:block\"><img id=\"pad\" src=\"/background33.gif\" alt=\"Video Movement\" border=\"0\" usemap=\"#controller\" /></DIV></td></tr>\r\n");
					printf("<tr><td class=\"contt\" valign=\"top\">\r\n");
	
							printf("<table border=0 cellspacing=0 cellpadding=0>\r\n");
								printf("<tr><td class=\"pannell\" height=\"15\" valign=\"bottom\">view mode</td></tr>\r\n");
								printf("<td class=\"pannel\" height=\"26\" valign=\"top\">\r\n");
								printf("<select name=\"viewmode\" size=\"1\" style=\"width:161px\"onChange=\"setViewmode(this.value);\">\r\n<option value=\"0\">default</option>\r\n<option value=\"1\">indoor</option>\r\n<option value=\"2\">outdoor</option>\r\n<option value=\"3\">low light</option>\r\n</select>\r\n");
								printf("</td>\r\n</tr>\r\n");
								printf("<tr><td><img src=\"/bottom_bar.jpg\" width=\"161\" height=\"5\" alt=\"\" border=\"0\"></td></tr>\r\n");
							printf("</table>\r\n");


							printf("<table border=2 cellspacing=0 cellpadding=0 bgcolor=\"#6666cc\" width=\"161\">\r\n");
							printf("<tr><td width=\"70\" height=\"5\" align=\"left\">brightness</td>\r\n<td width=\"80\" height=\"5\" align=\"left\">\r\n<DIV id=\"sliderDetail2\" style=\"font-weight:bolder;\"></DIV>\r\n<DIV  id=\"sliderDemo2\" style=\"height:20px;\"> </DIV>\r\n</td></tr>\r\n");
							printf("<tr><td width=\"75\" height=\"3\" alt=\"\" border=\"0\"></td></tr>\r\n");
							printf("<tr><td width=\"70\" height=\"5\" align=\"left\">contrast</td>\r\n<td width=\"80\" height=\"5\" align=\"left\">\r\n<DIV id=\"sliderDetail3\" style=\"font-weight:bolder;\"></DIV>\r\n<DIV  id=\"sliderDemo3\" style=\"height:20px;\"> </DIV>\r\n</td></tr>\r\n");
							printf("<tr><td width=\"75\" height=\"3\" alt=\"\" border=\"0\"></td></tr>\r\n");
							printf("<tr><td width=\"70\" height=\"5\" align=\"left\">hue</td>\r\n<td width=\"80\" height=\"5\" align=\"left\">\r\n<DIV id=\"sliderDetail4\" style=\"font-weight:bolder;\"></DIV>\r\n<DIV  id=\"sliderDemo4\" style=\"height:20px;\"> </DIV>\r\n</td></tr>\r\n");
							printf("<tr><td width=\"75\" height=\"3\" alt=\"\" border=\"0\"></td></tr>\r\n");
							printf("<tr><td width=\"70\" height=\"5\" align=\"left\">sharpness</td>\r\n<td width=\"80\" height=\"5\" align=\"left\">\r\n<DIV id=\"sliderDetail5\" style=\"font-weight:bolder;\"></DIV>\r\n<DIV  id=\"sliderDemo5\" style=\"height:20px;\"> </DIV>\r\n</td></tr>\r\n");
							printf("<tr><td width=\"75\" height=\"3\" alt=\"\" border=\"0\"></td></tr>\r\n");
							printf("<tr><td width=\"70\" height=\"5\" align=\"left\">gray</td>\r\n<td width=\"80\" height=\"5\" align=\"left\">\r\n<DIV id=\"sliderDetail6\" style=\"font-weight:bolder;\"></DIV>\r\n<DIV  id=\"sliderDemo6\" style=\"height:20px;\"> </DIV>\r\n</td></tr>\r\n");
							printf("<tr><td width=\"75\" height=\"3\" alt=\"\" border=\"0\"></td></tr>\r\n");
							printf("<tr><td width=\"70\" height=\"5\" align=\"left\">saturation</td>\r\n<td width=\"80\" height=\"5\" align=\"left\">\r\n<DIV id=\"sliderDetail7\" style=\"font-weight:bolder;\"></DIV>\r\n<DIV  id=\"sliderDemo7\" style=\"height:20px;\"> </DIV>\r\n</td></tr>\r\n");
							printf("</table>\r\n");
							printf("</td></tr>\r\n");
							
	printf("</td></tr>\r\n");

	
	printf("</td></tr>\r\n");	
	
	printf("</table>\r\n");
	printf("</tr>\r\n");
	printf("<td nowrap valign=\"top\" align=\"center\" width=\"739\"><table border=5 cellspacing=0 cellpadding=0><td nowrap >\r\n");
	char *ip;
	char	*ver;
	ip=get_IPAddress();
	ver=get_version();
	char  temp[64];
	sprintf(temp,"http://%s/HttpCtl.cab#version=%s",ip,ver);
	cgidebug("SRC=%s\n",temp);
	cgitrace;
	printf("<object classid=\"clsid:8647B51D-776B-461E-89E0-AEB23DD08DB5\"  id=\"PlayCtl\" WIDTH=\"640\" HEIGHT=\"480\" vspace=\"0\" codebase=\"%s\"></object>\r\n",temp);
	printf("</td>	</table></td>\r\n");
	printf("</tr>\r\n");
	printf("</table>\r\n");
	printf("</td>\r\n");

	printf("</tr>\r\n");	
	printf("<tr><td><img src=\"/top_bar.jpg\" width=\"900\" height=\"10\" alt=\"\" border=\"0\"></td></tr>\r\n");
	printf("</table>\r\n");
	printf("<iframe id=\"hiddenframe\" width=\"1\" height=\"2\" src=\"\"></iframe>");
	///////////////////
	printf("<map name=\"controller\" id=\"controller\">\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"13,4,59,50\" title=\"Pan camera up_left\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_135-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_135-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_135-2.gif\',\'/b_135-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_135-2.gif\',\'/b_135-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"58,4,101,50\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"102,4,147,50\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");	
	printf("<AREA SHAPE=\"rect\" COORDS=\"13,50,59,93\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"59,50,102,93\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"102,50,147,93\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"13,93,59,138\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"59,93,102,138\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	printf("<AREA SHAPE=\"rect\" COORDS=\"102,93,147,138\" title=\"Move camera up\"\r\n onmouseover=\"toggleButtonOver_pad(\'/background33.gif\',\' /b_90-2.gif\');\"\r\n onmouseout = \"toggleButtonOver_pad(\' /background33.gif\',\'/b_90-2.gif\');\"\r\n onmousedown=\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onmouseup =\"toggleButtonOver_pad(\'/b_90-2.gif\',\'/b_90-3.gif\');\"\r\n onclick=\"move(\'UL,5\');\"\r\n ondblclick=\"move(\'UL,5\');\"\r\n />\r\n");
	
	
	printf("</MAP>\r\n");
	///////////////////
	printf("<script type=\"text/javascript\">\r\n");

	printf("var sliderImage2 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo2\", sliderCss: \"imageSlider2\", 	barCss: \"imageBar2\", 	min: 0, 	max: 9, hints: \"move the slider\" });\r\n");
	printf("sliderImage2.onmouseup = function () \r\n");
	printf("{var  temp=this._value; URL_request(\"Brightness\",temp); };\r\n");	
	printf("sliderImage2.create();\r\n sliderImage2.setValue(%s);\r\n",Get_init_sensorvalue("Brightness"));
	

	printf("var sliderImage3 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo3\", sliderCss: \"imageSlider3\", 	barCss: \"imageBar3\", 	min: 0, 	max: 9, hints: \"move the slider\" });\r\n");	
	printf("sliderImage3.onmouseup = function () \r\n");
	printf("{var  temp=this._value; URL_request(\"Contrast\",temp); };\r\n");
	printf("sliderImage3.create();\r\n sliderImage3.setValue(%s);\r\n",Get_init_sensorvalue("Contrast"));


	printf("var sliderImage4 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo4\", sliderCss: \"imageSlider4\", 	barCss: \"imageBar4\", 	min: 0, 	max: 9, hints: \"move the slider\" });\r\n");	
	printf("sliderImage4.onmouseup = function () \r\n");
	printf("{var  temp=this._value; URL_request(\"Hue\",temp); };\r\n");
	printf("sliderImage4.create();\r\n sliderImage4.setValue(%s);\r\n",Get_init_sensorvalue("Hue"));

	printf("var sliderImage5 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo5\", sliderCss: \"imageSlider5\", 	barCss: \"imageBar5\", 	min: 0, 	max: 9, hints: \"move the slider\" });\r\n");	
	printf("sliderImage5.onmouseup = function () \r\n");
	printf("{var  temp=this._value; URL_request(\"Sharpness\",temp); };\r\n");
	printf("sliderImage5.create();\r\n sliderImage5.setValue(%s);\r\n",Get_init_sensorvalue("Sharpness"));

	printf("var sliderImage6 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo6\", sliderCss: \"imageSlider6\", 	barCss: \"imageBar6\", 	min: 0, 	max: 9, hints: \"move the slider\" });\r\n");	
	printf("sliderImage6.onmouseup = function () \r\n");
	printf("{var  temp=this._value;  URL_request(\"Gray\",temp); };\r\n");
	printf("sliderImage6.create();\r\n sliderImage6.setValue(%s);\r\n",Get_init_sensorvalue("Gray"));

	printf("var sliderImage7 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo7\", sliderCss: \"imageSlider7\", 	barCss: \"imageBar7\", 	min: 0, 	max: 9, hints: \"move the slider\" });\r\n");	
	printf("sliderImage7.onmouseup = function () \r\n");
	printf("{var  temp=this._value; URL_request(\"Saturation\",temp); };\r\n");
	printf("sliderImage7.create();\r\n sliderImage7.setValue(%s);\r\n",Get_init_sensorvalue("Saturation"));
	
	printf("var sliderImage1 = new neverModules.modules.slider\r\n");
	printf("(    {	targetId: \"sliderDemo1\", sliderCss: \"imageSlider1\", 	barCss: \"imageBar1\", 	min: 0, 	max: 50, hints: \"move the slider\" });\r\n");	
	printf("sliderImage1.onchange = function () \r\n");
	printf("{var  temp=sliderImage1.getValue(); getElementById(\"sliderDetail1\").innerHTML =(temp*2)+\"%%\" ; var  setvalue =(temp-25)*2; volume_set(setvalue);};\r\n");	
	printf("sliderImage1.create();\r\n sliderImage1.setValue(sliderImage1.max/2);\r\n");

	
	printf("</script>\r\n");
	//////////////////
	printf("\r\n</CENTER></BODY>\r\n</HTML>\r\n");



#endif


	return 0;

}

char *Get_init_sensorvalue(char *key)
{
	char  	value[16];
	char		buf[128];
	char		temp[10][128];
	char		*rep;
	FILE		*fp;
	int		i=0;
	memset(value,0,sizeof(value));
	if((fp=fopen(ACTIVE_CTRL_FILE,"r"))==NULL)
		{	
			return	NULL;
		}
	while((rep=fgets(buf, 128, fp)) != NULL)
		{			
			sprintf(temp[i],"%s",rep);
			if(strstr(rep,key))
				{	
					if(!strcmp(key,"Hue"))
					sprintf(value,"%s",temp[i]+strlen(key)+7);
					else
					sprintf(value,"%s",temp[i]+strlen(key)+6);
					break;
				}
			i++;
		}
	fclose(fp);
	
	return strdup(value);
}
char*	get_IPAddress()
{
	FILE*	fp;
	char*	p;
	char*	retptr;
	char		buf[128];
	char		temp[32];
	char		ipaddress[32]="0";
	int		i=0;
	
	system("ifconfig >/tmp/temp");

	if((fp=fopen("/tmp/temp","r"))==NULL)
		{	
			return	NULL;
		}
	while((retptr=fgets(buf, 1024, fp)) != NULL)
	{
		p=strstr(retptr,"addr:");
		if(p!=NULL)
		{	
			p=p+5;
			sprintf(temp,"%s",p);
			break;
		}
		continue;
	}
	fclose(fp);
	
	system("rm /tmp/temp");
	
	while(!isspace(temp[i]))
	{	
		ipaddress[i]=temp[i];
		i++;
	}
	cgidebug("%s\n",ipaddress);
	cgitrace;
	return strdup(ipaddress);		
}


char* get_version()
{
	char  	value[16];
	char		buf[128];
	char		temp[10][128];
	char		*rep;
	FILE		*fp;
	int		i=0;
	memset(value,0,sizeof(value));
	if((fp=fopen(OTHEROPTIONSFILE,"r"))==NULL)
		{	
			return	NULL;
		}
	while((rep=fgets(buf, 128, fp)) != NULL)
		{			
			sprintf(temp[i],"%s",rep);
			if(strstr(rep,"HttpCtlVer"))
				{						
					sprintf(value,"%s",temp[i]+strlen("HttpCtlVer")+1);
					break;
				}
			i++;
		}
	fclose(fp);
	
	return strdup(value);
}




