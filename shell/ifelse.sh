#! /bin/sh

[ 3 -eq 2 ]
RET_VAL=$?
if [ $RET_VAL != 0 ]; then
#can not use [ $ARG_NUM ]
        echo "RET_VAL=$RET_VAL"
else
        echo "0, TRUE, success"
fi

ARG_NUM=$#
if [ $ARG_NUM = 0 ]; then
#can not use [ ! $ARG_NUM ]
	echo "no arg input"
elif [ $ARG_NUM -gt 3 ]; then
#can not use [ $ARG_NUM > 3 ]
	echo "too many args input"
else
	echo "$ARG_NUM arg(s) input"
fi
