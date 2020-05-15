/* by wucz */
#include "_public.h"

int main(int argc,char *argv[])
{
  char strPathName[201]; // 归档日志的路径
  UINT uSaveCount=0;     // 归档日志存在的个数

  if (argc != 3)
  {
    printf("\n");
    printf("Using:/htidc/htidc/bin/deletearchive pathname savecount\n\n");

    printf("Example:/htidc/htidc/bin/procctl 300 /htidc/htidc/bin/deletearchive /oracle/archive 20\n\n");

    printf("本程序读取目定pathname录下的文件信息，并按时间降序，只保留最近savecount个文件，其它的都删除掉。\n");
    printf("本程序主要用于删除oracle数据库的归档日志文件。\n");
    printf("本程序不写日志文件，也不会在屏幕上输出任何信息。\n");
    printf("本程序调用/bin/ls -lt pathname获取归档日志文件信息。\n");
    printf("本程序可以手工运行，也可以由procctl调度。\n\n\n");

    printf("启用oracle归档日志的相关命令如下：\n");
    printf("sqlplus /nolog\n");
    printf("connect / as sysdba;\n");
    printf("alter system set log_archive_dest_1='location=/home/oracle/oradata/EJETDB/archive';\n");
    printf("shutdown immediate;\n");
    printf("startup mount;\n");
    printf("alter database archivelog;\n");
    printf("alter database open;\n");
    printf("alter system switch logfile;\n\n\n");

    return -1;
  }

  memset(strPathName,0,sizeof(strPathName));

  strcpy(strPathName,argv[1]);
  uSaveCount=atoi(argv[2]);

  // 关闭全部的信号和输入输出
  // 设置信号,在shell状态下可用 "kill + 进程号" 正常终止些进程
  // 但请不要用 "kill -9 +进程号" 强行终止
  CloseIOAndSignal(); 

  FILE *fp=NULL;

  char strCmd[301];
  memset(strCmd,0,sizeof(strCmd));
 
  sprintf(strCmd,"/bin/ls -lt %s",strPathName);

  if ( (fp=popen(strCmd,"r")) == NULL ) // 读取文件的内容
  {
    printf("popen %s failed.\n",strCmd); return -1;
  }

  CCmdStr CmdStr;

  int  uFetchedCount=0;
  char strBuffer[1024],strFullFileName[201];

  while (1)
  {
    memset(strBuffer,0,sizeof(strBuffer));

    if (Fgets(strBuffer,2000,fp) == false) break; // 读取1行

    uFetchedCount++;

    if (uFetchedCount < uSaveCount + 1 ) continue; // 文件个数小于uSaveCount不删除

    Trim(strBuffer);
    UpdateStr(strBuffer,"  "," "); 

    CmdStr.SplitToCmd(strBuffer," ");

    memset(strBuffer,0,sizeof(strBuffer));
    CmdStr.GetValue(CmdStr.CmdCount()-1,strBuffer,200); // 读取最后一个分段内容, 即文件名

    memset(strFullFileName,0,sizeof(strFullFileName));
    snprintf(strFullFileName,200,"%s/%s",strPathName,strBuffer); // 得到全路径文件名

    remove(strFullFileName);
  }

  pclose(fp);

  return 0;
}
