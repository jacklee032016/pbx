/*
* $Id: _pbx_state_handles.h,v 1.11 2007/07/07 09:31:48 lizhijie Exp $
*/
#ifndef	__PBX_STATE_HANDLES_H__
#define	__PBX_STATE_HANDLES_H__

INT8U	ext_idle_state_digits(INT8U devIndex);
INT8U	ext_idle_state_offhook(INT8U devIndex);
INT8U	ext_idle_state_ringing(INT8U devIndex);
INT8U	ext_idle_state_timeout(INT8U devIndex);
INT8U 	ext_idle_state_car(INT8U devIndex);
INT8U 	ext_idle_state_ringeron(INT8U devIndex);
INT8U 	ext_idle_state_ringeroff(INT8U devIndex);
void		ext_idle_state_enter(INT8U devIndex);

INT8U	ext_dialing_state_ringing(INT8U devIndex);
INT8U	ext_dialing_state_digits(INT8U devIndex);
INT8U	ext_dialing_state_onhook(INT8U devIndex);
INT8U	ext_dialing_state_timeout(INT8U devIndex);
void		ext_dialing_state_enter(INT8U devIndex);

INT8U	ext_redialing_state_onhook(INT8U devIndex);
INT8U	ext_redialing_state_timeout(INT8U devIndex);
void 	ext_redialing_state_enter(INT8U devIndex);


INT8U	ext_trying_state_timeout(INT8U devIndex);
INT8U	ext_trying_state_onhook(INT8U devIndex);
INT8U	ext_trying_state_OK(INT8U devIndex);
INT8U	ext_trying_state_flash(INT8U devIndex);

void		ext_trying_state_enter(INT8U devIndex);

INT8U	ext_ringing_state_offhook(INT8U devIndex);
INT8U	ext_ringing_state_timeout(INT8U devIndex);
INT8U	ext_ringing_state_cancel(INT8U devIndex);
void		ext_ringing_state_enter(INT8U devIndex);

INT8U	ext_incall_state_onhook(INT8U devIndex);
INT8U	ext_incall_state_timeout(INT8U devIndex);
INT8U	ext_incall_state_cancel(INT8U devIndex);
void		ext_incall_state_enter(INT8U devIndex);

INT8U	ext_incall_state_flash(INT8U devIndex);

INT8U	ext_busy_state_onhook(INT8U devIndex);
INT8U	ext_busy_state_timeout(INT8U devIndex);
void		ext_busy_state_enter(INT8U devIndex);


INT8U	ext_wait_state_onhook(INT8U devIndex);
INT8U	ext_wait_state_offhook(INT8U devIndex);
INT8U	ext_wait_state_ok(INT8U devIndex);
INT8U	ext_wait_state_cancel(INT8U devIndex);
void 	ext_wait_state_enter(INT8U devIndex);

INT8U	ext_wait_state_timeout(INT8U devIndex);

INT8U	ext_offhook_state_flash(INT8U devIndex);


INT8U	ext_programing_state_digits(INT8U devIndex);
INT8U	ext_programing_state_onhook(INT8U devIndex);
INT8U	ext_programing_state_timeout(INT8U devIndex);
void 	ext_programing_state_enter(INT8U devIndex);

INT8U	ext_rering_state_onhook(INT8U devIndex);

#endif

