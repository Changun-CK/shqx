# 该脚本用于启动上海气象数据中心的服务程序

# 生成全国地面站分钟数据文件
/htidc/shqx/bin/crtsurfdata1 /htidc/shqx/ini/stcode1.ini /data/shqx/ftp/surfdata /log/shqx/crtsurfdata_txt.log txt&
/htidc/shqx/bin/crtsurfdata1 /htidc/shqx/ini/stcode1.ini /data/shqx/ftp/surfdata /log/shqx/crtsurfdata_xml.log xml&
# 注意，目录/data/shqx/ftp/surfdata下0.1天之前的数据文件自动清理，在crontab中调度，脚本如下：
# */10 * * * *  /htidc/public/bin/deletefiles /data/shqx/ftp/surfdata 0.1 "*"

# 采集全国地面站分钟数据文件
/htidc/public/bin/ftpgetfile /log/shqx/ftpgetfile_surfdata.log "<host>118.89.50.198:21</host><port>21</port><mode>1</mode><username>oracle</username><password>te.st1234TES@T</password><localpath>/data/shqx/sdata/surfdata</localpath><remotepath>/data/shqx/ftp/surfdata</remotepath><matchname>SURF_*.TXT,SURF_*.XML</matchname><ptype>1</ptype><remotepathbak></remotepathbak><listfilename>/data/shqx/ftplist/ftpgetfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpgetfile_surfdata.xml</okfilename><timetvl>30</timetvl>" &

# 处理全国地面站分钟数据文件并入库
/htidc/shqx/bin/psurfdata  /data/shqx/sdata/surfdata /log/shqx/psurfdata.log  shqx/pwdidc@snorcl11g_198 10 &
/htidc/shqx/bin/psurfdata1 /data/shqx/sdata/surfdata /log/shqx/psurfdata1.log shqx/pwdidc@snorcl11g_198 10 &

# 清理T_SURFDATA表中三天之前的数据
/htidc/public/bin/deletetables "<logfilename>/log/shqx/deletetables_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_198</connstr><tname>T_SURFDATA</tname><where>where ddatetime<sysdate-3</where><hourstr>00,02,04,08,10,12,14,16,18,20,22</hourstr>" &

# 从中国天气网获取每小时的实况降雨图
/htidc/shqx/bin/wgetrain24 /log/shqx/wgetrain24.log /data/wgettmp /data/wfile/zhrain24 &
# 从中国天气网获取每小时的实况温度图
/htidc/shqx/bin/wgettemp24 /log/shqx/wgettemp24.log /data/wgettmp /data/wfile/zhtemp24 &

# 用于处理全国逐小时雨量实况图片文件。
/htidc/shqx/bin/pzhrain24file /log/shqx/pzhrain24file.log shqx/pwdidc@snorcl11g_198 /data/wfile/zhrain24 /qxfile/zhrain24 T_ZHRAIN24 30 &
# 清理T_ZHRAIN24表中两天之前的数据
/htidc/public/bin/deletetables "<logfilename>/log/shqx/deletetables_ZHRAIN24.log</logfilename><connstr>shqx/pwdidc@snorcl11g_198</connstr><tname>T_ZHRAIN24</tname><where>where ddatetime<sysdate-2</where><hourstr>00,02,04,08,10,12,14,16,18,20,22</hourstr>" &

# /data/wfile/zhrain24和/data/wfile/zhtemp24目录下两天前的文件要定期删除

