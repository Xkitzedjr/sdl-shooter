ip=127.0.1.127
port=1800
message=$1

printf "\nserver:\n"

./server $port &

sleep 3

printf "\n\nclient:\n"

./client $ip $port $message

kill `pidof server`


