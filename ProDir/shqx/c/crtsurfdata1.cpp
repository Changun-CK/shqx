/*
 * 本程序用于生成测试数据
 * 作者: 陈锟  日期: 2019-12-01
*/
#include "_public.h"
#include "_shqx.h"

// 存放全国气象站点参数的容器
vector<struct st_code> vstcode;

// 存放国气象站点分钟观测数据的容器
vector<struct st_surfdata> vsurfdata;

// 从站点参数文件中加载到vstcode容器中
bool LoadSTCode(const char *inifile);

// 创建全国气象站点分钟观测数据, 存放在vsurfdata容器中
void CrtSurfData();

// 把容器vsurfdata中的国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath, const char *dataformat);

// 实例化日志文件类
CLogFile logfile;

// 程序退出
void EXIT(int sig);

int main(int argc, char *argv[])
{
	if (argc!=5)
	{
		printf("\n本程序用于生成全国气象站点观测的分钟数据, XML格式或TXT格式.\n");
		printf("\n用法: /ProDir/shqx/bin/crtsurfdata1 站点参数 数据文件存放的目录 日志文件名 指定数据格式的后缀.\n");
		printf("\n例如: /ProDir/shqx/bin/crtsurfdata1 /ProDir/shqx/ini/stcode.ini /data/shqx/ftp/surfdata /log/shqx/crtsurfdata.log xml\n\n");
		return -1;
	}

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[3], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[3]); return -1;
	}

	while (1)
	{
		// 从站点参数文件中加载到vstcode容器中
		if (LoadSTCode(argv[1]) == false) { sleep(60); continue; }

		logfile.Write("加载参数文件(%s)成功.\n", argv[1]);

		// 创建全国气象站点分钟观测数据, 存放在vsurfdata容器中
		CrtSurfData();

		// 把容器vsurfdata中的全国气象站点分钟观测数据写入文件
		if (CrtSurfFile(argv[2], argv[4]) == false) { sleep(60); continue; }

		sleep (60);
	}

	return 0;
}

// 从站点参数文件中加载到vstcode容器中
bool LoadSTCode(const char *inifile)
{
	vstcode.clear();
	CFile File;
	CCmdStr CmdStr;
	char buffer[101]; 
	struct st_code stcode;

	if (File.Open(inifile, "r") == false)
	{
		logfile.Write("打开文件(%s)失败.\n", inifile); return false;
	}

	while (1)
	{
		memset(&stcode, 0, sizeof(stcode));
		if (File.Fgets(buffer, sizeof(buffer)) == false) break;

		CmdStr.SplitToCmd(buffer, ",", true);

		CmdStr.GetValue(0,  stcode.provname);
		CmdStr.GetValue(1,  stcode.obtid);
		CmdStr.GetValue(2,  stcode.cityname);
		CmdStr.GetValue(3, &stcode.lat);
		CmdStr.GetValue(4, &stcode.lon);
		CmdStr.GetValue(5, &stcode.height);

		vstcode.push_back(stcode);
	}
	
	return true;
}

// 创建全国气象站点分钟观测数据, 存放在vsurfdata容器中
void CrtSurfData()
{
	vsurfdata.clear();

	srand(time(0));

	char strLocalTime[21];
	LocalTime(strLocalTime, "yyyy-mm-dd hh:mi");
	strcat(strLocalTime, ":00");

	struct st_surfdata stsurfdata;
	for (int ii=0; ii<vstcode.size(); ii++)
	{
		memset(&stsurfdata, 0, sizeof(stsurfdata));
		strcpy(stsurfdata.obtid, vstcode[ii].obtid); // 站点代码
		strcpy(stsurfdata.ddatetime, strLocalTime);  // 当前时间 

		stsurfdata.t   = rand()%351;         // 气温: 单位, 0.1摄氏度
		stsurfdata.p   = rand()%265+10000;   // 气压: 单位: 0.1百帕
		stsurfdata.u   = rand()%100+1;       // 相对湿度, 0-100之间的值
		stsurfdata.wd  = rand()%360;         // 风向, 0-360之间的值
		stsurfdata.wf  = rand()%150;         // 风速: 单位: 0.1m/s
		stsurfdata.r   = rand()%16;          // 降雨量: 0.1mm
		stsurfdata.vis = rand()%5001+100000; // 能见度: 0.1m

		vsurfdata.push_back(stsurfdata);
	}
}

// 把容器vsurfdata中的国气象站点分钟观测数据写入文件
bool CrtSurfFile(const char *outpath, const char *dataformat)
{
	CFile File;

	char strLocalTime[21];
	LocalTime(strLocalTime, "yyyymmddhhmiss");

	char strFileName[301];
	memset(strFileName, 0, sizeof(strFileName));
	snprintf(strFileName, sizeof(strFileName), "%s/SURF_ZH_%s_%d.%s", outpath, strLocalTime, getpid(), dataformat);

	if (File.OpenForRename(strFileName, "w") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", strFileName); return false;
	}

	if (strcmp(dataformat, "xml") == 0)
	{
		File.Fprintf("<data>\n");
		for (int ii=0; ii<vsurfdata.size(); ii++)
		{
			File.Fprintf("<obtid>%s</obtid><ddatetime>%s</ddatetime><t>%.1f</t><p>%.1f</p><u>%d</u><wd>%d</wd><wf>%.1f</wf><r>%.1f</r><vis>%.1f</vis><endl/>\n",\
			vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t/10.0,\
			vsurfdata[ii].p/10.0, vsurfdata[ii].u, vsurfdata[ii].wd,\
			vsurfdata[ii].wf/10.0, vsurfdata[ii].r/10.0, vsurfdata[ii].vis/10.0);
		}
		File.Fprintf("</data>\n");
	}
	else
	{
		for (int ii=0; ii<vsurfdata.size(); ii++)
		{
			File.Fprintf("%s, %s, %.1f, %.1f, %d, %d, %.1f, %.1f, %.1f\n",\
			vsurfdata[ii].obtid, vsurfdata[ii].ddatetime, vsurfdata[ii].t/10.0,\
			vsurfdata[ii].p/10.0, vsurfdata[ii].u, vsurfdata[ii].wd,\
			vsurfdata[ii].wf/10.0, vsurfdata[ii].r/10.0, vsurfdata[ii].vis/10.0);
		}
	}

	File.CloseAndRename();  // 关闭文件

	logfile.Write("生成数据文件(%s)成功, 数据时间: %s, 记录数: %d.\n", strFileName, vsurfdata[0].ddatetime, vsurfdata.size());

	vstcode.clear(); vsurfdata.clear();

	return true;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n", sig);
	exit(0);
}
