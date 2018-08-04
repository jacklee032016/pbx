#ifndef DAEMON_HXX_
#define DAEMON_HXX_
/*
 * $Id: Daemon.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */


/** Utility which when called attempts to run the caller as a
 ** daemon and log its output to the file stdout.log rather than
 ** to stdout, if stdout_to_file is set to true (the default value).
 ** If stdout_to_file is not set, then stdout will be redirected to
 ** /dev/null. It checks the environment variable NO_DAEMON. If
 ** this is set, then it returns without starting the daemon. If
 ** the daemon is started successfully (or not at all due to the
 ** NO_DAEMON variable) it returns 0; otherwise, it returns a
 ** negative errorcode indicating that an attempt was made to
 ** start as a daemon and this attempt failed. 
 */
int Daemon(bool stdout_to_file = true);

// DAEMON_HXX_
#endif
