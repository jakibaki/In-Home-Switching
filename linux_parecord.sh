#!/bin/bash
# Credit: This code is based on answers provided by Waschtl and KrisWebDev
# https://askubuntu.com/questions/60837/record-a-programs-output-with-pulseaudio

cleanup(){
    if [ -z "$module_id" ]
    then
        exit 3
    fi
    # Revert the input sink back to it's original value
    pactl move-sink-input $index $curr_sink
    # Unload the module I've previously loaded
    pactl unload-module $module_id
    exit 0
}

# Call cleanup function when user press ctrl+c
trap "cleanup" SIGINT SIGTERM

if ! default_output=$(pacmd list-sinks | grep -A1 "* index" | grep -oP "<\K[^ >]+")
then
    echo "Can't seem to find proper output sink, are you using pulseaudio?"
    exit 1
fi

pacmd list-sink-inputs|grep -E '(index|application.name)'
while read -p "Choose recording index: " index
do
    if [ "$index" = "exit" ]
    then
        exit 0
    fi
    curr_sink=$(pacmd list-sink-inputs|grep -EA 8 "index: $index\b"|grep sink|sed -E 's/.*sink:\s*([0-9]+)\s*.*/\1/')
    if echo "$curr_sink"|grep -qE '^[0-9]+$'
    then
        break
    fi
    echo 'Invalid index, type exit to quit'
    echo '-------------------------------'
    pacmd list-sink-inputs|grep -E '(index|application.name)'
done

module_id=$(pactl load-module module-combine-sink sink_name=record-n-play slaves=$default_output sink_properties=device.description="Record-and-Play")

pactl move-sink-input $index record-n-play 
parec --raw --process-time=10 --latency=10 --rate 48000 --format=s16le --channels 2 -d record-n-play.monitor | nc 192.168.178.22 2224 #192.168.178.22 2222
# export my variables so that cleanup function can access them
export index,curr_sink,module_id
cleanup