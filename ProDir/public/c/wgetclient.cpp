#include "_freecplus.h"

void EXIT(int sig);

CLogFile       logfile;

int main(int argc, char *argv[])
{
  if(argc!=6)
  {
    printf("Usage:%s weburl tmpfilename outputfilename logfilename charset\n",argv[0]); 
    printf("本程序用于获取WEB网页的内容。\n");
    printf("weburl 网页WEB的地址。\n");
    printf("tmpfilename 获取到的网页的内容存放的全路径的临时文件名，该文件可能是utf-8或其它编码。\n");
    printf("outputfilename 最终的输出文件全路径文件名，该文件是gb18030编码，注意tmpfilename被转换为outputfilename后，tmpfilename文件被自动删除。\n");
    printf("logfilename 本程序的运行产生的日志文件名。\n");
    printf("charset 网页的字符集，如utf-8\n\n");
    exit(1);
  }

  // 关闭全部的信号和输入输出
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  CloseIOAndSignal(); signal(SIGINT,EXIT); signal(SIGTERM,EXIT);

  // 打开日志文件
  if (logfile.Open(argv[4],"a+") == false)
  {
    printf("logfile.Open(%s) failed.\n",argv[4]); return -1;
  }

  MKDIR(argv[2],true); MKDIR(argv[3],true);

  char strweburl[3001];
  memset(strweburl,0,sizeof(strweburl));

  strncpy(strweburl,argv[1],3000);
 
  char strcmd[3024];
  memset(strcmd,0,sizeof(strcmd));
  snprintf(strcmd,3000,"/usr/bin/wget -c -q -O %s \"%s\" 1>>/dev/null 2>>/dev/null",argv[2],strweburl);
  system(strcmd);
  logfile.Write("%s\n",strcmd);

  char strfilenametmp[301];
  memset(strfilenametmp,0,sizeof(strfilenametmp));
  snprintf(strfilenametmp,300,"%s.tmp",argv[3]);

   // 把获取到的网页转换为中文
  memset(strcmd,0,sizeof(strcmd));
  snprintf(strcmd,256,"iconv -c -f %s -t gb18030 %s -o %s",argv[5],argv[2],strfilenametmp);
  system(strcmd);
  logfile.Write("%s\n",strcmd);

  remove(argv[2]);   // 删除临时文件 

  RENAME(strfilenametmp,argv[3]);

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
