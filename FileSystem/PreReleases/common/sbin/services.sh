# /bin/sh
#Services startup script executing when startup for Assist IXP422
#Please don't edit this file manully
echo startup WebAdmin .....
/sbin/admin.sh

echo startup Telnet .....
/usr/sbin/telnetd

echo startup PrinterServer .....
/usr/sbin/cupsd

echo startup NameServer .....
/sbin/nmbd

echo startup FileServer .....
/sbin/smbd

echo startup SmtpServer .....
/sbin/smtpd.sh

echo startup Pop3ImapServer .....
/usr/sbin/dovecot

echo startup WWWServer .....
/sbin/www.sh
