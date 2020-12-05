#!/usr/bin/env bash

curl 127.0.0.1:8080/health > /dev/null

if [[ $? != 0 ]] ; then
	echo "server failed healthcheck. Restarting"
	sytemctl restart sunrise_alarm
fi
