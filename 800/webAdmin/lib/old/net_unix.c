/* net_unix.c
 *
 * Copyright 2004,Philippe Stellwag <linux@mp3s.name>, Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Publice License,
 * version 2 or any later.
 */

#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TIMEOUT (5*60)

static FILE *sfd;
static int srv;
static jmp_buf SFGstack;

extern int get_tcp_conn(char *, char *);

void to_sfgets(){ longjmp(SFGstack,1); }

void net_close() { close(srv); }

void net_open(char *server,int port)
{
    char tmp[8];

    sprintf(tmp,"%d",port);
    if ((srv = get_tcp_conn(server,tmp)) < 0) {
        fprintf(stderr,"Cannot open connection to server %s\n",server);
        exit(1);
    }
    if ((sfd=fdopen(srv,"r"))==NULL) {
        fprintf(stderr,"Cannot fdopen socket\n");
        exit(1);
    }
}

void net_read(char *buffer)
{
    int esave,rz;
    char *ret;

    if (setjmp(SFGstack)) {
        (void)alarm(0);    /* reset alarm clock */
        (void)signal(SIGALRM,SIG_DFL);

        #ifndef ETIMEDOUT
            errno=EPIPE;     /* USG doesn't have ETIMEDOUT */
        #else
            errno=ETIMEDOUT; /* connection timed out */
        #endif

        fprintf(stderr,"Read error on server socket: %d\n",errno);
        close(srv);
        exit(1);
    }
    (void)signal(SIGALRM,to_sfgets);
    (void)alarm(TIMEOUT);
    ret=fgets(buffer,BUFSIZ,sfd);
    esave=errno;
    (void)alarm(0);    /* reset alarm clock */
    (void)signal(SIGALRM,SIG_DFL); /* reset SIGALRM */
    errno=esave;
    rz=strlen(buffer);
    buffer[rz-(buffer[rz-2]=='\r'?2:1)] ='\0';
    if (ret==(char*)0) {
        fprintf(stderr,"Read error on server socket: %d\n",errno);
        fclose(sfd);
        exit(1);
    }
}

void net_write(char *buffer)
{
    register int sz;
    char buf[BUFSIZ];

    strcpy(buf,buffer);
    strcat(buf,"\r\n");
    sz=strlen(buf);
    if (write(srv,buf,sz)!=sz) {
        fprintf(stderr,"Write error on server socket\n");
        close(srv);
        exit(1);
    }
}
