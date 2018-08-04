# /bin/sh
# 启动SMTPD（postfix）服务器的脚本

echo "Start SMTP Server"
/usr/sbin/saslauthd -a shadow
/usr/sbin/postfix start
