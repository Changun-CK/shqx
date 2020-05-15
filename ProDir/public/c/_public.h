/*
 * 此程序用于演示把自定义函数声明在程序的外部的头文件中
 * 作者: 陈锟   日期: 2019-11-28
*/

#ifndef _PUBLIC_H
#define _PUBLIC_H 1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <stdarg.h>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <utime.h>
#include <algorithm>
#include <dirent.h>
#include <cstdlib>
#include <sys/time.h>
#include <fcntl.h>

using namespace std;

/* 全路径文件名, 大小, 时间的结构体 */
struct st_fileinfo
{
    char filename[301];
    int  filesize;
    char mtime[21];
};

//复制字符串，并返回
char *strcpy(char* dest,const size_t destlen,const char* src);

//复制字符串，并返回指定长度的字符串
char *strncpy(char* dest,const size_t destlen,const char* src,size_t n);

//拼接两个字符串
char *strcat(char* dest,const size_t destlen,const char* src);

//拼接两个字符串,指定拼接的长度
char *strncat (char* dest,const size_t destlen,const char* src,size_t n);

//从左往右查找字符串，并返回后面的字符串
char *STRCHR(const char *s, const int c);

//从右往左查找字符串，并返回后面的字符串
char *STRRCHR(const char *s, const int c);

// 删除字符串左边指定的字符
void DeleteLChar(char *in_string,const char in_char);

// 删除字符串右边指定的字符
void DeleteRChar(char *in_string,const char in_char);

// 删除字符串两边指定的字符
void DeleteLRChar(char *in_string,const char in_char);

// 在字符串的左边补字符到指定长度
void LPad(char *str,const char in_char,unsigned int in_len);

// 在字符串的右边补字符到指定长度
void RPad(char *str,const char in_char,unsigned int in_len);

// 把小写转换成大写，忽略不是字母的字符
void ToUpper(char *str);
void ToUpper(string &str);

// 把大写转换成小写，忽略不是字母的字符
void ToLower(char *str);
void ToLower(string &str);

// 判断内容是否全部是数字
bool IsDigit(const char *strBuffer);

// 判断内容是否全部是大写字母
bool IsUpper(const char *strBuffer);

// 判断内容是否全部是ASCII字符
bool IsASCII(const char *strBuffer);

// 判断内容是否全部是数字和空格
bool IsDigitOrSpace(const char *strLine);

// 把in_srcfilename改名为in_dstfilename，如果改名失败，会尝试in_times次
bool RENAME(const char *in_srcfilename,const char *in_dstfilename,const int in_times=3);

// 把整数的时间转换为字符串格式的时间，格式如："2019-02-08 12:05:08"，如果转换成功函数返回0，失败返回-1
int timetostr(const time_t ti,char *strtime);

// 把字符串格式的时间转换为整数的时间, 缺省格式为yyyy-mm-dd hh:mi:ss, 支持yyyymmddhhmiss
int strtotime(const char *strtime,time_t *ti,const char *in_fmt=0);

// 从文件文件中读取一行
// strEndStr是一行数据的结束标志，如果为空，则以换行符"\n"为结束标志。
// 本函数不会删除行的结束标志
bool FGETS(const FILE *fp,char *strBuffer,const int ReadSize,const char *strEndStr=0);

//文件操作类
class CFile
{
private:
  FILE *m_fp;
  bool  m_bEnBuffer;        //文件缓冲 
  char  m_filename[301];
  char  m_filenametmp[301]; //临时文件名

public:
  CFile();                  //构造函数

 ~CFile();                  //析构函数 

  bool IsOpened();  // 判断文件是否已打开
  bool Open(const char *filename,const char *openmode, bool bEnBuffer=true);     //打开文件按"openmode"方式
  bool OpenForRename(const char *filename,const char *openmode, bool bEnBuffer=true);     //打开文件按"openmode"方式, 并且重命名
  bool CloseAndRename();     //关闭文件并且重命名

  void Fprintf(const char *fmt, ... );                      //写入文件

  bool Fgets(char *strBuffer,const int Size,bool bDelCRT=false); //获取文件的行, Size最好为sizeof(strBuffer), bDelCRT为是否删除'\r\n', 默认不删除.

	/* 从文件文件中读取一行
	strEndStr是一行数据的结束标志，如果为空，则以换行符"\n"为结束标志。
	与Fgets不同，本函数不删除结束标志 */
	bool FFGETS(char *strBuffer,const int ReadSize,const char *strEndStr=0); //ReadSize最好为sizeof(strBuffer)

  void Close();                                             //关闭文件函数
};

// 拆分字符串的类
// 字符串的格式为:内容1+分隔字符串+内容2+分隔字符串+内容3
// 如:num~!~name~!~address,分隔符为"~!~"
class CCmdStr
{
public:
  vector<string> m_vCmdStr;  // 存放拆分后的字段内容。

  CCmdStr();

  // 拆分字符串到容器中
  void SplitToCmd(const string in_string,const char *in_sep,const bool bdeletespace=true);

  int CmdCount();

  // 获取字段的值，取每个字段的值inum从0开始
  bool GetValue(const int inum,char   *in_return);
  bool GetValue(const int inum,char   *in_return,const int in_len);
  bool GetValue(const int inum,int    *in_return);
  bool GetValue(const int inum,long   *in_return);
  bool GetValue(const int inum,double *in_return);

  ~CCmdStr();
};

/*
  取操作系统的时间
  out_stime是输出结果
  in_interval是偏移常量，单位是秒
  返回的格式由fmt决定，fmt目前的取值如下，如果需要，可以增加：
  yyyy-mm-dd hh:mi:ss，此格式是缺省格式
  yyyymmddhhmiss
  yyyy-mm-dd
  yyyymmdd
  hh:mi:ss
  hhmiss
  hh:mi
  hhmi
  hh
  mi
*/
void LocalTime(char *out_stime,const char *in_fmt=0,const int in_interval=0);

// 日志文件操作类
class CLogFile
{
public:
  FILE   *m_tracefp;           // 日志文件指针
  char    m_filename[301];     // 日志文件全名
  char    m_openmode[11];      // 日志文件的打开方式
  bool    m_bBackup;           // 日志文件超出100M，是否自动备份
  bool    m_bEnBuffer;         // 写入日志时，是否启用操作系统的缓冲机制

  CLogFile();

  // filename日志文件名
  // openmode打开文件的方式，操作日志文件的权限,同打开文件函数(fopen)使用方法一致，一>般采用"a+"
  // bBackup，true-备份，false-不备份，在多进程的服务程序中，如果多个进行共用一个日志>文件，bBackup必须为false
  // bEnBuffer:true-启用缓冲区，false-不启用缓冲区，如果启用缓冲区，那么写进日志文件中的内容不会立即写入文件
  bool Open(const char *in_filename,const char *in_openmode,bool bBackup=true,bool bEnBuffer=false);

  // 如果日志文件大于100M，就备份它
  // 备份后的文件会在源文件名后加上日期时间
  bool BackupLogFile();

  // 写日志文件,它是个可变参数的方法,同printf函数。
  // Write()方法会写入时间，WriteEx()方法不写时间。
  bool Write(const char *fmt,...);
  bool WriteEx(const char *fmt,...);

  // 关闭日志文件
  void Close();

  ~CLogFile();
};

// 关闭全部的信号和输入输出
void CloseIOAndSignal();

// 用某文件或目录的全路径中的目录创建目录，以及该目录下的各级子目录
// pathorfilename 目录名或文件名
// bisfilename true-pathorfilename是文件名，否则是目录名
bool MKDIR(const char *pathorfilename,bool bisfilename=true);

// 调用fopen函数打开文件，如果文件名中包含的目录不存在，就创建目录
FILE *FOPEN(const char *filename,const char *mode);

// 更改文件的修改时间属性，mtime指定了时间，格式不限，只要包括了yyyy,mm,dd,hh,mi,ss即可。
int UTime(const char *filename,const char *mtime);

// 把字符串格式的时间转换为time_t
// stime为输入的时间，格式不限，但一定要包括yyyymmddhhmiss
time_t UTCTime(const char *stime);

// 从一个字符串中提取数字、符号和小数点，并判断是否是一个合法的数
// 如果不合法，将返回空字符串
// bWithSign==true表示包括符号，bWithDOT==true表示包括圆点
void PickNumber(const char *strSrc,char *strDst,const bool bWithSign,const bool bWithDOT);

// 判断字符串中的负号和圆点是否合法
bool JudgeSignDOT(const char *strSrc,const char *strBZ);

// 读取某目录下的全部的文件
class CDir
{
public:
  char m_DirName[301];         // 目录名
  char m_FileName[301];        // 文件名，不包括目录名
  char m_FullFileName[301];    // 文件全名，包括目录名
  int  m_FileSize;             // 文件的大小
  char m_ModifyTime[21];       // 文件最后一次被修改的时间
  char m_CreateTime[21];       // 文件生成的时间
  char m_AccessTime[21];       // 文件最后一次被访问的时间

  int  m_uPOS;                 // 已读取m_vFileName容器的位置
  vector<string> m_vFileName;  // 存放OpenDir方法获取到的文件列表

  CDir();

  // 变量初始化
  void initdata();

  // 设置日期时间的格式，支持"yyyy-mm-dd hh:mi:ss"和"yyyymmddhhmiss"两种格式，缺省是前者
  char m_DateFMT[21];
  void SetDateFMT(const char *in_DateFMT);

  // 打开目录，获取文件名信息，存放于m_vFileName容器中
  // in_DirName，待打开的目录名
  // in_MatchStr，待获取文件名的匹配规则
  // in_MaxCount，获取文件的最大数量
  // bAndChild，是否打开各级子目录
  // bSort，是否对结果集进行排序
  bool OpenDir(const char *in_DirName,const char *in_MatchStr,const unsigned int in_MaxCount=10000,const bool bAndChild=false,bool bSort=false);

  // 这是一个递归函数，用于OpenDir()的调用。
  bool _OpenDir(const char *in_DirName,const char *in_MatchStr,const unsigned int in_MaxCount,const bool bAndChild);

  // 逐个读取目录中的文件信息
  bool ReadDir();

  ~CDir();
};

// 字符串替换函数
// 把in_string中的in_str1替换为in_str2
// bLoop是否循环执行替换
// 注意
// 1、如果in_str2比in_str1要长，替换后in_string会变长，所以必须保证in_string有足够的长度， 否则内存会溢出
// 2、如果in_str2中包函了in_str1的内容，且bLoop为true，就会进入死循环，最终导致内存溢出
void UpdateStr(char *in_string,const char *in_str1,const char *in_str2,bool bLoop=true);

// 判断文件名是否匹配in_MatchStr指定的规则
// in_FileName文件名
// in_MatchStr规则表达式，如"*.txt,*.xml"，中间用逗号分隔
bool MatchFileName(const string in_FileName,const string in_MatchStr);

// 获取文件的大小，返回字节数
int FileSize(const char *in_FullFileName);

/*
  把一个字符串表格的时间加上一个偏移量，得到偏移后的时间
  in_stime是传入的时间，任意格式，但是一定要包括yyyymmddhhmiss，是否有分隔符没有关系。
  把yyyy-mm-dd hh:mi:ss偏移in_interval秒
  传出的格式由fmt决定，fmt目前的取值如下，如果需要，可以增加：
  yyyy-mm-dd hh:mi:ss（此格式是缺省格式）
  yyyymmddhhmiss
  yyyymmddhhmiss
  yyyy-mm-dd
  yyyymmdd
  hh:mi:ss
  hhmiss
  hh:mi
  hhmi
  返回值：0-成功，-1-失败。
*/
int AddTime(const char *in_stime,char *out_stime,const int in_interval,const char *in_fmt=0);

// 获取文件的时间，即modtime
void FileMTime(const char *in_FullFileName,char *out_ModTime);

// 以下是XML格式字符串的相关操作函数和类

// 操作XMLBuffer的函数
// in_XMLBuffer，XML格式的字符串，如下：
// <filename>/tmp/readme.txt</filename><mtime>2018-01-01 12:20:35</mtime><size>10241</size>
// in_FieldName，字段的标签名
// out_Value，获取内容存放的变量的指针
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,bool   *out_Value);
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,int    *out_Value);
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,unsigned int *out_Value);
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,long   *out_Value);
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,unsigned long *out_Value);
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,double *out_Value);
bool GetXMLBuffer(const char *in_XMLBuffer,const char *in_FieldName,char   *out_Value,const int in_StrLen=0);

/* 用于生成一组随机数, 
 * 若不允许重复, 则生成区域内所有非重复的随机数, 或者指定组数的随机数
 * 若允许重复, 则生成指定组数的随机数.
 */
class CRand
{
public:
    CRand();
    ~CRand();

    vector <int> m_val;
    bool checkexit(const int aryyval, const int aryysize); // 用于检查是否为重复数据
    void Rand(const int minvalue,const int maxvalue,bool brep=true, const int nog=10000);                           //brep为是否允许重复; 默认为允许重复, nog为生成的组数, 缺省为区域内所有整数
};

// 复制文件，类似Linux系统的cp命令。
// srcfilename：原文件名，建议采用绝对路径的文件名。
// destfilename：目标文件名，建议采用绝对路径的文件名。
// 返回值：true-成功；false-失败，失败的主要原因是权限不足或磁盘空间不够。
// 注意：
// 1）在复制名文件之前，会自动创建destfilename参数中的目录名。
// 2）复制文件的过程中，采用临时文件命名的方法，复制完成后再改名为destfilename，避免中间状态的文件被读取。
// 3）复制后的文件的时间与原文件相同，这一点与Linux系统cp命令不同。
bool COPY(const char *srcfilename,const char *dstfilename);

#define TCPHEADLEN     4  // TCP报文头部的长度
#define TCPBUFLEN  10240  // TCP报文的最大长度，不能超过TCPHEADLEN能表达的长度。

// 用于TCP通讯的客户类
class CTcpClient
{
public:
  int  m_sockfd;        // socket id
  char m_ip[21];        // 服务端的IP
  int  m_port;          // 与服务端通讯的端口
  bool m_state;         // 连接状态
  bool m_btimeout;      // Read()方法是否超时，true-未超时，false-已超时
  int  m_buflen;        // 接收到的报文的包大小

  CTcpClient();

  // 向服务端发起连接
  bool ConnectToServer(const char *in_ip,const int in_port);                // 连接服务器

  // 没有超时机制的接收函数
  bool Read(char *strRecvBuffer);
  // 有超时机制的接收函数
  bool Read(char *strRecvBuffer,const int iTimeOut);

  // 向服务端发送字符串，长度为字符串的长度
  bool Write(char *strSendBuffer);

  // 向服务端发送流数据，需要指定长度
  bool Write(char *strSendBuffer,const int buflen);

  // 断开与服务端的连接
  void Close();

  ~CTcpClient();
};

// 用于TCP通讯的服务类
class CTcpServer
{
private:
  int m_socklen;                    // struct sockaddr_in的大小
  struct sockaddr_in m_clientaddr;  // 客户端的地址信息
  struct sockaddr_in m_servaddr;    // 服务端的地址信息
public:
  int  m_listenfd;                  // 服务端用于监听的socket
  int  m_connfd;                    // 客户端的socket
  bool m_btimeout;                  // Read()方法是否超时，true-未超时，false-已超时
  int  m_buflen;                    // 接收到的报文的包大小

  CTcpServer();
  bool InitServer(const unsigned int port); // 初始化TcpServer
  bool Accept();                            // 接受客户端的请求
  char *GetIP();                            // 获取客户端的ip地址


  // 没有超时机制的接收函数
  bool Read(char *strRecvBuffer);
  // 有超时机制的接收函数
  bool Read(char *strRecvBuffer,const int iTimeOut);

  // 向客户端发送字符串，长度为字符串的长度
  bool Write(char *strSendBuffer);

  // 向客户端发送流数据，需要指定长度
  bool Write(char *strSendBuffer,const int buflen);

  // 关闭监听的sock，用于多进程服务程序的子进程代码中
  void CloseListen();

  // 关闭客户端的sock，用于多进程服务程序的父进程代码中
  void CloseClient();

  ~CTcpServer();
};

// 读取socket的数据
// fd socket
// strRecvBuffer 读取结果存放的缓冲区
// buflen 读取到的报文的长度
// iTimeOut 读取超时的时间
bool TcpRead(const int fd,char *strRecvBuffer,int *buflen,const int iTimeOut=0);

// 把数据写入socket
// fd socket
// strSendBuffer 待写入数据存放的缓冲区
// buflen 待写入数据的长度，如果为0，就认为strSendBuffer是一个字符串。
// iTimeOut 写入超时的时间
bool TcpWrite(const int fd,const char *strSendBuffer,const int buflen=0,const int iTimeOut=0);

// 从socket中读取数据，专用于TcpRead函数
bool Readn(const int fd,char *vptr,const size_t n);

// 向socket中写入数据，专用于TcpWrite函数
bool Writen(const int fd,const char *vptr,const size_t n);

// 把文件通过sockfd发送给对端
bool SendFile(CLogFile *logfile,int sockfd,struct st_fileinfo *stfileinfo);

// 接收通过socdfd发送过来的文件
bool RecvFile(CLogFile *logfile,int sockfd,struct st_fileinfo *stfileinfo);


// 以上是TCP/IP通讯的函数和类
///////////////////////////////////// /////////////////////////////////////

// 这是一个精确到微秒的计时器
class CTimer
{
public:
  struct timeval m_start,m_end;

  CTimer();

  // 开始计时
  void Start();

  // 计算已逝去的时间，单位：秒，小数点后面是微秒
  double Elapsed();
};

#endif
