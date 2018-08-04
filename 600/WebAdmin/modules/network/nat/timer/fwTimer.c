/*
* $Id: fwTimer.c,v 1.2 2007/09/06 09:19:59 lizhijie Exp $
*/

#include "fwTimer.h"

ipt_timer_t  *__timer_get_value(FW_INFO *info)
{
	ipt_timer_t *timeValue;
	timeValue = (ipt_timer_t *)malloc(sizeof(ipt_timer_t) );
	if(!timeValue)
	{
		return NULL;		
	}
	memset(timeValue, 0 , sizeof(ipt_timer_t) );

	sprintf(timeValue->mimute, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_MINUTE) );
	sprintf(timeValue->hour, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_HOUR) );
	sprintf(timeValue->dayBegin, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_DAY_BEGIN) );
	sprintf(timeValue->dayEnd, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_DAY_END) );
	sprintf(timeValue->monthBegin, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_MONTH_BEGIN) );
	sprintf(timeValue->monthEnd, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_MONTH_END) );
	sprintf(timeValue->weekBegin, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_WEEK_BEGIN) );
	sprintf(timeValue->weekEnd, "%s",  GET_VALUE(&info->cgiVariables, IPT_TIMER_WEEK_END) );

	return timeValue;
}

char		*timer_item_to_key(ipt_timer_t *timerValue)
{
	char		value[128];
	sprintf(value, "%s|%s|%s|%s|%s|%s|%s|%s", timerValue->mimute, timerValue->hour, 
		timerValue->dayBegin, timerValue->dayEnd, 
		timerValue->monthBegin, timerValue->monthEnd,
		timerValue->weekBegin, timerValue->weekEnd);

	return strdup(value);		
}

/* key is the name of llist */
ipt_timer_t  *__timer_key_to_item(char *key)
{
	char		*tmp, *p;
	ipt_timer_t *timeValue;
	timeValue = (ipt_timer_t *)malloc(sizeof(ipt_timer_t) );
	if(!timeValue)
	{
		return NULL;		
	}
	memset(timeValue, 0 , sizeof(ipt_timer_t) );

	tmp = key;
	p = strchr(tmp, '|' );
	strncpy(timeValue->mimute, tmp, p-tmp );
	tmp = ++p; p = strchr(tmp, '|' );
	strncpy(timeValue->hour, tmp, p-tmp );
	tmp = ++p; p = strchr(tmp, '|' );
	strncpy(timeValue->dayBegin, tmp, p-tmp);
	tmp = ++p; p = strchr(tmp, '|' );
	strncpy(timeValue->dayEnd, tmp, p-tmp);
	tmp = ++p; p = strchr(tmp, '|' );
	strncpy(timeValue->monthBegin, tmp, p-tmp);
	tmp = ++p; p = strchr(tmp, '|' );
	strncpy(timeValue->monthEnd, tmp, p-tmp);
	tmp = ++p; p = strchr(tmp, '|' );
	strncpy(timeValue->weekBegin, tmp, p-tmp );
	++p;
	sprintf(timeValue->weekEnd, "%s", p );

	return timeValue;
}

#if 1
llist 	timersCfg;
int __timer_init()
{
	list_create(&timersCfg);
	if (readconfig(IPTABLES_TIMER_CONFIG, ITEM_TRI, NULL, &timersCfg) != 0)
	{
		cgi_error_page(NULL, gettext(CGI_STR_ERROR), gettext("No Config for Firewall Timer is found") );
		return -1;
	}
	return 0;
}
#endif

int __timer_save_task(FW_INFO *info)
{
	char	*key, *ruleType, *ruleName;
	node	*rule;
	ipt_timer_t *timerValue = __timer_get_value(info);

	key = timer_item_to_key(timerValue);
	ruleName = GET_VALUE(&info->cgiVariables, IPT_TIMER_TASK );
	rule = list_find_node_by_key(&info->ruleCfgs, ruleName);
	ruleType = rule->entry.type;
	
	list_append_raw(&info->timerCfgs, key, ruleName, ruleType, NULL);

	writeconfig(IPTABLES_TIMER_CONFIG, ITEM_TRI, NULL, &info->timerCfgs);
	
	return 0;
}

int cgiFw_Timer(FW_INFO *info)
{
	char *action;

#if 1
	if (__timer_init() )
	{
		return -1;
	}
#endif

	action = GET_VALUE(&info->cgiVariables, CGI_ACTION );
	if(!action || !strlen(action) )
	{
		cgi_info_page(gettext("Firewall Timer Configuration"), fwTimer_info(&timersCfg), fwTimer_buttons( info) );
		return 0;
	}
	
 	
	if(!strcasecmp(action, CGI_ACTION_ADD ) ||
		!strcasecmp(action, CGI_ACTION_DEL)  )
	{
		if(!strcasecmp(action, CGI_ACTION_ADD ) )
		{
			if(__timer_save_task(info)< 0)
				return 1;
		}
#if 0	
		else
		{
			if(fwNat_save_rule(rulesCfg, info))
				return 1;
		}
#endif

		/* renew this list to display in pages */
		if (__timer_init() )
		{
			return -1;
		}
		cgi_info_page(gettext("Firewall Timer Configuration"), fwTimer_info(&timersCfg), fwTimer_buttons(info) );
		
	}
	return 0;	
}

