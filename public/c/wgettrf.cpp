#include "_freecplus.h"

void EXIT(int sig);

CLogFile       logfile;

int main(int argc, char *argv[])
{
  if(argc!=4)
  {
    printf("Usage:%s weburl outputfilename logfilename\n",argv[0]); 
    printf("本程序用于获取WEB网页的内容。\n");
    printf("weburl 网页WEB的地址。\n");
    printf("outputfilename 最终的输出全路径目录\n");
    printf("logfilename 本程序的运行产生的日志文件名。\n");
    exit(1);
  }

  // 关闭全部的信号和输入输出
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // 打开日志文件
  if (logfile.Open(argv[3],"a+") == false)
  {
    printf("logfile.Open(%s) failed.\n",argv[3]); return -1;
  }

  MKDIR(argv[2],false);
  if (chdir(argv[2]) != 0)
  {
    logfile.Write("打开目录%s失败.\n", argv[2]); return false;
  }

	char strtime[20];
    char strweburl[3001];
    char strcmd[3024];

  for (int ii = 0; ii < 24; ii++)
  {
    memset(strweburl,0,sizeof(strweburl));
	memset(strtime, 0, sizeof(strtime));

	if (AddTime("20200324210000", strtime, ii*60*60, "yyyymmddhh24miss") == false)
	{
		logfile.Write("时间偏移出错, 不能取出指定的图片.\n"); break;
	}
	sprintf(strweburl, "%s%s", argv[1], strtime);
	strcat(strweburl, "000.JPG");

    memset(strcmd,0,sizeof(strcmd));
    snprintf(strcmd,3000,"/usr/bin/wget \"%s\" 1>>/dev/null 2>>/dev/null",strweburl);
    system(strcmd);
    logfile.Write("%s\n",strcmd);
  }

  return 0;
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
