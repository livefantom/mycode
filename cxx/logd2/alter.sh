#! /bin/sh
ipcs -m
echo "remove shm ..."
ipcrm -M 0x444444
ipcs -m

