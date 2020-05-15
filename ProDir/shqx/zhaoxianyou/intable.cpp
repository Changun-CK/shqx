/*`
 *  本程序用于把数据表文件计算在插入表中 
 *  作者：赵先佑 日期：20200225
*/
#include "_public.h"
#include "_ooci.h"
#include "_shqx.h"
// 主程序的参数
struct st_arg
{
	char connstr[101];
	char charset[51];
	char tname[51];
	char thour[51];
	char train[51];
	char cols[1001];
	char fieldname[1001];
	char fieldlen[501];
	char andstr_hour[501];
	char andstr_new[501];
	char andstr_rain[501];
	int  timetvl;
} starg;
struct st_jisuan m_stable;
CLogFile logfile;

connection conn;

// 本程序的业务流程主函数
bool _intable();


// 程序退出
void EXIT(int sig);

// 显示程序帮助
void Help(char *argv[]);

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer);

int main(int argc, char *argv[])
{
	if (argc != 3) { Help(argv); return -1; }

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[1]); return -1;
	}

	// 将main函数参数分解保存到starg结构体中
	if (xmltoarg(argv[2]) == false) return -1;

	// 连接数据库
	if (conn.connecttodb(starg.connstr, starg.charset) != 0)
	{
            logfile.Write("connect database %s failed.\n", starg.connstr); sleep(starg.timetvl); return -1;
	}

	// 本程序的业务流程主函数
	if (_intable() == false) { logfile.Write("import tables failed.\n"); return -1; }
	logfile.Write("import tables success.\n");

	conn.disconnect();

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n", sig);
	exit(0);
}

// 本程序的业务流程主函数
bool _intable()
{

    Cjisuan jisuan(&conn, &logfile);
/*
    if (jisuan.thour(starg.cols, starg.tname, starg.andstr_hour) != 0) return false;

    conn.commit();
    logfile.Write("表T_GDOBTHOURD处理完成.\n");
*/
   /* if (jisuan.tnew(starg.cols, starg.tname, starg.andstr_new) != 0)  return false;

    conn.commit();
    logfile.Write("表T_GDOBTREALD处理完成.\n");
*/
    if (jisuan.tday(starg.cols, starg.thour, starg.andstr_rain) != 0)  return false;

    conn.commit();
    logfile.Write("表T_GDAWSDAYD处理完成.\n");

    
    return true;
}

// 显示程序帮助
void Help(char *argv[])
{
	printf("\n本程序是数据计算的程序, 用于将计算后的数据入库到相应的表中.\n");
	printf("\nUsing: /oracle/htidc/public/bin/intable logfilename xmlbuffer\n");

	printf("\nSample: /oracle/htidc/shqx/bin/intable /oracle/log/shqx/intable.log \"<connstr>shqx/pwdidc@snorcl11g_128</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_AREAOBTMIND</tname><thour>T_GDOBTHOURD</thour><train>T_GDOBTRAIND</train><cols>to_char(ddatetime, 'yyyymmddhh24miss'), obtid, station, baoformat, bps, ys, w3a, wdidf, wdidd, wd2df, wd2dd, wd10df, wd10dd, wd3smaxdf, wd3smaxdd, wd3smaxtime, wd10maxdf, wd10maxdd, wd10maxtime, wd3daymax, wf3daymax, wd3daymaxtime, wd10daymax, wf10daymax, wd10daymaxtime, t, maxt, maxttime, mint, minttime, daymaxt, daymaxttime, daymint, dayminttime, rh, maxrh, maxrhtime, minrh, minrhtime, dp, p, maxp, maxptime, minp, minptime, p0, hourrf, dayrf, rfmark1, minrf, wpv, othfields, rddatetime, procsts</cols><fieldname>ddatetime, obtid, station, baoformat, bps, ys, w3a, wdidf, wdidd, wd2df, wd2dd, wd10df, wd10dd, wd3smaxdf, wd3smaxdd, wd3smaxtime, wd10maxdf, wd10maxdd, wd10maxtime, wd3daymax, wf3daymax, wd3daymaxtime, wd10daymax, wf10daymax, wd10daymaxtime, t, maxt, maxttime, mint, minttime, daymaxt, daymaxttime, daymint, dayminttime, rh, maxrh, maxrhtime, minrh, minrhtime, dp, p, maxp, maxptime, minp, minptime, p0, hourrf, dayrf, rfmark1, minrf, wpv, othfields, rddatetime, procsts</fieldname><andstr_hour>and procsts = 1 and substr(to_char(ddatetime, 'yyyy-mm-dd hh24:mi:ss'), 15, 5) = '00:00'</andstr_hour><andstr_new>and procsts = 1</andstr_new><andstr_rain>and procsts = 1</andstr_rain>\"\n\n");
	printf("logfilename 程序运行的日志文件名.\n");
	printf("xmlbuffer 程序的传输方式, 如下:\n");
	printf("<connstr>shqx/pwdidc@snorcl11g_31</connstr> 数据库的连接参数.\n");
	printf("<charset>SIMPLIFIED CHINESE_CHINA.AL32UTF8</charset> 数据库的字符集, 这个参数要与数据源数据库保持一致, 否则会出现中文乱码.\n");
	printf("<tname>T_SURFDATA</tname> 待导出数据的表名.\n");
	printf("<cols>obtid, to_char(ddatetime, 'yyyymmddhh24miss'), t, p, u, wd, wf, r, vis</cols> 需要导出字段的列表, 可以采用函数.\n");
	printf("<fieldname>obtid, ddatetime, t, p, u, wd, wf, r, vis</fieldname> 导出字段的别名列表, 必须与cols一一对应.\n");
	printf("<andstr_hour> and obtid in ('59293','50745')</andstr_hour> 整点数据的附加条件, 注意, 关键字and不能缺.\n");
	printf("<andstr_new> and procsts = 1</andstr_new> 最新数据的附加条件, 注意, 关键字and不能缺.\n");
	printf("<andstr_rain> and procsts = 1</andstr_rain> 最新数据的附加条件, 注意, 关键字and不能缺.\n");
	printf("<andstr_day> and procsts = 1</andstr_day> 日数据的附加条件, 注意, 关键字and不能缺.\n");
	printf("<timetvl>30</timetvl> 导出数据的时间间隔, 单位:秒, 建议大于10.\n\n");
	printf("以上参数，除了andstr_hour和andstr_new，其它字段都不允许为空.\n\n");
}

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer)
{
	memset(&starg, 0, sizeof(struct st_arg));

	GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr);
	if (strlen(starg.connstr) == 0) { logfile.Write("connstr is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "charset", starg.charset);
	if (strlen(starg.charset) == 0) { logfile.Write("charset is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "tname", starg.tname);
	if (strlen(starg.tname) == 0) { logfile.Write("tname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "thour", starg.thour);
	if (strlen(starg.thour) == 0) { logfile.Write("thour is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "cols", starg.cols);
	if (strlen(starg.cols) == 0) { logfile.Write("cols is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "fieldname", starg.fieldname);
	if (strlen(starg.fieldname) == 0) { logfile.Write("fieldname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "andstr_hour", starg.andstr_hour);
	if (strlen(starg.andstr_hour) == 0) { logfile.Write("andstr_hour is NULL.\n");  return false; }

        GetXMLBuffer(strxmlbuffer, "andstr_new", starg.andstr_new);
	if (strlen(starg.andstr_new) == 0) { logfile.Write("andstr_new is NULL.\n");  return false; }

        GetXMLBuffer(strxmlbuffer, "andstr_rain", starg.andstr_rain);
	if (strlen(starg.andstr_rain) == 0) { logfile.Write("andstr_rain is NULL.\n");  return false; }


	return true;
};
