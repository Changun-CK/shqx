#include "_freecplus.h"

struct st_arg
{
	char ip[31];             // 服务端的IP地址
	int  port;               // 服务端的端口
	int  ptype;              // 文件发送成功后文件的处理方式:1-保留文件; 2-删除文件; 3-移动到备份>目录
	char clientpath[301];    // 客户端文件存放的目录
	char clientpathbak[301]; // 文件成功发送后, 本地文件备份的根目录, 当ptype==3时有效
	char srvpath[301];       // 服务端文件存放的目录
	bool andchild;           // 是否发送clientpath目录下各级子目录的文件, true-是; false-否; 缺>省为false
	char matchname[301];     // 待发送文件名的匹配方式
	char okfilename[301];    // 已发送成功文件名清单
	int  timetvl;            // 扫描本地目录文件的时间间隔, 单位: 秒
}starg;

CLogFile logfile;
CTcpClient TcpClient;

char strRecvBuffer[10241];    // 接收报文的缓冲区
char strSendBuffer[10241];    // 发送报文的缓冲区

vector<struct st_fileinfo> vlistfile, vlistfile1;
vector<struct st_fileinfo> vokfilename, vokfilename1;

// 把clientpath目录下的文件加载到vlistfile容器中
bool LoadListFile();

// 把okfilename文件内容加载到vokfilename容器中
bool LoadOKFileName();

// 把vlistfile中的文件与vokfilename容器中文件对比, 得到两个容器
// 1-在vlistfile中存在, 并已经采集成功的文件vokfilename1
// 2-在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1
bool CompVector();

// 把vokfilename容器中的内容先写入okfilename文件中, 覆盖之前旧的okfilename文件
bool WriteToOKFileName();

// 如果ptype=1, 把采集到的文件记录追加到okfilename文件中
bool AppendToOKFileName(struct st_fileinfo *stfileinfo);

// 程序的帮助文档
void _help(char *argv[]);

// 解析参数到starg
bool _xmltoarg(const char *xml);

void EXIT(int sig); 

// 业务流程的主函数
bool _tcpputfiles();

// 向服务器发起连接并登录
bool ClientLogin(const char *argv);

// 向服务器发送心跳报文
bool ActiveTest();

int main(int argc, char *argv[])
{
	if (argc != 3) { _help(argv); return -1; }

	// 关闭全部的信号和输入输出
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	// 打开日志文件
	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[1]); return -1;
	}

	if (_xmltoarg(argv[2]) == false) return -1;

	while (1)
	{
		// 向服务器发起连接并登录
		ClientLogin(argv[2]);

		// 实现文件发送的功能
		_tcpputfiles();

		if (vlistfile.size() == 0)
		{
			// 向服务器发送心跳报文
			ActiveTest();

			sleep(starg.timetvl);
		}
	}

	return 0;
}

void _help(char *argv[])
{
	printf("\n这是一个通用的功能模块, 采用TCP协议发送文件的客户端.\n");
	printf("\nExample:/ProDir/public/bin/tcpputfiles logfilename xmlbuffer\n");
	printf("\nUsing:/ProDir/public/bin/tcpputfiles /log/shqx/tcpputfiles_surfdata.log "\
	"\"<ip>39.108.127.31</ip><port>5050</port><ptype>1</ptype><clientpath>/data/shqx/ftp/surfdata</clientpath>"\
	"<clientpathbak>/data/shqx/sdata/surfdatabak</clientpathbak><srvpath>/data/shqx/tcp/surfdata</srvpath>"\
	"<andchild>true</andchild><matchname>SURF_*.TXT,*.DAT</matchname><okfilename>/data/shqx/tcplist/tcpputfiles_surfdata.xml</okfilename>"\
	"<timetvl>10</timetvl>\"\n\n");
	printf("logfilename   本程序的日志文件名.\n");
	printf("xmlbuffer     本程序的运行参数.\n");
	printf("ip            服务器的IP地址.\n");
	printf("port          服务器的端口.\n");
	printf("ptype         文件发送成功后的处理方式:1-保留文件; 2-删除文件; 3-移动到备份目录.\n");
	printf("clientpath    客户端文件存在的目录.\n");
	printf("clientpathbak 客户端备份文件存在的目录, 当ptype=3时有效.\n");
	printf("srvpath       服务端文件存在的目录.\n");
	printf("andchild      是否发送clientpath目录下的子目录的文件, true-是; false-否; 缺省为false.\n");
	printf("matchname     待发送文件名的匹配方式, 如\"*.TXT,*.XML\".\n");
	printf("okfilename    已发送成功文件名清单, 缺省为空.\n");
	printf("timetvl       扫描本地目录的时间间隔, 单位: 秒, 取值在1-50之间.\n\n");
}

// 解析参数到starg
bool _xmltoarg(const char *xml)
{
	GetXMLBuffer(xml, "ip", starg.ip);
	if (strlen(starg.ip) == 0) { logfile.Write("ip is NULL.\n"); return false; }

	GetXMLBuffer(xml, "port", &starg.port);
	if (starg.port == 0) { logfile.Write("port is NULL.\n"); return false; }

	GetXMLBuffer(xml, "ptype", &starg.ptype);
	if ( (starg.ptype != 1) && (starg.ptype != 2) && (starg.ptype != 3) )
	{ logfile.Write("ptype is not in(1, 2, 3).\n"); return false; }

	GetXMLBuffer(xml, "clientpath", starg.clientpath);
	if (strlen(starg.clientpath) == 0) { logfile.Write("clientpath is NULL.\n"); return false; }

	GetXMLBuffer(xml, "clientpathbak", starg.clientpathbak);
	if ( (strlen(starg.clientpathbak) == 0) && (starg.ptype == 3) )
	{ logfile.Write("clientpathbak is NULL.\n"); return false; }

	GetXMLBuffer(xml, "srvpath", starg.srvpath);
	if (strlen(starg.srvpath) == 0) { logfile.Write("srvpath is NULL.\n"); return false; }

	GetXMLBuffer(xml, "andchild", &starg.andchild);

	GetXMLBuffer(xml, "matchname", starg.matchname);
	if (strlen(starg.matchname) == 0) { logfile.Write("matchname is NULL.\n"); return false; }

	GetXMLBuffer(xml, "okfilename", starg.okfilename);
	if ( (strlen(starg.okfilename) == 0) && (starg.ptype == 1) )
	{ logfile.Write("okfilename is NULL.\n"); return false; }

	GetXMLBuffer(xml, "timetvl", &starg.timetvl);
	if (starg.timetvl == 0) { logfile.Write("timetvl is NULL.\n"); return false; }

	return true;
}

void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);

	TcpClient.Close();

	exit(0);
}

// 业务流程的主函数, 实现发送文件功能
bool _tcpputfiles()
{
	// 把clientpath目录下的文件加载到vlistfile容器中
	if (LoadListFile() == false)
	{
		logfile.Write("LoadListFile() failed.\n"); return false;
	}

	if (starg.ptype == 1)
	{
		// 加载okfilename文件中的内容到容器vokfilename中
		LoadOKFileName();

		// 把vlistfile容器中的文件与vokfilename容器中文件对比, 得到两个容器
		// 1-在vlistfile中存在, 并已经采集成功的文件vokfilename1
		// 2-在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1
		CompVector();

		// 把vokfilename1容器中的内容先写入okfilename文件中, 覆盖旧的okfilename文件
		WriteToOKFileName();

		// 把vilstfile1容器中的内容复制到vlistfile容器中
		vlistfile.clear(); vlistfile.swap(vlistfile1);
	}

	// 把客户端的新文件或已改动过后的文件发送给服务端
	for (int ii = 0; ii < vlistfile.size(); ii++)
	{
		logfile.Write("put %s ...", vlistfile[ii].filename);

		// 把文件发送给服务端
		if (SendFile(TcpClient.m_sockfd, &vlistfile[ii], &logfile) == false)
		{
			logfile.Write("SendFile() failed.\n"); TcpClient.Close(); return false;
		}

		logfile.WriteEx("ok.\n");

		// 删除文件
		if (starg.ptype == 2) REMOVE(vlistfile[ii].filename);

		// 转存到备份目录
		if (starg.ptype == 3)
		{
			char strfilenamebak[301];
			memset(strfilenamebak, 0, sizeof(strfilenamebak));
			strcpy(strfilenamebak, vlistfile[ii].filename);
			UpdateStr(strfilenamebak, starg.clientpath, starg.clientpathbak, false); // 注意, 此时第三个参数只能为false, 否则会出现段错误
			if (RENAME(vlistfile[ii].filename, strfilenamebak) == false)
			{
				logfile.Write("RENAME %s to %s failed.\n", vlistfile[ii].filename, strfilenamebak); return false;
			}
		}

		// 如果ptype==1, 增量采集, 把采集到的文件记录追加到okfilename文件中
		if (starg.ptype == 1) AppendToOKFileName(&vlistfile[ii]);
	}

	return true;
}

// 向服务器发起连接并登录
bool ClientLogin(const char *argv)
{
	if (TcpClient.m_sockfd > 0) return true;

	int ii = 0;

	while (1)
	{
		if (ii++ > 0) sleep(20); // 第一次进入循环不休眠

		// 向服务器发起连接
		if (TcpClient.ConnectToServer(starg.ip, starg.port) == false)
		{
			logfile.Write("TcpClient.ConnectToServer(%s, %d) failed.\n", starg.ip, starg.port); continue;
		}

		memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
		memset(strSendBuffer, 0, sizeof(strSendBuffer));

		strcpy(strSendBuffer, argv); strcat(strSendBuffer, "<clienttype>1</clienttype>");

		// logfile.Write("strSendBuffer = %s\n", strSendBuffer); // xxxx
		if (TcpClient.Write(strSendBuffer) == false)
		{
			logfile.Write("1 TcpClient.Write() failed.\n"); continue;
		}

		if (TcpClient.Read(strRecvBuffer) == false)
		{
			logfile.Write("1 TcpClient.Write() failed.\n"); continue;
		}
		// logfile.Write("strRecvBuffer = %s\n", strRecvBuffer); // xxxx

		break;
	}

	logfile.Write("login(%s, %d) ok.\n", starg.ip, starg.port);

	return true;
}

// 向服务器发送心跳报文
bool ActiveTest()
{
	memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
	memset(strSendBuffer, 0, sizeof(strSendBuffer));

	strcpy(strSendBuffer, "<activetest>ok</activetest>");

	// logfile.Write("strSendBuffer = %s\n", strSendBuffer); // xxxx
	if (TcpClient.Write(strSendBuffer) == false)
	{
		logfile.Write("2 TcpClient.Write() failed.\n"); TcpClient.Close(); return false;
	}

	if (TcpClient.Read(strRecvBuffer) == false)
	{
		logfile.Write("2 TcpClient.Read() failed.\n"); TcpClient.Close(); return false;
	}
	// logfile.Write("strRecvBuffer = %s\n", strRecvBuffer); // xxxx

	if (strcmp(strRecvBuffer, "ok") != 0) { TcpClient.Close(); return false; }

	return true;
}

// 把clientpath目录下的文件加载到vlistfile容器中
bool LoadListFile()
{
	vlistfile.clear();
	
	CDir Dir; // 目录操作类

	// 注意, 如果目录下的总文件数超过50000, 增量采集发送文件功能将有问题
	if (Dir.OpenDir(starg.clientpath, starg.matchname, 50000, starg.andchild, false) == false)
	{
		logfile.Write("Dir.OpenDir(%s) failed.\n", starg.clientpath); return false;
	}

	struct st_fileinfo stfileinfo;

	while (1)
	{
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

		if (Dir.ReadDir() == false) break;

		strcpy(stfileinfo.filename, Dir.m_FullFileName);
		strcpy(stfileinfo.mtime, Dir.m_ModifyTime);
		stfileinfo.filesize = Dir.m_FileSize;

		vlistfile.push_back(stfileinfo);
	}

	return true;
}

// 把okfilename文件内容加载到vokfilename容器中
bool LoadOKFileName()
{
	vokfilename.clear();

	CFile File;

	// 注意, 如果程序是第一次采集, okfilename是不存在的, 并不是错误, 所以也返回true
	if (File.Open(starg.okfilename, "r") == false) return true;

	struct st_fileinfo stfileinfo;

	char strbuffer[301];
	while (1)
	{
		memset(&stfileinfo, 0, sizeof(stfileinfo));

		if (File.Fgets(strbuffer, sizeof(strbuffer), true) == false) break;

		GetXMLBuffer(strbuffer, "filename", stfileinfo.filename);
		GetXMLBuffer(strbuffer, "mtime", stfileinfo.mtime);

		vokfilename.push_back(stfileinfo);
	}

	return true;
}

// 把vlistfile中的文件与vokfilename容器中文件对比, 得到两个容器
// 1-在vlistfile中存在, 并已经采集成功的文件vokfilename1
// 2-在vlistfile中存在, 新文件或需要重新采集的文件vlistfile1
bool CompVector()
{
	vokfilename1.clear(); vlistfile1.clear();

	int jj;
	for (int ii = 0; ii < vlistfile.size(); ii++)
	{
		jj = 0;
		for (jj = 0; jj < vokfilename.size(); jj++)
		{// 比较vlistfile和vokfilename, 若相等, 则加入新容器vokfilename1
			if ( (strcmp(vlistfile[ii].filename, vokfilename[jj].filename) == 0) &&
				 (strcmp(vlistfile[ii].mtime, vokfilename[jj].mtime) == 0) )
			{
				vokfilename1.push_back(vlistfile[ii]); break; //得到真正未采集的文件
			}
		}

		// 若比较vlistfile和vokfilename全都不相等, 则都需要采集, 加入vlistfile1
		if (jj == vokfilename.size())
		{
			vlistfile1.push_back(vlistfile[ii]); //得到真正需要采集的文件信息
		}
	}

	return true;
}

// 把vokfilename容器中的内容先写入okfilename文件中, 覆盖之前旧的okfilename文件
bool WriteToOKFileName()
{
	CFile File;

	if (File.Open(starg.okfilename, "w") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename); return false;
	}

	for (int ii = 0; ii < vokfilename1.size(); ii++)
	{
		File.Fprintf("<filename>%s</filename><mtime>%s</mtime>\n", vokfilename1[ii].filename, vokfilename1[ii].mtime);
	}

	return true;
}

// 如果ptype=1, 把采集到的文件记录追加到okfilename文件中
bool AppendToOKFileName(struct st_fileinfo *stfileinfo)
{
	CFile File;

	if (File.Open(starg.okfilename, "a") == false)
	{
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename); return false;
	}

	File.Fprintf("<filename>%s</filename>%s<mtime></mtime>\n", stfileinfo->filename, stfileinfo->mtime);

	return true;
}
