#ifndef	__AS_TAPER_H__ 
#define __AS_TAPER_H__
/* Interface */

#define false 0
#define true 1

#define VOICE_RATE 160

#define ERR_UNABLE   -1
#define ERR_SUCCESS   0  // 0 successful
#define ERR_OPEN_F   1  //  open file error
#define ERR_OPEN_D   2
#define ERR_OPEN_C   3
#define ERR_REGIST   4 //  register signal error

#define ERR_OVERTIME 0xFF


struct str_parameter
{
	char device_type[10];
	int device_fd;
	char file_path[256];
	int time_lenght;
	char starttip_file[256];
	char endtip_file[256];
	char action_type[10];
	int waite_time;

};

typedef struct str_parameter recorder_parameter_t;

//////////////////////////////////////////////////////////////////////////
void signal_handler(int);
int register_signal();
int start_recorder();
int start_player(const char*);
void* recorder(void*);
//void set_parameter(recorder_parameter_t *param);

#endif

