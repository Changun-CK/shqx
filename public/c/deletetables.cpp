/*
 * 本程序用于删除表中指定时间前的记录, 参数可配置
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
	int  maxcounts;         // 每批删除的最大条数, 确保不会产生大事务
} starg;

CLogFile logfile;
connection conn;

// 本程序的业务流程主函数
bool _deletetables();

// 当前时间
char localhour[21];

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

	if (logfile.Open(starg.logfilename, "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", starg.logfilename); return -1;
	}

	while (1)
	{
		// 判断当前时间是否在启动时间之内
		memset(localhour, 0, sizeof(localhour));
		LocalTime(localhour, "hh");
		if (strstr(starg.hourstr, localhour) == 0) { sleep(60); continue; }

		// 连接数据库
		if (conn.m_state == 0)
		{
			if (conn.connecttodb(starg.connstr, "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
			{
				logfile.Write("connect database(%s) failed.\n%s\n", starg.connstr, conn.m_cda.message); sleep(60); continue;
			}
		}

		logfile.Write("delete table %s.\n",starg.tname);

		/* 主函数 */
		if (_deletetables() == false) logfile.Write("deletetables failed.\n");

		if (conn.m_state == 1) conn.disconnect();

		sleep(60);
	}

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}

/* 显示程序帮助 */
void Help(char *argv[])
{
	printf("\n本程序是数据中心的公共的功能模块，用于清理表中的历史数据，参数可配置.\n");
	printf("\nUsing: /ProDir/public/bin/deletetables xmlbuffer\n");

	printf("\nSample: /ProDir/public/bin/deletetables \"<logfilename>/log/shqx/deletetables_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_31</connstr><tname>T_SURFDATA</tname><where>where ddatetime < sysdate-3</where><hourstr>00,02,04,06,08,10,12,14,16,18,20,22</hourstr><maxcounts>500</maxcounts>\"\n\n");

	printf("xmlbuffer 解释如下:\n");
	printf("<logfilename>/log/shqx/deletetables_SURFDATA.log</logfilename> 程序运行的日志文件名.\n");
	printf("<connstr>shqx/pwdidc@snorcl11g_31</connstr> 目标数据库的连接参数.\n");
	printf("<tname>T_SURFDATA</tname> 待清理的表名.\n");
	printf("<where>where ddatetime < sysdate-5</where> 待清理数据的条件.\n");
	printf("<hourstr>01,02,03</hourstr> 本程序启动的时次, 小时, 时次之间用半角的逗号隔开.\n");
	printf("<maxcounts>500</maxcounts> 每次删除的最大条数, 确保不会产生大事务, 切记不可大于1000, 否则会报错:ORA-01795.\n\n");
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
	GetXMLBuffer(strxmlbuffer, "maxcounts", &starg.maxcounts);

	if (strlen(starg.logfilename) == 0) { logfile.Write("logfilename is NULL.\n"); return false; }
	if (strlen(starg.connstr) == 0) { logfile.Write("connstr is NULL.\n"); return false; }
	if (strlen(starg.tname) == 0) { logfile.Write("tname is NULL.\n");  return false; }
	if (strlen(starg.where) == 0) { logfile.Write("where is NULL.\n");  return false; }
	if (strlen(starg.hourstr) == 0) { logfile.Write("hourstr is NULL.\n");  return false; }
	if (starg.maxcounts == 0) { logfile.Write("maxcounts is FALSE.\n");  return false; }

	return true;
}

/* 删除表中指定条件的数据 */
bool _deletetables()
{
	int ccount = 0;
	char strrowid[51], strrowidn[starg.maxcounts][51];

	// 获取符合条件的记录的rowid
	sqlstatement selstmt(&conn);
	selstmt.prepare("select rowid from %s %s", starg.tname, starg.where);
	selstmt.bindout(1, strrowid, 50);

	if (selstmt.execute() != 0) { logfile.Write("selstmt.execute() failed.\n%s\n%s\n", selstmt.m_sql, selstmt.m_cda.message); return false; }

	// 生成删除数据的SQL语句, 一次删除maxcounts条记录
	int ii = 0;
	char strDelteSQL[10241];
	memset(strDelteSQL, 0, sizeof(strDelteSQL));
	sprintf(strDelteSQL, "delete from %s where rowid in(", starg.tname);

	char strtemp[11];
	for (ii=0; ii<starg.maxcounts; ii++)
	{
		memset(strtemp, 0, sizeof(strtemp));
		if (ii == 0) sprintf(strtemp, ":%d", ii+1);
		if (ii >  0) sprintf(strtemp, ",:%d", ii+1);
		strcat(strDelteSQL, strtemp);
	}
	strcat(strDelteSQL, ")");

	sqlstatement delstmt(&conn);
	delstmt.prepare(strDelteSQL);

	for (ii=0; ii<starg.maxcounts; ii++) delstmt.bindin(ii+1, strrowidn[ii], 50);

	while (1)
	{
		memset(strrowid, 0, sizeof(strrowid));
		if (selstmt.next() != 0) break;
		strcpy(strrowidn[ccount], strrowid);
		ccount++;

		if (ccount == starg.maxcounts)
		{
			if (delstmt.execute() != 0) { logfile.Write("delete %s failed.\n%s\n",starg.tname,delstmt.m_cda.message); return false; }

			conn.commit();

			memset(strrowidn, 0, sizeof(strrowidn));

			ccount = 0;
		}

		if (fmod(selstmt.m_cda.rpc, 10000) < 1)
		{
			logfile.Write("rows %d deleted.\n", selstmt.m_cda.rpc);

			// 判断当前时间是否在启动时间之内
			memset(localhour, 0, sizeof(localhour));
			LocalTime(localhour, "hh");
			if (strstr(starg.hourstr, localhour) == 0) return true;
		}
	}

	// 在以上循环处理的时候, 如果不足maxcounts, 那么我们单独处理
	for (ii=0; ii<ccount; ii++)
	{
		delstmt.prepare("delete from %s where rowid = :1", starg.tname);
		delstmt.bindin(1, strrowidn[ii], 50);

		if ( (delstmt.execute()!=0) && (delstmt.m_cda.rc!=1) )
		{
			logfile.Write("delete %s failed.\n%s\n", starg.tname, delstmt.m_cda.message); return false;
		}
	}

	logfile.Write("rows %d deleted.completed.\n",selstmt.m_cda.rpc);

	conn.commit();

	return true;
}
