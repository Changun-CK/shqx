/*
 *这是一个通用的功能模块，采用TCP协议实现文件传输的服务端，多进程。
 *作者：陈锟
*/
#include "_freecplus.h"

struct st_arg
{
	char ip[31];              // 服务器端的IP地址。
	int  port;                // 服务器端的端口。
	int  ptype;               // 文件发送成功后文件的处理方式：1-保留文件；2-移动到备份目录；3-删除文件。
	char clientpath[301];     // 本地文件存放的根目录。
	char clientpathbak[301];  // 文件成功发送后，本地文件备份的根目录，当ptype==2时有效。
	char srvpath[301];        // 服务端文件存放的根目录。
	char srvpathbak[301];     // 文件成功接收后，服务端文件备份的根目录，当ptype==2时有效。
	bool andchild;            // 是否发送clientpath目录下各级子目录的文件，true-是；false-否。
	char matchname[301];      // 待发送文件名的匹配方式，如"*.TXT,*.XML"，注意用大写。
	char okfilename[301];     // 已发送成功文件名清单。
	int  timetvl;             // 扫描本地目录文件的时间间隔，单位：秒。
} starg;

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer);

CTcpServer TcpServer;
CLogFile logfile;

char strRecvBuffer[10241]; // 接收报文的缓冲区
char strSendBuffer[10241]; // 发送报文的缓冲区

int clienttype=0;

// 等待登录
bool ClientLogin();

// 列出srvpath目录下文件的清单，返回给客户端。
bool ListFile();

// 程序退出时调用的函数
void FathEXIT(int sig);
void ChldEXIT(int sig);

// 接收文件主函数
void RecvFilesMain();

// 发送文件主函数
void SendFilesMain();

int main(int argc,char *argv[])
{
	if (argc != 3)
	{
		printf("\n");
		printf("Using:/ProDir/public/bin/tcpfileserver logfilename port\n");
		
		printf("Example:/ProDir/public/bin/tcpfileserver /log/shqx/tcpfileserver.log 5050\n\n");
		printf("本程序是一个公共功能模块，采用TCP/IP传输文件的服务端。\n");
		printf("logfilename 日志文件名。\n");
		printf("port 用于传输文件的TCP端口。\n");
		
		return -1;
	}

	// 关闭全部的信号和输入输出
	// 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
	// 但请不要用 "kill -9 +进程号" 强行终止
	CloseIOAndSignal(); signal(SIGINT,FathEXIT); signal(SIGTERM,FathEXIT);
	
	// 打开程序运行日志，这是一个多进程程序，日志不能自动切换
	if (logfile.Open(argv[1],"a+",false) == false)
	{
		printf("logfile.Open(%s) failed.\n",argv[1]); return -1;
	}
	
	logfile.Write("fileserver started(%s).\n",argv[2]);

	if (TcpServer.InitServer(atoi(argv[2])) == false)
	{
		logfile.Write("TcpServer.InitServer(%s) failed.\n", argv[2]); exit(-1);
	}

	while (1)
	{
		// 等待客户端的连接
		if (TcpServer.Accept() == false)
		{
			logfile.Write("TcpServer.Accept() failed.\n"); continue;
		}

		// 新的客户端连接
		if (fork() > 0)
		{
			// 父进程关闭刚建立起来的sock连接, 并回到Accept继续监听
			TcpServer.CloseClient(); continue;
		}

		// 进入子进程的流程
		signal(SIGINT, ChldEXIT); signal(SIGTERM, ChldEXIT);

		// 子进程关闭监听的sock
		TcpServer.CloseListen();

		// 等待客户端的登录
		if (ClientLogin() == false) ChldEXIT(0);

		// 接收文件主函数
		if (1 == clienttype) RecvFilesMain();

		// 发送文件的主函数
		if (2 == clienttype) SendFilesMain();

		ChldEXIT(0);
	}

	return 0;
}

// 父进程退出时调用的函数
void FathEXIT(int sig)
{
	if (sig > 0)
	{
		signal(sig, SIG_IGN);
		logfile.Write("catching the signal(%d).\n", sig);
	}

	TcpServer.CloseListen();

	kill(0, 15);

	logfile.Write("fileserver EXIT.\n");

	exit(0);
}

// 子进程退出时调用的函数
void ChldEXIT(int sig)
{
	if (sig > 0) signal(sig, SIG_IGN);
	TcpServer.CloseClient();
	exit(0);
}

// 等待登录
bool ClientLogin()
{
	memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
	memset(strSendBuffer, 0, sizeof(strSendBuffer));

	if (TcpServer.Read(strRecvBuffer, 20) == false)
	{
		logfile.Write("1 TcpServer.Read() failed.\n"); return false;
	}
	// logfile.Write("1 strRecvBuffer = %s\n", strRecvBuffer); // xxxxx

	GetXMLBuffer(strRecvBuffer, "clienttype", &clienttype);

	if ( (clienttype == 1) || (clienttype == 2) )
		strcpy(strSendBuffer, "ok");
	else
		strcpy(strSendBuffer, "failed");

	// logfile.Write("1 strSendBuffer = %s\n", strSendBuffer); // xxxxx
	if (TcpServer.Write(strSendBuffer) == false)
	{
		logfile.Write("1 TcpServer.Write() failed.\n"); return false;
	}

	logfile.Write("%s login %s.\n", TcpServer.GetIP(), strSendBuffer);

	if (strcmp(strSendBuffer, "failed") == 0) return false;

	// 把参数解析出来
	_xmltoarg(strRecvBuffer);

	return true;
}

// 接收文件主函数
void RecvFilesMain()
{
	while (1)
	{
		memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
		memset(strSendBuffer, 0, sizeof(strSendBuffer));

		if (TcpServer.Read(strRecvBuffer, 80) == false)
		{
			logfile.Write("2 TcpServer.Read() failed.\n"); ChldEXIT(-1);
		}
		// logfile.Write("2 strRecvBuffer = %s.\n", strRecvBuffer); // xxxxx

		// 处理心跳报文
		if (strstr(strRecvBuffer, "activetest") != 0)
		{
			strcpy(strSendBuffer, "ok");
			// logfile.Write("2 strRecvBuffer = %s.\n", strRecvBuffer); // xxxxx
			if (TcpServer.Write(strSendBuffer) == false)
			{
				logfile.Write("2 TcpServer.Write() failed.\n"); ChldEXIT(-1);
			}
			continue;
		}

		struct st_fileinfo stfileinfo;
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));

		// 获取待接收的文件的时间和大小
		GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
		GetXMLBuffer(strRecvBuffer, "filesize",&stfileinfo.filesize);
		GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

		// 把文件名中的clientpath替换成srvpath, 要小心第三个参数, 必须为false
		UpdateStr(stfileinfo.filename, starg.clientpath, starg.srvpath, false);

		// 接收文件的内容
		if (RecvFile(TcpServer.m_connfd, &stfileinfo, &logfile) == false)
		{
			logfile.Write("RecvFile() failed.\n"); ChldEXIT(-1);
		}
		logfile.Write("recv %s success.\n", stfileinfo.filename);
	}
}

// 发送文件主函数
void SendFilesMain()
{
	while (1)
	{
		memset(strRecvBuffer, 0, sizeof(strRecvBuffer));
		if (TcpServer.Read(strRecvBuffer, 80) == false)
		{
			logfile.Write("3 TcpServer.Read() failed.\n"); ChldEXIT(-1);
		}
		// logfile.Write("3 strRecvBuffer = %s\n", strRecvBuffer); // xxxxx

		// 处理心跳报文
		if (strstr(strRecvBuffer, "activetest") != 0)
		{
			memset(strSendBuffer, 0, sizeof(strSendBuffer));
			strcpy(strSendBuffer, "ok");
			// logfile.Write("3 strSendBuffer = %s\n", strSendBuffer); // xxxxx
			if (TcpServer.Write(strSendBuffer) == false)
			{
				logfile.Write("3 TcpServer.Write() failed.\n"); ChldEXIT(-1);
			}

			continue;
		}

		// 处理获取文件列表报文
		if (strcmp(strRecvBuffer, "<list>") == 0)
		{
			if (ListFile() == false)
			{
				logfile.Write("ListFile() failed.\n"); ChldEXIT(-1);
			}

			continue;
		}

		// 取文件报文
		if (strncmp(strRecvBuffer, "<filename>", 10) == 0)
		{
			// 获取待接收的文件的时间和大小
			struct st_fileinfo stfileinfo;
			memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
			GetXMLBuffer(strRecvBuffer, "filename", stfileinfo.filename);
			GetXMLBuffer(strRecvBuffer, "filesize",&stfileinfo.filesize);
			GetXMLBuffer(strRecvBuffer, "mtime", stfileinfo.mtime);

			// 把文件发送给客户端
			if (SendFile(TcpServer.m_connfd, &stfileinfo, &logfile) == false) ChldEXIT(-1);

			logfile.Write("put %s ...ok.\n", stfileinfo.filename);

			// 删除服务端的文件
			if (starg.ptype == 2) REMOVE(stfileinfo.filename);

			// 备份服务端的文件
			if (starg.ptype == 3)
			{
				char strfilenamebak[301];
				memset(strfilenamebak, 0, sizeof(strfilenamebak));
				strcpy(strfilenamebak, stfileinfo.filename);
				// 小心第三个参数, 必须为false, 否则段错误
				UpdateStr(strfilenamebak, starg.srvpath, starg.srvpathbak, false);
				if (RENAME(stfileinfo.filename, strfilenamebak) == false)
				{
					logfile.Write("RENAME %s to %s failed.\n", stfileinfo.filename, strfilenamebak); ChldEXIT(-1);
				}
			}
		}
	}
}

// 把xml解析到参数starg结构中
bool _xmltoarg(char *strxmlbuffer)
{
  memset(&starg,0,sizeof(struct st_arg));
  GetXMLBuffer(strxmlbuffer,"ip",starg.ip);
  GetXMLBuffer(strxmlbuffer,"port",&starg.port);
  GetXMLBuffer(strxmlbuffer,"ptype",&starg.ptype);
  GetXMLBuffer(strxmlbuffer,"clientpath",starg.clientpath);
  GetXMLBuffer(strxmlbuffer,"clientpathbak",starg.clientpathbak);
  GetXMLBuffer(strxmlbuffer,"srvpath",starg.srvpath);
  GetXMLBuffer(strxmlbuffer,"srvpathbak",starg.srvpathbak);
  GetXMLBuffer(strxmlbuffer,"andchild",&starg.andchild);
  GetXMLBuffer(strxmlbuffer,"matchname",starg.matchname);
  GetXMLBuffer(strxmlbuffer,"okfilename",starg.okfilename);
  GetXMLBuffer(strxmlbuffer,"timetvl",&starg.timetvl);

  return true;
}

// 列出srvpath目录下文件的清单，返回给客户端。
bool ListFile()
{
	CDir Dir;

	// 注意, 如果目录下的总文件数超过50000, 增量发送文件功能将有问题
	if (Dir.OpenDir(starg.srvpath, starg.matchname, 50000, starg.andchild, false) == false)
	{
		logfile.Write("Dir.OpenDir(%s) failed.\n", starg.srvpath); return false;
	}

	// 先把文件总数返回给客户端
	memset(strSendBuffer, 0, sizeof(strSendBuffer));
	sprintf(strSendBuffer, "<totalfile>%d</totalfile>", Dir.m_vFileName.size());
	// logfile.Write("4 strSendBuffer = %s\n", strSendBuffer); // xxxxx
	if (TcpServer.Write(strSendBuffer) == false)
	{
		logfile.Write("4 TcpServer.Write() failed.\n"); return false;
	}

	// 把文件信息一条一条的返回客户端
	while (1)
	{
		if (Dir.ReadDir() == false) break;

		memset(strSendBuffer, 0, sizeof(strSendBuffer));
		sprintf(strSendBuffer, "<filename>%s</filename><mtime>%s</mtime><filesize>%d</filesize>", Dir.m_FullFileName, Dir.m_ModifyTime, Dir.m_FileSize);
		// logfile.Write("5 strSendBuffer = %s\n", strSendBuffer); // xxxxx

		if (TcpServer.Write(strSendBuffer) == false)
		{
			logfile.Write("5 TcpServer.Write() failed.\n"); return false;
		}
	}

	return true;
}
