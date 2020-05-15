/* 
 * 本程序用于数据迁移, 对历史数据的处理
 * 作者: 陈锟, 日期: 2020-03-27
 */
#include "_freecplus.h" 
#include "_ooci.h" 

char logfilename[301];
char connstr[101];
char srctname[51];
char dsttname[51];
char where[1024];
char hourstr[101];
char localhour[21];
int  maxcount = 1;

connection conn;
CLogFile logfile;

void EXIT(int sig);

// 获取dsttname表的全部的列
char strColumnStr[2048];
bool GetColumnStr();

// 显示程序的帮助
void _help(char *argv[]);

bool _hsmtables();

int main(int argc,char *argv[])
{
	if (argc != 2) { _help(argv); return -1; }
	
	memset(logfilename,0,sizeof(logfilename));
	memset(connstr,0,sizeof(connstr));
	memset(srctname,0,sizeof(srctname));
	memset(dsttname,0,sizeof(dsttname));
	memset(where,0,sizeof(where));
	memset(hourstr,0,sizeof(hourstr));
	
	GetXMLBuffer(argv[1],"logfilename",logfilename,300);
	GetXMLBuffer(argv[1],"connstr",connstr,100);
	GetXMLBuffer(argv[1],"srctname",srctname,50);
	GetXMLBuffer(argv[1],"dsttname",dsttname,50);
	GetXMLBuffer(argv[1],"where",where,1000);
	GetXMLBuffer(argv[1],"maxcount",&maxcount);
	GetXMLBuffer(argv[1],"hourstr",hourstr,2000);

	if (strlen(logfilename) == 0) { printf("logfilename is null.\n"); return -1; }
	if (strlen(connstr) == 0)     { printf("connstr is null.\n"); return -1; }
	if (strlen(srctname) == 0)    { printf("srctname is null.\n"); return -1; }
	if (strlen(dsttname) == 0)    { printf("dsttname is null.\n"); return -1; }
	if (strlen(where) == 0)       { printf("where is null.\n"); return -1; }
	if ( (maxcount<1) || (maxcount>1000) ) { printf("maxcount %d is invalid,should in 1-1000.\n",maxcount); return -1; }
	if (strlen(hourstr) == 0)     { printf("hourstr is null.\n"); return -1; }

	// 关闭全部的信号和输入输出
	CloseIOAndSignal();
	
	// 处理程序退出的信号
	signal(SIGINT,EXIT); signal(SIGTERM,EXIT);
	
	// 打开日志文件
	if (logfile.Open(logfilename,"a+") == false)
	{
	  printf("logfile.Open(%s) failed.\n",logfilename); return -1;
	}

	while (1)
	{
		// 判断当前时间是否在启动时间之内
		memset(localhour, 0, sizeof(localhour));
		LocalTime(localhour, "hh24");
		if (strstr(hourstr, localhour) == 0) { sleep(60);continue; }

		// 连接数据库
		if (conn.connecttodb(connstr, "SIMPLIFIED CHINESE_CHINA.AL32UTF8") != 0)
		{
			logfile.Write("connect database %s failed.\n", connstr); sleep(60); continue;
		}

		logfile.Write("from table %s to %s.\n", srctname, dsttname);

		if (_hsmtables() == false) logfile.Write("_hsmtables failed.\n");

		conn.disconnect();
		sleep(60);
	}

	return 0;
}

void EXIT(int sig)
{
	printf("程序退出，sig=%d\n\n",sig);
	exit(0);
}

// 显示程序的帮助
void _help(char *argv[])
{
	printf("\nUsing:/ProDir/public/bin/hsmtables \"<logfilename>/log/shqx/hsmtables_SURFDATA.log</logfilename><connstr>shqx/pwdidc@snorcl11g_31</connstr><srctname>T_SURFDATA</srctname><dsttname>T_SURFDATA_HIS</dsttname><where>where ddatetime<sysdate-2</where><maxcount>500</maxcount><hourstr>23,00,01,02,03,04,05,06</hourstr>\"\n\n");
	
	printf("这是一个工具程序，用于清理表中的数据。\n");
	printf("<logfilename>/log/shqx/hsmtables_SURFDATA.log</logfilename> 本程序运行日志文件名。\n");
	printf("<connstr>szidc/pwdidc@SZQX_10.153.97.251</connstr> 目的数据库的连接参数。\n");
	printf("<srctname>T_SURFDATA</srctname> 数据源表名。\n");
	printf("<dsttname>T_SURFDATA_HIS</dsttname> 目的数据表名。\n");
	printf("<where>where ddatetime<sysdate-10</where> 待迁移数据的条件。\n");
	printf("<maxcount>500</maxcount> 单次执行数据迁移的记录数，取值在1-1000之间。\n");
	printf("<hourstr>23,01,02,03,04,05,06</hourstr> 本程序启动的时次，小时，时次之间用半角的逗号分隔开。\n\n");
	
	return;
}

bool _hsmtables()
{
	// 获取dsttname表的全部的列
	if (GetColumnStr() == false) return false;

	int ccount = 0;
	char strrowid[51], strrowidn[maxcount][51];

	// 把数据的rowid从srctname表中查出来
	sqlstatement selstmt(&conn);
	selstmt.prepare("select rowid from %s %s",srctname,where);
	selstmt.bindout(1, strrowid,50);
	if (selstmt.execute() != 0)
	{
		logfile.Write("%s failed.\n%s\n",selstmt.m_sql,selstmt.m_cda.message); return false;
	}

	// 生成插入dsttname表和删除srctname表的SQL
	int ii=0;
	char strInsertSQL[10241],strDeleteSQL[10241];
	memset(strInsertSQL,0,sizeof(strInsertSQL));
	memset(strDeleteSQL,0,sizeof(strDeleteSQL));
	sprintf(strInsertSQL,"insert into %s(%s) select %s from %s where rowid in (",dsttname,strColumnStr,strColumnStr,srctname);
	sprintf(strDeleteSQL,"delete from %s where rowid in (",srctname);

	char strtemp[11];
	for (ii=0; ii<maxcount; ii++)
	{
		memset(strtemp,0,sizeof(strtemp));
		if (ii==0) sprintf(strtemp,":%d",ii+1);
		if (ii >0) sprintf(strtemp,",:%d",ii+1);
		strcat(strInsertSQL,strtemp);
		strcat(strDeleteSQL,strtemp);
	}
	strcat(strInsertSQL,")");
	strcat(strDeleteSQL,")");

	sqlstatement insstmt(&conn);
	insstmt.prepare(strInsertSQL);

	sqlstatement delstmt(&conn);
	delstmt.prepare(strDeleteSQL);

	for (ii=0; ii<maxcount; ii++)
	{
		insstmt.bindin(ii+1,strrowidn[ii],50);
		delstmt.bindin(ii+1,strrowidn[ii],50);
	}

	// 每maxcount记录就执行一次
	while (true)
	{
		memset(strrowid,0,sizeof(strrowid));
		
		if (selstmt.next() != 0) break;
		
		strcpy(strrowidn[ccount],strrowid);
		
		ccount++;
		
		if (ccount == maxcount)
		{
			if (insstmt.execute() != 0)
			{
				if (insstmt.m_cda.rc != 1)
				{
					logfile.Write("_hsmtables insert %s failed.\n%s\n",dsttname,insstmt.m_cda.message); return false;
				}
			}
			
			if (delstmt.execute() != 0)
			{
				logfile.Write("_hsmtables delete %s failed.\n%s\n",dsttname,insstmt.m_cda.message); return false;
			}
			
			conn.commit();
			
			memset(strrowidn,0,sizeof(strrowidn));
			
			ccount=0;
		}

		// 当影响记录数为10000的倍数时, 就记录一次, 并判断一次是否在启动时次之内 
		if (fmod(selstmt.m_cda.rpc,10000) < 1)
		{
			logfile.Write("%s to %s ok(%d).\n",srctname,dsttname,selstmt.m_cda.rpc);
		
			// 判断当前时间是否在启动时间之内
			memset(localhour,0,sizeof(localhour));
			LocalTime(localhour,"hh24");
			if (strstr(hourstr,localhour)==0) return true;
		}
	}
	
	// 在以上循环处理的时候，如果不足maxcount，就在这里处理
	for (ii=0; ii<ccount; ii++)
	{
		insstmt.prepare("\
		BEGIN\
	      insert into %s(%s) select %s from %s where rowid=:1;\
	      delete from %s where rowid=:2;\
	    END;",dsttname,strColumnStr,strColumnStr,srctname,srctname);
		insstmt.bindin(1,strrowidn[ii],50);
		insstmt.bindin(2,strrowidn[ii],50);
		if (insstmt.execute() != 0)
		{
			if (insstmt.m_cda.rc != 1)
			{
				logfile.Write("_hsmtables insert %s or delete %s failed.\n%s\n",dsttname,srctname,insstmt.m_cda.message); return false;
			}
		}
	}

	conn.commit();
	
	logfile.Write("%s to %s finish(%d).\n",srctname,dsttname,selstmt.m_cda.rpc);
	
	return true;
}

// 获取dsttname表的全部的列
bool GetColumnStr()
{
	memset(strColumnStr,0,sizeof(strColumnStr));
	
	char column_name[51];
	sqlstatement stmt(&conn);
	stmt.prepare("select lower(column_name) from USER_TAB_COLUMNS where table_name=upper('%s') order by column_id",dsttname);
	stmt.bindout(1,column_name,50);
	
	if (stmt.execute() != 0)
	{
		logfile.Write("%s failed.\n%s\n",stmt.m_sql,stmt.m_cda.message); return false;
	}
	
	while(true)
	{
		memset(column_name,0,sizeof(column_name));
		if (stmt.next()!=0) break;
		
		if (stmt.m_cda.rpc>1) strcat(strColumnStr,",");
		
		strcat(strColumnStr,column_name);
	}
	
	if (stmt.m_cda.rpc==0) { logfile.Write("表%s不存在。\n",dsttname); return false; }

	return true;
}
