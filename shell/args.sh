#! /bin/sh

ARG_NUM=$#
if [ $ARG_NUM -eq 0 ]; then
	echo "no args input"
else
	echo "args num=$#"
fi

NOW_ENV=$1
echo "NOW_ENV=$NOW_ENV"
if [ "$NOW_ENV" = "arm" ];then
	echo "env set to ARM"
elif [ "$NOW_ENV" = "x11" ] ; then
	echo "env set to X11"
else
	echo "env not changed"
fi
