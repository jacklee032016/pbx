/*
$Id: ask_stack.c,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include <errno.h>
#include <string.h>

#include "ask_isdn.h"
// #include <stdio.h>

int ask_isdn_get_stack_count(int fid)
{
	iframe_t	ifr;
	int		ret;

	set_wrrd_atomic(fid);
	ret = ask_isdn_write_frame(fid, &ifr, 0, MGR_GETSTACK | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if (ret)
	{
		clear_wrrd_atomic(fid);
		return(ret);
	}
	ret = ask_isdn_read_frame(fid, &ifr, sizeof(iframe_t), 0, MGR_GETSTACK | CONFIRM, TIMEOUT_1SEC);
	clear_wrrd_atomic(fid);
	if (ret != AS_ISDN_HEADER_LEN)
	{
		if (ret > 0)
			ret = -EINVAL; 
	}
	else
	{
		if (ifr.len)
			ret = ifr.len;
		else
			ret = ifr.dinfo;
	}
	return(ret);
}

int ask_isdn_new_stack(int fid, stack_info_t *s_info)
{
	u_char		buf[sizeof(stack_info_t) + AS_ISDN_HEADER_LEN];
	iframe_t	ifr;
	int		ret;

	set_wrrd_atomic(fid);
	ret = ask_isdn_write_frame(fid, buf, 0, MGR_NEWSTACK | REQUEST, 0, sizeof(stack_info_t), s_info, TIMEOUT_1SEC);
	if (ret)
	{
		clear_wrrd_atomic(fid);
		return(ret);
	}
	ret = ask_isdn_read_frame(fid, &ifr, sizeof(iframe_t), 0,  MGR_NEWSTACK | CONFIRM, TIMEOUT_1SEC);
	clear_wrrd_atomic(fid);
	if (ret == AS_ISDN_HEADER_LEN)
	{
		if (ifr.len)
			ret = ifr.len;
		else
			ret = ifr.dinfo;
	}
	return(ret);
}

int ask_isdn_set_stack(int fid, int stack, AS_ISDN_pid_t *pid)
{
	u_char		buf[sizeof(AS_ISDN_pid_t) + AS_ISDN_HEADER_LEN];
	iframe_t	ifr;
	int		ret;

	set_wrrd_atomic(fid);
	ret = ask_isdn_write_frame(fid, buf, stack, MGR_SETSTACK | REQUEST, 0, sizeof(AS_ISDN_pid_t), pid, TIMEOUT_1SEC);
	if (ret)
	{
		clear_wrrd_atomic(fid);
		return(ret);
	}

	ret = ask_isdn_read_frame(fid, &ifr, sizeof(iframe_t), stack, MGR_SETSTACK | CONFIRM, TIMEOUT_1SEC);
	clear_wrrd_atomic(fid);
	if (ret == AS_ISDN_HEADER_LEN)
		ret = ifr.len;
	else if (ret>0)
		ret = -EINVAL;
	return(ret);
}

int ask_isdn_clear_stack(int fid, int stack)
{
	iframe_t	ifr;
	int		ret;

	set_wrrd_atomic(fid);
	ret = ask_isdn_write_frame(fid, &ifr, stack, MGR_CLEARSTACK | REQUEST,
		0, 0, NULL, TIMEOUT_1SEC);
	if (ret)
	{
		clear_wrrd_atomic(fid);
		return(ret);
	}
	ret = ask_isdn_read_frame(fid, &ifr, sizeof(iframe_t), stack, MGR_CLEARSTACK | CONFIRM, TIMEOUT_1SEC);
	clear_wrrd_atomic(fid);
	if (ret == AS_ISDN_HEADER_LEN)
		ret = ifr.len;
	else if (ret>0)
		ret = -EINVAL;
	return(ret);
}

int ask_isdn_get_stack_info(int fid, int stack, void *info, size_t max_len)
{
	iframe_t	ifr;
	int		ret;

	set_wrrd_atomic(fid);

	ret = ask_isdn_write_frame(fid, &ifr, stack, MGR_GETSTACK | REQUEST, 0, 0, NULL, TIMEOUT_1SEC);
	if (ret) 
	{
		clear_wrrd_atomic(fid);
		return(ret);
	}
	ret = ask_isdn_read_frame(fid, info, max_len, stack, MGR_GETSTACK | CONFIRM, TIMEOUT_1SEC);
	clear_wrrd_atomic(fid);
	return(ret);
}

void ask_isdn_print_stack_info(FILE *file, stack_info_t *s_info)
{
	int i;

	fprintf(file, "STACK INFO\n\tID : \t%08x\n", s_info->id);
	fprintf(file, "\tExt : \t%08x\n", s_info->extentions);
	
	fprintf(file, "\tProtocol Info\n" );
	for(i=0;i<=MAX_LAYER_NR;i++)
		fprintf(file, "\t\tProtocol[%d] %08x\n", i, s_info->pid.protocol[i]);
	
	fprintf(file, "\tInstance Info : count : %d\n",  s_info->instcnt);
	for(i=0;i<s_info->instcnt;i++)
		fprintf(file, "\t\tInstance[%d] %08x\n", i, s_info->inst[i]);
	
	fprintf(file, "\tMGR %08x\n", s_info->mgr);
	
	fprintf(file, "\tChild Info : count : %d\n", s_info->childcnt);
	for(i=0;i<s_info->childcnt;i++)
		fprintf(file, "\t\tChild[%d] %08x\n", i, s_info->child[i]);
}

