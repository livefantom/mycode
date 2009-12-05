#! /bin/sh
# my first example bash script

ARG_NUM=$#;
if [ $ARG_NUM = 0 ]; then
	echo "no arg input"
elif [ $ARG_NUM = 1 ]; then
	mkdir $1
	if [ $? != 0 ]; then
		echo -n "folder '$1' exists, delete it(y,N)?"
		read ANSWER
		if [ "$ANSWER" = "y" ]; then
			rmdir $1
			echo "folder '$1' removed"
		fi
	else
		echo "folder '$1' created"
	fi
else
	echo "too many args input"
fi
