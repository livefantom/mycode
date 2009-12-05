#!/bin/sh
FNAME=$1
if [ -n $FNAME ]; then
	echo $FNAME
fi
[ -a $FNAME ]	#whether the file is exist or not
echo "[ -a $FNAME ] return $?"
[ -e $FNAME ]
echo "[ -e $FNAME ] return $?"
[ -h $FNAME ]	#file is symbol link
echo "[ -h $FNAME ] return $?"
[ -L $FNAME ]
echo "[ -L $FNAME ] return $?"
[ -h $FNAME ]
echo "[ -h $FNAME ] return $?"
[ -h $FNAME ]
echo "[ -h $FNAME ] return $?"
[ -h $FNAME ]
echo "[ -h $FNAME ] return $?"
[ -h $FNAME ]
echo "[ -h $FNAME ] return $?"
[ -h $FNAME ]
echo "[ -h $FNAME ] return $?"
[ -h $FNAME ]
echo "[ -h $FNAME ] return $?"
