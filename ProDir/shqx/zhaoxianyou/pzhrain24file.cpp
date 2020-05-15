/*
 *  本程序用于处理全国逐小时雨量实况图片文件。
 *  作者：码农有道 日期：20191105
*/
#include "_public.h"
#include "_ooci.h"
#include "_shqx.h"

CLogFile logfile;

CDir Dir;

// 处理数据文件
bool _pzhrain24file(char *strargv2,char *strargv4,char *strargv5);

connection conn;

void EXIT(int sig);

int main(int argc,char *argv[])
{
  if (argc!=7)
  {
    printf("\n本程序用于处理全国逐小时雨量实况图片文件。\n\n");
    printf("/htidc/shqx/bin/pzhrain24file logfilename connstr srcpathname dstpathname tname timetvl\n");
    printf("例如：/htidc/shqx/bin/pzhrain24file /log/shqx/pzhrain24file.log shqx/pwdidc@snorcl11g_198 /data/wfile/zhrain24 /qxfile/zhrain24 T_ZHRAIN24 30\n");
    printf("logfilename 本程序运行的日志文件名。\n");
    printf("connstr 数据库的连接参数。\n");
    printf("srcpathname 原始文件存放的目录，文件命名如PWCP_TWC_WEAP_SFER_ER1_TWC_L88_P9_20191101070000000.JPG。\n");
    printf("dstpathname 目标文件存放的目录，文件按yyyy/mm/dd组织目录，重命名为zhrain24_yyyymmddhh24miss.jpg。\n");
    printf("tname 数据存放的表名。\n");
    printf("timetvl 本程序运行的时间间隔，单位：秒。\n");
    return -1;
  }

  // 关闭全部的信号和输入输出
  CloseIOAndSignal();

  // 处理程序退出的信号
  signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  if (logfile.Open(argv[1],"a+")==false)
  {
    printf("打开日志文件失败（%s）。\n",argv[1]); return -1;
  }

  logfile.Write("程序启动。\n");

  while (true)
  {
    // logfile.Write("开始扫描目录。\n");

    // 扫描数据文件存放的目录，只匹配"PWCP_TWC_WEAP_SFER_ER1_TWC_L88_P9_20*.JPG"
    if (Dir.OpenDir(argv[3],"PWCP_TWC_WEAP_SFER_ER1_TWC_L88_P9_20*.JPG",1000,true,true)==false)
    {
      logfile.Write("Dir.OpenDir(%s) failed.\n",argv[3]); sleep(atoi(argv[6])); continue;
    }

    // 逐个处理目录中的数据文件
    while (true)
    {
      if (Dir.ReadDir()==false) break;
  
      // 处理数据文件
      if (_pzhrain24file(argv[2],argv[4],argv[5])==false) 
      {
        logfile.WriteEx("失败。\n"); continue;
      }
    }

    // 断开与数据库的连接
    if (conn.m_state==1) conn.disconnect(); 

    sleep(atoi(argv[6]));
  }

  return 0;
}

void EXIT(int sig)
{
  logfile.Write("程序退出，sig=%d\n\n",sig);

  exit(0);
}
     
// 处理数据文件
bool _pzhrain24file(char *strargv2,char *strargv4,char *strargv5)
{
  char strddatetime[21];   // 文件的数据时间，格式yyyymmddhh24miss
  memset(strddatetime,0,sizeof(strddatetime));
  strncpy(strddatetime,strstr(Dir.m_FileName,"20"),14);

  char strdstfilename[301];  // 目标文件名，不带路径
  memset(strdstfilename,0,sizeof(strdstfilename));
  snprintf(strdstfilename,300,"zhrain24_%s.jpg",strddatetime);
  
  char strdstfilepath[301];  // 目标文件存放的目录
  memset(strdstfilepath,0,sizeof(strdstfilepath));
  snprintf(strdstfilepath,300,"%s/",strargv4);
  strncat(strdstfilepath,strddatetime,4);     strcat(strdstfilepath,"/");  // 年的子目录
  strncat(strdstfilepath,strddatetime+4,2);   strcat(strdstfilepath,"/");  // 月的子目录
  strncat(strdstfilepath,strddatetime+6,2);   strcat(strdstfilepath,"/");  // 日的子目录

  char strfulldstfilename[301]; // 目标文件名，全路径
  memset(strfulldstfilename,0,sizeof(strfulldstfilename));
  snprintf(strfulldstfilename,300,"%s%s",strdstfilepath,strdstfilename);

  // 如果文件已处理(目标文件已存在)，直接返回成功。
  if (access(strfulldstfilename,F_OK) == 0) return true;

  if (conn.m_state==0)
  {
    if (conn.connecttodb(strargv2,"Simplified Chinese_China.ZHS16GBK")!=0)
    {
      logfile.Write("connect database(%s) failed.\n%s\n",strargv2,conn.m_cda.message); return false;
    }
    // logfile.Write("连接数据库成功。\n");
  }
  
  // 把源文件复制到目标文件
  if (COPY(Dir.m_FullFileName,strfulldstfilename)==false) 
  {
    logfile.Write("复制文件COPY(%s,%s)...failed.\n",Dir.m_FullFileName,strfulldstfilename); return false;
  }

  // 把非结构化数据文件写入oracle数据库的表中
  if (FileToTable(&conn,&logfile,strargv5,strfulldstfilename,strddatetime)!=0)
  {
    logfile.Write("把文件%s存入%s...failed.\n",strfulldstfilename,strargv5);  return false;
  }

  logfile.Write("把文件%s存入%s...ok.\n",strfulldstfilename,strargv5);  

  return true;
}

