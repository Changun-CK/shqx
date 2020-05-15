/*
 * 本程序用于FTP采集文件, 参数可配置
 * 作者: 陈锟, 时间: 2019-12-09
 */
#include "_public.h"
#include "_ooci.h"

struct st_arg
{
    char logfilename[301];  // 程序运行的日志文件名 
    char connstr[81];       // 目标数据库的连接参数 
    char tname[51];         // 待清理的表名
    char where[101];        // 待清理数据的条件
    char hourstr[101];      // 程序启动的时次, 小时, 时次之间用半角的逗号隔开
} starg;

// 实例化日志文件类
CLogFile logfile;

// oracle数据库连接池
connection conn;

// 本程序的业务流程主函数
bool _deletetables();

/* 删除表中指定条件的数据 */
bool deleteWhereTab();

// 程序退出
void EXIT(int sig);

// 显示程序帮助
void Help(char *argv[]);

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer);

int main(int argc, char *argv[])
{
	if (argc!=2) { Help(argv); return -1; }

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	// 将main函数参数分解保存到starg结构体中
	if (xmltoarg(argv[1]) == false) return -1;

	// 判断当前时间是否在启动时间之内
	char localhour[21]; memset(localhour, 0, sizeof(localhour));
	LocalTime(localhour, "hh");
	if (strstr(starg.hourstr, localhour) == 0) return 0;

	if (logfile.Open(starg.logfilename, "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", starg.logfilename); return -1;
	}

	logfile.Write("开始清理表中(%s)数据.\n", starg.tname);
	while (1)
	{
		/* 主函数 */
		if (_deletetables() == false) { sleep(60); continue; }
		sleep(60);
	}
	logfile.Write("清理完成.\n\n");

	if (conn.m_state == 1) conn.disconnect();

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}

// 本程序的业务流程主函数
bool _deletetables()
{
	if (conn.m_state == 0)
	{
		if (conn.connecttodb(starg.connstr, "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
		{
			logfile.Write("connect database(%s) failed.\n%s\n", starg.connstr, conn.m_cda.message); return false;
		}
	}

	/* 开始删除表中指定条件的数据 */
	if (deleteWhereTab() == false) return false;

	return true;
}

/* 显示程序帮助 */
void Help(char *argv[])
{
	printf("\n本程序是数据中心的公共的功能模块，用于清理表中的历史数据，参数可配置.\n");
	printf("\nUsing: /ProDir/public/bin/deletetables xmlbuffer\n");

	printf("\nSample: /ProDir/public/bin/deletetables \"<logfilename>/log/shqx/deletetables_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_31</connstr><tname>T_SURFDATA</tname><where>where ddatetime < sysdate-5</where><hourstr>01,02,03</hourstr>\"\n\n");

	printf("xmlbuffer 解释如下:\n");
	printf("<logfilename>/log/shqx/deletetables_SURFDATA.log</logfilename> 程序运行的日志文件名.\n");
	printf("<connstr>shqx/pwdidc@snorcl11g_31</connstr> 目标数据库的连接参数.\n");
	printf("<tname>T_SURFDATA</tname> 待清理的表名.\n");
	printf("<where>where ddatetime < sysdate-5</where> 待清理数据的条件.\n");
	printf("<hourstr>01,02,03<hourstr> 本程序启动的时次, 小时, 时次之间用半角的逗号隔开.\n\n");
}

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer)
{
	memset(&starg, 0, sizeof(struct st_arg));

	GetXMLBuffer(strxmlbuffer, "logfilename", starg.logfilename);
	GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr);
	GetXMLBuffer(strxmlbuffer, "tname", starg.tname);
	GetXMLBuffer(strxmlbuffer, "where", starg.where);
	GetXMLBuffer(strxmlbuffer, "hourstr", starg.hourstr);

	if (strlen(starg.logfilename) == 0) { logfile.Write("logfilename is NULL.\n"); return false; }
	if (strlen(starg.connstr) == 0) { logfile.Write("connstr is NULL.\n"); return false; }
	if (strlen(starg.tname) == 0) { logfile.Write("tname is NULL.\n");  return false; }
	if (strlen(starg.where) == 0) { logfile.Write("where is NULL.\n");  return false; }
	if (strlen(starg.hourstr) == 0) { logfile.Write("hourstr is NULL.\n");  return false; }

	return true;
}

/* 删除表中指定条件的数据 */
bool deleteWhereTab()
{
	sqlstatement stmt;
	char strPrepare[1024];
	memset(strPrepare, 0, sizeof(strPrepare));

	sprintf(strPrepare, "delete from %s %s", starg.tname, starg.where);

	if (stmt.m_state == 0)
	{
		stmt.connect(&conn);
		stmt.prepare(strPrepare);
	}

	if (stmt.execute() != 0) { logfile.Write("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message); return false; }

	// 提交事务
	conn.commit();

	return true;
}
