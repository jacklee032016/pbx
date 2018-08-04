/*
* $Id: partySave.c,v 1.2 2007/03/17 09:07:34 lizhijie Exp $
*/

#include "cgi_lib.h"
#include "cgi_partysip.h"

int	__write_userinfos(FILE *fp, PARTY_INFO *info)
{
	char	*filtername;
	node 		*current ;
	
	filtername = "userinfo";
	current = info->userCfgs.head;

	fprintf(fp, "<%s>\n", filtername);

	while(current )
	{
		fprintf(fp, "%s\t%s\t%s\t%s\n",current->entry.name, current->entry.type, current->entry.value, current->entry.rule );
		current = current->next;
	}
	fprintf(fp, "</%s>\n\n\n", filtername);

	return 0;
}

int	__write_filter(FILE *fp, PARTY_INFO *info, int type)
{
	char			*filtername;
	node 		*current ;
	llist 			*list ;
	char			*tmp;
	
	if(type==PARTY_FILTER_TYPE_EXT)
	{
		filtername = "filter_external";
		list = &info->extFilterCfgs;
//		current = info->extFilterCfgs.head;
	}	
	else
	{
		filtername = "filter_internal";
		list = &info->intFilterCfgs;
	}

	fprintf(fp, "<%s>\n", filtername);
	tmp = GET_VALUE(list, PARTY_CFG_FILTER_DIAL_PLAN);
	if(tmp && strlen(tmp) )
		fprintf(fp, "%s\t%s\n", PARTY_CFG_FILTER_DIAL_PLAN, tmp);

	tmp = GET_VALUE(list, PARTY_CFG_FILTER_MODE );
	if(tmp && strlen(tmp) )
		fprintf(fp, "%s\t%s\n", PARTY_CFG_FILTER_MODE, tmp);

	tmp = GET_VALUE(list, PARTY_CFG_FILTER_ROUTE_RECORD);
	if(tmp && strlen(tmp) )
		fprintf(fp, "%s\t%s\n", PARTY_CFG_FILTER_ROUTE_RECORD, tmp);

	current = list->head;
	while(current )
	{
		if(!strcasecmp(current->entry.name, "forward") )
			fprintf(fp, "forward\t%s|%s\n", current->entry.value, current->entry.type );
		current = current->next;
	}
	fprintf(fp, "</%s>\n\n\n", filtername);

	return 0;
}

int	party_save_config(PARTY_INFO *info)
{
	FILE *fp;
	int res = 0;
	char		*value;
	writeconfig(SIPPARTY_CONF_FILE,ITEM_EQU, NULL, &info->globalCfgs );
		
	if( (fp=fopen(SIPPARTY_CONF_FILE, "a" ) )==NULL)
		return -1;

	fprintf(fp, "\n\n##end of of global configuration\n\n");

	value = GET_VALUE(&info->cgiVariables, PARTY_CFG_AUTHEN_407);
	fprintf(fp, "<auth>\n%s\t\t%s\n</auth>\n\n", PARTY_CFG_AUTHEN_407, (!value||strlen(value)==0)?info->proxyAuth:value );
	
	res = __write_filter(fp, info, PARTY_FILTER_TYPE_EXT);
	res = __write_filter(fp, info, PARTY_FILTER_TYPE_INT);

	__write_userinfos( fp,  info);

	fprintf(fp, "<syntax>\n%s\n</syntax>\n\n\n", "allowed_schemes sip,sips");
	
	fprintf(fp, "<ls_sfull>\n%s\n</ls_sfull>\n\n\n", "record-route off");
	
	fprintf(fp, "<ls_localdb>\n%s\n%s\n%s\n</ls_localdb>\n\n\n", "mode sf_forking", "mode sf_sequential", "record-route off");
	
	fclose(fp);
	
	return 0;
}

