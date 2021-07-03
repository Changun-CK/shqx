/*
 * 本程序用于收集CUP运行情况, 参数可配置
 * 作者: 陈锟, 时间: 2020-03-29
 */
#include "_freecplus.h"

struct st_cpuinfo
{
	double user;    // 用户态时间
	double nice;    // 用户态时间(低优先级，nice>0)
	double system;  // 内核态时间
	double idle;    // 空闲时间
	double iowait;  // I/O等待时间
	double irq;     // 硬中断
	double softirq; // 软中断
	double total;   // 总时间
};

CLogFile logfile;

/* 程序退出 */
void EXIT(int sig);

/* 显示程序帮助 */
void Help(char *argv[]);

/* 业务主流程函数 */
bool _cpuinfo(const char *hostname, const char *outputpath);

/* 加载CPU第一行的参数信息 */
bool LoadCPUInfo(struct st_cpuinfo &stcpuinfo);

int main(int argc, char *argv[])
{
	if (argc != 4) { Help(argv); return -1; }

	/* 关闭全部的信号 */
	CloseIOAndSignal();

	/* 处理程序退出的信号 */
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[2], "a+") == false)
	{
		printf("日志文件(%s)打开失败.\n", argv[2]);
	}

	char Hostname[51]; memset(Hostname, 0, sizeof(Hostname));
	char Outpath[301]; memset(Outpath, 0, sizeof(Outpath));
	strcpy(Hostname, argv[1]); strcpy(Outpath, argv[3]);

	/* 主函数 */
	if (_cpuinfo(Hostname, Outpath) == false)
	{
		logfile.Write("CPU information collection failed.\n"); return -1;
	}

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
	printf("\n此程序运行在需要监控的服务器上(只适用于Linux系统),读取/proc/stat的信息, 把CPU使用率信息写入xml文件.\n");
	printf("\nUsing:/ProDir/public/bin/cpuinfo hostname logfilename outputpath\n");
	printf("\nSample:/ProDir/public/bin/cpuinfo 39.108.127.31 /log/systempara/cpuinfo.log /data/systempara/cpu\n\n");
	printf("hostname:   本服务器的主机名, 方便识别, 可以用IP.\n");
	printf("logfilename:日志文件名.\n");
	printf("outputpath: 存放磁盘使用率的xml文件所在的目录.\n");
	printf("\n此程序运行在需要监控的服务器上（本程序只适用Linux系统），采集后的xml文件由文件传输程序发送给数据处理服务程序入库。\n\n\n");
}

/* 主函数 */
bool _cpuinfo(const char *hostname, const char *outputpath)
{
	struct st_cpuinfo stcpuinfo1, stcpuinfo2, stcpuinfo3;

	memset(&stcpuinfo1, 0, sizeof(struct st_cpuinfo));
	memset(&stcpuinfo2, 0, sizeof(struct st_cpuinfo));
	memset(&stcpuinfo3, 0, sizeof(struct st_cpuinfo));

	if (LoadCPUInfo(stcpuinfo1) == false) return false;

	sleep(60);

	if (LoadCPUInfo(stcpuinfo2) == false) return false;

	stcpuinfo3.user = stcpuinfo2.user - stcpuinfo1.user;
	stcpuinfo3.system  = stcpuinfo2.system  - stcpuinfo1.system;
	stcpuinfo3.iowait = stcpuinfo2.iowait - stcpuinfo1.iowait;
	stcpuinfo3.nice = stcpuinfo2.nice - stcpuinfo1.nice;
	stcpuinfo3.idle = stcpuinfo2.idle - stcpuinfo1.idle;
	stcpuinfo3.irq  = stcpuinfo2.irq  - stcpuinfo1.irq;
	stcpuinfo3.softirq = stcpuinfo2.softirq - stcpuinfo1.softirq;

	stcpuinfo3.total = stcpuinfo3.user + stcpuinfo3.system + stcpuinfo3.iowait + stcpuinfo3.nice + stcpuinfo3.idle + stcpuinfo3.irq + stcpuinfo3.softirq;

	// 得到当前时间的字符串
	char strlocaltime[21];
	memset(strlocaltime, 0, sizeof(strlocaltime));
	LocalTime(strlocaltime, "yyyymmddhh24miss");

	// 设计xml文件名
	char xmlfilename[301];
	memset(xmlfilename, 0, sizeof(xmlfilename));
	sprintf(xmlfilename, "%s/cpuinfo_%s_%s.xml", outputpath, strlocaltime, hostname);

	CFile xmlFile;
	// 以命名.tmp的方式打开xml文件
	if (xmlFile.OpenForRename(xmlfilename, "w+") == false)
	{
		logfile.Write("xmlFile.OpenForRename(%s) failed.\n", xmlfilename);
	}

	// 写入文件数据
	xmlFile.Fprintf("</data>\n");

	xmlFile.Fprintf("<nodip>%s</nodip>"\
					"<crttime>%s</crttime>"\
					"<user>%0.02f</user>"\
					"<nice>%0.02f</nice>"\
					"<system>%0.02f</system>"\
					"<idle>%0.02f</idle>"\
					"<iowait>%0.02f</iowait>"\
					"<usep>%0.02f</usep>"\
					"<endl/>\n",\
					hostname,\
					strlocaltime,\
					stcpuinfo3.user/stcpuinfo3.total*100.0,\
					stcpuinfo3.nice/stcpuinfo3.total*100.0,\
					stcpuinfo3.system/stcpuinfo3.total*100.0,\
					stcpuinfo3.idle/stcpuinfo3.total*100.0,\
					stcpuinfo3.iowait/stcpuinfo3.total*100.0,\
					100.0-stcpuinfo3.nice/stcpuinfo3.total*100.0);

	xmlFile.Fprintf("</data>\n");

	xmlFile.CloseAndRename(); // 关闭文件并重命名(去掉.tmp)

	logfile.Write("create %s is success.\n", xmlfilename);

	return true;
}

/* 加载CPU的参数信息 */
bool LoadCPUInfo(struct st_cpuinfo &stcpuinfo)
{
	CFile File;
	CCmdStr CmdStr;

	if (File.Open("/proc/stat", "r") == false)
	{
		logfile.Write("File.Open(\"/proc/stat\", \"r\") failed.\n"); return false;
	}

	char strbuffer[1024];
	while (1)
	{
		memset(&stcpuinfo, 0, sizeof(struct st_cpuinfo));
		memset(strbuffer, 0, sizeof(strbuffer));
		// 读文件同时删除文件末尾的换行符
		if (File.Fgets(strbuffer, sizeof(strbuffer), true) == false) break;

		DeleteLRChar(strbuffer, ' ');  // 删除两边的空格

		UpdateStr(strbuffer, "  ", " "); // 将两个空格替换为一个空格

		ToLower(strbuffer);   // 转化为小写

		CmdStr.SplitToCmd(strbuffer, " ");

		if (strcmp(CmdStr.m_vCmdStr[0].c_str(), "cpu") == 0)
		{
			stcpuinfo.user = atof(CmdStr.m_vCmdStr[1].c_str());    // 用户态时间
			stcpuinfo.nice = atof(CmdStr.m_vCmdStr[2].c_str());    // 用户态时间(低优先级，nice>0)
			stcpuinfo.system = atof(CmdStr.m_vCmdStr[3].c_str());  // 内核态时间
			stcpuinfo.idle = atof(CmdStr.m_vCmdStr[4].c_str());    // 空闲时间
			stcpuinfo.iowait = atof(CmdStr.m_vCmdStr[5].c_str());  // I/O等待时间
			stcpuinfo.irq = atof(CmdStr.m_vCmdStr[6].c_str());     // 硬中断
			stcpuinfo.softirq = atof(CmdStr.m_vCmdStr[7].c_str()); // 软中断
			return true;
		}
	}

	logfile.Write("read /proc/stat failed.\n");

	return false;
}
