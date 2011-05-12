all:
	g++ -g -c SvrSocket.cpp -o SvrSocket.o
	g++ -g -c LogSvr.cpp -o LogSvr.o
	g++ -g main.cpp SvrSocket.o LogSvr.o -o logd
clean:
	rm -rf *.o logd
