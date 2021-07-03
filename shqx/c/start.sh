# 该脚本用于启动上海气象数据中心的服务程序

# 生成全国地面站分钟数据文件
/ProDir/shqx/bin/crtsurfdata /ProDir/shqx/ini/stcode.ini /data/shqx/ftp/surfdata /log/shqx/crtsurfdata.log &

# 采集全国地面站分钟数据文件
/ProDir/public/bin/ftpgetfiles /log/shqx/ftpgetfile_surfdata.log "<host>39.108.127.31:21</host><mode>1</mode><username>oracle</username><password>uys%#(736@^%__^daAA&%#_-@FX73)+#</password><localpath>/data/shqx/sdata/surfdata</localpath><remotepath>/data/shqx/ftp/surfdata</remotepath><matchname>SURF_*.TXT</matchname><ptype>1</ptype><remotepathbak></remotepathbak><listfilename>/data/shqx/ftplist/ftpgetfile_surfdata.list</listfilename><okfilename>/data/shqx/ftplist/ftpgetfile_surfdata.xml</okfilename><timetvl>30</timetvl>" &

# 处理国地面站分钟数据文件, 并入库
/ProDir/shqx/bin/psurfdata /data/shqx/sdata/surfdata /log/shqx/psurfdata.log shqx/pwdidc@snorcl11g_31 10 &

# 删除表中指定时间(3天)前的数据
/ProDir/public/bin/deletetables "<logfilename>/log/shqx/deletetables_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_31</connstr><tname>T_SURFDATA</tname><where>where ddatetime < sysdate-3</where><hourstr>00,02,04,06,08,10,12,14,16,18,20,22</hourstr><maxcounts>500</maxcounts>" &

# 从中国天气网获取每小时的实况降雨图
/ProDir/shqx/bin/wgetrain24 /log/shqx/wgetrain24.log /data/wgettmp /data/wfile/zhrain24 &

# /data/wfile/zhrain24和/data/wfile/zhtemp24目录下两天前的文件要定期删除
# 从中国天气网获取每小时的实况气温图
/ProDir/shqx/bin/wgettemp24 /log/shqx/wgettemp24.log /data/wgettmp /data/wfile/zhtemp24 &

# 用于处理全国逐小时雨量实况图片
/ProDir/shqx/bin/pzhrain24file /log/shqx/pzhrain24file.log shqx/pwdidc@snorcl11g_31 /data/wfile/zhrain24 /qxfile/zhrain24 T_ZHRAIN24 30 &

# 清理T_ZHRAIN24表中两天之前的数据
/ProDir/public/bin/deletetables "<logfilename>/log/shqx/deletetables_ZHRAIN24.log</logfilename><connstr>shqx/pwdidc@snorcl11g_31</connstr><tname>T_ZHRAIN24</tname><where>where ddatetime<sysdate-2</where><hourstr>00,02,04,08,10,12,14,16,18,20,22</hourstr>" &
