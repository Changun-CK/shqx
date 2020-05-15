/*
 * 本程序用于FTP向服务器传输文件, 参数可配置
 * 作者: 陈锟, 时间: 2020-03-12
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
	char localpathbak[301];  // 本地服务器存放文件的备份目录, 此参数只有当ptype = 3时才有效
	char okfilename[301];    // 已采集成功的文件名XML清单
	int  timetvl;            // 采集文件的间隔时间
} starg;

Cftp ftp;
CLogFile logfile;

vector<struct st_fileinfo> vlistfile, vlistfile1;
vector<struct st_fileinfo> vokfilename, vokfilename1;

// 把localpath目录下的文件加载到vlistfile容器中
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
bool _ftpputfiles();

// 程序退出
void EXIT(int sig);

// 显示程序帮助
void Help(char *argv[]);

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer);

int main(int argc, char *argv[])
{
	if (argc != 3) { Help(argv); return -1; }

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
		_ftpputfiles();

		ftp.logout();

		sleep(starg.timetvl);
	}

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);
	exit(0);
}

// 本程序的业务流程主函数
bool _ftpputfiles()
{
	// 创建远程服务器上的目录, 不关心返回值
	ftp.mkdir(starg.remotepath);

	// 把localpath目录下的文件记载到vlistfile容器中
	if (LoadListFile() == false)
	{
		logfile.Write("LoadListFile() failed.\n"); return false;
	}


	if (starg.ptype == 1)
	{
		/* 加载okfilename文件中的内容到容器vokfilename中 */
		LoadOKFileName();

		/* 把vlistfile容器中的文件与容器vokfilename中文件对比, 得到两个容器
		 * 1. 在vlistfile中存在, 并已经采集成功的文件vokfilename1
		 * 2. 在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1 */
		CompVector();
		 
		/* 把vokfilename1容器中的内容先写入okfilename文件中, 覆盖之前的旧okfilename文件 */
		WriteToOKFileName();
		 
		/* 把vlistfile1容器中的内容复制到vlistfile容器中 */ 
		vlistfile.clear(); vlistfile.swap(vlistfile1);
	}

	/* 把客户端的新文件或已改动过的文件发送给服务端 */
	for (int ii=0; ii<vlistfile.size(); ii++)
	{
		char strremotefilename[301], strlocalfilename[301];
		memset(strremotefilename, 0, sizeof(strremotefilename));
		memset(strlocalfilename, 0, sizeof(strlocalfilename));

		sprintf(strlocalfilename, "%s/%s", starg.localpath, vlistfile[ii].filename);
		sprintf(strremotefilename, "%s/%s", starg.remotepath, vlistfile[ii].filename);

		logfile.Write("put %s ...", strlocalfilename);

		// 发送文件
		if (ftp.put(strlocalfilename, strremotefilename, true) == false)
		{
			logfile.WriteEx("failed.\n"); break;
		}
		logfile.WriteEx("ok.\n");

		// 删除文件
		if (starg.ptype == 2) remove(strlocalfilename);

		// 转存到备份目录
		if (starg.ptype == 3)
		{
			char strfilenamebak[301];
			memset(strfilenamebak, 0, sizeof(strfilenamebak));
			sprintf(strfilenamebak, "%s/%s", starg.localpathbak, vlistfile[ii].filename);
			if (RENAME(strlocalfilename, strfilenamebak) == false)
			{
				logfile.Write("RENAME %s to %s failed.\n", strlocalfilename, strfilenamebak); return false;
			}
		}

		/* 如果ptype==1, 把采集成功的文件记录追加到okfilename文件中 */
		if (starg.ptype == 1) AppendToOKFileName(&vlistfile[ii]);
	}
	
	return true;
}

// 把localpath目录下的文件加载到vlistfile容器中
bool LoadListFile()
{
	vlistfile.clear();

	CDir Dir;

	// 不包括子目录
	// 这里不会将子目录的文件传输. 注意, 如果目录下的总文件数超过50000, 增量发送文件功能将会有问题
	if (Dir.OpenDir(starg.localpath,starg.matchname,50000,false,false) == false)
	{
		logfile.Write("Dir.OpenDir(%s) 失败。\n",starg.localpath); return false;
	}

	struct st_fileinfo stfileinfo;

    while (1)
    {
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

		if (Dir.ReadDir() == false) break;

		strcpy(stfileinfo.filename,Dir.m_FileName);  // 文件名，不包括目录名
		strcpy(stfileinfo.mtime,Dir.m_ModifyTime);
		stfileinfo.filesize=Dir.m_FileSize;

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

	// 注意, 打开文件不要采用缓冲机制
	if (File.Open(starg.okfilename, "w", false) == false)
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

	// 注意, 打开文件不要采用缓冲机制
	if (File.Open(starg.okfilename, "a", false) == false)
	{
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename); return false;
	}

	File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", stfileinfo -> filename, stfileinfo -> mtime);

	return true;
}

// 显示程序帮助
void Help(char *argv[])
{
	printf("\n本程序是数据中心的公共的功能模块，用于把本地目录中的文件发送到远程的FTP服务器.\n");
	printf("\nUsing: /ProDir/public/bin/ftpputfiles logfilename xmlbuffer\n");

	printf("\nSample: /ProDir/public/bin/ftpputfiles /log/shqx/ftpputfile_surfdata.log"\
" \"<host>39.108.127.31:21</host><mode>1</mode><username>oracle</username><password>uy"\
"s%#(736@^%__^daAA&%#_-@FX73)+#</password><localpath>/data/shqx/ftp/surfdata</localpath>"\
"<remotepath>/data/shqx/sdata/surfdata</remotepath><matchname>SURF_*.TXT, *.DAT</matchname><ptype>1"\
"</ptype><localpathbak></localpathbak>"\
"<okfilename>/data/shqx/ftplist/ftpputfile_surfdata.xml</okfilename><timetvl>30</timetvl>\"\n\n");

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
	printf("<localpathbak>/home/changun/qxidc/data/ftp/surfdatabak</localpathbak> 远程服务器存放文件的备份目录, 此参数只有当ptype = 3时才有效.\n");
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

	GetXMLBuffer(strxmlbuffer, "localpathbak", starg.localpathbak);
	if ( (starg.ptype == 3) && (strlen(starg.localpathbak) == 0) ) { logfile.Write("starg.localpathbak is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "okfilename", starg.okfilename);
	if ((strlen(starg.okfilename) == 0) && (starg.ptype == 1)) { logfile.Write("okfilename is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "timetvl", &starg.timetvl);
	if (starg.timetvl == 0) { logfile.Write("timetvl is NULL.\n");  return false; }

	return true;
}
