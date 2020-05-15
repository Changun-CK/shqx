/*
 * 本程序用于处理台风最佳路径数据, 并保存到数据库的T_TYPHOON_BEST表中
 * 作者: 陈锟  日期: 2020-02-19
*/
#include "_ooci.h"
#include "_shqx.h"

// 实例化日志文件类
CLogFile logfile;

// 实例化目录操作类
CDir Dir;

// oracle数据库连接池
connection conn;

// 处理数据文件
bool _ptyphoonbest();

// 程序退出
void EXIT(int sig);

int main(int argc, char *argv[])
{
	if (argc!=5)
	{
		printf("\n本程序用于处理台风最佳路径数据, 保存到数据库的T_TYPHOON_BEST表中.\n");
		printf("\n用法: /ProDir/shqx/bin/ptyphoonbest 数据文件存放的目录 日志文件名 数据库连接参数 程序运行时间间隔\n");
		printf("\n例如: /ProDir/shqx/bin/ptyphoonbest /data/shqx/sdata/typhoonbest /log/shqx/ptyphoonbest.log shqx/pwdidc@snorcl11g_31 10\n\n");
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
		if (Dir.OpenDir(argv[1], "CH*.txt", 1000, false, true) == false)
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
			if (_ptyphoonbest() == false)
			{
				logfile.WriteEx("处理文件失败.\n"); break;
			}
		}

		// 断开与数据库的连接
		if (conn.m_state == 1) conn.disconnect();

		sleep(atoi(argv[4]));
	}

	return 0;
}

// 处理数据文件
bool _ptyphoonbest()
{
	// 打开文件
	CFile File;
	// 实例化处理数据的类
	CTYPHOONBEST TYPHOONBEST(&conn, &logfile);

	if (File.Open(Dir.m_FullFileName, "r") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", Dir.m_FullFileName); return false;
	}

	// 读取文件中的每一行记录, 写入数据库的表中
	char strbuffer[101];

	while (1)
	{
		if (File.Fgets(strbuffer, sizeof(strbuffer), true) == false) break;
		UpdateStr(strbuffer, "  ", " ", true); // 把内容中的两个空格替换成一个空格

		if (strstr(strbuffer, "66666") != 0)
		{
			if (TYPHOONBEST.SplithdBuffer(strbuffer) == false)
			{
				logfile.Write("%s\n", strbuffer);
			}
			continue;
		}

		if (TYPHOONBEST.SplitinBuffer(strbuffer) == false) { logfile.Write("%s\n", strbuffer); continue; }

		long rc = TYPHOONBEST.InsertTable();

		if ( (rc>=3113) && (rc<=3115) ) return false;

		if (rc != 0) logfile.Write("%s\n", strbuffer);
	}

	File.Close();
	conn.commit();
	remove(Dir.m_FullFileName);

	logfile.WriteEx("成功(总记录=%u, 插入=%u, 更新=%u, 丢弃=%u).\n", TYPHOONBEST.m_total, TYPHOONBEST.m_inserted, TYPHOONBEST.m_updated, TYPHOONBEST.m_discarded);

	return true;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}
