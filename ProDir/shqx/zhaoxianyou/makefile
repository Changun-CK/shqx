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
PUBINCL = -I/oracle/htidc/public/c

# 通用框架库文件路径
PUBCPP =  /oracle/htidc/public/c/_public.cpp 
FTPCPP = /oracle/htidc/public/c/_ftp.cpp 
OCICPP = /oracle/htidc/public/c/_ooci.cpp

CC=g++

# CFLAGS = -O2
# CFLAGS = -O2 -Wall

CFLAGS = -g -Wno-write-strings -Wno-unused-variable


all: crtsurfdata crtsurfdata1 psurfdata_old psurfdata psurfdata1 psignallog wgetrain24\
     wgettemp24 pzhrain24file typthon forecast test intable


crtsurfdata:crtsurfdata.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f crtsurfdata ../bin/.
test:test.c
	$(CC) $(CFLAGS) -o test test.c $(PUBINCL) $(PUBCPP) 


crtsurfdata1:crtsurfdata1.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o crtsurfdata1 crtsurfdata1.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f crtsurfdata1 ../bin/.

psurfdata_old: psurfdata_old.cpp 
	$(CC) $(CFLAGS) -o psurfdata_old psurfdata_old.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) -lm -lc
	cp -f psurfdata_old ../bin/.

psurfdata: psurfdata.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS) -o psurfdata psurfdata.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f psurfdata ../bin/.

pzhrain24file: pzhrain24file.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o pzhrain24file pzhrain24file.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f pzhrain24file ../bin/.

psurfdata1: psurfdata1.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o psurfdata1 psurfdata1.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f psurfdata1 ../bin/.

psignallog: psignallog.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o psignallog psignallog.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f psignallog ../bin/.

typthon: typthon.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o typthon typthon.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f typthon ../bin/.
forecast: forecast.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS)  -o forecast forecast.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f forecast ../bin/.
adid: adid.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS)  -o adid adid.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f adid ../bin/.
visibility: visibility.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS)  -o visibility visibility.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f visibility ../bin/.
autostation: autostation.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS)  -o autostation autostation.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f autostation ../bin/.
intable: intable.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS)  -o intable intable.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f intable ../bin/.
rain: rain.cpp _shqx.h _shqx.cpp
	$(CC) -g $(CFLAGS)  -o rain rain.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp -lm -lc
	cp -f rain ../bin/.


wgetrain24: wgetrain24.cpp 
	$(CC) $(CFLAGS) -o wgetrain24 wgetrain24.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp -f wgetrain24 ../bin/.

wgettemp24: wgetrain24.cpp 
	$(CC) $(CFLAGS) -o wgettemp24 wgettemp24.cpp $(PUBINCL) $(PUBCPP) -lm -lc
	cp -f wgettemp24 ../bin/.


clean:
	rm -rf crtsurfdata crtsurfdata1 psurfdata_old psurfdata psurfdata1 psignallog wgetrain24
	rm -rf wgettemp24 pzhrain24file forecast typthon visibility
