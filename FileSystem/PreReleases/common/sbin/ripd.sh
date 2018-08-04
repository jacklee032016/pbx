# /bin/sh
# 启动RIP动态路由协议的脚本

echo "Start RIP Daemon"
/usr/bin/zebra -d
/usr/bin/ripd -d &
