ifconfig eno1 | grep 'inet ' | awk '{print $2}'

BROKER_PORT=$1
BROKER_MODE=$2
NUM_SUBSCRIBERS=$3
TOPIC="cpu-usage"

killall broker
killall publisher

./broker --port $BROKER_PORT --mode $BROKER_MODE &2>1 > /dev/null &
sleep 1
./publisher --ip 0.0.0.0 --port $BROKER_PORT --topic $TOPIC &2>1 > /dev/null &

BROKER_IP=$(ifconfig eno1 | grep 'inet ' | awk '{print $2}')

echo "./generate_latency_data.sh $BROKER_IP $BROKER_PORT $BROKER_MODE $NUM_SUBSCRIBERS 100"