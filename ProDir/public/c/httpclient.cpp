/******* http客户端程序 httpclient.cpp ************/
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
*/
#include "_public.h"

CLogFile       logfile;

void EXIT(int sig);


//////////////////////////////httpclient.c 开始///////////////////////////////////////////

/********************************************
功能：搜索字符串右边起的第一个匹配字符
********************************************/
char * Rstrchr(char * s, char x)  
{
  int i = strlen(s);
  if(!(*s))  return 0;
  while(s[i-1]) if(strchr(s + (i - 1), x))  return (s + (i - 1));  else i--;
  return 0;
}

/********************************************
功能：把字符串转换为全小写
********************************************/
/*
void ToLowerCase(char * s)  
{
  while(*s)  *s=tolower(*s++);
}
*/

/**************************************************************
功能：从字符串src中分析出网站地址和端口，并得到用户要下载的文件
***************************************************************/
void GetHost(char * src, char * web, char * file, int * port)  {
  char * pA=0;
  char * pB=0;

  *port = 0;
  if(!(*src))  return;
  pA = src;
  if(!strncmp(pA, "http://", strlen("http://")))  pA = src+strlen("http://");
  else if(!strncmp(pA, "https://", strlen("https://")))  pA = src+strlen("https://");
  pB = strchr(pA, '/');
  if(pB)  {
    memcpy(web, pA, strlen(pA) - strlen(pB));
    if(pB+1)  {
      memcpy(file, pB + 1, strlen(pB) - 1);
      file[strlen(pB) - 1] = 0;
    }
  }
  else  memcpy(web, pA, strlen(pA));
  if(pB)  web[strlen(pA) - strlen(pB)] = 0;
  else  web[strlen(pA)] = 0;
  pA = strchr(web, ':');
  if(pA)  *port = atoi(pA + 1);
  else *port = 80;

  char *pC=0;
  pC=strchr(web,':');
  if (pC!=0) pC[0]=0;
}

/*********************************************************************
*filename: httpclient.c
*purpose: HTTP协议客户端程序，可以用来下载网页
*wrote by: zhoulifa(zhoulifa@163.com) 周立发(http://zhoulifa.bokee.com)
           Linux爱好者 Linux知识传播者 SOHO族 开发者 最擅长C语言
*date time:2006-03-11 21:49:00
*Note: 任何人可以任意复制代码并运用这些代码，当然包括你的商业用途
*                         但请遵循GPL
*********************************************************************/
int main(int argc, char *argv[])
{
  int sockfd=0;
  char buffer[2048];
  struct sockaddr_in server_addr;
  struct hostent *host=0;
  int portnumber=0;
  int nbytes=0;
  char host_addr[2048];
  char host_file[2048];
  char local_file[2048];
  FILE *fp=0;
  char request[2048];
  int send=0;
  int totalsend=0;
  int i=0;
  char *pt=0;

  memset(buffer,0,sizeof(buffer));
  memset(&server_addr,0,sizeof(server_addr));
  memset(host_addr,0,sizeof(host_addr));
  memset(host_file,0,sizeof(host_file));
  memset(request,0,sizeof(request));


  if(argc!=6)
  {
    printf("Usage:%s weburl tmpfilename outputfilename logfilename charset\n",argv[0]); 
    printf("本程序用于获取WEB网页的内容。\n");
    printf("weburl 网页WEB的地址。\n");
    printf("tmpfilename 获取到的网页的内容存放的临时文件，该文件可能是utf-8或其它编码。\n");
    printf("outputfilename 最终的输出文件，该文件是gb18030编码，注意tmpfilename被转换为outputfilename后，tmpfilename文件被自动删除。\n");
    printf("logfilename 本程序的运行产生的日志文件名。\n");
    printf("charset 网页的字符集，如utf-8\n\n");
    printf("建议弃用本程序，采用wgetclient程序更合适。\n");
    return -1;
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

  logfile.Write("parameter.1 is: %s\n", argv[1]);

  memset(host_addr,0,sizeof(host_addr));
  memset(host_file,0,sizeof(host_file));

  GetHost(argv[1], host_addr, host_file, &portnumber);/*分析网址、端口、文件名等*/

  logfile.Write("webhost:%s\n", host_addr);
  logfile.Write("hostfile:%s\n", host_file);
  logfile.Write("portnumber:%d\n\n", portnumber);

  /*取得主机IP地址*/
  if((host=gethostbyname(host_addr))==NULL)
  {
    fprintf(stderr,"Gethostname error, %s\n", strerror(errno)); return -1;
  }

  /* 客户程序开始建立 sockfd描述符 */
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*建立SOCKET连接*/
  {
    fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); return -1;
  }

  /* 客户程序填充服务端的资料 */
  bzero(&server_addr,sizeof(server_addr));
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(portnumber);
  server_addr.sin_addr=*((struct in_addr *)host->h_addr);

  /* 客户程序发起连接请求 */
  if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)/*连接网站*/
  {
    fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); close(sockfd); return -1;
  }

  sprintf(request, "GET /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n\
Host: %s:%d\r\nConnection: Close\r\n\r\n", host_file, host_addr, portnumber);

  logfile.Write("%s", request);/*准备request，将要发送给主机*/

  /*取得真实的文件名*/
  if(host_file && *host_file)  pt = Rstrchr(host_file, '/');
  else pt = 0;

  memset(local_file, 0, sizeof(local_file));
  if(pt && *pt)  {
    if((pt + 1) && *(pt+1))  strcpy(local_file, pt + 1);
    else  memcpy(local_file, host_file, strlen(host_file) - 1);
  }
  else if(host_file && *host_file)  strcpy(local_file, host_file);
  else  strcpy(local_file, "index.html");

  memset(local_file, 0, sizeof(local_file));
  strncpy(local_file,argv[2],255);
  
  logfile.Write("local filename to write:%s\n\n", local_file);

  /*发送http请求request*/
  send = 0;totalsend = 0;
  nbytes=strlen(request);
  while(totalsend < nbytes) {
    send = write(sockfd, request + totalsend, nbytes - totalsend);
    if(send==-1)  {printf("send error!%s\n", strerror(errno)); close(sockfd); return -1;}
    totalsend+=send;
    logfile.Write("%d bytes send OK!\n", totalsend);
  }

  fp = fopen(local_file, "w+");
  if(!fp)  {
    logfile.Write("create file error! %s\n", strerror(errno));
    return 0;
  }
  logfile.Write("\nThe following is the response header:\n");
  i=0;
  /* 连接成功了，接收http响应，response */
  while((nbytes=read(sockfd,buffer,1))==1)
  {
    if(i < 4)  {
      if(buffer[0] == '\r' || buffer[0] == '\n')  i++;
      else i = 0;
      // logfile.Write("%c", buffer[0]);/*把http头信息打印在屏幕上*/
    }
    else  {
      fwrite(buffer, 1, 1, fp);/*将http主体信息写入文件*/
      i++;
      if(i%1024 == 0)  fflush(fp);/*每1K时存盘一次*/
    }
  }
  fclose(fp);
  /* 结束通讯 */
  close(sockfd);

  char strfilenametmp[301];
  memset(strfilenametmp,0,sizeof(strfilenametmp));
  snprintf(strfilenametmp,300,"%s.tmp",argv[3]);

   // 把获取到的网页转换为中文
  char strcmd[1024];
  memset(strcmd,0,sizeof(strcmd));
  snprintf(strcmd,256,"iconv -c -f %s -t gb18030 %s -o %s",argv[5],local_file,strfilenametmp);
  system(strcmd);
  logfile.Write("%s\n",strcmd);

  // REMOVE(local_file); // 删除临时文件 

  RENAME(strfilenametmp,argv[3]);

  return 0;
}
//////////////////////////////httpclient.c 结束///////////////////////////////////////////


void EXIT(int sig)
{
  if (sig > 0) signal(sig,SIG_IGN);

  logfile.Write("catching the signal(%d).\n",sig);

  logfile.Write("httpclient exit.\n");

  exit(0);
}


/*
- 评论人：caihanfeng   2010-05-11 10:26:47   　  
还行。。。。

 
 - 评论人：chris_fox   2009-12-24 09:37:41   　  
不好意思 
修正： 
memset(web, 0, sizeof(web)); 
memset(file, 0, sizeof(file)); 
其实就是将web和file指向的缓冲区前面的4个字节设成0而已 
因为sizeof（指针） ＝ 4；

 
 - 评论人：chris_fox   2009-12-24 09:33:14   　  
BUG: 
memset(web, 0, sizeof(web)); 
memset(file, 0, sizeof(file)); 
这两行代码就是 
web ＝ NULL； 
file ＝ NULL； 
与作者的意图不符哦。。。

 
 - 评论人：   2009-12-08 10:16:59   　  
你的代码如太阳一般普照大地！考一份哦。

 
 - 评论人：绊命鸭子   2009-08-15 14:34:47   　  
int i = strlen(s); 
if(!(*s)) return 0; 
//这两句该对调一下位置吧

 
 - 评论人：luvi   2009-05-04 11:05:16   　  
非常感谢！正在找这样的学习资料呢，你给了我光明。

 
 - 评论人：周立发   2008-04-23 21:12:30   　  
Bug： 
上述代码中的：while(*s) *s=tolower(*s++); 
应该改为：while(s && *s) {*s=tolower(*s);s++;}

iconv -f utf-8 -t gb18030 szmb121>szmb121.txt 

./httpclient "http://172.22.1.175/di/http.action?userId=sqxt&pwd=shengqxt123&interfaceId=getStationInfo&dataFormat=xml2&type=PROVINCE&value=广东" /tmp/tmp.xml /tmp/out.xml /log/httpclient.log utf-8

*/
