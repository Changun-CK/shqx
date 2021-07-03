/* 
 * 本程序用于处理非机构化数据, 并写入到Oracle数据库中
 * 作者: 陈锟, 日期: 2020-3-26 */
#include "_freecplus.h"
#include "_ooci.h"

CLogFile logfile;
connection conn;
CDir Dir;

// 程序退出
void EXIT(int sig);

// 处理数据文件
bool _pzhrain24();

int main(int argc, char *argv[])
{
	if (argc != 5)
	{
		printf("\n程序用于处理非机构化数据, 并写入到Oracle数据库中.\n");
		printf("Using: %s logfilename readpath dbbuffer timestep\n", argv[0]);
		printf("Sample: %s /log/shqx/zhrain24.log /data/wfile/zhrain24 shqx/pwdidc@snorcl11g_31 30\n\n", argv[0]);
		return 0;
	}

	// 关闭全部的信号和输入输出
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[1]); return -1;
	}

	logfile.Write("程序启动.\n");

	while (1)
	{
		if (Dir.OpenDir(argv[2], "*") == false)
		{
			logfile.Write("Dir.OpenDir(%s) failed.\n", argv[2]); sleep(atoi(argv[4])); continue;
		}
		Dir.SetDateFMT("yyyymmddhh24miss");

		// 逐个处理目录中的数据文件
		while (1)
		{
			if (conn.connecttodb(argv[3], "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
			{
				logfile.Write("connect database(%s) failed.\n%s\n", argv[3], conn.m_cda.message); break;
			}

			// 处理数据文件
			if (_pzhrain24() == false) { logfile.WriteEx("失败.\n"); break; }
		}

		// 断开与数据库的连接
		conn.disconnect();

		sleep(atoi(argv[4]));
	}

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}

bool _pzhrain24()
{
	while (1)
	{
		if (Dir.ReadDir() == false) break;
		logfile.Write("开始处理数据文件 %s...", Dir.m_FullFileName);

		sqlstatement stmt(&conn);
		stmt.prepare("insert into T_ZHRAIN24 (filename, ddatetime, filesize, crttime, keyid) values(:1, to_date(:2, 'yyyymmddhh24miss'), :3, sysdate, SEQ_ZHRAIN24.nextval)");
		stmt.bindin(1,  Dir.m_FullFileName, 300);
		stmt.bindin(2,  Dir.m_ModifyTime, 20);
		stmt.bindin(3, &Dir.m_FileSize);
		if (stmt.execute() != 0)
		{
			if (stmt.m_cda.rc != 1) // 当rc == 1时, 为违反唯一约束条件, 不过在此条件下不太可能出现, 因为文件名在同一目录下不可能重复
			{
				logfile.Write("%s.\n", Dir.m_FullFileName);
				logfile.Write("stmt.execute() failed.\n%s\n%s\n", stmt.m_sql, stmt.m_cda.message);
				// 只要不是数据库session的错误, 程序就继续
				if ( (stmt.m_cda.rc >= 3113) && (stmt.m_cda.rc <= 3115) ) return false;
			}
		}
		logfile.WriteEx("成功.\n");
	}

	conn.commit();

	return true;
}
