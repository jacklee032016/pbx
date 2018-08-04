SERVER_ADDR=192.168.3.113

echo "Starting....."

camclient -w $SERVER_ADDR:7070 &
camclient -w $SERVER_ADDR:7071 &

echo "Done"

