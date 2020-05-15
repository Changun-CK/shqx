/*`
 *  本程序用于把数据表文件计算在插入表中 
 *  作者：赵先佑 日期：20200225
*/
#include "_public.h"
#include "_ooci.h"
#include "_shqx.h"
// 主程序的参数
struct st_arg
{
	char connstr[101];
	char charset[51];
	char tname[51];
	char cols[1001];
	char fieldname[1001];
	char fieldlen[501];
	char andstr_rain[501];
	int  timetvl;
} starg;
struct st_jisuan m_stjisuan;
struct st_rain m_strain;
CLogFile logfile;

connection conn;

// 本程序的业务流程主函数
bool _rain();
bool insertrain();

// 程序退出
void EXIT(int sig);

// 显示程序帮助
void Help(char *argv[]);

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer);

int main(int argc, char *argv[])
{
	if (argc != 3) { Help(argv); return -1; }

	// 关闭全部的信号
	CloseIOAndSignal();

	// 处理程序退出的信号
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	if (logfile.Open(argv[1], "a+") == false)
	{
		printf("打开日志文件(%s)失败.\n", argv[1]); return -1;
	}

	// 将main函数参数分解保存到starg结构体中
	if (xmltoarg(argv[2]) == false) return -1;

	// 连接数据库
	if (conn.connecttodb(starg.connstr, starg.charset) != 0)
	{
            logfile.Write("connect database %s failed.\n", starg.connstr); sleep(starg.timetvl); return -1;
	}

	// 本程序的业务流程主函数
	if (_rain() == false) { logfile.Write("import tables failed.\n"); return -1; }
	logfile.Write("import tables success.\n");

	conn.disconnect();

	return 0;
}

// 程序退出
void EXIT(int sig)
{
	logfile.Write("程序退出, sig = %d\n", sig);
	exit(0);
}



//我只是个查询雨量的函数
bool selectrain()
{
  // 生成查询滑动雨量的SQL语句
  int  valrf = 0;
  int  valrf2 = 0;
  char strsql[4096];
  char timetmp5[20];//第一个5分钟
  char timetmp10[20];//第一个5分钟
  char timetmp20[20];//第一个5分钟
  char timetmp30[20];//第一个5分钟
  char timetmp60[20];//第一个5分钟
  char timetmp120[20];//第一个5分钟
  char timetmp180[20];//第一个5分钟
  char timetmp360[20];//第一个5分钟
  char timetmp720[20];//第一个5分钟
  char timetmp1440[20];//第一个5分钟
  char timetmp2880[20];//第一个5分钟
  memset(strsql,0,sizeof(strsql));
  sprintf(strsql,"select %s from %s where 1=1 %s ",starg.cols,starg.tname,starg.andstr_rain);
  sqlstatement stmtsel(&conn);//查询当前雨量
  sqlstatement stmtsel_rain5(&conn);//5分钟之前雨量
  sqlstatement stmtsel_rain10(&conn);
  sqlstatement stmtsel_rain30(&conn);
  sqlstatement stmtsel_rain30_2(&conn);
  sqlstatement stmtsel_rain60(&conn);
  sqlstatement stmtsel_rain120(&conn);
  sqlstatement stmtsel_rain180(&conn);
  sqlstatement stmtsel_rain360(&conn);
  sqlstatement stmtsel_rain720(&conn);
  sqlstatement stmtsel_rain1440(&conn);
  sqlstatement stmtsel_rain2880(&conn);
  sqlstatement stmtsel_rain4320(&conn);
  stmtsel.prepare(strsql);
  //查询当前的雨量
  stmtsel.bindout(1,m_strain.obtid,10);
  stmtsel.bindout(2,m_strain.ddatetime,19);
  stmtsel.bindout(3,&m_strain.hourrf);
  if (stmtsel.execute() != 0)
    {
        logfile.Write("select %s failed.\n%s\n%s\n", starg.tname,stmtsel.m_cda.message,stmtsel.m_sql); return false;
    }
    while (1)
    {
        memset(&m_strain,0,sizeof(struct st_rain));
        memset(timetmp5,0,sizeof(timetmp5));
        memset(timetmp10, 0, sizeof(timetmp10));
        memset(timetmp20, 0, sizeof(timetmp20));
        memset(timetmp30, 0, sizeof(timetmp30));
        memset(timetmp60, 0, sizeof(timetmp60));
        memset(timetmp120, 0, sizeof(timetmp120));
        memset(timetmp180, 0, sizeof(timetmp180));
        memset(timetmp360, 0, sizeof(timetmp360));
        memset(timetmp720, 0, sizeof(timetmp720));
        memset(timetmp1440, 0, sizeof(timetmp1440));
        memset(timetmp2880, 0, sizeof(timetmp2880));
        if (stmtsel.next() != 0) break;
        AddTime(m_strain.ddatetime, timetmp5, 0 - 5 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp10, 0 - 10 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp20, 0 - 20 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp30, 0 - 30 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp60, 0 - 60 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp120, 0 - 120 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp180, 0 - 180 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp360, 0 - 360 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp720, 0 - 720 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp1440, 0 - 1440 * 60, "yyyymmddhh24miss");
        AddTime(m_strain.ddatetime, timetmp2880, 0 - 2880 * 60, "yyyymmddhh24miss");

        if (strcmp(timetmp5+10, "0000") == 0)
        {
            m_strain.r05m = m_strain.minr = m_strain.hourrf;
        }
        else
        {

	    // 查询前五分钟的雨量数据
            stmtsel_rain5.prepare("select hourrf from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
            stmtsel_rain5.bindin(1, m_strain.obtid, 10);
            stmtsel_rain5.bindin(2, timetmp5, 19);
            stmtsel_rain5.bindout(1, &valrf);
            if (stmtsel_rain5.execute() != 0)
            {
                logfile.Write("stmtsel_rain5.execute() failed.\n%s\n%s\n", stmtsel_rain5.m_sql, stmtsel_rain5.m_cda.message);
                return stmtsel_rain5.m_cda.rc;
            }
            valrf = 0; 
	    if (stmtsel_rain5.next() == 0) {m_strain.minr = m_strain.r05m = m_strain.hourrf - valrf;}//减去上一个5分钟的雨量
            else {m_strain.r05m = 0; m_strain.minr = 0; }
        }
            //十分钟的
            stmtsel_rain10.prepare("select r05m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");              
                stmtsel_rain10.bindin(1, m_strain.obtid, 10);
                stmtsel_rain10.bindin(2, timetmp5, 19);
                stmtsel_rain10.bindout(1, &valrf);
                if (stmtsel_rain10.execute() != 0)
                {
                        logfile.Write("stmtsel_rain10.execute() failed.\n%s\n%s\n", stmtsel_rain10.m_sql, stmtsel_rain10.m_cda.message);                       
                        return stmtsel_rain10.m_cda.rc;
                }       
                                
                valrf = 0;
                if (stmtsel_rain10.next() == 0)
                {   
                        m_strain.r10m = m_strain.r05m + valrf; 
                }       
                else m_strain.r10m = m_strain.r05m;
            //30分钟的
            stmtsel_rain30.prepare("select r10m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
            stmtsel_rain30.bindin(1, m_strain.obtid, 10);
            stmtsel_rain30.bindin(2, timetmp10, 19);
            stmtsel_rain30.bindout(1, &valrf);
            if (stmtsel_rain30.execute() != 0)
            {
              logfile.Write("stmtsel_rain30.execute() failed.\n%s\n%s\n", stmtsel_rain30.m_sql, stmtsel_rain30.m_cda.message);
              return stmtsel_rain30.m_cda.rc;
            }
              valrf = 0;
              if (stmtsel_rain30.next() != 0) valrf = 0;

                 stmtsel_rain30_2.prepare("select r10m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                 stmtsel_rain30_2.bindin(1, m_strain.obtid, 10);
                 stmtsel_rain30_2.bindin(2, timetmp20, 19);
                 stmtsel_rain30_2.bindout(1, &valrf2);
                 if (stmtsel_rain30_2.execute() != 0)
                 {
                     logfile.Write("stmtsel_rain30_2.execute() failed.\n%s\n%s\n", stmtsel_rain30_2.m_sql, stmtsel_rain30_2.m_cda.message);
                     return stmtsel_rain30_2.m_cda.rc;
                 }
                 valrf2 = 0;
                if (stmtsel_rain30_2.next() != 0) valrf2 = 0;

                m_strain.r30m = valrf + valrf2 + m_strain.r10m;
                //60分钟的
                stmtsel_rain60.prepare("select r30m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain60.bindin(1, m_strain.obtid, 10);
                stmtsel_rain60.bindin(2, timetmp30, 19);
                stmtsel_rain60.bindout(1, &valrf);
                if (stmtsel_rain60.execute() != 0)
                {
                    logfile.Write("stmtsel_rain60.execute() failed.\n%s\n%s\n", stmtsel_rain60.m_sql, stmtsel_rain60.m_cda.message);
                    return stmtsel_rain60.m_cda.rc;
                }
                valrf = 0;

                if (stmtsel_rain60.next() != 0) valrf = 0;
                m_strain.r01h = valrf + m_strain.r30m;
                //120分钟
                stmtsel_rain120.prepare("select r01h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain120.bindin(1, m_strain.obtid, 10);
                stmtsel_rain120.bindin(2, timetmp60, 19);
                stmtsel_rain120.bindout(1, &valrf);
                if (stmtsel_rain120.execute() != 0)
                {   
                    logfile.Write("stmtsel_rain120.execute() failed.\n%s\n%s\n", stmtsel_rain120.m_sql, stmtsel_rain120.m_cda.message);
                    return stmtsel_rain120.m_cda.rc;
                }
                valrf = 0;

                if (stmtsel_rain120.next() != 0) valrf = 0;
                m_strain.r02h = valrf + m_strain.r01h;  

                // 180分钟的
                stmtsel_rain180.prepare("select r02h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain180.bindin(1, m_strain.obtid, 10);
                stmtsel_rain180.bindin(2, timetmp60, 19);
                stmtsel_rain180.bindout(1, &valrf);
                if (stmtsel_rain180.execute() != 0)
                {
                    logfile.Write("stmtsel_rain180.execute() failed.\n%s\n%s\n", stmtsel_rain180.m_sql, stmtsel_rain180.m_cda.message);
                    return stmtsel_rain180.m_cda.rc;
                }
                valrf = 0;
                if (stmtsel_rain180.next() != 0) valrf = 0;
                m_strain.r03h = valrf + m_strain.r01h;  

                // 360分钟
                stmtsel_rain360.prepare("select r03h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain360.bindin(1, m_strain.obtid, 10);
                stmtsel_rain360.bindin(2, timetmp180, 19);
                stmtsel_rain360.bindout(1, &valrf);
                if (stmtsel_rain360.execute() != 0)
                {
                    logfile.Write("stmtsel_rain360.execute() failed.\n%s\n%s\n", stmtsel_rain360.m_sql, stmtsel_rain360.m_cda.message);
                    return stmtsel_rain360.m_cda.rc;
                }
                valrf = 0;

                if (stmtsel_rain360.next() != 0) valrf = 0;
                m_strain.r06h = valrf + m_strain.r03h;  

                stmtsel_rain720.prepare("select r06h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain720.bindin(1, m_strain.obtid, 10);
                stmtsel_rain720.bindin(2, timetmp360, 19);
                stmtsel_rain720.bindout(1, &valrf);
                if (stmtsel_rain720.execute() != 0)
                {
                    logfile.Write("stmtsel_rain720.execute() failed.\n%s\n%s\n", stmtsel_rain720.m_sql, stmtsel_rain720.m_cda.message);
                    return stmtsel_rain720.m_cda.rc;
                }
                valrf = 0;

                if (stmtsel_rain720.next() != 0) valrf = 0;
                m_strain.r12h = valrf + m_strain.r06h;  

                stmtsel_rain1440.prepare("select r12h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain1440.bindin(1, m_strain.obtid, 10);
                stmtsel_rain1440.bindin(2, timetmp720, 19);
                stmtsel_rain1440.bindout(1, &valrf);
                if (stmtsel_rain1440.execute() != 0)
                {
                    logfile.Write("stmtsel_rain1440.execute() failed.\n%s\n%s\n", stmtsel_rain1440.m_sql, stmtsel_rain1440.m_cda.message);
                    return stmtsel_rain1440.m_cda.rc;
                }
                valrf = 0;

                if (stmtsel_rain1440.next() != 0) valrf = 0;
                m_strain.r24h = valrf + m_strain.r12h;
                //2880分钟的
                stmtsel_rain2880.prepare("select r24h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain2880.bindin(1, m_strain.obtid, 10);
                stmtsel_rain2880.bindin(2, timetmp1440, 19);
                stmtsel_rain2880.bindout(1, &valrf);
                if (stmtsel_rain2880.execute() != 0)
                {
                    logfile.Write("stmtsel_rain2880.execute() failed.\n%s\n%s\n", stmtsel_rain2880.m_sql, stmtsel_rain2880.m_cda.message);
                    return stmtsel_rain2880.m_cda.rc;
                }
                valrf = 0;

                if (stmtsel_rain2880.next() != 0) valrf = 0;
                m_strain.r48h = valrf + m_strain.r24h;  

                stmtsel_rain4320.prepare("select r24h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
                stmtsel_rain4320.bindin(1, m_strain.obtid, 10);
                stmtsel_rain4320.bindin(2, timetmp2880, 19);
                stmtsel_rain4320.bindout(1, &valrf);
                if (stmtsel_rain4320.execute() != 0)
                {
                    logfile.Write("stmtsel_rain4320.execute() failed.\n%s\n%s\n", stmtsel_rain4320.m_sql, stmtsel_rain4320.m_cda.message);
                    return stmtsel_rain4320.m_cda.rc;
                }
                valrf = 0;
                if (stmtsel_rain4320.next() != 0) valrf = 0;
                m_strain.r72h = valrf + m_strain.r48h;  

                strcpy(m_strain.hdrbz, "1");
                if (insertrain() != 0)
                {
                   logfile.Write("插入失败了打印结构体的值ddatetime=%s,obtid=%s,hourrf=%s",m_strain.ddatetime,m_strain.obtid,m_strain.hourrf); 
                   return false; 
                }

    }

    return true;
}
/*
bool addrain()
{
/*
    int r10m;
    int r30m;
    int r01h;
    int r02h;
    int r03h;
    int r06h;
    int r12h;
    int r24h;
    int r48h;
    int r72h;
    int hourrf;

  //10分钟滑动雨量计算，又本次相加数据加上上一次数据 
 	 char timetmp2[20];//第二个前5分钟 即10分钟
 	 AddTime(m_strain.ddatetime, timetmp2, 0 - 10 * 60, "yyyymmddhh24miss");//往前查询我需要查询10分钟前的5分钟滑动雨量
  
 	 sqlstatement stmtrain5(&conn);//5分钟之前雨量
	    // 查询前五分钟的雨量数据
         stmtselrain.prepare("select hourrf from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
         stmtselrain.bindin(1, m_strain.obtid, 10);
         stmtselrain.bindin(2, timetmp5, 19);
	 stmtselrain.bindout(1, &valrf);
         if (stmtsel_rain.execute() != 0)
          {
              logfile.Write("stmtsel_rain.execute() failed.\n%s\n%s\n", stmtsel_rain.m_sql, stmtsel_rain.m_cda.message);
              return stmtsel_rain.m_cda.rc;
          }
              valrf = 0; stmtsel_rain.next();

              m_strain.minr = m_strain.r05m = m_strain.hourrf - valrf;//减去上一个5分钟的雨量
        }

  
}
*/
bool insertrain()
{
 
  //插入之前我需要都计算好

    sqlstatement stmtins(&conn);
    if (stmtins.m_state == 0)
    
        stmtins.connect(&conn);
        // 生成sql语句
        stmtins.prepare("insert into T_GDOBTRAIND (keyid,crttime,ddatetime, obtid,hourrf,minr,r05m,r10m,r30m,r01h,r02h,r03h,r06h,r12h,r24h,r48h,r72h,hdrbz) values (SEQUENCE_6.NEXTVAL,sysdate,to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,'1')");
        stmtins.bindin(1,  m_strain.ddatetime,19);
        stmtins.bindin(2,  m_strain.obtid, 10);
        stmtins.bindin(3, &m_strain.hourrf);
        stmtins.bindin(4, &m_strain.minr);
        stmtins.bindin(5, &m_strain.r05m);
        stmtins.bindin(6, &m_strain.r10m);
        stmtins.bindin(7, &m_strain.r30m);
        stmtins.bindin(8, &m_strain.r01h);
        stmtins.bindin(9, &m_strain.r02h);
        stmtins.bindin(10,&m_strain.r03h);
        stmtins.bindin(11,&m_strain.r06h);
        stmtins.bindin(12,&m_strain.r12h);
        stmtins.bindin(13,&m_strain.r24h);
        stmtins.bindin(14,&m_strain.r48h);
        stmtins.bindin(15,&m_strain.r72h);
	  
          
        /* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
        if (stmtins.execute() != 0)
        {
            logfile.Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
            return stmtins.m_cda.rc;
        }
	   return 0;

}
//本程序的主流程函数
bool _rain()
{
  //查询
        logfile.Write("程序启动");
        //以下是滑动雨量的查询在查询的时候调用了插入函数insertrain()
        if (selectrain() == false) { logfile.Write("查询滑动雨量失败"); return false;}
        //以下是滑动雨量的相加计算
        //if (addrain() == false) { logfile.Write("相加滑动雨量失败"); return false;}

         conn.commit();
    return true;
}



// 显示程序帮助
void Help(char *argv[])
{
	printf("\n本程序是数据计算的程序, 用于将计算后的数据入库到相应的表中.\n");
	printf("\nUsing: /oracle/htidc/public/bin/rain logfilename xmlbuffer\n");

	printf("\nSample: /oracle/htidc/shqx/bin/rain /oracle/log/shqx/rain.log \"<connstr>shqx/pwdidc@snorcl11g_128</connstr><charset>Simplified Chinese_China.ZHS16GBK</charset><tname>T_AREAOBTMIND</tname><cols>obtid, to_char(ddatetime, 'yyyymmddhh24miss'), hourrf </cols><fieldname>ddatetime, obtid, hourrf</fieldname><andstr_rain>and procsts = 1 order by obtid,ddatetime</andstr_rain>\"\n\n");

	printf("logfilename 程序运行的日志文件名.\n");
	printf("xmlbuffer 程序的传输方式, 如下:\n");
	printf("<connstr>shqx/pwdidc@snorcl11g_31</connstr> 数据库的连接参数.\n");
	printf("<charset>SIMPLIFIED CHINESE_CHINA.AL32UTF8</charset> 数据库的字符集, 这个参数要与数据源数据库保持一致, 否则会出现中文乱码.\n");
	printf("<tname>T_SURFDATA</tname> 待导出数据的表名.\n");
	printf("<cols>obtid, to_char(ddatetime, 'yyyymmddhh24miss'), t, p, u, wd, wf, r, vis</cols> 需要导出字段的列表, 可以采用函数.\n");
	printf("<fieldname>obtid, ddatetime, t, p, u, wd, wf, r, vis</fieldname> 导出字段的别名列表, 必须与cols一一对应.\n");
	printf("<andstr_rain> and procsts = 1</andstr_rain> 最新数据的附加条件, 注意, 关键字and不能缺.\n");
	printf("<timetvl>30</timetvl> 导出数据的时间间隔, 单位:秒, 建议大于10.\n\n");
	printf("以上参数，除了andstr_hour和andstr_new，其它字段都不允许为空.\n\n");
}

// 将main函数参数分解保存到starg结构体中
bool xmltoarg(char *strxmlbuffer)
{
	memset(&starg, 0, sizeof(struct st_arg));

	GetXMLBuffer(strxmlbuffer, "connstr", starg.connstr);
	if (strlen(starg.connstr) == 0) { logfile.Write("connstr is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "charset", starg.charset);
	if (strlen(starg.charset) == 0) { logfile.Write("charset is NULL.\n"); return false; }

	GetXMLBuffer(strxmlbuffer, "tname", starg.tname);
	if (strlen(starg.tname) == 0) { logfile.Write("tname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "cols", starg.cols);
	if (strlen(starg.cols) == 0) { logfile.Write("cols is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "fieldname", starg.fieldname);
	if (strlen(starg.fieldname) == 0) { logfile.Write("fieldname is NULL.\n");  return false; }

	GetXMLBuffer(strxmlbuffer, "andstr_rain", starg.andstr_rain);
	if (strlen(starg.andstr_rain) == 0) { logfile.Write("andstr_rain is NULL.\n");  return false; }



	return true;
}
