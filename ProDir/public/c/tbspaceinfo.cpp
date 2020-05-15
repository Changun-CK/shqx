/*
 * 本程序用于收集Oracle数据库表空间信息, 参数可配置
 * 作者: 陈锟, 时间: 2020-04-01
 */
#include "_freecplus.h"
#include "_ooci.h"

CLogFile logfile;

connection conn;

struct st_tbspaceinfo
{
	char   tbspacename[101];
	double total_bytes;
	double free_bytes;
	double use_bytes;
	double use;
};

/* 程序退出 */
void EXIT(int sig);

/* 显示程序帮助 */
void Help(char *argv[]);

/* 业务主流程函数 */
bool _tbspaceinfo(const char *hostname, const char *outputpath);

int main(int argc, char *argv[])
{
	if (argc != 5) { Help(argv); return -1; }

	/* 关闭全部的信号和输入输出 */
	CloseIOAndSignal();

	/* 处理程序退出的信号, 在shell状态下可用"kill + 进程号", 正常终止进程, 但是请不要"kill -9 +进程号"强行终止 */
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("日志文件(%s)打开失败.\n", argv[1]);
	}

	char Outpath[301]; memset(Outpath, 0, sizeof(Outpath));
	char Hostname[301]; memset(Hostname, 0, sizeof(Hostname));
	strcpy(Outpath, argv[2]); strcpy(Hostname, argv[4]);

	// 连接数据库
	if (conn.connecttodb(argv[3], "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
	{
		logfile.Write("connect database(%s) failed.\n", argv[3]); return -1;
	}

	/* 主函数 */
	if (_tbspaceinfo(Hostname, Outpath) == false)
	{
		logfile.Write("Failed to collect tablespace information of Oracle database(%s).\n", argv[3]); return -1;
	}

	if (conn.m_state == 1) conn.disconnect();

	return 0;
}

/* 程序退出 */
void EXIT(int sig)
{
	printf("程序退出, sig = %d\n\n", sig);
	exit(0);
}

/* 显示程序帮助 */
void Help(char *argv[])
{
	printf("\n此程序运行在数据中心应用的服务器上, 收集Oracle表空间的信息, 并写入xml文件.\n");
	printf("\nUsing:/ProDir/public/bin/tbspaceinfo logfilename outputpath username hostname\n");
	printf("\nSample:/ProDir/public/bin/tbspaceinfo /log/systempara/tbspaceinfo.log /data/systempara/oracle shqx/pwdidc@snorcl11g_31 39.108.127.31\n\n");
	printf("logfilename:日志文件名.\n");
	printf("outputpath: 存放磁盘使用率的xml文件所在的目录.\n");
	printf("username:   监控的远程数据库的用户名/密码@连接名.\n");
	printf("\n此程序运行在数据中心应用的服务器上.\n\n\n");
}

/* 主函数 */
bool _tbspaceinfo(const char *hostname, const char *outputpath)
{
	struct st_tbspaceinfo sttbspaceinfo;

	sqlstatement stmtsel(&conn);
	stmtsel.prepare("select * from (\n\
Select a.tablespace_name,\n\
to_char(a.bytes/1024/1024,'99999.999') total_bytes,\n\
to_char(b.bytes/1024/1024,'99999.999') free_bytes,\n\
to_char(a.bytes/1024/1024 - b.bytes/1024/1024,'99,999.999') use_bytes,\n\
to_char((1 - b.bytes/a.bytes)*100,'99.99') || % use\n\
from (select tablespace_name,\n\
sum(bytes) bytes\n\
from dba_data_files\n\
group by tablespace_name) a,\n\
(select tablespace_name,\n\
sum(bytes) bytes\n\
from dba_free_space\n\
group by tablespace_name) b\n\
where a.tablespace_name = b.tablespace_name\n\
union all\n\
select c.tablespace_name,\n\
to_char(c.bytes/1024/1024,'99,999.999') total_bytes,\n\
to_char( (c.bytes-d.bytes_used)/1024/1024,'99,999.999') free_bytes,\n\
to_char(d.bytes_used/1024/1024,'99,999.999') use_bytes,\n\
to_char(d.bytes_used*100/c.bytes,'99.99') || % use\n\
from\n\
(select tablespace_name,sum(bytes) bytes\n\
from dba_temp_files group by tablespace_name) c,\n\
(select tablespace_name,sum(bytes_cached) bytes_used\n\
from v$temp_extent_pool group by tablespace_name) d\n\
where c.tablespace_name = d.tablespace_name\n\
)");

	stmtsel.bindout(1,  sttbspaceinfo.tbspacename, 100);
	stmtsel.bindout(2, &sttbspaceinfo.total_bytes);
	stmtsel.bindout(3, &sttbspaceinfo.free_bytes);
	stmtsel.bindout(4, &sttbspaceinfo.use_bytes);
	stmtsel.bindout(5, &sttbspaceinfo.use);

	if (stmtsel.execute() != 0)
	{
		logfile.Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message); return false;
	}

	// 得到当前时间的字符串
	char strlocaltime[21];
	memset(strlocaltime, 0, sizeof(strlocaltime));
	LocalTime(strlocaltime, "yyyymmddhh24miss");

	// 设计xml文件名
	char xmlfilename[301];
	memset(xmlfilename, 0, sizeof(xmlfilename));
	sprintf(xmlfilename, "%s/tbspaceinfo_%s_%s.xml", outputpath, strlocaltime, hostname);

	CFile xmlFile;
	// 以命名.tmp的方式打开xml文件
	if (xmlFile.OpenForRename(xmlfilename, "w+") == false)
	{
		logfile.Write("xmlFile.OpenForRename(%s) failed.\n", xmlfilename);
	}

	// 写入文件数据
	xmlFile.Fprintf("<data>\n");

	while(1)
	{
		memset(&sttbspaceinfo, 0, sizeof(struct st_tbspaceinfo));
		if (stmtsel.next() != 0) break;
		xmlFile.Fprintf("<nodip>%s</nodip>"\
					"<crttime>%s</crttime>"\
					"<tbspacename>%s</tbspacename>"\
					"<total_bytes>%0.03lf</total_bytes>"\
					"<free_bytes>%0.03lf</free_bytes>"\
					"<use_bytes>%0.03lf</use_bytes>"\
					"<use>%0.02lf</use>"\
					"<endl/>\n",\
					hostname,\
					strlocaltime,\
					sttbspaceinfo.tbspacename,\
					sttbspaceinfo.total_bytes,\
					sttbspaceinfo.free_bytes,\
					sttbspaceinfo.use_bytes,\
					sttbspaceinfo.use);
	}
	xmlFile.Fprintf("</data>\n");

	xmlFile.CloseAndRename(); // 关闭文件并重命名(去掉.tmp)

	logfile.Write("create %s is success.\n", xmlfilename);

	return true;
}
