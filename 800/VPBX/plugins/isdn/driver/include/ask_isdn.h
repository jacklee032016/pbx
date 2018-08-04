/*
* $Id: ask_isdn.h,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
* Assist ISDN library for Kernel ISDN Protocol stack
*/

#ifndef __AS_KISDN_LIB_H__
#define __AS_KISDN_LIB_H__

#ifdef  __cplusplus
	extern "C"
	{
#endif

/* API library to use with /dev/isdn */

/* we need some extentions */
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

typedef unsigned short 	u16;

#include <sys/types.h>
#include <stdio.h>
#include "asISDN.h"

#define AS_ISDN_INBUFFER_SIZE	0x20000

typedef struct _iframe
{
	u_int	addr;
	u_int	prim;
	int		dinfo;
	int		len;
	union
	{
		u_char	b[4];
		void		*p;
		int		i;
	}data;
} iframe_t;


#define TIMEOUT_1SEC		1000000
#define TIMEOUT_10SEC		10000000
#define TIMEOUT_INFINIT 	-1

/* return:    the file descriptor or -1 on error and the error cause in errno */
extern int ask_isdn_open(void);

/* close and frees related memory.
 * parameter:   fid - file descriptor returned by ask_isdn_open
 * return:    0 on success or -1 on error and the error cause in errno
 */
extern int ask_isdn_close(int fid);

/* read one message from device or buffer
 * utimeout - maximum time in microseconds to wait for data, if -1 wait until some data is ready
 * return:   length of read data or -1 on error and the error cause in errno
 */
extern int ask_isdn_read(int fid, void *buf, size_t count, int utimeout);

/* read one message for address (addr) and message type (msgtype) from device or buffer
 * parameter:
 * addr     - address of frame
 * msgtype  - message type of frame
 */
extern int ask_isdn_read_frame(int fid, void *buf, size_t count, u_int addr, u_int msgtype, int utimeout);

/* */
extern int ask_isdn_write(int fid, void *buf, size_t count, int utimeout);

/* write a frame to device
 * parameter:
 * addr     - address for frame
 * msgtype  - message type of frame
 * dinfo    - data info parameter
 * dlen     - len of dbuf data, if negativ it is an error code (dbuf len=0)
 * dbuf     - pointer to frame payload data
 */
extern int ask_isdn_write_frame(int fid, void *fbuf, u_int addr, u_int msgtype,
		int dinfo, int dlen, void *dbuf, int utimeout);


/* select call which handles AS_ISDN readbuffer */
extern int ask_isdn_select(int n, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);


/* Prototypes from stack.c */
extern int ask_isdn_get_stack_count(int fid);

/* get the info about ISDN stack
 * parameter:
 * stack    - ID of the stack
 * info     - buffer to store info
 * max_len  - size of above buffer
 */
extern int ask_isdn_get_stack_info(int fid, int stack, void *info, size_t max_len);

/* create a new stack
 * parameter: s_info   - info for the new stack
 * return:   stack id or negativ error code
 */
extern int ask_isdn_new_stack(int fid, stack_info_t *s_info);

/* setup a stack for the given protocol
 * parameter:
 * stack    - stack id
 * pid      - description of the stack protocol
 */
extern int ask_isdn_set_stack(int fid, int stack, AS_ISDN_pid_t *pid);

/* clear the protocol stack */
extern int ask_isdn_clear_stack(int fid, int stack);

/*print out the stacask_info in readable output */
extern void ask_isdn_print_stacask_info(FILE *file, stack_info_t *s_info);


/* Prototypes from layer.c */

/* get the id of the layer given by stack and layer number 
 * parameter: stack    - ID of the stack;  layer    - layer number
 * return:     layer id or negativ error code
 */
extern int ask_isdn_get_layerid(int fid, int stack, int layer);

/* create a new layer
 * parameter: l_info   - info for the layer
 * return:
 *    layer id or negativ error code
 */
extern int ask_isdn_new_layer(int fid, layer_info_t *l_info);

int ask_isdn_del_layer(int fid, int layerId);

/* create a new connection
 * parameter: i_info   - info for the connection
 * return:    0 on success or error code */
extern int ask_isdn_connect(int fid, interface_info_t *i_info);

extern int ask_isdn_disconnect(int fid, interface_info_t *i_info);

/* get the info about ISDN layer
 * parameter:
 * lid      - ID of the layer
 * info     - buffer to store info
 * max_len  - size of above buffer
 * return:
 *    length of stored info, negativ values are errors  */
extern int ask_isdn_get_layer_info(int fid, int lid, void *info, size_t max_len);

/* get the info about ISDN layer interface
 * parameter:
 * i_info   - contains the info about layer (i_info->owner) and which interface (i_info->stat) and gives requested info back
 * return:
 *    0 on sucess other values are errors  */
extern int ask_isdn_get_interface_info(int fid, interface_info_t *i_info);

/* Prototypes and defines for status.c */

/* l1 status_info */
typedef struct _status_info_l1
{
	int	len;
	int	typ;
	int	protocol;
	int	status;
	int	state;
	int	Flags;
	int	T3;
	int	delay;
	int	debug;
} status_info_l1_t;

/* State values for l1 state machine (status_info_l1_t state field) */
extern char *strL1SState[];

/* l2 status_info */
typedef struct _laddr
{
	u_char  A;
	u_char  B;
} laddr_t;
                
                
typedef struct _status_info_l2
{
	int		len;
	int		typ;
	int		protocol;
	int		state;
	int		sapi;
	int		tei;
	laddr_t 	addr;
	int		maxlen;
	u_int	flag;
	u_int	vs;
	u_int	va;
	u_int	vr;
	int		rc;
	u_int	window;
	u_int	sow;
	int		T200;
	int		N200;
	int		T203;
	int		len_i_queue;
	int		len_ui_queue;
	int		len_d_queue;
	int		debug;
	int		tei_state;
	int		tei_ri;
	int		T202;
	int		N202;
	int		tei_debug;
}status_info_l2_t;

/* State values for l2 state machine (status_info_l2_t state field) */
extern char *strL2State[];

/* get status infos about layer instances in the ISDN stack
 * parameter:
 * id       - ID of the instance
 * info     - buffer to store info
 * return:    length of stored info, negativ values are errors
 */
extern int ask_isdn_get_status_info(int fid, int id, void *info, size_t max_len);

/* return:  0 on success or -1 if no known status_info */
extern int ask_isdn_print_status(FILE *file, status_info_t *si);
void ask_isdn_print_stack_info(FILE *file, stack_info_t *s_info);

/* private functions */

extern int set_wrrd_atomic(int fid);
extern int clear_wrrd_atomic(int fid);

#define ASK_DEBUG	1

 #define TRACE	printf("%s:%s:%d\r\n", __FILE__, __FUNCTION__, __LINE__)

#if ASK_DEBUG
void ask_hex_dump(void *buf, int len);
#endif

int ask_isdn_read_without_lock(int fid, void *buf, size_t count, int utimeout) ;
int  ask_isdn_write_without_lock(int fid, void *buf, size_t count, int utimeout);

#ifdef __cplusplus
	};
#endif


#endif
