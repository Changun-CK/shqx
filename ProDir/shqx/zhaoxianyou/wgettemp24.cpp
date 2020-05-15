#include "_public.h"

void EXIT(int sig);

CLogFile       logfile;

bool GetURL(char *strBuffer,char *strURL,char *strFileName);

int main(int argc, char *argv[])
{
  if(argc!=4)
  {
    printf("Usage:%s logfilename tmpfilename outputfilename\n",argv[0]); 
    printf("Sample:./wgettemp24 /log/shqx/wgettemp24.log /data/wgettmp /data/wfile/zhtemp24\n\n");
    printf("本程序用于从中国天气网获取逐小时温度实况图。\n");
    printf("中国天气网的url是http://products.weather.com.cn/product/Index/index/procode/JC_WD_ZH.shtml\n");
    printf("如果中国天气网的url改变，程序也在做改动。\n");
    printf("logfilename 本程序的运行产生的日志文件名。\n");
    printf("tmpfilename 本程序运行产生的临时文件存放的目录。\n");
    printf("获取逐小时温度实况图存放的目录。\n\n");
    exit(1);
  }

  // 关闭全部的信号和输入输出
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // 打开日志文件
  if (logfile.Open(argv[1],"a+") == false)
  {
    printf("logfile.Open(%s) failed.\n",argv[1]); return -1;
  }

  MKDIR(argv[2],false); MKDIR(argv[3],false);

  while (true)
  {
    // 调用wgetclient获取网页内容
    char strwgetclient[2001];
    memset(strwgetclient,0,sizeof(strwgetclient));
    snprintf(strwgetclient,2000,"/htidc/public/bin/wgetclient \"http://products.weather.com.cn/product/Index/index/procode/JC_WD_ZH.shtml\" %s/wgetclient_%d.tmp  %s/wgetclient_%d.html %s/wgetclient.log utf-8",argv[2],getpid(),argv[2],getpid(),argv[2]);
    system(strwgetclient);
    // logfile.Write("%s\n",strwgetclient);
  
    // 打开网页内容文件
    char stroutputfile[301];
    memset(stroutputfile,0,sizeof(stroutputfile));
    snprintf(stroutputfile,300,"%s/wgetclient_%d.html",argv[2],getpid());
    CFile File;
    if (File.Open(stroutputfile,"r")==false)
    {
      logfile.Write("File.Open(%s) failed.\n",stroutputfile); sleep(60); continue;
    }
    
    char strBuffer[1001],strURL[501],strFullFileName[301],strFileName[101];
  
    // 得到全部的图片文件名
    while (true)
    {
      memset(strBuffer,0,sizeof(strBuffer));
      memset(strURL,0,sizeof(strURL));
      memset(strFullFileName,0,sizeof(strFullFileName));
      memset(strFileName,0,sizeof(strFileName));
  
      if (File.Fgets(strBuffer,1000)==false) break;
  
      if (MatchFileName(strBuffer,"*PWCP_TWC_WEAP_S99_ETO_TWC_L88_P9_20*.JPG*")==false) continue;
  
      // logfile.Write("%s",strBuffer);
  
      // 解析出url和文件名
      if (GetURL(strBuffer,strURL,strFileName)==false) continue;
  
      // 文件已存在，不采集
      snprintf(strFullFileName,300,"%s/%s",argv[3],strFileName);
      if (access(strFullFileName,F_OK)==0) continue;
  
      // 调用wget获取文件
      logfile.Write("download %s ",strFileName);
      memset(strwgetclient,0,sizeof(strwgetclient));
      snprintf(strwgetclient,500,"wget \"%s\" -o %s/wgettemp24.log -O %s",strURL,argv[2],strFullFileName);
      system(strwgetclient);
  
      if (access(strFullFileName,F_OK)==0) logfile.WriteEx("ok.\n");
      else logfile.WriteEx("failed.\n");
    }
  
    File.CloseAndRemove();

    sleep(60);
  }

  return 0;
}

bool GetURL(char *strBuffer,char *strURL,char *strFileName)
{
  char *start,*end;
  start=end=0;

  if ((start=strstr(strBuffer,"http"))==0) return false;
  if ((end=strstr(start,"\""))==0) return false;

  strncpy(strURL,start,end-start);

  strcpy(strFileName,strstr(strURL,"PWCP"));

  return true;
}

void EXIT(int sig)
{
  if (sig > 0) signal(sig,SIG_IGN);

  logfile.Write("catching the signal(%d).\n",sig);

  logfile.Write("wgetclient exit.\n");

  exit(0);
}

/*
除了普通的字母，数字，中文，还有特殊字符，但是规范的使用应该是使用字符转义。
十六进制值 
1. +  URL 中+号表示空格 %2B 
2. 空格 URL中的空格可以用+号或者编码 %20 
3. /  分隔目录和子目录 %2F  
4. ?  分隔实际的 URL 和参数 %3F  
5. % 指定特殊字符 %25  
6. # 表示书签 %23  
7. & URL 中指定的参数间的分隔符 %26  
8. = URL 中指定参数的值 %3D 
*/

