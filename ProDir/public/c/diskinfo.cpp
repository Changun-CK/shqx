/*
 * 本程序用于收集系统参数, 参数可配置
 * 作者: 陈锟, 时间: 2020-03-29
 */
#include "_freecplus.h"

CLogFile logfile;
/* 程序退出 */
void EXIT(int sig);

/* 显示程序帮助 */
void Help(char *argv[]);

/* 业务主流程函数 */
bool _diskinfo(const char *hostname, const char *outputpath);

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
	char Hostname[20], Outpath[301];
	memset(Hostname, 0, sizeof(Hostname)); memset(Outpath, 0, sizeof(Outpath));
	strcpy(Hostname, argv[1]);
	strcpy(Outpath, argv[3]);

	/* 主函数 */
	if (_diskinfo(Hostname, Outpath) == false)
	{
		logfile.Write("Gathering disk information failed.\n"); return -1;
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
	printf("\n此程序调用df命名，把本服务器的磁盘使用率信息写入xml文件.\n");
	printf("\nUsing:/ProDir/public/bin/diskinfo hostname logfilename outputpath\n");
	printf("\nSample:/ProDir/public/bin/diskinfo 39.108.127.31 /log/syspara/diskinfo.log /data/syspara/disk\n\n");
	printf("hostname:   本服务器的主机名, 方便识别, 可以用IP.\n");
	printf("logfilename:日志文件名.\n");
	printf("outputpath: 存放磁盘使用率的xml文件所在的目录.\n");
	printf("\n此程序运行在需要监控的服务器上（本程序只适用Linux系统），采集后的xml文件由文件传输程序发送给数据处理服务程序入库。\n\n\n");
}

/* 主函数 */
bool _diskinfo(const char *hostname, const char *outputpath)
{
	FILE *fp = 0;

	if ( (fp = popen("df -h --block-size=1M", "r")) == NULL )
	{
		logfile.Write("popen(df -h --block-size=1M) failed.\n"); return false;
	}

	char XMLfilename[301]; memset(XMLfilename, 0, sizeof(XMLfilename)); // XML文件名, 不是全路径
	char strLocalTime[301]; memset(strLocalTime, 0, sizeof(strLocalTime)); // 当前时间
	LocalTime(strLocalTime, "yyyymmddh24miss");
	sprintf(XMLfilename, "%s/diskinfo_%s_%s.xml", outputpath, strLocalTime, hostname);

	CFile File;
	if (File.OpenForRename(XMLfilename, "w+") == false)
	{
		logfile.Write("XMLfile.OpenForRename(%s) failed.\n", XMLfilename); pclose(fp);  return false;
	}

	CCmdStr CmdStr;

	char strbuffer[1024]; memset(strbuffer, 0, sizeof(strbuffer));
	char strline[500]; memset(strline, 0, sizeof(strline));

	File.Fprintf("<data>\n");
	while (1)
	{
		memset(strbuffer, 0, sizeof(strbuffer));

		if (FGETS(fp, strbuffer, 500) == false) break;
		if (strstr(strbuffer, "/") == 0) continue;

		// 如果没有查到"%", 就再读取一行, 与strbuffer拼接起来
		if (strstr(strbuffer, "%") == 0)
		{
			memset(strline, 0, sizeof(strline));
			if (FGETS(fp, strline, 500) == false) break;
			strcat(strbuffer, " "); strcat(strbuffer, strline);
		}

		// 删除字符串前后的空格和换行符
		DeleteLRChar(strbuffer, ' '); DeleteLRChar(strbuffer, '\n'); DeleteLRChar(strbuffer, ' ');

		// 把字符串中间的多个空格全部转换为一个空格
		UpdateStr(strbuffer,"  "," ");

		// 把全部内容转换为小写
		ToLower(strbuffer);

		// 除了磁盘信息, 还可能是内存, SMB等其他文件, 都要丢弃
		if (strncmp(strbuffer, "/dev", 4) != 0) continue;

		CmdStr.SplitToCmd(strbuffer, " ");
		if (CmdStr.CmdCount() != 6) continue;

		/*查出已用的百分比
		char strusep[21]; memset(strusep, 0, sizeof(strusep));
		strcpy(strusep, CmdStr.m_vCmdStr[4].c_str());
		UpdateStr(strusep, "%", "");
		*/

		memset(strLocalTime, 0, sizeof(strLocalTime)); // 再次获取当前时间
		LocalTime(strLocalTime, "yyyymmddh24miss");
		File.Fprintf(\
				"<nodip>%s</nodip>"\
				"<crttime>%s</crttime>"\
				"<filesystem>%s</filesystem>"\
				"<total>%0.02f</total>"\
				"<used>%0.02f</used>"\
				"<available>%0.02f</available>"\
				"<usep>%0.02f</usep>"\
				"<mount>%s</mount><endl/>\n",
				hostname,
				strLocalTime,
				CmdStr.m_vCmdStr[0].c_str(),
				atof(CmdStr.m_vCmdStr[1].c_str())/1024.0,
				atof(CmdStr.m_vCmdStr[2].c_str())/1024.0,
				atof(CmdStr.m_vCmdStr[3].c_str())/1024.0,
				(atof(CmdStr.m_vCmdStr[2].c_str())/atof(CmdStr.m_vCmdStr[1].c_str()))*100.0,
				CmdStr.m_vCmdStr[5].c_str());
	}
	File.Fprintf("</data>\n");

	pclose(fp); // 关闭文件指针
	File.CloseAndRename(); // 重命名以上文件, 避免产生中间状态的文件, 对应以上OpenForRename

	logfile.Write("create %s is success.\n", XMLfilename);
	

	return true;
}
