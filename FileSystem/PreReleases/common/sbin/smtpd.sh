# /bin/sh
# ����SMTPD��postfix���������Ľű�

echo "Start SMTP Server"
/usr/sbin/saslauthd -a shadow
/usr/sbin/postfix start
