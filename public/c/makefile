#########################################
# 请注意，编译时，确定以下几点：
# 1、已安装oracle的客户端或oracle服务端程序，并且必须安装好OCI软件包
# 2、设置好ORACLE_HOME环境变量
#
#########################################

# oracle头文件路径
ORAINCL = -I$(ORACLE_HOME)/rdbms/public
# oracle库文件路径
ORALIB =  -L$(ORACLE_HOME)/lib -L.
# oracle的oci库
ORALIBS = -lclntsh

# 通用框架头文件路径
PUBINCL = -I/ProDir/public/c
# 通用框架库文件路径
PUBCPP = /ProDir/public/c/_public.cpp 
FTPCPP = /ProDir/public/c/_ftp.cpp 
OCICPP = /ProDir/public/c/_ooci.cpp

CC=g++
# CFLAGS = -O2
# CFLAGS = -O2 -Wall
CFLAGS = -g -Wno-write-strings -Wno-unused-variable

$(shell if [ ! -d "../bin" ]; then mkdir ../bin; fi)

all: libftp.a ftpgetfiles deletefiles deletetables cJSONtest exptables \
     ftpputfiles deletearchive dminoracle wgetclient wgettrf hsmtables \
     diskinfo cpuinfo tbspaceinfo tbspaceinfo_wucz tcpputfiles tcpfileserver\
	 tcpgetfiles tcpfileserver1

libftp.a: ftplib.c
	gcc -c -o libftp.a ftplib.c

cJSONtest: cJSONtest.c cJSON.h cJSON.c
	gcc -g -o cJSONtest cJSONtest.c cJSON.c -lm

deletetables: deletetables.cpp _public.h _public.cpp _ooci.h _ooci.cpp
	$(CC) $(CFLAGS) -o deletetables deletetables.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) _public.cpp _ooci.cpp -lm -lc
	cp -f deletetables ../bin/.

ftpgetfiles: ftpgetfiles.cpp _public.h _public.cpp _ftp.h _ftp.cpp libftp.a
	$(CC) $(CFLAGS) -o ftpgetfiles ftpgetfiles.cpp $(PUBINCL) _public.cpp _ftp.cpp libftp.a
	cp ftpgetfiles ../bin/. 

ftpputfiles: ftpputfiles.cpp _public.h _public.cpp _ftp.h _ftp.cpp libftp.a
	$(CC) $(CFLAGS) -o ftpputfiles ftpputfiles.cpp $(PUBINCL) _public.cpp _ftp.cpp libftp.a
	cp ftpputfiles ../bin/. 

deletefiles: deletefiles.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o deletefiles deletefiles.cpp lib_freecplus.so
	cp deletefiles ../bin/. 

deletearchive: deletearchive.cpp _public.h _public.cpp
	$(CC) $(CFLAGS) -o deletearchive deletearchive.cpp _public.cpp 
	cp deletearchive ../bin/. 

exptables: exptables.cpp _public.h _public.cpp _ooci.h _ooci.cpp
	$(CC) $(CFLAGS) -o exptables exptables.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) _public.cpp _ooci.cpp -lm -lc
	cp -f exptables ../bin/.

dminoracle: dminoracle.cpp _public.h _public.cpp _ooci.h _ooci.cpp
	$(CC) $(CFLAGS) -o dminoracle dminoracle.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) _public.cpp _ooci.cpp -lm -lc
	cp -f dminoracle ../bin/.

wgetclient: wgetclient.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o wgetclient wgetclient.cpp lib_freecplus.so
	cp wgetclient ../bin/. 

wgettrf: wgettrf.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o wgettrf wgettrf.cpp lib_freecplus.so
	cp wgettrf ../bin/. 

hsmtables: hsmtables.cpp lib_freecplus.so _ooci.h _ooci.cpp
	$(CC) $(CFLAGS) -o hsmtables hsmtables.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) _ooci.cpp lib_freecplus.so -lm -lc
	cp -f hsmtables ../bin/.

diskinfo: diskinfo.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o diskinfo diskinfo.cpp lib_freecplus.so
	cp diskinfo ../bin/. 

cpuinfo: cpuinfo.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o cpuinfo cpuinfo.cpp lib_freecplus.so
	cp cpuinfo ../bin/. 

tbspaceinfo: tbspaceinfo.cpp lib_freecplus.so _ooci.h _ooci.cpp
	$(CC) $(CFLAGS) -o tbspaceinfo tbspaceinfo.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) _ooci.cpp lib_freecplus.so -lm -lc
	cp tbspaceinfo ../bin/. 

tbspaceinfo_wucz: tbspaceinfo_wucz.cpp lib_freecplus.so _ooci.h _ooci.cpp
	$(CC) $(CFLAGS) -o tbspaceinfo_wucz tbspaceinfo_wucz.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) _ooci.cpp lib_freecplus.so -lm -lc
	cp tbspaceinfo_wucz ../bin/. 

tcpputfiles: tcpputfiles.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o tcpputfiles tcpputfiles.cpp lib_freecplus.so
	cp tcpputfiles ../bin/. 

tcpgetfiles: tcpgetfiles.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o tcpgetfiles tcpgetfiles.cpp lib_freecplus.so
	cp tcpgetfiles ../bin/. 

tcpfileserver: tcpfileserver.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o tcpfileserver tcpfileserver.cpp lib_freecplus.so -lm -lc
	cp tcpfileserver ../bin/. 

tcpfileserver1: tcpfileserver1.cpp lib_freecplus.so
	$(CC) $(CFLAGS) -o tcpfileserver1 tcpfileserver1.cpp lib_freecplus.so -lm -lc -lpthread
	cp tcpfileserver1 ../bin/. 

clean:
	rm -rf libftp.a ftpgetfiles deletefiles deletetables cJSONtest exptables \
    dminoracle ftpputfiles deletearchive wgetclient wgettrf hsmtables diskinfo \
    tcpfileserver cpuinfo tbspaceinfo tbspaceinfo_wucz tcpputfiles tcpgetfiles \
    tcpfileserver1 *.a *.so ../bin/*
