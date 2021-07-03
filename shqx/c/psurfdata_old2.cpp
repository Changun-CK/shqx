/*
 * 本程序用于处理测试数据, 并保存到数据库的t_surfdata表中
 * 作者: 陈锟  日期: 2019-12-01
*/

#include "_ooci.h"
#include "_public.h"
#include "_shqx.h"

// 实例化日志文件类
CLogFile logfile;

// 实例化目录操作类
CDir Dir;

// oracle数据库连接池
connection conn;

// 处理数据文件
bool _psurfdata();

// 程序退出
void EXIT(int sig);

int main(int argc, char *argv[])
{
	if (argc!=5)
	{
		printf("\n本程序用于处理全国气象站点观测的分钟数据, 保存到数据库的t_surfdata表中.\n");
		printf("\n用法: /ProDir/shqx/bin/psurfdata 数据文件存放的目录 日志文件名 数据库连接参数 程序运行时间间隔\n");
		printf("\n例如: /ProDir/shqx/bin/psurfdata /data/shqx/sdata/surfdata /log/shqx/psurfdata.log shqx/pwdidc@snorcl11g_31 10\n\n");
		return -1;
	}

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[2], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[2]); return -1;
	}

	logfile.Write("程序启动.\n");

	while (1)
	{
		/* 扫描数据文件存放的目录 */
		if (Dir.OpenDir(argv[1], "SURF_ZH_*.txt", 100, false, true) == false)
		{
			logfile.Write("Dir.OpenDir(%s) failed.\n", argv[1]); sleep(atoi(argv[4])); continue;
		}

		/* 逐个处理目录中的数据文件 */
		while (1)
		{
			if (Dir.ReadDir() == false) break;

			if (conn.m_state == 0)
			{
				if (conn.connecttodb(argv[3], "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
				{
					logfile.Write("connect database(%s) failed.\n%s\n", argv[3], conn.m_cda.message); break;
				}
			}

			logfile.Write("开始处理文件 %s...", Dir.m_FullFileName);

			// 处理数据文件
			if (_psurfdata() == false)
			{
				logfile.WriteEx("处理文件失败.\n"); break;
			}

			logfile.WriteEx("成功.\n");
		}

		// 断开与数据库的连接
		if (conn.m_state == 1) conn.disconnect();

		sleep(atoi(argv[4]));
	}

	return 0;
}

// 处理数据文件
bool _psurfdata()
{
	// 打开文件
	CFile File;
	if (File.Open(Dir.m_FullFileName, "r") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", Dir.m_FullFileName); return false;
	}

	CSURFDATA SURFDATA(&conn, &logfile);

	// 读取文件中的每一行记录
	// 写入数据库的表中
	char strbuffer[301];

	while (1)
	{
		if (File.Fgets(strbuffer, sizeof(strbuffer)) == false) break;

		if (SURFDATA.SplitBuffer(strbuffer) == false) { logfile.Write("%s\n", strbuffer); continue; }

		long rc = SURFDATA.InsertTable();

		if ( (rc>=3113) && (rc<=3115) ) return false;

		if (rc != 0) { logfile.Write("%s\n", strbuffer); }
	}

/*
1.判断记录是否存在。
2.如果存在，不更新，返回。如果不存在，插入，返回。
3.如果存在，更新，返回，如果不存在，插入，返回。如果在更新和插入的时间，记录无效的，写出日志。
*/

	File.Close();
	// 提交事务
	conn.commit();
	// 删除文件
	remove(Dir.m_FullFileName);

	return true;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}
