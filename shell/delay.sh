#!/bin/sh
FNAME=$1
if [ -n $FNAME ]; then
	echo $FNAME
fi
[ -a $FNAME ]
echo "[ -a $FNAME ] return $?"
echo "3..."
sleep 1s
echo "2..."
sleep 1s
echo "1..."
sleep 1s
[ -e $FNAME ]
echo "[ -e $FNAME ] return $?"
