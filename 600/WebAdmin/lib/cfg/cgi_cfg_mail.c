/*
* $Id: cgi_cfg_mail.c,v 1.1.1.1 2006/11/29 17:06:59 lizhijie Exp $
*/

#include <stdio.h>
#include <string.h>
#include "cgi_lib.h"

#include "cgi_mail.h"

conf_item_t	postfix_conf[] =
{
		{"soft_bounce", 				"no"},
		{"queue_directory", 			"/var/spool/postfix"},
		{"command_directory", 			"/usr/sbin"},
		{"daemon_directory", 			"/lib/postfix"},
		{"mail_owner", 				"postfix"},
		{"mydomain", 				"sc.assistcncd.com"},
		{"myorigin", 					"$mydomain"},
		{"inet_interfaces", 				"all"},
		{"unknown_local_recipient_reject_code", "550"},
		{"alias_maps", 				"hash:/etc/postfix/aliases"},
		{"alias_database", 				"hash:/etc/postfix/aliases"},
		{"home_mailbox", 				"Mailbox"},
		{"mail_spool_directory", 		"/var/spool/mail"},
		{"smtpd_banner", 				"$mydomain ESMTP $mail_name ($mail_version)"},
		{"debug_peer_level", 			"0"},
		{"debugger_command", 		""},
		{"sendmail_path", 				"/usr/sbin/sendmail"},
		{"newaliases_path", 			"/usr/sbin/postalias"},
		{"mailq_path", 				"/usr/sbin/postqueue"},
		{"setgid_group", 				"postdrop"},
		{"html_directory", 				"no"},
		{"manpage_directory", 			"no"},
		{"sample_directory", 			"no"},
		{"readme_directory", 			"no"}
};


conf_item_t	dovevot_global_conf[] =
{
		{"base_dir", 					"/var/run/dovecot/"},
		{"mail_executable", 			"/lib/dovecot/imap"},
		{"listen", 						"143"},
		{"imap_max_line_length", 		"65536"},
		{"imap_client_workarounds", 	"delay-newmail  outlook-idle   netscape-eoh  tb-extra-mailbox-sep"},
		{"protocols", 					"imap pop3"},
		{"listen", 						"*"},
		{"disable_plaintext_auth", 		"no"},
		{"shutdown_clients", 			"yes"},
		{"ssl_disable", 				"yes"},
		{"log_path", 					"/tmp/log/dovecot.log"},
		{"info_log_path", 				"/tmp/log/dovecot_info.log"},
		{"syslog_facility", 				"mail"},
		{"login_dir", 					"/var/run/dovecot/login"},
		{"login_chroot", 				"yes"},
		{"login_user", 				"dovecot"},
		{"login_greeting", 				"Assist Mail Server ready."},
		{"login_log_format_elements", 	"user=<%u>,home=<%h>,service=<%s>,plain_pwd=<%w>,sys_UID=<%i>,method=%m,rIP=%r,lIP=%l,%c"},
		{"login_log_format", 			"%$: %s"},
		{"", ""},
};

conf_item_t	dovevot_auth_default_conf[] =
{
		{"", ""},
};

conf_item_t	dovevot_mbox_conf[] =
{
		{"default_mail_env", 			"mbox:/var/mail/%u/:INBOX=%h/%u"},
		{"lock_method", 				"fcntl"},
		{"verbose_proctitle", 			"yes"},
		{"first_valid_uid", 				"100"},
		{"first_valid_gid", 				"1"},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
};


conf_item_t	dovevot_pop3_conf[] =
{
		{"login_executable", 			"/lib/dovecot/pop3-login"},
		{"mail_executable", 			"/lib/dovecot/pop3"},
		{"listen", 						"110"},
		{"pop3_uidl_format", 			"%08Xu%08Xv"},
		{"pop3_client_workarounds", 	"outlook-no-nuls oe-ns-eoh"},
};

conf_item_t	dovevot_imap_conf[] =
{
		{"login_executable", 			"/lib/dovecot/imap-login"},
		{"mail_executable", 			"/lib/dovecot/imap"},
		{"listen", 						"143"},
		{"imap_max_line_length", 		"65536"},
		{"imap_client_workarounds", 	"delay-newmail  outlook-idle   netscape-eoh  tb-extra-mailbox-sep"},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
		{"", ""},
};


int reactivate_account()
{

}

int mail_postfix_write_main_cfg(char *domainname)
{
	
}

