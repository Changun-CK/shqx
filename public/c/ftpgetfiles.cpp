/*
 * 本程序用于FTP采集文件, 参数可配置
 * 作者: 陈锟, 时间: 2019-12-09
 */
#include "_public.h"
#include "_ftp.h"

struct st_arg
{
	char host[51];           // 远程服务器的IP和端口 
	int  mode;               // 传输模式, 1和2, 可选字段, 缺省采用1模式, 即被动模式
	char username[31];       // 远程服务器FTP的用户名
	char password[41];       // 远程服务器FTP的密码
	char localpath[301];     // 本地文件存放的目录
	char remotepath[301];    // 远程服务器存放文件的目录
	char matchname[301];     // 待采集文件匹配的文件名, 采用大写匹配, 不匹配的文件不会被采集, 本字段尽可能设置精确, 不允许用*匹配全部的文件
	int  ptype;              // 文件的处理方式: 1-什么也不做; 2-删除; 3-备份, 如果为3, 还要指定备份的目录
	char remotepathbak[301]; // 远程服务器存放文件的备份目录, 此参数只有当ptype = 3时才有效
	char listfilename[301];  // 采集前列出服务器文件名的文件
	char okfilename[301];    // 已采集成功的文件名XML清单
	int  timetvl;            // 采集文件的间隔时间
} starg;

// 实例化ftp类
Cftp ftp;

// 实例化日志文件类
CLogFile logfile;

vector<struct st_fileinfo> vlistfile, vlistfile1;
vector<struct st_fileinfo> vokfilename, vokfilename1;

// 把nlist方法获取到的list文件名加载到vlistfile容器中
bool LoadListFile();

// 把okfilename文件内容加载到vokfilename容器中
bool LoadOKFileName();

/* 把vlistfile容器中的文件与容器vokfilename中文件对比, 得到两个容器
 * 1. 在vlistfile中存在, 并已经采集成功的文件vokfilename1
 * 2. 在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1 */
bool CompVector();

/* 把vokfilename1容器中的内容先写入okfilename文件中, 覆盖之前旧的okfilename文件 */
bool WriteToOKFileName();

/* 如果ptype == 1, 把采集成功的文件记录追加到okfilename文件中 */
bool AppendToOKFileName(struct st_fileinfo *stfileinfo);

// 本程序的业务流程主函数
bool _ftpgetfiles();

// 程序退出
void EXIT(int sig);

// 显示程序帮助
void Help(char *argv[]);

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer);

int main(int argc, char *argv[])
{
	if (argc!=3) { Help(argv); return -1; }

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[1]); return -1;
	}

	// 将main函数参数分解保存到starg结构体中
	if (xmltoarg(argv[2]) == false) return -1;

	while (1)
	{
		if (ftp.login(starg.host, starg.username, starg.password, starg.mode) == false)
		{
			logfile.Write("ftp.login(%s, %s, %s)failed.\n", starg.host, starg.username, starg.password); sleep(10); continue;
		}

		// 本程序的业务流程主函数
		_ftpgetfiles();

		ftp.logout();

		sleep(starg.timetvl);
	}

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n", sig);
	exit(0);
}

// 本程序的业务流程主函数
bool _ftpgetfiles()
{
	// 进入服务器文件存放的目录 
	if (ftp.chdir(starg.remotepath) == false)
	{
		logfile.Write("ftp.chdir(%s) failed.\n", starg.remotepath); return false;
	}

	// 列出服务器目录文件, 注意若starg.listfilename目录不存在, 这里的nlist可自动创建该目录
	if (ftp.nlist(".", starg.listfilename) == false)
	{
		logfile.Write("ftp.nlist(%s) failed.\n", starg.remotepath); return false;
	}

	// 把nlist方法获取到的list文件加载到vlistfile容器中
	if (LoadListFile() == false)
	{
		logfile.Write("LoadListFile failed.\n"); return false;
	}

	if (starg.ptype == 1)
	{
		/* 加载okfilename文件中的内容到容器vokfilename中
		 * 把vlistfile容器中的文件与容器vokfilename中文件对比, 得到两个容器
		 * 1. 在vlistfile中存在, 并已经采集成功的文件vokfilename1
		 * 2. 在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1
		 *
		 * 把vokfilename1容器中的内容先写入okfilename文件中, 覆盖之前的旧okfilename文件
		 *
		 * 把vlistfile1容器中的内容复制到vlistfile容器中 */ 

		/* 把okfilename文件内容加载到vokfilename容器中 */
		LoadOKFileName();

		/* 把vlistfile容器中的文件与容器vokfilename中文件对比, 得到两个容器 */
		CompVector();

		/* 把vokfilename1容器中的内容先写入okfilename文件中, 覆盖之前旧的okfilename文件 */
		WriteToOKFileName();

		/* 把vlistfile1容器中的内容复制到vlistfile容器中 */
		vlistfile.clear(); vlistfile.swap(vlistfile1);
	}

	for (int ii=0; ii<vlistfile.size(); ii++)
	{
		char strremotefilename[301], strlocalfilename[301];
		memset(strremotefilename, 0, sizeof(strremotefilename));
		memset(strlocalfilename, 0, sizeof(strlocalfilename));

		sprintf(strlocalfilename, "%s/%s", starg.localpath, vlistfile[ii].filename);
		sprintf(strremotefilename, "%s/%s", starg.remotepath, vlistfile[ii].filename);

		logfile.Write("get %s ...", strremotefilename);
		// 获取文件
		if (ftp.get(strremotefilename, strlocalfilename) == false)
		{
			logfile.WriteEx("failed\n"); break;
		}
		logfile.WriteEx("ok\n");

		// 删除文件
		if (starg.ptype == 2) ftp.ftpdelete(strremotefilename);

		// 转存到备份目录
		if (starg.ptype == 3)
		{
			char strremotefilenamebak[301];
			sprintf(strremotefilenamebak, "%s/%s", starg.remotepathbak, vlistfile[ii].filename);
			ftp.ftprename(strremotefilename, strremotefilenamebak);
		}
		/* 如果ptype==1, 把采集成功的文件记录追加到okfilename文件中 */
		if (starg.ptype == 1) AppendToOKFileName(&vlistfile[ii]);
	}
	
	return true;
}

// 把nlist方法获取到的list文件加载到vlistfile容器中
bool LoadListFile()
{
	vlistfile.clear();

	CFile File;

    if (File.Open(starg.listfilename, "r") == false)
    {
        logfile.Write("打开文件(%s)失败.\n", starg.listfilename); return false;
    }

	struct st_fileinfo stfileinfo;

    while (1)
    {
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

		if (File.Fgets(stfileinfo.filename, sizeof(stfileinfo.filename), true) == false) break;

		// 如果不符合starg.matchname的匹配要求, 将会放弃对该文件的采集
		if (MatchFileName(stfileinfo.filename, starg.matchname) == false) continue;

		if (starg.ptype == 1)
		{
			// 获取对方服务器文件的时间
			if (ftp.mtime(stfileinfo.filename) == false)
			{
				logfile.Write("ftp.mtime(%s) failed.\n", stfileinfo.filename); return false;
			}

			strcpy(stfileinfo.mtime, ftp.m_mtime);

			logfile.Write("ptype=1(增量采集),vlistfile filename = %s, mtime = %s\n", stfileinfo.filename, stfileinfo.mtime);
		}
		else if (starg.ptype == 2)
		{
			logfile.Write("ptype=2(删除服务器目录文件),vlistfile filename = %s\n", stfileinfo.filename);
		}
		else 
		{
			logfile.Write("ptype=3(转存至服务器指定目录),vlistfile filename = %s\n", stfileinfo.filename);
		}

        vlistfile.push_back(stfileinfo);
    }

	return true;
}

// 把okfilename文件内容加载到vokfilename容器中
bool LoadOKFileName()
{
	vokfilename.clear();

	CFile File;

	// 注意: 如果程序是第一次采集, okfilename是不存在的, 所以也返回true
	if (File.Open(starg.okfilename, "r") == false) return true;

	struct st_fileinfo stfileinfo;

	char buffer[301];

    while (1)
    {
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

		if (File.Fgets(buffer, sizeof(buffer), true) == false) break;

		GetXMLBuffer(buffer, "filename", stfileinfo.filename, 300);
		GetXMLBuffer(buffer, "mtime", stfileinfo.mtime, 20);

        vokfilename.push_back(stfileinfo);

		logfile.Write("vokfilename filename = %s, mtime = %s\n", stfileinfo.filename, stfileinfo.mtime);
    }

	return true;
}

/* 把vlistfile容器中的文件与容器vokfilename中文件对比, 得到两个容器
 * 1. 在vlistfile中存在, 并已经采集成功的文件vokfilename1
 * 2. 在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1 */
bool CompVector()
{
	vokfilename1.clear(); vlistfile1.clear();

	for (int i=0; i<vlistfile.size(); i++)
	{
		int j = 0;
		for (j=0; j<vokfilename.size(); j++)
		{
			if ( (strcmp(vlistfile[i].filename, vokfilename[j].filename) == 0) && 
				 (strcmp(vlistfile[i].mtime, vokfilename[j].mtime) == 0)  )
			{
				vokfilename1.push_back(vlistfile[i]); break;
			}
		}

		if (j == vokfilename.size())
		{
			vlistfile1.push_back(vlistfile[i]);
		}
	}
/*
 * 以下是调测代码
	for (int i=0; i<vokfilename1.size(); i++)
	{
		logfile.Write("vokfilename1 filename = %s, mtime = %s\n", vokfilename1[i].filename, vokfilename1[i].mtime);
	}

	for (int i=0; i<vlistfile1.size(); i++)
	{
		logfile.Write("vlistfile1 filename = %s, mtime = %s\n", vlistfile1[i].filename, vlistfile1[i].mtime);
	}
*/
	return true;
}

/* 把vokfilename1容器中的内容先写入okfilename文件中, 覆盖之前旧的okfilename文件 */
bool WriteToOKFileName()
{
	CFile File;

	if (File.Open(starg.okfilename, "w") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename); return false;
	}

	for (int i=0; i<vokfilename1.size(); i++)
	{
		File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vokfilename1[i].filename, vokfilename1[i].mtime);
	}

	return true;
}

/* 如果ptype == 1, 把采集成功的文件记录追加到okfilename文件中 */
bool AppendToOKFileName(struct st_fileinfo *stfileinfo)
{
	CFile File;

	if (File.Open(starg.okfilename, "a") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename); return false;
	}

	File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo -> filename, stfileinfo -> mtime);

	return true;
}

// 显示程序帮助
void Help(char *argv[])
{
	printf("\n本程序是数据中心的公共的功能模块，从远程FTP服务器上采集文件，参数可配置, 注意: 若ptype == 1, 实现增量采集时若FTP服务器文件过多会造成性能低下的问题.\n");
	printf("\nUsing: /ProDir/public/bin/ftpgetfiles logfilename xmlbuffer\n");

	printf("\nSample: /ProDir/public/bin/ftpgetfiles /log/shqx/ftpgetfile_surfdata.log"\
" \"<host>39.108.127.31:21</host><mode>1</mode><username>oracle</username><password>uy"\
"s%#(736@^%__^daAA&%#_-@FX73)+#</password><localpath>/data/shqx/sdata/surfdata</localpath>"\
"<remotepath>/data/shqx/ftp/surfdata</remotepath><matchname>SURF_*.TXT</matchname><ptype>1"\
"</ptype><remotepathbak></remotepathbak><listfilename>/data/shqx/ftplist/ftpgetfile_surfdata.list"\
"</listfilename><okfilename>/data/shqx/ftplist/ftpgetfile_surfdata.xml</okfilename><timetvl>30</timetvl>\"\n\n");

	printf("logfilename 程序运行的日志文件名.\n");
	printf("xmlbuffer 程序的传输方式, 如下:\n");
	printf("<host>39.108.127.31:21</host> 远程服务器的IP和端口\n");
	printf("<mode>1</mode> 传输模式, 1和2, 可选字段, 缺省采用1模式\n");
	printf("<username>changun</username> 远程服务器FTP的用户名\n");
	printf("<password>974753ck</password> 远程服务器FTP的密码\n");
	printf("<localpath>/oracle/qxidc/data/surfdata</localpath> 本地文件存放的目录\n");
	printf("<remotepath>/tmp/gzrad</remotepath> 远程服务器存放文件的目录\n");
	printf("<matchname>*.GIF</matchname> 待采集文件匹配的文件名, 采用大写匹配, 不匹配的文件不会被采集, 本字段尽可能设置精确, 不允许用*匹配全部的文件\n");
	printf("<ptype>2</ptype> 文件的处理方式: 1-什么也不做; 2-删除; 4-备份, 如果为3, 还要指定备份的目录\n");
	printf("<remotepathbak>/home/changun/qxidc/data/ftp/surfdatabak</remotepathbak> 远程服务器存放文件的备份目录, 此参数只有当ptype = 3时才有效.\n");
	printf("<listfilename>/oracle/qxidc/list/ftpgetfile_surfdata.list</listfilename> 采集前列出服务器文件名的文件.\n");
	printf("<okfilename>/oracle/qxidc/list/ftpgetfile_surfdata.xml</okfilename> 已采集成功的文件名XML清单, 只有当ptype = 1时有效.\n");
	printf("<timetvl>30</timetvl> 采集时间间隔, 建议单位: 秒, 建议大于10.\n\n");
}

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer)
{
	memset(&starg, 0, sizeof(struct st_arg));

	GetXMLBuffer(strxmlbuffer, "host", starg.host);
	if (strlen(starg.host) == 0) { logfile.Write("host is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "mode", &starg.mode);
	if ( (starg.mode != 1) && (starg.mode != 2) ) starg.mode = 1;

	GetXMLBuffer(strxmlbuffer, "username", starg.username);
	if (strlen(starg.username) == 0) { logfile.Write("username is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "password", starg.password);
	if (strlen(starg.password) == 0) { logfile.Write("password is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "localpath", starg.localpath);
	if (strlen(starg.localpath) == 0) { logfile.Write("localpath is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "remotepath", starg.remotepath);
	if (strlen(starg.remotepath) == 0) { logfile.Write("remotepath is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "matchname", starg.matchname);
	if (strlen(starg.matchname) == 0) { logfile.Write("matchname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "ptype", &starg.ptype);
	if ( (starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3) ) { logfile.Write("ptype is Error.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "remotepathbak", starg.remotepathbak);
	if ( (starg.ptype == 3) && (strlen(starg.remotepathbak) == 0) ) { logfile.Write("starg.remotepathbak is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "listfilename", starg.listfilename);
	if (strlen(starg.listfilename) == 0) { logfile.Write("listfilename is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename);
	if ((strlen(starg.okfilename) == 0) && (starg.ptype == 1)) { logfile.Write("okfilename is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
	if (starg.timetvl == 0) { logfile.Write("timetvl is NULL.\n");  return false; }

	return true;
}
