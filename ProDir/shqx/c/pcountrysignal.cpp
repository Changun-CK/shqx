/*
 * 本程序用于处理全国预警信号, 并保存到数据库的T_COUNTRYSIGNAL表中
 * 作者: 陈锟  日期: 2020-02-16
*/
#include "_ooci.h"
#include "_shqx.h"
#include "cJSON.h"

// 实例化日志文件类
CLogFile logfile;

// 实例化目录操作类
CDir Dir;

// oracle数据库连接池
connection conn;

// 处理数据文件
bool _pcountrysignal();

// 程序退出
void EXIT(int sig);

int main(int argc, char *argv[])
{
	if (argc!=5)
	{
		printf("\n本程序用于处理分区预警信号发布日志, 保存到数据库的T_T_COUNTRYSIGNAL表中.\n");
		printf("\n用法: /ProDir/shqx/bin/pcountrysignal 数据文件存放的目录 日志文件名 数据库连接参数 程序运行时间间隔\n");
		printf("\n例如: /ProDir/shqx/bin/pcountrysignal /data/shqx/sdata/countrysignal /log/shqx/pcountrysignal.log shqx/pwdidc@snorcl11g_31 10\n\n");
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
		if (Dir.OpenDir(argv[1], "*.json", 1000, false, true) == false)
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
			if (_pcountrysignal() == false)
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
bool _pcountrysignal()
{
	int fp = open(Dir.m_FullFileName, O_RDONLY);
	if (fp == -1) { logfile.Write("File.Open(%s) failed.\n", Dir.m_FullFileName); return false; }
	// 实例化处理数据的类
	CCOUNTRYSIGNAL COUNTRYSIGNAL(&conn, &logfile);

	// 读取文件中的每一行记录, 写入数据库的表中
	char strbuffer[102401];

	int red = read(fp, strbuffer,102400);
	if (red == -1)
	{
		logfile.Write("read %s is failed.\n", Dir.m_FullFileName);
		close(fp); remove(Dir.m_FullFileName); return false;
	}
	if (strstr(strbuffer, "w1") == 0) { close(fp); remove(Dir.m_FullFileName); return false; }
	if (strstr(strbuffer, "w2") == 0) { close(fp); remove(Dir.m_FullFileName); return false; }
	if (strstr(strbuffer, "w3") == 0) { close(fp); remove(Dir.m_FullFileName); return false; }
	if (strstr(strbuffer, "w4") == 0) { close(fp); remove(Dir.m_FullFileName); return false; }
	if (strstr(strbuffer, "w8") == 0) { close(fp); remove(Dir.m_FullFileName); return false; }

	if (COUNTRYSIGNAL.SplitBuffer(strbuffer) == false)
	{
		logfile.Write("%s\n", strbuffer);
		close(fp); remove(Dir.m_FullFileName); return false;
	}

	long rc = COUNTRYSIGNAL.InsertTable();

	if ( (rc>=3113) && (rc<=3115) ) { close(fp); remove(Dir.m_FullFileName); return false; }

	if (rc != 0) logfile.Write("%s\n", strbuffer);

	close(fp);
	conn.commit();
	remove(Dir.m_FullFileName);

	logfile.WriteEx("成功(总记录=%u, 插入=%u, 更新=%u, 丢弃=%u).\n", COUNTRYSIGNAL.m_total, COUNTRYSIGNAL.m_inserted, COUNTRYSIGNAL.m_updated, COUNTRYSIGNAL.m_discarded);

	return true;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}
