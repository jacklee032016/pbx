/*
* $Id: restore_tftp.c,v 1.1 2006/12/19 17:34:24 lizhijie Exp $
* When device start up, check flag(default.flag). if flag not exist,
* download configuration files from TFTP server and release it.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "cgiLib.h"
#include "libCmn.h"
#include "cgi_data.h"

#define	OK					0
#define	FAILURE				-1
#define	TRY_NUM				3

#define	DIR_TFTP			"/usr/bin"
#define	PRE_CONFIG			"assist_config"


//Default TFTP server IP 
static char  	server_IP[20]="172.17.0.2";

/*
  *Check net status. it's empty for a while.
  *RETUNS:
  	success =0
  	failure = -1
  */
static int config_check_network()
{
	return OK;
}



/*Download configuration files (.tar) from TFTP server.
   *And check file's (.tar) validity.
   *RETUNS:
  	success =0
  	failure = -1
  */
static int  config_get_config_data( char *filename)
{
	char		path_name[256] = "\0";
	char 	cmd[256] = "\0";
	char 	curr_path[256] = "\0";;
	struct stat data_stat;
	int 		fd;
     
	//download
	CGI_SPRINTF(cmd, sizeof(cmd), "%s/tftp -r %s -g %s", DIR_TFTP, filename, server_IP);
	fprintf(stdout, "Start reccive file: %s\n", filename);
	chdir("/tmp");

	if (system(cmd) == -1)
	{
		fprintf(stderr, "Get_config:Call system() error!\n");
		return FAILURE;
	}
		
 	//Check file(*.tar) size
 	//NULL方式调用不推荐
	//curr_path=getcwd(NULL,128);

	//Check file size, if file not empty, it's legal.
	getcwd(curr_path, sizeof(curr_path));
	CGI_SPRINTF(path_name, sizeof(path_name), "%s/%s", curr_path, filename);
	if((fd = open(path_name,O_RDONLY)) == -1)
	{
		fprintf(stderr, "Get_config:Open %s error!\n", path_name);
		return FAILURE;
	}

	fstat(fd,&data_stat);
	close(fd);
	if(data_stat.st_size <= 0)
	{
		//fprintf(stderr, "config_get_config_data:%s is empty!\n", path_name);
		//Delete empty files.
		CGI_SPRINTF(cmd, sizeof(cmd), "/bin/rm -f %s", path_name);
		system(cmd);
		return FAILURE;
	}
		
	fprintf(stdout, "Reccived file: %s\n",filename);

	return OK;
}


/*释放文件包(*.tar)
  * 到相应目录.(/etc/ /usr/etc/)	
  *RETUNS:
  	success =0
  	failure = -1
  */
static int config_release_tar(char *filename )
{
	char cmd[256]="\0";

	chdir("/tmp");
	if( access(filename,F_OK) != 0)
	{
		fprintf(stderr, "Error: %s not exist!\n", filename);
		return FAILURE;
	}
	
	//Check file types
	if( strstr(filename,".tar.gz"))
		sprintf(cmd,"/bin/tar zxf %s -C /",filename);
	else if (strstr(filename,".tar") )
		sprintf(cmd,"/bin/tar xf %s -C /",filename);
	else
	{
		fprintf(stderr, "Error:File's type isn't .tar.\n");
		return FAILURE;
	}

	//Backup some old files.
	system("/bin/mkdir cnf_bak;/bin/cp /etc/sys/* cnf_bak/");
	//Release
	if (system(cmd) ==-1)
	{
		fprintf(stderr, "Release %s error.\n",  filename);
		system("/bin/mv cnf_bak/* /etc/sys;/bin/rm -rf cnf_bak");
		return FAILURE;
	}

	//Delete .tar
	CGI_SPRINTF(cmd, sizeof(cmd), "/bin/rm -f %s;/bin/rm -rf cnf_bak",filename);
	system(cmd);

	return OK;
}


/*Master function
  *1.Call config_get_config_data() get configuration files (.tar)
  *2.Call config_release_tar() release pack.
  *RETUNS:N/A
  */
static void config_main(char *device_ID)
{	
	int 			release_ret = -1;
	int 			i = 0;
	char	  	 	packname[256];

	/*
	//If default.flg exist,That not need to get configuration files.
	if( !access(DEFAULT_FLAG,F_OK) )
	{
		fprintf(stdout,"Check config is done.\n");
		return ;
	}
	*/
	
	CGI_SPRINTF(packname, sizeof(packname), "%s_%s.tar", PRE_CONFIG, device_ID);
	//At most, try three times.
	while(i < TRY_NUM)
	{
		fprintf(stdout, "Get configuration files from TFTP server:%s\n",server_IP);
		if( !config_check_network())
		{
			if(config_get_config_data(packname) == OK )
			{
				release_ret=config_release_tar(packname);
				break;
			}
			sleep(2);
		}
		i++;
	}

	if(release_ret != OK )
	{
		fprintf(stdout, "Config update is faild.\n");
		return;
	}
	
	fprintf(stdout, "Config update is successful.\n");

	//Reboot device with new config.
	system("reboot");
	
	//return;
}

/*
  * Main process. 
  * 1.Get run parameter.
  * 2.Call read_deviceID()
  * 3.Call config_main()
  * RETURNS:
  */
int main(int argc ,char * *argv )
{
	int 		c;
	int 		option_index = 0;
	char		dev_id[CMN_FLASH_ITEM_LENGTH];
	llist		cfgs;
	
	static struct option long_options[] = 
	{
		{"server", 1, 0, 0},
		{"file",1,0,0},
		{0, 0, 0, 0}
	};

	//Get run parameter
	while (1)
	{
		c = getopt_long (argc, argv, "s:",long_options, (int *) 0);
		if (c == -1)
			break;
		switch (c)
		{
			case 's':
				strcpy(server_IP,optarg);
				//printf("serverIP:%s\n",server_IP);
				break;
		}
		option_index++;
	}

	if (readconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &cfgs ) != 0)
	{
		printf("No Restore configuration file\n");
		return -1;
	}
	
	list_replace_value_by_key(&cfgs, CGIDATA_KEYWORD_RESET_FLAG, CGIDATA_KEYWORD_RESET_NO );
	writeconfig(CGIDATA_CFG_FILE, ITEM_EQU, NULL, &cfgs );

//for test
//strcpy(dev_id, "00000000000000000001");
///////
	if (as_read_serial_no(dev_id, sizeof(dev_id)) )
	{
		printf("DeviceID of this device is not found!May it is not initialized until now!\n");
		printf("Please Contact to support@assistcn.com.\n");
		return -1;
	}
	//Master process
	config_main(dev_id);
	
	return 0;
}


