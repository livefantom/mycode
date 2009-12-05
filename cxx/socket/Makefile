CC = gcc
CXX = g++
CFLAGS = -g


####### Implicit rules

.SUFFIXES: .c .cpp .cc .cxx .C

.cpp.o:
	    $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cc.o:
	    $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.cxx.o:
	    $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.C.o:
	    $(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

.c.o:
	    $(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<


all :
	$(CC) $(CFLAGS) $(INCPATH) multi-svr.c -o svr
	$(CC) $(CFLAGS) $(INCPATH) multi-clt.c -o clt

clean:
	rm -f svr
	rm -f clt
