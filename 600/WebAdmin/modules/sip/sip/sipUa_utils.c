/*
* $Id: sipUa_utils.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

 int sip_readconfig(char *filename,int itemnum,char *ctype,llist *l)
{
	FILE *cfgfp;
	entrytype tmpentry;

	if((cfgfp=fopen(filename,"r"))==NULL)
	{
		return -1;
	}
	
	tmpentry.name=(char *)malloc(sizeof(char)*ITEM_LENGTH+1);
	tmpentry.value=(char *)malloc(sizeof(char)*ITEM_LENGTH+1);
	
	if(l->head!=NULL)
		list_clear(l);
	list_create(l);
	while (!readline(&tmpentry, itemnum, cfgfp))
	{
		#if 0
		if(!strstr(tmpentry.name,PBXPC)&&
			!strstr(tmpentry.name,PBXNL)&&
			!strstr(tmpentry.name,PBXSR))
			{
				continue;
			}
		#endif
		list_append(l, &tmpentry);
	}
	
	free(tmpentry.name);
	free(tmpentry.value);
	
	fclose(cfgfp);
	
	return 0;
}

