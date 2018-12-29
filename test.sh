ffmpeg -video_size 1280x720 \
-framerate 60 -f x11grab -i :0.0+0,0 \
-f mpegts \
-vcodec libx264 -profile baseline -preset ultrafast -tune zerolatency \
-pix_fmt yuv420p -g 10 \
-x264-params nal-hrd=cbr \
-b:v 10M -minrate 10M -maxrate 10M -bufsize 2M \
udp://192.168.178.22:2222
