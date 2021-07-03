/*
 * 这是一个通用的功能模块, 采用TCP协议获取文件的客户端
 * 作者: 陈锟 时间:20200108
 */
#include "_freecplus.h"

struct st_arg
{
	char ip[31];             // 服务端的IP地址
	int  port;               // 服务端的端口
	int  ptype;              // 文件获取成功后服务端文件的处理方式:1-保留文件; 2-删除文件; 3-移动到备份>目录
	char clientpath[301];    // 客户端文件存放的目录
	char srvpath[301];       // 服务端文件存放的目录
	char srvpathbak[301];    // 文件成功获取后, 服务端文件备份的根目录, 当ptype==3时有效
	bool andchild;           // 是否获取srvpath目录下各级子目录的文件, true-是; false-否; 缺>省为false
	char matchname[301];     // 待获取文件名的匹配方式, 如"*.TXT,*.XML", 注意用大写
	char okfilename[301];    // 已获取成功文件名清单
	int  timetvl;            // 扫描服务端目录文件的时间间隔, 单位: 秒
} starg;

CLogFile logfile;
CTcpClient TcpClient;

char strRecvBuffer[10241];    // 接收报文的缓冲区
char strSendBuffer[10241];    // 发送报文的缓冲区

vector<struct st_fileinfo> vlistfile, vlistfile1;
vector<struct st_fileinfo> vokfilename, vokfilename1;

// 把srvpath目录下的文件加载到vlistfile容器中
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
bool _tcpgetfiles();

// 向服务器发起连接并登录
bool ClientLogin(const char *argv);

// 向服务器发送心跳报文
bool ActiveTest();

int main(int argc, char *argv[])
{
	if (argc != 3) { _help(argv); return -1; }

	// 关闭全部的信号和输入输出
	CloseIOAndSignal();

	// 处理程序退出的信号, 注意不要用kill -9 强行杀死进程
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

		// 实现文件获取的功能
		_tcpgetfiles();

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
	printf("\nExample:/ProDir/public/bin/tcpgetfiles logfilename xmlbuffer\n");
	printf("\nUsing:/ProDir/public/bin/tcpgetfiles /log/shqx/tcpgetfiles_surfdata.log "\
	"\"<ip>39.108.127.31</ip><port>5050</port><ptype>1</ptype><clientpath>/data/shqx/sdata/surfdata</clientpath>"\
	"<srvpathbak>/data/shqx/sdata/surfdatabak</srvpathbak><srvpath>/data/shqx/tcp/surfdata</srvpath>"\
	"<andchild>true</andchild><matchname>SURF_*.TXT,*.DAT</matchname><okfilename>/data/shqx/tcplist/tcpgetfiles_surfdata.xml</okfilename>"\
	"<timetvl>10</timetvl>\"\n\n");
	printf("logfilename   本程序的日志文件名.\n");
	printf("xmlbuffer     本程序的运行参数.\n");
	printf("ip            服务器的IP地址.\n");
	printf("port          服务器的端口.\n");
	printf("ptype         文件获取成功后的处理方式:1-保留文件; 2-删除文件; 3-移动到备份目录.\n");
	printf("clientpath    客户端文件存在的目录.\n");
	printf("srvpathbak    服务端备份文件存在的目录, 当ptype=3时有效.\n");
	printf("srvpath       服务端文件存在的目录.\n");
	printf("andchild      是否发送srvpath目录下的子目录的文件, true-是; false-否; 缺省为false.\n");
	printf("matchname     待获取文件名的匹配方式, 如\"*.TXT,*.XML\".\n");
	printf("okfilename    已获取成功文件名清单, 缺省为空.\n");
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

	GetXMLBuffer(xml, "srvpathbak", starg.srvpathbak);
	if ( (strlen(starg.srvpathbak) == 0) && (starg.ptype == 3) )
	{ logfile.Write("srvpathbak is NULL.\n"); return false; }

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

	if (starg.timetvl > 50) starg.timetvl = 50;

	return true;
}

void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n\n", sig);

	TcpClient.Close();

	exit(0);
}

// 业务流程的主函数, 实现发送文件功能
bool _tcpgetfiles()
{
	// 把服务端srvpath目录下的文件加载到vlistfile容器中
	if (LoadListFile() == false)
	{
		logfile.Write("LoadListFile() failed.\n"); TcpClient.Close(); return false;
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

	// 从服务端逐个获取新文件或已改动过后的文件
	for (int ii = 0; ii < vlistfile.size(); ii++)
	{
		// 向服务端发送将获取(下载)的文件名信息
		memset(strSendBuffer, 0, sizeof(strSendBuffer));
		sprintf(strSendBuffer, "<filename>%s</filename><filesize>%d</filesize><mtime>%s</mtime>", vlistfile[ii].filename, vlistfile[ii].filesize, vlistfile[ii].mtime);
		// logfile.Write("strSendBuffer = %s\n", strSendBuffer); // xxxxx

		if (TcpClient.Write(strSendBuffer) == false)
		{
			logfile.Write("3 TcpClient.Write() failed.\n"); TcpClient.Close(); return false;
		}

		// 此报文有些多余, 这是为了兼容SendFile和RecvFile函数, 对性能几乎不会产生影响
		if (TcpClient.Read(strRecvBuffer) == false)
		{
			logfile.Write("3 TcpClient.Read() failed.\n"); TcpClient.Close(); return false;
		}
		// logfile.Write("strRecvBuffer = %s\n", strRecvBuffer); // xxxxx

		// 把文件名中的clientpath替换为srvpath, 要小心第3个参数, 必须为false
		struct st_fileinfo stfileinfo;
		memset(&stfileinfo, 0, sizeof(stfileinfo));
		strcpy(stfileinfo.filename, vlistfile[ii].filename);
		strcpy(stfileinfo.mtime, vlistfile[ii].mtime);
		stfileinfo.filesize = vlistfile[ii].filesize;
		UpdateStr(stfileinfo.filename, starg.srvpath, starg.clientpath, false);

		logfile.Write("get %s ...", stfileinfo.filename);

		// 接收文件的内容
		if (RecvFile(TcpClient.m_sockfd, &stfileinfo, &logfile) == false)
		{
			logfile.Write("RecvFile() failed.\n"); TcpClient.Close(); return false;
		}

		logfile.WriteEx("ok.\n");

		// 如果ptype=1, 把采集成功的文件记录追加到okfilename文件中
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

		strcpy(strSendBuffer, argv); strcat(strSendBuffer, "<clienttype>2</clienttype>");

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
	/*1.客户端向服务端发送一个请求报文;
	 *2.服务端Dir扫描srvpath下面文件, 得到一个文件清单;
	 *3.服务端向客户端发送一个文件总数的报文;
	 *4.把文件信息一条一条的传给客户端;
	 *5.客户端接收到的记录直接存放在vlistfile容器中.*/

	vlistfile.clear();
	
	memset(strSendBuffer, 0, sizeof(strSendBuffer));
	strcpy(strSendBuffer, "<list>");

	if (TcpClient.Write(strSendBuffer) == false)
	{
		logfile.Write("4 TcpClient.Write() failed.\n"); return false;
	}

	memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
	if (TcpClient.Read(strRecvBuffer) == false)
	{
		logfile.Write("4 TcpClient.Read() failed.\n"); return false;
	}

	int totalfile = 0;
	GetXMLBuffer(strRecvBuffer, "totalfile", &totalfile);

	struct st_fileinfo stfileinfo;

	for (int ii = 0; ii < totalfile; ii++)
	{
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
		memset(strRecvBuffer, 0, sizeof(strRecvBuffer));

		if (TcpClient.Read(strRecvBuffer, 20) == false)
		{
			logfile.Write("5 TcpClient.Read() failed.\n"); return false;
		}
		// logfile.Write("strRecvBuffer=%s\n", strRecvBuffer); // xxxxx

		GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
		GetXMLBuffer(strRecvBuffer, "filesize",&stfileinfo.filesize);
		GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

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

	// 注意, 打开文件不要采用缓冲机制
	if (File.Open(starg.okfilename, "w", false) == false)
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

	// 注意, 打开文件不要采用缓冲机制
	if (File.Open(starg.okfilename, "a", false) == false)
	{
		logfile.Write("File.Open(%s) failed.\n", starg.okfilename); return false;
	}

	File.Fprintf("<filename>%s</filename>%s<mtime></mtime>\n", stfileinfo->filename, stfileinfo->mtime);

	return true;
}
