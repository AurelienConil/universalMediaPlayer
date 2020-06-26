#!/bin/sh
cd /home/pi/Documents/openFrameworks/apps/universalMediaPlayer
echo "GIT PULL"
git pull
#better call git reset --hard HEAD
echo "BUILD OF APP"
cd of_universalMediaPlayer
sudo make -j2
echo "UPDATE IS DONE"

