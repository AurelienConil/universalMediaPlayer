#!/bin/sh
cd /home/pi/of/apps/universalMediaPlayer
echo "GIT PULL"
git pull
#better call git reset --hard HEAD
cd node/public
echo "RUN BUILD OF WEBAPP"
npm run build
cd ../../
echo "BUILD OF APP"
cd of_universalMediaPlayer
make -j2
echo "UPDATE IS DONE"

