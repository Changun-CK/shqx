#########################################
# 请注意，编译时，确定以下几点：
# 1、已安装oracle的客户端或oracle服务端程序，并且必须安装好OCI软件包
# 2、设置好ORACLE_HOME环境变量
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

# cJSON框架头文件路径
cJSON = -I/ProDir/public/cJSON-master
# cJSON框架库文件路径
cJSON_H = /ProDir/public/cJSON-master/cJSON.h
cJSON_C = /ProDir/public/cJSON-master/cJSON.c
# cJSON的库
#cJSON_L = -lcjson

CC=g++
# CFLAGS = -O2
# CFLAGS = -O2 -Wall
CFLAGS = -g -Wno-write-strings -Wno-unused-variable

$(shell if [ ! -d "../bin" ]; then mkdir ../bin; fi)

all: crtsurfdata crtsurfdata1 psurfdata psurfdata1 psurfdata1_self rand pobtcode psignallog ptyphoonbest pcloudobt test test1 prefinedata pacidobt pautostvis quyujisuan pzhrain24 wgetrain24 wgettemp24 pzhrain24file pcountrysignal

pcountrysignal: pcountrysignal.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o pcountrysignal pcountrysignal.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f pcountrysignal ../bin/.

crtsurfdata:crtsurfdata.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o crtsurfdata crtsurfdata.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f crtsurfdata ../bin/.

crtsurfdata1:crtsurfdata1.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o crtsurfdata1 crtsurfdata1.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f crtsurfdata1 ../bin/.

psurfdata: psurfdata.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o psurfdata psurfdata.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f psurfdata ../bin/.

psurfdata1: psurfdata1.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o psurfdata1 psurfdata1.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f psurfdata1 ../bin/.

psurfdata1_self: psurfdata1_self.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o psurfdata1_self psurfdata1_self.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f psurfdata1_self ../bin/.

psignallog: psignallog.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o psignallog psignallog.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f psignallog ../bin/.

ptyphoonbest: ptyphoonbest.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o ptyphoonbest ptyphoonbest.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f ptyphoonbest ../bin/.

pcloudobt: pcloudobt.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o pcloudobt pcloudobt.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f pcloudobt ../bin/.

prefinedata: prefinedata.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o prefinedata prefinedata.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f prefinedata ../bin/.

pacidobt: pacidobt.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o pacidobt pacidobt.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f pacidobt ../bin/.

pobtcode: pobtcode.cpp
	$(CC) $(CFLAGS) -o pobtcode pobtcode.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) -lm -lc
	cp -f pobtcode ../bin/.

pautostvis: pautostvis.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o pautostvis pautostvis.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f pautostvis ../bin/.

quyujisuan: quyujisuan.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o quyujisuan quyujisuan.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(PUBCPP) $(OCICPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f quyujisuan ../bin/.

pzhrain24: pzhrain24.cpp /ProDir/public/c/lib_freecplus.a
	$(CC) $(CFLAGS) -o pzhrain24 pzhrain24.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(OCICPP) /ProDir/public/c/lib_freecplus.a -lm -lc
	cp -f pzhrain24 ../bin/.

wgetrain24: wgetrain24.cpp /ProDir/public/c/lib_freecplus.a
	$(CC) $(CFLAGS) -o wgetrain24 wgetrain24.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(OCICPP) /ProDir/public/c/lib_freecplus.a -lm -lc
	cp -f wgetrain24 ../bin/.

wgettemp24: wgettemp24.cpp /ProDir/public/c/lib_freecplus.a
	$(CC) $(CFLAGS) -o wgettemp24 wgettemp24.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(OCICPP) /ProDir/public/c/lib_freecplus.a -lm -lc
	cp -f wgettemp24 ../bin/.

pzhrain24file: pzhrain24file.cpp _shqx.h _shqx.cpp
	$(CC) $(CFLAGS) -o pzhrain24file pzhrain24file.cpp $(ORAINCL) $(ORALIB) $(ORALIBS) $(PUBINCL) $(OCICPP) $(PUBCPP) _shqx.cpp $(cJSON) $(cJSON_C) -lm -lc
	cp -f pzhrain24file ../bin/.

rand: rand.cpp /ProDir/public/c/lib_freecplus.a
	$(CC) $(CFLAGS) -o rand rand.cpp $(PUBINCL) /ProDir/public/c/lib_freecplus.a

test: test.cpp /ProDir/public/c/lib_freecplus.a
	$(CC) $(CFLAGS) -o test test.cpp $(PUBINCL) /ProDir/public/c/lib_freecplus.a

test1: test1.cpp /ProDir/public/c/lib_freecplus.a
	$(CC) $(CFLAGS) -o test1 test1.cpp $(PUBINCL) /ProDir/public/c/lib_freecplus.a -lpthread

clean:
	rm -rf crtsurfdata psurfdata pobtcode crtsurfdata1 psurfdata1 psurfdata1_self psignallog ptyphoonbest pcloudobt test test1 prefinedata pacidobt pautostvis quyujisuan pzhrain24 wgetrain24 wgettemp24 pzhrain24file pcountrysignal ../bin/*
