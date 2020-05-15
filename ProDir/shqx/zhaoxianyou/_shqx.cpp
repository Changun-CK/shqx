#include "_shqx.h"

int c;
//int i=0;//自编号代替值
CSURFDATA::CSURFDATA(connection *conn,CLogFile *logfile)
{
  initdata();

  m_conn=conn; m_logfile=logfile;
}

void CSURFDATA::initdata()
{
  totalcount=insertcount=updatecount=invalidcount=0;
  m_conn=0; m_logfile=0;
  memset(&m_stsurfdata,0,sizeof(struct st_surfdata));
}

CSURFDATA::~CSURFDATA()
{
}

// 把用逗号分隔的记录拆分到m_stsurfdata结构中。
bool CSURFDATA::SplitBuffer(const char *strBuffer)
{
  totalcount++;

  memset(&m_stsurfdata,0,sizeof(struct st_surfdata));

  CCmdStr CmdStr;

  CmdStr.SplitToCmd(strBuffer,",",true);
  if (CmdStr.CmdCount()!=9) { invalidcount++; return false; }

  CmdStr.GetValue(0,m_stsurfdata.obtid,5);      // 站点代码
  CmdStr.GetValue(1,m_stsurfdata.ddatetime,19); // 数据时间：格式yyyy-mm-dd hh:mi:ss。
  double dtmp=0;
  CmdStr.GetValue(2,&dtmp); m_stsurfdata.t=(int)(dtmp*10);  // 气温：单位，0.1摄氏度
  CmdStr.GetValue(3,&dtmp); m_stsurfdata.p=(int)(dtmp*10);  // 气压：0.1百帕
  CmdStr.GetValue(4,&m_stsurfdata.u);  // 相对湿度，0-100之间的值。
  CmdStr.GetValue(5,&m_stsurfdata.wd); // 风向，0-360之间的值。
  CmdStr.GetValue(6,&dtmp); m_stsurfdata.wf=(int)(dtmp*10);  // 风速：单位0.1m/s
  CmdStr.GetValue(7,&dtmp); m_stsurfdata.r=(int)(dtmp*10);   // 降雨量：0.1mm
  CmdStr.GetValue(8,&dtmp); m_stsurfdata.vis=(int)(dtmp*10); // 能见度：0.1米

  return true;
}

// 把xml格式的记录拆分到m_stsurfdata结构中。
bool CSURFDATA::SplitBuffer1(const char *strBuffer)
{
  totalcount++;

  memset(&m_stsurfdata,0,sizeof(struct st_surfdata));

  GetXMLBuffer(strBuffer,"obtid",m_stsurfdata.obtid,5);      // 站点代码
  GetXMLBuffer(strBuffer,"ddatetime",m_stsurfdata.ddatetime,19); // 数据时间：格式yyyy-mm-dd hh:mi:ss。
  double dtmp=0;
  GetXMLBuffer(strBuffer,"t",&dtmp); m_stsurfdata.t=(int)(dtmp*10);  // 气温：单位，0.1摄氏度
  GetXMLBuffer(strBuffer,"p",&dtmp); m_stsurfdata.p=(int)(dtmp*10);  // 气压：0.1百帕
  GetXMLBuffer(strBuffer,"u",&m_stsurfdata.u);  // 相对湿度，0-100之间的值。
  GetXMLBuffer(strBuffer,"wd",&m_stsurfdata.wd);  // 风向，0-360之间的值。
  GetXMLBuffer(strBuffer,"wf",&dtmp); m_stsurfdata.wf=(int)(dtmp*10);  // 风速：单位0.1m/s
  GetXMLBuffer(strBuffer,"r",&dtmp); m_stsurfdata.r=(int)(dtmp*10);   // 降雨量：0.1mm
  GetXMLBuffer(strBuffer,"vis",&dtmp);  m_stsurfdata.vis=(int)(dtmp*10); // 能见度：0.1米

  return true;
}

// 把m_stsurfdata结构中的值更新到T_SURFDATA表中。
long CSURFDATA::InsertTable()
{
  if (stmtsel.m_state==0)
  {
    stmtsel.connect(m_conn);
    stmtsel.prepare("select count(*) from T_SURFDATA where obtid=:1 and ddatetime=to_date(:2,'yyyy-mm-dd hh24:mi:ss')");
    stmtsel.bindin( 1, m_stsurfdata.obtid,5);
    stmtsel.bindin( 2, m_stsurfdata.ddatetime,19);
    stmtsel.bindout(1,&iccount);
  }

  if (stmtins.m_state==0)
  {
    stmtins.connect(m_conn);
    stmtins.prepare("insert into T_SURFDATA(obtid,ddatetime,t,p,u,wd,wf,r,vis,crttime,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,:4,:5,:6,:7,:8,:9,sysdate,SEQ_SURFDATA.nextval)");
    stmtins.bindin( 1, m_stsurfdata.obtid,5);
    stmtins.bindin( 2, m_stsurfdata.ddatetime,19);
    stmtins.bindin( 3,&m_stsurfdata.t);
    stmtins.bindin( 4,&m_stsurfdata.p);
    stmtins.bindin( 5,&m_stsurfdata.u);
    stmtins.bindin( 6,&m_stsurfdata.wd);
    stmtins.bindin( 7,&m_stsurfdata.wf);
    stmtins.bindin( 8,&m_stsurfdata.r);
    stmtins.bindin( 9,&m_stsurfdata.vis);
  }

  if (stmtupt.m_state==0)
  {
    stmtupt.connect(m_conn);
    stmtupt.prepare("update T_SURFDATA set t=:1,p=:2,u=:3,wd=:4,wf=:5,r=:6,vis=:7 where obtid=:8 and ddatetime=to_date(:2,'yyyy-mm-dd hh24:mi:ss')");
    stmtupt.bindin( 1,&m_stsurfdata.t);
    stmtupt.bindin( 2,&m_stsurfdata.p);
    stmtupt.bindin( 3,&m_stsurfdata.u);
    stmtupt.bindin( 4,&m_stsurfdata.wd);
    stmtupt.bindin( 5,&m_stsurfdata.wf);
    stmtupt.bindin( 6,&m_stsurfdata.r);
    stmtupt.bindin( 7,&m_stsurfdata.vis);
    stmtupt.bindin( 8, m_stsurfdata.obtid,5);
    stmtupt.bindin( 9, m_stsurfdata.ddatetime,19);
  }

  if (stmtsel.execute() != 0)
  {
    invalidcount++; 
    m_logfile->Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
    return stmtsel.m_cda.rc;
  }

  iccount=0;
  stmtsel.next();

  if (iccount>0) 
  {
    // 执行更新的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtupt.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);
      return stmtupt.m_cda.rc;
    }
    updatecount++;
  }
  else
  {
    // 执行插入的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtins.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      return stmtins.m_cda.rc;
    }
    insertcount++;
  }

  return 0;
}

CSIGNALLOG::CSIGNALLOG(connection *conn,CLogFile *logfile)
{
  initdata();

  m_conn=conn; m_logfile=logfile;
}

void CSIGNALLOG::initdata()
{
  totalcount=insertcount=updatecount=invalidcount=0;
  m_conn=0; m_logfile=0;
  memset(&m_stsignallog,0,sizeof(struct st_signallog));
  vsignallog.clear();
}

CSIGNALLOG::~CSIGNALLOG()
{
}

// 把记录拆分到vsignallog容器中。
bool CSIGNALLOG::SplitBuffer(const char *strBuffer)
{
  vsignallog.clear();
  memset(&m_stsignallog,0,sizeof(struct st_signallog));


CCmdStr CmdStr;
 
 CmdStr.SplitToCmd(strBuffer," ",true);
  if (CmdStr.CmdCount()<3) { invalidcount++; return false; }

  CmdStr.GetValue(0,m_stsignallog.ddatetime,12); // 数据时间：格式yyyymmddhh24mi
  strcat(m_stsignallog.ddatetime,"00");
  AddTime(m_stsignallog.ddatetime,m_stsignallog.ddatetime,8*60*60,"yyyy-mm-dd hh24:mi:ss");
  CmdStr.GetValue(1,m_stsignallog.obtid,4);      // 站点代码
  char strtemp[11];
  for (int ii=3;ii<=CmdStr.CmdCount();ii++)
  {
    memset(strtemp,0,sizeof(strtemp));
    CmdStr.GetValue(ii-1,strtemp,5);
    m_stsignallog.signalname[0]=strtemp[0];
    m_stsignallog.signalcolor[0]=strtemp[1];
    vsignallog.push_back(m_stsignallog);
    totalcount++;
  }

  return true;
}

// 把vsignallog容器中的值更新到T_SIGNALDATA表中。
long CSIGNALLOG::InsertTable()
{
  if (stmtsel.m_state==0)
  {
    stmtsel.connect(m_conn);
    stmtsel.prepare("select count(*) from T_SIGNALLOG where obtid=:1 and ddatetime=to_date(:2,'yyyy-mm-dd hh24:mi:ss') and signalname=:3");
    stmtsel.bindin( 1, m_stsignallog.obtid,4);
    stmtsel.bindin( 2, m_stsignallog.ddatetime,19);
    stmtsel.bindin( 3, m_stsignallog.signalname,1);
    stmtsel.bindout(1,&iccount);
  }

  if (stmtins.m_state==0)
  {
    stmtins.connect(m_conn);
    stmtins.prepare("insert into T_SIGNALLOG(obtid,ddatetime,signalname,signalcolor,crttime,keyid) values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,:4,sysdate,SEQ_SIGNALLOG.nextval)");
    stmtins.bindin( 1, m_stsignallog.obtid,4);
    stmtins.bindin( 2, m_stsignallog.ddatetime,19);
    stmtins.bindin( 3, m_stsignallog.signalname,1);
    stmtins.bindin( 4, m_stsignallog.signalcolor,1);
  }

  if (stmtupt.m_state==0)
  {
    stmtupt.connect(m_conn);
    stmtupt.prepare("update T_SIGNALLOG set signalcolor=:1 where obtid=:2 and ddatetime=to_date(:3,'yyyy-mm-dd hh24:mi:ss') and signalname=:4");
    stmtupt.bindin( 1, m_stsignallog.signalcolor,1);
    stmtupt.bindin( 2, m_stsignallog.obtid,4);
    stmtupt.bindin( 3, m_stsignallog.ddatetime,19);
    stmtupt.bindin( 4, m_stsignallog.signalname,1);
  }

  for (int ii=0;ii<vsignallog.size();ii++)
  {
    memcpy(&m_stsignallog,&vsignallog[ii],sizeof(struct st_signallog));
    m_logfile->Write("%s,%s,%s,%s\n",m_stsignallog.obtid,m_stsignallog.ddatetime,m_stsignallog.signalname,m_stsignallog.signalcolor);

    if (stmtsel.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
      return stmtsel.m_cda.rc;
    }
  
    iccount=0;
    stmtsel.next();
  
    if (iccount>0) 
    {
      // 执行更新的SQL语句，一定要判断返回值，0-成功，其它-失败。
      if (stmtupt.execute() != 0)
      {
        invalidcount++; 
        m_logfile->Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);
        return stmtupt.m_cda.rc;
      }
      updatecount++;
    }
    else
    {
      // 执行插入的SQL语句，一定要判断返回值，0-成功，其它-失败。
      if (stmtins.execute() != 0)
      {
        invalidcount++; 
        m_logfile->Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
        return stmtins.m_cda.rc;
      }
      insertcount++;
    }
  }

  return 0;
}



// 把非结构化数据文件写入oracle数据库的表中
int FileToTable(connection *in_conn,CLogFile *in_logfile,char *in_tname,char *in_filename,char *in_ddatetime)
{
  sqlstatement stmt(in_conn);

  // 判断文件记录在表中是否已存在
  int icount=0;
  stmt.prepare("select count(*) from %s where filename=:1",in_tname);
  stmt.bindin(1,in_filename,300);
  stmt.bindout(1,&icount);
  if (stmt.execute() != 0)
  {
    in_logfile->Write("FileToTable() failed.%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return stmt.m_cda.rc;
  }
  stmt.next();
  // 如果记录已存在，直接返回0-成功。
  if (icount>0) return 0;

  // 把文件信息插入表中。
  int ifilesize=FileSize(in_filename);
  stmt.prepare("\
       insert into %s(filename,ddatetime,filesize,filecontent,crttime,keyid)\
               values(:1,to_date(:2,'yyyymmddhh24miss'),:3,empty_blob(),sysdate,SEQ_%s.nextval)",\
       in_tname,in_tname+2);
  stmt.bindin(1,in_filename,300);
  stmt.bindin(2,in_ddatetime,14);
  stmt.bindin(3,&ifilesize);
  
  if (stmt.execute() != 0)
  {
    in_logfile->Write("FileToTable() failed.%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return stmt.m_cda.rc;
  }
    
  // 把文件内容更新到BLOB字段中。
  stmt.prepare("select filecontent from %s where filename=:1 for update",in_tname);
  stmt.bindin(1,in_filename,300);
  stmt.bindblob();

  // 执行SQL语句，一定要判断返回值，0-成功，其它-失败。
  if (stmt.execute() != 0)
  {
    in_logfile->Write("FileToTable() failed.%s\n%s\n",stmt.m_sql,stmt.m_cda.message); return stmt.m_cda.rc;
  }

  // 获取一条记录，一定要判断返回值，0-成功，1403-无记录，其它-失败。
  if (stmt.next() != 0) return -1;

  // 把磁盘文件pic_in.jpg的内容写入BLOB字段，一定要判断返回值，0-成功，其它-失败。
  if (stmt.filetolob((char *)in_filename) != 0)
  {
    in_logfile->Write("FileToTable() stmt.filetolob() failed.\n%s\n",stmt.m_cda.message); return -1;
  }

  // 提交事务
  in_conn->commit();

  return 0;
}




//以下是台风路径
//TYPTHON
CTYPTHON::CTYPTHON(connection *conn,CLogFile *logfile)
{
  initdata();

  m_conn=conn; m_logfile=logfile;
}

void CTYPTHON::initdata()
{
  totalcount=insertcount=updatecount=invalidcount=0;
  m_conn=0; m_logfile=0;
  memset(&m_sttypthon,0,sizeof(struct st_typthon));
  memset(&m_sttypthon1,0,sizeof(struct st_typthon1));
}

CTYPTHON::~CTYPTHON()
{
}

// 把用逗号分隔的记录拆分到m_sttypthon结构中。
bool CTYPTHON::SplitBuffer(const char *strBuffer)
{
  totalcount++;

//  memset(&m_sttypthon1,0,sizeof(struct st_typthon));

  CCmdStr CmdStr;
  CmdStr.SplitToCmd(strBuffer," ",true);

  c= CmdStr.CmdCount();
  if(CmdStr.CmdCount()==9)
  {
    memset(&m_sttypthon,0,sizeof(struct st_typthon));
    CmdStr.GetValue(3,m_sttypthon.cyclone,4); 
    CmdStr.GetValue(4,m_sttypthon.num,4); 
    CmdStr.GetValue(5,m_sttypthon.ws,1); 
    CmdStr.GetValue(6,m_sttypthon.hours,1); 
    CmdStr.GetValue(7,m_sttypthon.t_name,29); 
    CmdStr.GetValue(8,m_sttypthon.datasettime,19); 
    //i++;//读到新的台风的时候     自编号加一  
    //m_sttypthon1.t_no=i;  //读到新的台风的时候     自编号加一  

  }
  if (CmdStr.CmdCount()==6)
  {

    memset(&m_sttypthon1,0,sizeof(struct st_typthon1));
    CmdStr.GetValue(0,m_sttypthon1.ddatetime,10); // 数据时间：格式yyyymmddhh24
    strcat(m_sttypthon1.ddatetime,"0000");
    AddTime(m_sttypthon1.ddatetime,m_sttypthon1.ddatetime,8*60*60,"yyyy-mm-dd hh24:mi:ss");
    CmdStr.GetValue(1,m_sttypthon1.h_level,1);
    double dtmp=0;int ii=0;
    CmdStr.GetValue(2,&dtmp); m_sttypthon1.lat=dtmp; 
    CmdStr.GetValue(3,&dtmp); m_sttypthon1.lon=dtmp;  
    CmdStr.GetValue(4,m_sttypthon1.pres,1);
    CmdStr.GetValue(5,&ii); m_sttypthon1.wnd=ii;
    CmdStr.GetValue(5,&ii); m_sttypthon1.owd=ii;
    
  }
/*
  if (CmdStr.CmdCount()!=6 && CmdStr.CmdCount()!=9) 
 { invalidcount++; return false; }
*/
  if(CmdStr.CmdCount()==9) {invalidcount++;}
  return true;
}
//判断是不是同一个台风，根据2分钟平均风速*30*60  每小时小于20公里来判断


// 把m_sttypthon结构中的值更新到T_TYPTHON表中。
long CTYPTHON::InsertTable()
{
  if (stmtsel.m_state==0)
  {
    stmtsel.connect(m_conn);
    stmtsel.prepare("select count(*) from T_TYPTHON where cyclone=:1 and t_name=:2 and ddatetime=to_date(:3,'yyyy-mm-dd hh24:mi:ss')");
    stmtsel.bindin( 1, m_sttypthon.cyclone,4);
    stmtsel.bindin( 2, m_sttypthon.t_name,29);
    stmtsel.bindin( 3, m_sttypthon1.ddatetime,20);
    stmtsel.bindout(1,&iccount);
    //m_logfile->Write("\nt_name==%s\n,ddatetime==%s",m_sttypthon.t_name,m_sttypthon1.ddatetime); 
  }

  //先判断是不是读的具体数据，在执行插入即c的值是不是6

  if (stmtins.m_state==0)
  {
    stmtins.connect(m_conn);
    stmtins.prepare("insert into T_TYPTHON(cyclone,num,ws,hours,t_name,datasettime,ddatetime,h_level,lat,lon,pres,wnd,owd,crttime,keyid) values(:1,:2,:3,:4,:5,to_date(:6,'yyyy-mm-dd'),to_date(:7,'yyyy-mm-dd hh24:mi:ss'),:8,:9,:10,:11,:12,:13,sysdate,SEQ_TYPTHON.nextval)");
    stmtins.bindin( 1, m_sttypthon.cyclone,4);
    stmtins.bindin( 2, m_sttypthon.num,4);
    stmtins.bindin( 3, m_sttypthon.ws,1);
    stmtins.bindin( 4, m_sttypthon.hours,4);
    stmtins.bindin( 5, m_sttypthon.t_name,29);
    stmtins.bindin( 6, m_sttypthon.datasettime,20);
    stmtins.bindin( 7, m_sttypthon1.ddatetime,20);
    stmtins.bindin( 8, m_sttypthon1.h_level,1);
    stmtins.bindin( 9, &m_sttypthon1.lat);
    stmtins.bindin( 10, &m_sttypthon1.lon);
    stmtins.bindin( 11, m_sttypthon1.pres,1);
    stmtins.bindin( 12, &m_sttypthon1.wnd);
    stmtins.bindin( 13, &m_sttypthon1.owd);
    //m_logfile->Write(" 插入%s,%s,%s,%s,%s,%s,%s,%s,%lf,%lf,%s,%d,%d\n",m_sttypthon.cyclone,m_sttypthon.num,m_sttypthon.ws,m_sttypthon.hours,m_sttypthon.t_name,m_sttypthon.datasettime,m_sttypthon1.ddatetime,m_sttypthon1.h_level,m_sttypthon1.lat,m_sttypthon1.lon,m_sttypthon1.pres,m_sttypthon1.wnd,m_sttypthon1.owd);
  }
  if (stmtupt.m_state==0)
  {
    stmtupt.connect(m_conn);
    stmtupt.prepare("update T_TYPTHON set ddatetime=to_date(:1,'yyyy-mm-dd hh24:mi:ss'),h_level=:2,lat=:3,lon=:4,pres=:5,owd=:6,wnd=:7 where cyclone=:8 and t_name=:9 and ddatetime=to_date(:10,'yyyy-mm-dd hh24:mi:ss')");
    stmtupt.bindin( 1, m_sttypthon1.ddatetime,20);
    stmtupt.bindin( 2, m_sttypthon1.h_level,1);
    stmtupt.bindin( 3, &m_sttypthon1.lat);
    stmtupt.bindin( 4, &m_sttypthon1.lon);
    stmtupt.bindin( 5, m_sttypthon1.pres,1);
    stmtupt.bindin( 6, &m_sttypthon1.owd);
    stmtupt.bindin( 7, &m_sttypthon1.wnd);
    stmtupt.bindin( 8, m_sttypthon.cyclone,4);
    stmtupt.bindin( 9, m_sttypthon.t_name,29);
    stmtupt.bindin( 10, m_sttypthon1.ddatetime,20);
  }
    if (stmtsel.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
      //m_logfile->Write("%s,abcderf=%s",m_sttypthon.datasettime,m_sttypthon1.ddatetime);
      //m_logfile->Write("%s,==%s",m_sttypthon.t_name,m_sttypthon1.ddatetime); 
      return stmtsel.m_cda.rc;
    }
  
  iccount=0;
  stmtsel.next();
   // 必须是正确的数据才进行插入和查询
  if(c==6){
  if (iccount>0) 
  {
    // 执行更新的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtupt.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);
      return stmtupt.m_cda.rc;
    }
    updatecount++;
  }
  else
  {
    // 执行插入的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtins.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      return stmtins.m_cda.rc;
    }
    insertcount++;
  }
  }
  return 0;
}









//以下是全国精细化预报
//FORECAST
CFORECAST::CFORECAST(connection *conn,CLogFile *logfile)
{
  initdata();

  m_conn=conn; m_logfile=logfile;
}

void CFORECAST::initdata()
{
  totalcount=insertcount=updatecount=invalidcount=0;
  m_conn=0; m_logfile=0;
  memset(&m_stforecast,0,sizeof(struct st_forecast));
  memset(&m_stforecast1,0,sizeof(struct st_forecast1));
  memset(&m_stforecast2,0,sizeof(struct st_forecast2));
}

CFORECAST::~CFORECAST()
{
}

// 把用逗号分隔的记录拆分到m_stforecast结构中。
bool CFORECAST::SplitBuffer(const char *strBuffer)
{
  totalcount++;

  CCmdStr CmdStr;
  CmdStr.SplitToCmd(strBuffer," ",true);

  c = CmdStr.CmdCount();
  if(CmdStr.CmdCount()==2)
  {
    memset(&m_stforecast,0,sizeof(struct st_forecast));
    CmdStr.GetValue(0,m_stforecast.product,5);
    CmdStr.GetValue(1,m_stforecast.foretime,10);
    strcat(m_stforecast.foretime,"0000");
    AddTime(m_stforecast.foretime,m_stforecast.foretime,8*60*60,"yyyy-mm-dd hh24:mi:ss");
  }
  if(CmdStr.CmdCount()==6)
  {
    memset(&m_stforecast1,0,sizeof(struct st_forecast1));
    CmdStr.GetValue(0,m_stforecast1.obtid,5); 
  }
  if (CmdStr.CmdCount()==22)
  {
    memset(&m_stforecast2,0,sizeof(struct st_forecast2));
    CmdStr.GetValue(0,m_stforecast2.sx,3);
    CmdStr.GetValue(1,m_stforecast2.wd,5); 
    CmdStr.GetValue(2,m_stforecast2.csd,5); 
    CmdStr.GetValue(3,m_stforecast2.wind,5);
    CmdStr.GetValue(4,m_stforecast2.speed,5);
    CmdStr.GetValue(5,m_stforecast2.pressure,5);
    CmdStr.GetValue(6,m_stforecast2.p,5); 
    CmdStr.GetValue(7,m_stforecast2.cloud,5);
    CmdStr.GetValue(8,m_stforecast2.lcloud,5); 
    CmdStr.GetValue(9,m_stforecast2.weather,5);
    CmdStr.GetValue(10,m_stforecast2.vis,5);
    CmdStr.GetValue(11,m_stforecast2.ht,5);
    CmdStr.GetValue(12,m_stforecast2.lt,5);
    CmdStr.GetValue(13,m_stforecast2.maxsd,5);
    CmdStr.GetValue(14,m_stforecast2.minsd,5);
    CmdStr.GetValue(15,m_stforecast2.p2,5);
    CmdStr.GetValue(16,m_stforecast2.p1,5);
    CmdStr.GetValue(17,m_stforecast2.c2,5);
    CmdStr.GetValue(18,m_stforecast2.c1,5);
    CmdStr.GetValue(19,m_stforecast2.weather1,5); 
    CmdStr.GetValue(20,m_stforecast2.wind1,5); 
    CmdStr.GetValue(21,m_stforecast2.speed1,5);
  }

  return true;
}


// 把m_stforecast结构中的值更新到T_FORECAST表中。
long CFORECAST::InsertTable()
{
  if (stmtsel.m_state==0)
  {
    stmtsel.connect(m_conn);
    stmtsel.prepare("select count(*) from T_FORECAST where product=:1 and foretime=to_date(:2,'yyyy-mm-dd hh24:mi:ss') and obtid=:3 and sx=:4");
    stmtsel.bindin(1,m_stforecast.product,5);
    stmtsel.bindin(2,m_stforecast.foretime,19);
    stmtsel.bindin(3,m_stforecast1.obtid,5);
    stmtsel.bindin(4,m_stforecast2.sx,3);
    stmtsel.bindout(1,&iccount);
  }

  //先判断是不是读的具体数据，在执行插入即c的值是不是22

  if (stmtins.m_state==0)
  {
    stmtins.connect(m_conn);
    stmtins.prepare("insert into T_FORECAST values(:1,to_date(:2,'yyyy-mm-dd hh24:mi:ss'),:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,SEQ_FORECAST.nextval,sysdate)");
    stmtins.bindin( 1, m_stforecast.product,5);
    stmtins.bindin( 2, m_stforecast.foretime,19);
    stmtins.bindin( 3, m_stforecast1.obtid,5);
    stmtins.bindin( 4, m_stforecast2.sx,3);
    stmtins.bindin( 5, m_stforecast2.wd,5);
    stmtins.bindin( 6, m_stforecast2.csd,5);
    stmtins.bindin( 7, m_stforecast2.wind,5);
    stmtins.bindin( 8, m_stforecast2.speed,5);
    stmtins.bindin( 9, m_stforecast2.pressure,5);
    stmtins.bindin( 10,m_stforecast2.p,5);
    stmtins.bindin( 11,m_stforecast2.cloud,5);
    stmtins.bindin( 12,m_stforecast2.lcloud,5);
    stmtins.bindin( 13,m_stforecast2.weather,5);
    stmtins.bindin( 14,m_stforecast2.vis,5);
    stmtins.bindin( 15,m_stforecast2.ht,5);
    stmtins.bindin( 16,m_stforecast2.lt,5);
    stmtins.bindin( 17,m_stforecast2.maxsd,5);
    stmtins.bindin( 18,m_stforecast2.minsd,5);
    stmtins.bindin( 19,m_stforecast2.p2,5);
    stmtins.bindin( 20,m_stforecast2.p1,5);
    stmtins.bindin( 21,m_stforecast2.c2,5);
    stmtins.bindin( 22,m_stforecast2.c1,5);
    stmtins.bindin( 23,m_stforecast2.weather1,5);
    stmtins.bindin( 24,m_stforecast2.wind1,5);
    stmtins.bindin( 25,m_stforecast2.speed1,5);
  
  }
  if (stmtupt.m_state==0)
  {
    stmtupt.connect(m_conn);
    stmtupt.prepare("update T_FORECAST set sx:1,wd=:2,csd=:3,wind=:4,speed=:5 where product=:6 and foretime=to_date(:7,'yyyy-mm-dd hh24:mi:ss') and obtid=:8 and sx=:9");
    stmtupt.bindin( 1,m_stforecast2.sx,3);
    stmtupt.bindin( 2,m_stforecast2.wd,5);
    stmtupt.bindin( 3,m_stforecast2.csd,5);
    stmtupt.bindin( 4,m_stforecast2.wind,5);
    stmtupt.bindin( 5,m_stforecast2.speed,5);
    stmtupt.bindin( 6,m_stforecast.product,5);
    stmtupt.bindin( 7,m_stforecast.foretime,19);
    stmtupt.bindin( 8,m_stforecast1.obtid,5);
    stmtupt.bindin( 9,m_stforecast2.sx,3);
  }
    if (stmtsel.execute() != 0)
    {
      //m_logfile->Write("%s,%s",m_stforecast.foretime,m_stforecast2.sx);
      invalidcount++; 
      m_logfile->Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
      return stmtsel.m_cda.rc;
    }
  
  iccount=0;
  stmtsel.next();
   // 必须是正确的数据才进行插入和查询
  if (c!=22){invalidcount++;}
  if(c==22){
  if (iccount>0) 
  {
    // 执行更新的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtupt.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);
      return stmtupt.m_cda.rc;
    }
    updatecount++;
  }
  else
  {
    // 执行插入的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtins.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("\n%s,m_stforecast.foretime=%s,%s,%s,%lf,%s\n",m_stforecast.product,m_stforecast.foretime,m_stforecast1.obtid,m_stforecast2.sx,m_stforecast2.wd,m_stforecast2.csd);
      m_logfile->Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      return stmtins.m_cda.rc;
    }
    insertcount++;
  }
  }
  return 0;
}

//以下是酸雨观测数据
//CADID,stadid,st_adid
CADID::CADID(connection *conn,CLogFile *logfile)
{
  initdata();

  m_conn=conn; m_logfile=logfile;
}

void CADID::initdata()
{
  totalcount=insertcount=updatecount=invalidcount=0;
  m_conn=0; m_logfile=0;
  memset(&m_stadid,0,sizeof(struct st_adid));
  memset(&m_stadid1,0,sizeof(struct st_adid1));
}

CADID::~CADID()
{
}

// 把用逗号分隔的记录拆分到m_stadid结构中。
bool CADID::SplitBuffer(const char *strBuffer)
{
  totalcount++;

  CCmdStr CmdStr;
  CmdStr.SplitToCmd(strBuffer," ",true);

  c = CmdStr.CmdCount();
  if(CmdStr.CmdCount()==5)
  {
    memset(&m_stadid,0,sizeof(struct st_adid));
    CmdStr.GetValue(0,m_stadid.obtid,5); 
    CmdStr.GetValue(1,m_stadid.lat,5); 
    CmdStr.GetValue(2,m_stadid.lon,6); 
    CmdStr.GetValue(3,m_stadid.height,6);
    CmdStr.GetValue(4,m_stadid.mode,1);
  }
  if (CmdStr.CmdCount()==28)
  {
    memset(&m_stadid1,0,sizeof(struct st_adid1));
    CmdStr.GetValue(0,m_stadid1.adidate,16);
    AddTime(m_stadid1.adidate,m_stadid1.adidate,8*60*60,"yyyy-mm-dd hh24:mi:ss");
    CmdStr.GetValue(1,m_stadid1.astart,6); 
    CmdStr.GetValue(2,m_stadid1.aend,6); 
    CmdStr.GetValue(3,m_stadid1.r,5);
    CmdStr.GetValue(4,m_stadid1.t,3);
    CmdStr.GetValue(5,m_stadid1.ph1,4);
    CmdStr.GetValue(6,m_stadid1.ph2,4); 
    CmdStr.GetValue(7,m_stadid1.ph3,4);
    CmdStr.GetValue(8,m_stadid1.ph_av,4); 
    char strtemp[11];
    memset(strtemp,0,sizeof(strtemp));
    CmdStr.GetValue(22,strtemp,6);
    m_stadid1.w[0]=strtemp[0];
    m_stadid1.w[1]=strtemp[1];
    m_stadid1.w[2]=strtemp[2];
    m_stadid1.s[0]=strtemp[3];
    m_stadid1.s[1]=strtemp[4];
    m_stadid1.s[2]=strtemp[5];
    CmdStr.GetValue(26,m_stadid1.pheno,8);
  }

  return true;
}


// 把m_stadid结构中的值更新到T_ADID表中。
long CADID::InsertTable()
{
  if (stmtsel.m_state==0)
  {
    stmtsel.connect(m_conn);
    stmtsel.prepare("select count(*) from T_ADID where obtid=:1 and adidate=to_date(:2,'yyyy-mm-dd hh24:mi:ss')");
    stmtsel.bindin(1,m_stadid.obtid,5);
    stmtsel.bindin(2,m_stadid1.adidate,19);
    stmtsel.bindout(1,&iccount);
  }

  //先判断是不是读的具体数据，在执行插入即c的值是不是22

  if (stmtins.m_state==0)
  {
    stmtins.connect(m_conn);
    stmtins.prepare("insert into T_ADID values(:1,:2,:3,:4,:5,to_date(:6,'yyyy-mm-dd hh24:mi:ss'),:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,sysdate,SEQ_ADID.nextval)");
    stmtins.bindin( 1, m_stadid.obtid,5);
    stmtins.bindin( 2, m_stadid.lat,5);
    stmtins.bindin( 3, m_stadid.lon,6);
    stmtins.bindin( 4, m_stadid.height,6);
    stmtins.bindin( 5, m_stadid.mode,1);
    stmtins.bindin( 6, m_stadid1.adidate,19);
    stmtins.bindin( 7, m_stadid1.astart,6);
    stmtins.bindin( 8, m_stadid1.aend,6);
    stmtins.bindin( 9, m_stadid1.r,5);
    stmtins.bindin( 10,m_stadid1.t,3);
    stmtins.bindin( 11,m_stadid1.ph1,4);
    stmtins.bindin( 12,m_stadid1.ph2,4);
    stmtins.bindin( 13,m_stadid1.ph3,4);
    stmtins.bindin( 14,m_stadid1.ph_av,4);
    stmtins.bindin( 15,m_stadid1.w,3);
    stmtins.bindin( 16,m_stadid1.s,3);
    stmtins.bindin( 17,m_stadid1.pheno,8);
  
  }
  if (stmtupt.m_state==0)
  {
    stmtupt.connect(m_conn);
    stmtupt.prepare("update T_ADID set w=:1,pheno=:2 where obtid=:3 and adidate=to_date(:4,'yyyy-mm-dd hh24:mi:ss')");
    stmtupt.bindin(1,m_stadid1.w,3);
    stmtupt.bindin(2,m_stadid1.pheno,8);
    stmtupt.bindin(3,m_stadid.obtid,5);
    stmtupt.bindin(4,m_stadid1.adidate,19);
  }
    if (stmtsel.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
      return stmtsel.m_cda.rc;
    }
  
  iccount=0;
  stmtsel.next();
   // 必须是正确的数据才进行插入和查询
  if (c!=28){invalidcount++;}
  if(c==28){
  if (iccount>0) 
  {
    // 执行更新的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtupt.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);
      return stmtupt.m_cda.rc;
    }
    updatecount++;
  }
  else
  {
    // 执行插入的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtins.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      return stmtins.m_cda.rc;
    }
    insertcount++;
  }
  }
  return 0;
}

//以下是自动站能见度数据
//T_VISIBILITY st_visbility st_visbility1 m_stvis m_stvis1
CVISIBILITY::CVISIBILITY(connection *conn,CLogFile *logfile)
{
  initdata();

  m_conn=conn; m_logfile=logfile;
}

void CVISIBILITY::initdata()
{
  totalcount=insertcount=updatecount=invalidcount=0;
  m_conn=0; m_logfile=0;
  memset(&m_stvis,0,sizeof(struct st_visbility));
  memset(&m_stvis1,0,sizeof(struct st_visbility1));
}

CVISIBILITY::~CVISIBILITY()
{
}

// 把用逗号分隔的记录拆分到m_stvis结构中。
bool CVISIBILITY::SplitBuffer(const char *strBuffer)
{
  totalcount++;

  CCmdStr CmdStr;
  CmdStr.SplitToCmd(strBuffer," ",true);

  c = CmdStr.CmdCount();
  if(CmdStr.CmdCount()==3)
  {
    memset(&m_stvis,0,sizeof(struct st_visbility));
    char strtemp[101];
    memset(strtemp,0,sizeof(strtemp));
    CmdStr.GetValue(0,strtemp); 
    STRNCPY(m_stvis.obtid,5,strtemp,5);
    STRNCPY(m_stvis.time,19,strtemp+5,8);

  }
  if (CmdStr.CmdCount()==1)
   {
     memset(&m_stvis1,0,sizeof(struct st_visbility1));
     char strtemp[101];
     memset(strtemp,0,sizeof(strtemp));
     CmdStr.GetValue(0,strtemp); 
     STRNCPY(m_stvis1.vtime,4,strtemp,4);
     STRNCPY(m_stvis1.vseeing,6,strtemp+4,6);
     STRNCPY(m_stvis1.max_vseeing,6,strtemp+10,6);
     STRNCPY(m_stvis1.max_vseetime,4,strtemp+16,4);
     STRNCPY(m_stvis1.min_vseeing,6,strtemp+20,6);
     STRNCPY(m_stvis1.min_vseetime,4,strtemp+26,4);
     STRNCPY(m_stvis1.vseeing10,6,strtemp+30,6);
     STRNCPY(m_stvis1.max_vseeing10,6,strtemp+36,6);
     STRNCPY(m_stvis1.max_vseetime10,4,strtemp+42,6);
     STRNCPY(m_stvis1.min_vseeing10,6,strtemp+48,6);
     STRNCPY(m_stvis1.min_vseetime10,4,strtemp+54,4);
     STRNCPY(m_stvis1.qcc,10,strtemp+58,10);
   }
   return true;
}


// 把m_stvis结构中的值更新到T_VISIBILITY表中。
long CVISIBILITY::InsertTable()
{
  if (stmtsel.m_state==0)
  {
    stmtsel.connect(m_conn);
    stmtsel.prepare("select count(*) from T_VISIBILITY where obtid=:1 and time=to_date(:2,'yyyymmdd') and vtime=:3");
    stmtsel.bindin(1,m_stvis.obtid,5);
    stmtsel.bindin(2,m_stvis.time,19);
    stmtsel.bindin(3,m_stvis1.vtime,19);
    stmtsel.bindout(1,&iccount);
  }

  //先判断是不是读的具体数据，在执行插入即c的值是不是22

  if (stmtins.m_state==0)
  {
    stmtins.connect(m_conn);
    stmtins.prepare("insert into T_VISIBILITY  values(:1,to_date(:2,'yyyymmdd'),:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,sysdate,SEQ_VISIBILITY.nextval)");
    stmtins.bindin( 1, m_stvis.obtid,5);
    stmtins.bindin( 2, m_stvis.time,19);
    stmtins.bindin( 3, m_stvis1.vtime,4);
    stmtins.bindin( 4, m_stvis1.vseeing,6);
    stmtins.bindin( 5, m_stvis1.max_vseeing,6);
    stmtins.bindin( 6, m_stvis1.max_vseetime,4);
    stmtins.bindin( 7, m_stvis1.min_vseeing,6);
    stmtins.bindin( 8, m_stvis1.min_vseetime,4);
    stmtins.bindin( 9, m_stvis1.vseeing10,6);
    stmtins.bindin( 10, m_stvis1.max_vseeing10,6);
    stmtins.bindin( 11, m_stvis1.max_vseetime10,4);
    stmtins.bindin( 12, m_stvis1.min_vseeing10,6);
    stmtins.bindin( 13, m_stvis1.min_vseetime10,4);
    stmtins.bindin( 14, m_stvis1.qcc,10);
  
  }
  if (stmtupt.m_state==0)
  {
    stmtupt.connect(m_conn);
    stmtupt.prepare("update T_VISIBILITY set obtid=:1,vtime=:2 where obtid=:1 and vtime=:2");
    stmtupt.bindin(1,m_stvis.obtid,5);
    stmtupt.bindin(2,m_stvis1.vtime,19);
  }
    if (stmtsel.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtsel.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel.m_cda.message); 
      return stmtsel.m_cda.rc;
    }
  
  iccount=0;
  stmtsel.next();
   // 必须是正确的数据才进行插入和查询
  if (c!=1){invalidcount++;}
  if(c==1){
  if (iccount>0) 
  {
    // 执行更新的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtupt.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtupt.execute() failed.\n%s\n%s\n",stmtupt.m_sql,stmtupt.m_cda.message);
      return stmtupt.m_cda.rc;
    }
    updatecount++;
  }
  else
  {
    // 执行插入的SQL语句，一定要判断返回值，0-成功，其它-失败。
    if (stmtins.execute() != 0)
    {
      invalidcount++; 
      m_logfile->Write("stmtins.execute() failed.\n%s\n%s\n",stmtins.m_sql,stmtins.m_cda.message);
      return stmtins.m_cda.rc;
    }
    insertcount++;
  }
  }
  return 0;
}

/* Cjisuan类 */
Cjisuan::Cjisuan(connection *conn, CLogFile *logfile)
{
    initdata();
    m_conn = conn; m_logfile = logfile;
}

void Cjisuan::initdata()
{
    m_conn = 0; m_logfile = 0;
    memset(&m_stjisuan, 0, sizeof(struct st_jisuan));
    vjisuan.clear();
}

Cjisuan::~Cjisuan() {}

bool Cjisuan::selectforjs(const char *cols, const char *tname, const char *andstr)
{
    char strsql[4096];
    memset(strsql, 0, sizeof(strsql));
    sprintf(strsql, "select %s from %s where 1 = 1 %s order by ddatetime, obtid", cols, tname, andstr);

    sqlstatement stmtsel(m_conn);
    stmtsel.prepare(strsql);
    memset(&m_stjisuan,0,sizeof(struct st_jisuan));
    stmtsel.bindout(1,m_stjisuan.ddatetime,19);
    stmtsel.bindout(2,m_stjisuan.obtid,10);
    stmtsel.bindout(3,m_stjisuan.station,2);
    stmtsel.bindout(4,m_stjisuan.baoformat,1);
    stmtsel.bindout(5,m_stjisuan.bps,6);
    stmtsel.bindout(6,m_stjisuan.ys,8);
    stmtsel.bindout(7,m_stjisuan.w3a,6);
    stmtsel.bindout(8,m_stjisuan.wdidf,6);
    stmtsel.bindout(9,m_stjisuan.wdidd,6);
    stmtsel.bindout(10,m_stjisuan.wd2df,6);
    stmtsel.bindout(11,m_stjisuan.wd2dd,6);
    stmtsel.bindout(12,m_stjisuan.wd10df,6);
    stmtsel.bindout(13,m_stjisuan.wd10dd,6);
    stmtsel.bindout(14,m_stjisuan.wd3smaxdf,6);
    stmtsel.bindout(15,m_stjisuan.wd3smaxdd,6);
    stmtsel.bindout(16,m_stjisuan.wd3smaxtime,2);
    stmtsel.bindout(17,m_stjisuan.wd10maxdf,6);
    stmtsel.bindout(18,m_stjisuan.wd10maxdd,6);
    stmtsel.bindout(19,m_stjisuan.wd10maxtime,2);
    stmtsel.bindout(20,m_stjisuan.wd3daymax,6);
    stmtsel.bindout(21,m_stjisuan.wf3daymax,6);
    stmtsel.bindout(22,m_stjisuan.wd3daymaxtime,4);
    stmtsel.bindout(23,m_stjisuan.wd10daymax,6);
    stmtsel.bindout(24,m_stjisuan.wf10daymax,6);
    stmtsel.bindout(25,m_stjisuan.wd10daymaxtime,4);
    stmtsel.bindout(26,m_stjisuan.t,6);
    stmtsel.bindout(27,m_stjisuan.maxt,6);
    stmtsel.bindout(28,m_stjisuan.maxttime,2);
    stmtsel.bindout(29,m_stjisuan.mint,6);
    stmtsel.bindout(30,m_stjisuan.minttime,2);
    stmtsel.bindout(31,m_stjisuan.daymaxt,6);
    stmtsel.bindout(32,m_stjisuan.daymaxttime,4);
    stmtsel.bindout(33,m_stjisuan.daymint,6);
    stmtsel.bindout(34,m_stjisuan.dayminttime,4);
    stmtsel.bindout(35,m_stjisuan.rh,6);
    stmtsel.bindout(36,m_stjisuan.maxrh,6);
    stmtsel.bindout(37,m_stjisuan.maxrhtime,2);
    stmtsel.bindout(38,m_stjisuan.minrh,6);
    stmtsel.bindout(39,m_stjisuan.minrhtime,2);
    stmtsel.bindout(40,m_stjisuan.dp,6);
    stmtsel.bindout(41,m_stjisuan.p,6);
    stmtsel.bindout(42,m_stjisuan.maxp,6);
    stmtsel.bindout(43,m_stjisuan.maxptime,2);
    stmtsel.bindout(44,m_stjisuan.minp,6);
    stmtsel.bindout(45,m_stjisuan.minptime,2);
    stmtsel.bindout(46,m_stjisuan.p0,6);
    stmtsel.bindout(47,m_stjisuan.hourrf,6);
    stmtsel.bindout(48,m_stjisuan.dayrf,6);
    stmtsel.bindout(49,m_stjisuan.rfmark1,6);
    stmtsel.bindout(50,m_stjisuan.minrf,50);
    stmtsel.bindout(51,m_stjisuan.wpv,6);
    stmtsel.bindout(52,m_stjisuan.othfields,2000);
    stmtsel.bindout(53,m_stjisuan.rddatetime,2);
    stmtsel.bindout(54,m_stjisuan.procsts,1);

    if (stmtsel.execute() != 0)
    {
        m_logfile -> Write("select %s failed.\n%s\n%s\n", tname,stmtsel.m_cda.message,stmtsel.m_sql); return false;
    }

    while (1)
    {
        memset(&m_stjisuan,0,sizeof(struct st_jisuan));
        if (stmtsel.next() != 0) break;
        vjisuan.push_back(m_stjisuan); // 将查询到的数据绑定好结构体然后将结构体放入容器中
    }

    if (stmtsel.m_cda.rpc > 0) m_logfile -> Write("本次一共%d条记录。\n",stmtsel.m_cda.rpc);

    return true;
}
long Cjisuan::thour(const char *cols, const char *tname, const char*andstr)
{
    selectforjs(cols, tname, andstr);
    if (stmtins.m_state == 0)
    {
        stmtins.connect(m_conn);
        // 生成sql语句字符串
        stmtins.prepare("insert into T_GDOBTHOURD (keyid,crttime,ddatetime, obtid, station, baoformat, bps, ys, w3a, wdidf, wdidd, wd2df, wd2dd, wd10df, wd10dd, wd3smaxdf, wd3smaxdd, wd3smaxtime, wd10maxdf, wd10maxdd, wd10maxtime, wd3daymax, wf3daymax, wd3daymaxtime, wd10daymax, wf10daymax, wd10daymaxtime, t, maxt, maxttime, mint, minttime, daymaxt, daymaxttime, daymint, dayminttime, rh, maxrh, maxrhtime, minrh, minrhtime, dp, p, maxp, maxptime, minp, minptime, p0, hourrf, dayrf, rfmark1, minrf, wpv, othfields, rddatetime, procsts) values (SEQ_AREAOBTHOURD.nextval,sysdate,to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44,:45,:46,:47,:48,:49,:50,:51,:52,:53,:54)");
        stmtins.bindin(1,m_stjisuan.ddatetime,19);
        stmtins.bindin(2,m_stjisuan.obtid,10);
        stmtins.bindin(3,m_stjisuan.station,2);
        stmtins.bindin(4,m_stjisuan.baoformat,1);
        stmtins.bindin(5,m_stjisuan.bps,6);
        stmtins.bindin(6,m_stjisuan.ys,8);
        stmtins.bindin(7,m_stjisuan.w3a,6);
        stmtins.bindin(8,m_stjisuan.wdidf,6);
        stmtins.bindin(9,m_stjisuan.wdidd,6);
        stmtins.bindin(10,m_stjisuan.wd2df,6);
        stmtins.bindin(11,m_stjisuan.wd2dd,6);
        stmtins.bindin(12,m_stjisuan.wd10df,6);
        stmtins.bindin(13,m_stjisuan.wd10dd,6);
        stmtins.bindin(14,m_stjisuan.wd3smaxdf,6);
        stmtins.bindin(15,m_stjisuan.wd3smaxdd,6);
        stmtins.bindin(16,m_stjisuan.wd3smaxtime,2);
        stmtins.bindin(17,m_stjisuan.wd10maxdf,6);
        stmtins.bindin(18,m_stjisuan.wd10maxdd,6);
        stmtins.bindin(19,m_stjisuan.wd10maxtime,2);
        stmtins.bindin(20,m_stjisuan.wd3daymax,6);
        stmtins.bindin(21,m_stjisuan.wf3daymax,6);
        stmtins.bindin(22,m_stjisuan.wd3daymaxtime,4);
        stmtins.bindin(23,m_stjisuan.wd10daymax,6);
        stmtins.bindin(24,m_stjisuan.wf10daymax,6);
        stmtins.bindin(25,m_stjisuan.wd10daymaxtime,4);
        stmtins.bindin(26,m_stjisuan.t,6);
        stmtins.bindin(27,m_stjisuan.maxt,6);
        stmtins.bindin(28,m_stjisuan.maxttime,2);
        stmtins.bindin(29,m_stjisuan.mint,6);
        stmtins.bindin(30,m_stjisuan.minttime,2);
        stmtins.bindin(31,m_stjisuan.daymaxt,6);
        stmtins.bindin(32,m_stjisuan.daymaxttime,4);
        stmtins.bindin(33,m_stjisuan.daymint,6);
        stmtins.bindin(34,m_stjisuan.dayminttime,4);
        stmtins.bindin(35,m_stjisuan.rh,6);
        stmtins.bindin(36,m_stjisuan.maxrh,6);
        stmtins.bindin(37,m_stjisuan.maxrhtime,2);
        stmtins.bindin(38,m_stjisuan.minrh,6);
        stmtins.bindin(39,m_stjisuan.minrhtime,2);
        stmtins.bindin(40,m_stjisuan.dp,6);
        stmtins.bindin(41,m_stjisuan.p,6);
        stmtins.bindin(42,m_stjisuan.maxp,6);
        stmtins.bindin(43,m_stjisuan.maxptime,2);
        stmtins.bindin(44,m_stjisuan.minp,6);
        stmtins.bindin(45,m_stjisuan.minptime,2);
        stmtins.bindin(46,m_stjisuan.p0,6);
        stmtins.bindin(47,m_stjisuan.hourrf,6);
        stmtins.bindin(48,m_stjisuan.dayrf,6);
        stmtins.bindin(49,m_stjisuan.rfmark1,6);
        stmtins.bindin(50,m_stjisuan.minrf,50);
        stmtins.bindin(51,m_stjisuan.wpv,6);
        stmtins.bindin(52,m_stjisuan.othfields,2000);
        stmtins.bindin(53,m_stjisuan.rddatetime,2);
        stmtins.bindin(54,m_stjisuan.procsts,1);
    }

    for (int ii = 0; ii < vjisuan.size(); ii++)
    {
        memset(&m_stjisuan, 0, sizeof(struct st_jisuan));
        memcpy(&m_stjisuan, &vjisuan[ii], sizeof(struct st_jisuan));
        /* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
        if (stmtins.execute() != 0)
        {
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
            return stmtins.m_cda.rc;
        }
    }

    vjisuan.clear();

    return 0;
}
long Cjisuan::tnew(const char *cols, const char *tname, const char*andstr)
{
    selectforjs(cols, tname, andstr);

    if (stmtsel_n.m_state == 0)
    {
        stmtsel_n.connect(m_conn);
        stmtsel_n.prepare("select count(*) from T_GDOBTREALD where obtid = :1");
        stmtsel_n.bindin(1, m_stjisuan.obtid, 10);
        stmtsel_n.bindout(1, &iccount);
    }

    if (stmtins_n.m_state == 0)
    {
        stmtins_n.connect(m_conn);
        // 生成sql语句字符串
        stmtins_n.prepare("insert into T_GDOBTREALD (keyid,crttime,ddatetime, obtid, station, baoformat, bps, ys, w3a, wdidf, wdidd, wd2df, wd2dd, wd10df, wd10dd, wd3smaxdf, wd3smaxdd, wd3smaxtime, wd10maxdf, wd10maxdd, wd10maxtime, wd3daymax, wf3daymax, wd3daymaxtime, wd10daymax, wf10daymax, wd10daymaxtime, t, maxt, maxttime, mint, minttime, daymaxt, daymaxttime, daymint, dayminttime, rh, maxrh, maxrhtime, minrh, minrhtime, dp, p, maxp, maxptime, minp, minptime, p0, hourrf, dayrf, rfmark1, minrf, wpv, othfields, rddatetime, procsts) values (SEQ_AREAOBTHOURD.nextval,sysdate,to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44,:45,:46,:47,:48,:49,:50,:51,:52,:53,:54)");
        stmtins_n.bindin(1,m_stjisuan.ddatetime,19);
        stmtins_n.bindin(2,m_stjisuan.obtid,10);
        stmtins_n.bindin(3,m_stjisuan.station,2);
        stmtins_n.bindin(4,m_stjisuan.baoformat,1);
        stmtins_n.bindin(5,m_stjisuan.bps,6);
        stmtins_n.bindin(6,m_stjisuan.ys,8);
        stmtins_n.bindin(7,m_stjisuan.w3a,6);
        stmtins_n.bindin(8,m_stjisuan.wdidf,6);
        stmtins_n.bindin(9,m_stjisuan.wdidd,6);
        stmtins_n.bindin(10,m_stjisuan.wd2df,6);
        stmtins_n.bindin(11,m_stjisuan.wd2dd,6);
        stmtins_n.bindin(12,m_stjisuan.wd10df,6);
        stmtins_n.bindin(13,m_stjisuan.wd10dd,6);
        stmtins_n.bindin(14,m_stjisuan.wd3smaxdf,6);
        stmtins_n.bindin(15,m_stjisuan.wd3smaxdd,6);
        stmtins_n.bindin(16,m_stjisuan.wd3smaxtime,2);
        stmtins_n.bindin(17,m_stjisuan.wd10maxdf,6);
        stmtins_n.bindin(18,m_stjisuan.wd10maxdd,6);
        stmtins_n.bindin(19,m_stjisuan.wd10maxtime,2);
        stmtins_n.bindin(20,m_stjisuan.wd3daymax,6);
        stmtins_n.bindin(21,m_stjisuan.wf3daymax,6);
        stmtins_n.bindin(22,m_stjisuan.wd3daymaxtime,4);
        stmtins_n.bindin(23,m_stjisuan.wd10daymax,6);
        stmtins_n.bindin(24,m_stjisuan.wf10daymax,6);
        stmtins_n.bindin(25,m_stjisuan.wd10daymaxtime,4);
        stmtins_n.bindin(26,m_stjisuan.t,6);
        stmtins_n.bindin(27,m_stjisuan.maxt,6);
        stmtins_n.bindin(28,m_stjisuan.maxttime,2);
        stmtins_n.bindin(29,m_stjisuan.mint,6);
        stmtins_n.bindin(30,m_stjisuan.minttime,2);
        stmtins_n.bindin(31,m_stjisuan.daymaxt,6);
        stmtins_n.bindin(32,m_stjisuan.daymaxttime,4);
        stmtins_n.bindin(33,m_stjisuan.daymint,6);
        stmtins_n.bindin(34,m_stjisuan.dayminttime,4);
        stmtins_n.bindin(35,m_stjisuan.rh,6);
        stmtins_n.bindin(36,m_stjisuan.maxrh,6);
        stmtins_n.bindin(37,m_stjisuan.maxrhtime,2);
        stmtins_n.bindin(38,m_stjisuan.minrh,6);
        stmtins_n.bindin(39,m_stjisuan.minrhtime,2);
        stmtins_n.bindin(40,m_stjisuan.dp,6);
        stmtins_n.bindin(41,m_stjisuan.p,6);
        stmtins_n.bindin(42,m_stjisuan.maxp,6);
        stmtins_n.bindin(43,m_stjisuan.maxptime,2);
        stmtins_n.bindin(44,m_stjisuan.minp,6);
        stmtins_n.bindin(45,m_stjisuan.minptime,2);
        stmtins_n.bindin(46,m_stjisuan.p0,6);
        stmtins_n.bindin(47,m_stjisuan.hourrf,6);
        stmtins_n.bindin(48,m_stjisuan.dayrf,6);
        stmtins_n.bindin(49,m_stjisuan.rfmark1,6);
        stmtins_n.bindin(50,m_stjisuan.minrf,50);
        stmtins_n.bindin(51,m_stjisuan.wpv,6);
        stmtins_n.bindin(52,m_stjisuan.othfields,2000);
        stmtins_n.bindin(53,m_stjisuan.rddatetime,2);
        stmtins_n.bindin(54,m_stjisuan.procsts,1);
    }

    if (stmtupd_n.m_state == 0)
    {
        stmtupd_n.connect(m_conn);
        stmtupd_n.prepare("update T_GDOBTREALD set crttime=sysdate,ddatetime=to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),station=:2,baoformat=:3,bps=:4,ys=:5,w3a=:6,wdidf=:7,wdidd=:8,wd2df=:9,wd2dd=:10,wd10df=:11,wd10dd=:12,wd3smaxdf=:13,wd3smaxdd=:14,wd3smaxtime=:15,wd10maxdf=:16,wd10maxdd=:17,wd10maxtime=:18,wd3daymax=:19,wf3daymax=:20,wd3daymaxtime=:21,wd10daymax=:22,wf10daymax=:23,wd10daymaxtime=:24,t=:25,maxt=:26,maxttime=:27,mint=:28,minttime=:29,daymaxt=:30,daymaxttime=:31,daymint=:32,dayminttime=:33,rh=:34,maxrh=:35,maxrhtime=:36,minrh=:37,minrhtime=:38,dp=:39,p=:40,maxp=:41,maxptime=:42,minp=:43,minptime=:44,p0=:45,hourrf=:46,dayrf=:47,rfmark1=:48,minrf=:49,wpv=:50,othfields=:51,rddatetime=:52,procsts=:53 where obtid=:54 and ddatetime<to_date(:55, 'yyyy-mm-dd hh24:mi:ss')");
        stmtupd_n.bindin(1,m_stjisuan.ddatetime,19);
        stmtupd_n.bindin(2,m_stjisuan.station,2);
        stmtupd_n.bindin(3,m_stjisuan.baoformat,1);
        stmtupd_n.bindin(4,m_stjisuan.bps,6);
        stmtupd_n.bindin(5,m_stjisuan.ys,8);
        stmtupd_n.bindin(6,m_stjisuan.w3a,6);
        stmtupd_n.bindin(7,m_stjisuan.wdidf,6);
        stmtupd_n.bindin(8,m_stjisuan.wdidd,6);
        stmtupd_n.bindin(9,m_stjisuan.wd2df,6);
        stmtupd_n.bindin(10,m_stjisuan.wd2dd,6);
        stmtupd_n.bindin(11,m_stjisuan.wd10df,6);
        stmtupd_n.bindin(12,m_stjisuan.wd10dd,6);
        stmtupd_n.bindin(13,m_stjisuan.wd3smaxdf,6);
        stmtupd_n.bindin(14,m_stjisuan.wd3smaxdd,6);
        stmtupd_n.bindin(15,m_stjisuan.wd3smaxtime,2);
        stmtupd_n.bindin(16,m_stjisuan.wd10maxdf,6);
        stmtupd_n.bindin(17,m_stjisuan.wd10maxdd,6);
        stmtupd_n.bindin(18,m_stjisuan.wd10maxtime,2);
        stmtupd_n.bindin(19,m_stjisuan.wd3daymax,6);
        stmtupd_n.bindin(20,m_stjisuan.wf3daymax,6);
        stmtupd_n.bindin(21,m_stjisuan.wd3daymaxtime,4);
        stmtupd_n.bindin(22,m_stjisuan.wd10daymax,6);
        stmtupd_n.bindin(23,m_stjisuan.wf10daymax,6);
        stmtupd_n.bindin(24,m_stjisuan.wd10daymaxtime,4);
        stmtupd_n.bindin(25,m_stjisuan.t,6);
        stmtupd_n.bindin(26,m_stjisuan.maxt,6);
        stmtupd_n.bindin(27,m_stjisuan.maxttime,2);
        stmtupd_n.bindin(28,m_stjisuan.mint,6);
        stmtupd_n.bindin(29,m_stjisuan.minttime,2);
        stmtupd_n.bindin(30,m_stjisuan.daymaxt,6);
        stmtupd_n.bindin(31,m_stjisuan.daymaxttime,4);
        stmtupd_n.bindin(32,m_stjisuan.daymint,6);
        stmtupd_n.bindin(33,m_stjisuan.dayminttime,4);
        stmtupd_n.bindin(34,m_stjisuan.rh,6);
        stmtupd_n.bindin(35,m_stjisuan.maxrh,6);
        stmtupd_n.bindin(36,m_stjisuan.maxrhtime,2);
        stmtupd_n.bindin(37,m_stjisuan.minrh,6);
        stmtupd_n.bindin(38,m_stjisuan.minrhtime,2);
        stmtupd_n.bindin(39,m_stjisuan.dp,6);
        stmtupd_n.bindin(40,m_stjisuan.p,6);
        stmtupd_n.bindin(41,m_stjisuan.maxp,6);
        stmtupd_n.bindin(42,m_stjisuan.maxptime,2);
        stmtupd_n.bindin(43,m_stjisuan.minp,6);
        stmtupd_n.bindin(44,m_stjisuan.minptime,2);
        stmtupd_n.bindin(45,m_stjisuan.p0,6);
        stmtupd_n.bindin(46,m_stjisuan.hourrf,6);
        stmtupd_n.bindin(47,m_stjisuan.dayrf,6);
        stmtupd_n.bindin(48,m_stjisuan.rfmark1,6);
        stmtupd_n.bindin(49,m_stjisuan.minrf,50);
        stmtupd_n.bindin(50,m_stjisuan.wpv,6);
        stmtupd_n.bindin(51,m_stjisuan.othfields,2000);
        stmtupd_n.bindin(52,m_stjisuan.rddatetime,2);
        stmtupd_n.bindin(53,m_stjisuan.procsts,1);
        stmtupd_n.bindin(54,m_stjisuan.obtid,10);
        stmtupd_n.bindin(55,m_stjisuan.ddatetime,19);
    }

    for (int ii = 0; ii < vjisuan.size(); ii++)
    {
        memset(&m_stjisuan, 0, sizeof(struct st_jisuan));
        memcpy(&m_stjisuan, &vjisuan[ii], sizeof(struct st_jisuan));


        if (stmtsel_n.execute() != 0)
        {
            m_logfile->Write("stmtins_n.execute() failed.\n%s\n%s\n",stmtsel.m_sql,stmtsel_n.m_cda.message);
            return stmtsel_n.m_cda.rc;
        }

        iccount = 0;
        stmtsel_n.next();

        if (iccount > 0)
        {
            /* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
            if (stmtupd_n.execute() != 0)
            {
                m_logfile -> Write("stmtupd_n.execute() failed.\n%s\n%s\n", stmtupd_n.m_sql, stmtupd_n.m_cda.message);
                return stmtupd_n.m_cda.rc;
            }
        }
        else
        {
            /* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
            if (stmtins_n.execute() != 0)
            {
                m_logfile -> Write("stmtins_n.execute() failed.\n%s\n%s\n", stmtins_n.m_sql, stmtins_n.m_cda.message);
                return stmtins_n.m_cda.rc;
            }
        }
    }

    vjisuan.clear();

    return 0;
}
//日数据表的查询
//日数据表  本程序08:00执行一次插入   20:00的时候程序自动调到下一天，直到小于最后一天
long Cjisuan::tday(const char *cols, const char *tname, const char*andstr)
{
    selectforjs(cols, tname, andstr);//将整点数据表查询完插入容器中
    char strsql[4096];   
    char timetmp[20];   
    char timetmp1[20];   
    char time_02[20]="2019-5-26 02:00:00";   
    char time_08[20]="2019-5-26 08:00:00";    
    char time_14[20]="2019-5-26 14:00:00"; 
    char time_20[20]="2019-5-26 20:00:00"; 
    char time_start[20]="2019-5-25 20:00:00";   
    char time_end[20]="2019-5-26 20:00:00";   
    int r12h = 0;
    int maxttime = 0;
    //数据时间为2019-5-25 20:00:00 到 2019-6-5 20:00:00  
    memset(strsql,0,sizeof(strsql));
    memset(time_02,0,sizeof(time_02));
    memset(time_08,0,sizeof(time_08));
    memset(time_14,0,sizeof(time_14));
    memset(time_20,0,sizeof(time_20));
    memset(timetmp,0,sizeof(timetmp));
    memset(timetmp1,0,sizeof(timetmp1));
    stmtsel_rain.connect(m_conn);
    stmtsel_rain1.connect(m_conn);
    stmtsel_rain2.connect(m_conn);
    stmtsel_rain3.connect(m_conn);
    stmtsel_maxt.connect(m_conn);
    stmtsel_mint.connect(m_conn);
    stmtsel_avgt.connect(m_conn);
    stmtsel_maxrh.connect(m_conn);
    stmtsel_minrh.connect(m_conn);
    stmtsel_avgrh.connect(m_conn);
    stmtsel_maxp0.connect(m_conn);
    stmtsel_minp0.connect(m_conn);
    stmtsel_avgp0.connect(m_conn);
    stmtsel_maxp.connect(m_conn);
    stmtsel_minp.connect(m_conn);
    stmtsel_avgp.connect(m_conn);
    stmtsel_wd2max.connect(m_conn);
    stmtsel_wd2avg.connect(m_conn);
    stmtsel_wd10max.connect(m_conn);
    stmtsel_wd10avg.connect(m_conn);
    stmtsel_mincount.connect(m_conn);
    stmtsel_hourcount.connect(m_conn);
    
    
      sprintf(strsql,"select r12h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");//查询20-08时雨量
      stmtsel_rain.prepare(strsql);
      stmtsel_rain.bindin(1, m_stjisuan.obtid,10);
      stmtsel_rain.bindin(2, timetmp,19);
      stmtsel_rain.bindout(1,&r12h);

      stmtsel_rain1.prepare("select r12h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");//查询08-20时雨量
      stmtsel_rain1.bindin(1, m_stjisuan.obtid,10);
      stmtsel_rain1.bindin(2, timetmp1,19);
      stmtsel_rain1.bindout(1,&r12h);
      //查询一天数据中10分钟雨量最大值和时间
      stmtsel_rain2.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), r10m from T_GDOBTRAIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by r10m desc,ddatetime");
      stmtsel_rain2.bindin(1, m_stjisuan.obtid,10);
      stmtsel_rain2.bindin(2, time_start,19);
      stmtsel_rain2.bindin(3, time_end,19);
      stmtsel_rain2.bindout(1, m_day.maxr10mtime,19);
      stmtsel_rain2.bindout(2, &m_day.maxr10m);
      //查询一天数据1小时滑动雨量最大值和时间
      stmtsel_rain3.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), r01h from T_GDOBTRAIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by r01h desc,ddatetime");
      stmtsel_rain3.bindin(1, m_stjisuan.obtid,10);
      stmtsel_rain3.bindin(2, time_start,19);
      stmtsel_rain3.bindin(3, time_end,19);
      stmtsel_rain3.bindout(1, m_day.maxr01htime,19);
      stmtsel_rain3.bindout(2, &m_day.maxr01h);
      //最高气温和最高气温时间把最大值的分钟的两位数替换到ddatetime中的分钟数
      stmtsel_maxt.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), maxt, maxttime from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by maxt desc,ddatetime");
      stmtsel_maxt.bindin(1, m_stjisuan.obtid,10);
      stmtsel_maxt.bindin(2, time_start,19);
      stmtsel_maxt.bindin(3, time_end,19);
      stmtsel_maxt.bindout(1, m_day.maxttime,19);
      stmtsel_maxt.bindout(2, &m_day.maxt);
      stmtsel_maxt.bindout(3,&maxttime);
      //最低气温和最低气温时间
      stmtsel_mint.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), mint from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by mint desc,ddatetime");
      stmtsel_mint.bindin(1, m_stjisuan.obtid,10);
      stmtsel_mint.bindin(2, time_start,19);
      stmtsel_mint.bindin(3, time_end,19);
      stmtsel_mint.bindout(1, m_day.minttime,19);
      stmtsel_mint.bindout(2, &m_day.mint);
      //查询平均温度
      stmtsel_avgt.prepare("select ceil(avg(maxt)) from T_AREAOBTMIND where ddatetime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') and obtid = :2 or ddatetime = to_date(:3, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4 or ddatetime = to_date(:5, 'yyyy-mm-dd hh24:mi:ss') and obtid = :6 or ddatetime = to_date(:7, 'yyyy-mm-dd hh24:mi:ss') and obtid = :8");
      stmtsel_avgt.bindin(1, time_02,19);
      stmtsel_avgt.bindin(2, m_stjisuan.obtid,10);
      stmtsel_avgt.bindin(3, time_08,19);
      stmtsel_avgt.bindin(4, m_stjisuan.obtid,10);
      stmtsel_avgt.bindin(5, time_14,19);
      stmtsel_avgt.bindin(6, m_stjisuan.obtid,10);
      stmtsel_avgt.bindin(7, time_20,19);
      stmtsel_avgt.bindin(8, m_stjisuan.obtid,10);
      stmtsel_avgt.bindout(1, &m_day.avgt);

      //最大湿度和最大湿度时间
      stmtsel_maxrh.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), rh from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by rh desc,ddatetime");
      stmtsel_maxrh.bindin(1, m_stjisuan.obtid,10);
      stmtsel_maxrh.bindin(2, time_start,19);
      stmtsel_maxrh.bindin(3, time_end,19);
      stmtsel_maxrh.bindout(1, m_day.maxrhtime,19);
      stmtsel_maxrh.bindout(2, &m_day.maxrh);
      //最小湿度和最小湿度时间
      stmtsel_minrh.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), minrh from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by minrh desc,ddatetime");
      stmtsel_minrh.bindin(1, m_stjisuan.obtid,10);
      stmtsel_minrh.bindin(2, time_start,19);
      stmtsel_minrh.bindin(3, time_end,19);
      stmtsel_minrh.bindout(1, m_day.minrhtime,19);
      stmtsel_minrh.bindout(2, &m_day.minrh);
      //查询平均湿度
      stmtsel_avgrh.prepare("select ceil(avg(rh)) from T_AREAOBTMIND where ddatetime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') and obtid = :2 or ddatetime = to_date(:3, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4 or ddatetime = to_date(:5, 'yyyy-mm-dd hh24:mi:ss') and obtid = :6 or ddatetime = to_date(:7, 'yyyy-mm-dd hh24:mi:ss') and obtid = :8");
      stmtsel_avgrh.bindin(1, time_02,19);
      stmtsel_avgrh.bindin(2, m_stjisuan.obtid,10);
      stmtsel_avgrh.bindin(3, time_08,19);
      stmtsel_avgrh.bindin(4, m_stjisuan.obtid,10);
      stmtsel_avgrh.bindin(5, time_14,19);
      stmtsel_avgrh.bindin(6, m_stjisuan.obtid,10);
      stmtsel_avgrh.bindin(7, time_20,19);
      stmtsel_avgrh.bindin(8, m_stjisuan.obtid,10);
      stmtsel_avgrh.bindout(1, &m_day.avgrh);
      //最高海面气压和最高海面气压时间
      stmtsel_maxp0.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), p0 from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by p0 desc,ddatetime");
      stmtsel_maxp0.bindin(1, m_stjisuan.obtid,10);
      stmtsel_maxp0.bindin(2, time_start,19);
      stmtsel_maxp0.bindin(3, time_end,19);
      stmtsel_maxp0.bindout(1, m_day.maxp0time,19);
      stmtsel_maxp0.bindout(2, &m_day.maxp0);
      //最低海面气压和最低海面气压时间
      stmtsel_minp0.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), p0 from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by p0 asc,ddatetime");
      stmtsel_minp0.bindin(1, m_stjisuan.obtid,10);
      stmtsel_minp0.bindin(2, time_start,19);
      stmtsel_minp0.bindin(3, time_end,19);
      stmtsel_minp0.bindout(1, m_day.minp0time,19);
      stmtsel_minp0.bindout(2, &m_day.minp0);
      //查询平均海面气压
      stmtsel_avgp0.prepare("select ceil(avg(p0)) from T_AREAOBTMIND where ddatetime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') and obtid = :2 or ddatetime = to_date(:3, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4 or ddatetime = to_date(:5, 'yyyy-mm-dd hh24:mi:ss') and obtid = :6 or ddatetime = to_date(:7, 'yyyy-mm-dd hh24:mi:ss') and obtid = :8");
      stmtsel_avgp0.bindin(1, time_02,19);
      stmtsel_avgp0.bindin(2, m_stjisuan.obtid,10);
      stmtsel_avgp0.bindin(3, time_08,19);
      stmtsel_avgp0.bindin(4, m_stjisuan.obtid,10);
      stmtsel_avgp0.bindin(5, time_14,19);
      stmtsel_avgp0.bindin(6, m_stjisuan.obtid,10);
      stmtsel_avgp0.bindin(7, time_20,19);
      stmtsel_avgp0.bindin(8, m_stjisuan.obtid,10);
      stmtsel_avgp0.bindout(1, &m_day.avgp0);
      //最大气压和最大气压时间
      stmtsel_maxp.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), maxp from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by maxp desc,ddatetime");
      stmtsel_maxp.bindin(1, m_stjisuan.obtid,10);
      stmtsel_maxp.bindin(2, time_start,19);
      stmtsel_maxp.bindin(3, time_end,19);
      stmtsel_maxp.bindout(1, m_day.maxptime,19);
      stmtsel_maxp.bindout(2, &m_day.maxp);
      //最低气压和最低气压时间
      stmtsel_minp.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'), minp from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss') order by minp desc,ddatetime");
      stmtsel_minp.bindin(1, m_stjisuan.obtid,10);
      stmtsel_minp.bindin(2, time_start,19);
      stmtsel_minp.bindin(3, time_end,19);
      stmtsel_minp.bindout(1, m_day.minptime,19);
      stmtsel_minp.bindout(2, &m_day.minp);
      //查询平均气压
      stmtsel_avgp.prepare("select ceil(avg(p)) from T_AREAOBTMIND where ddatetime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') and obtid = :2 or ddatetime = to_date(:3, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4 or ddatetime = to_date(:5, 'yyyy-mm-dd hh24:mi:ss') and obtid = :6 or ddatetime = to_date(:7, 'yyyy-mm-dd hh24:mi:ss') and obtid = :8");
      stmtsel_avgp.bindin(1, time_02,19);
      stmtsel_avgp.bindin(2, m_stjisuan.obtid,10);
      stmtsel_avgp.bindin(3, time_08,19);
      stmtsel_avgp.bindin(4, m_stjisuan.obtid,10);
      stmtsel_avgp.bindin(5, time_14,19);
      stmtsel_avgp.bindin(6, m_stjisuan.obtid,10);
      stmtsel_avgp.bindin(7, time_20,19);
      stmtsel_avgp.bindin(8, m_stjisuan.obtid,10);
      stmtsel_avgp.bindout(1, &m_day.avgp);
      //最大两分钟风速和风向及出现时间
      stmtsel_wd2max.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'),wd2df,wd2dd from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss')order by wd2df desc,ddatetime");
      stmtsel_wd2max.bindin(1, m_stjisuan.obtid,10);
      stmtsel_wd2max.bindin(2, time_start,19);
      stmtsel_wd2max.bindin(3, time_end,19);
      stmtsel_wd2max.bindout(1, m_day.wd2maxtime,19);
      stmtsel_wd2max.bindout(2, &m_day.wd2maxdf);
      stmtsel_wd2max.bindout(3, &m_day.wd2maxdd);
      //查询2分钟平均风速
      stmtsel_wd2avg.prepare("select ceil(avg(wd2df)) from T_AREAOBTMIND where ddatetime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') and obtid = :2 or ddatetime = to_date(:3, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4 or ddatetime = to_date(:5, 'yyyy-mm-dd hh24:mi:ss') and obtid = :6 or ddatetime = to_date(:7, 'yyyy-mm-dd hh24:mi:ss') and obtid = :8");
      stmtsel_wd2avg.bindin(1, time_02,19);
      stmtsel_wd2avg.bindin(2, m_stjisuan.obtid,10);
      stmtsel_wd2avg.bindin(3, time_08,19);
      stmtsel_wd2avg.bindin(4, m_stjisuan.obtid,10);
      stmtsel_wd2avg.bindin(5, time_14,19);
      stmtsel_wd2avg.bindin(6, m_stjisuan.obtid,10);
      stmtsel_wd2avg.bindin(7, time_20,19);
      stmtsel_wd2avg.bindin(8, m_stjisuan.obtid,10);
      stmtsel_wd2avg.bindout(1, &m_day.avgwd2df);
      //最大十分钟风速和风向及出现时间
      stmtsel_wd10max.prepare("select to_char(ddatetime,'yyyy-mm-dd hh24:mi:ss'),wd10df,wd10dd from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss')order by wd10df desc,ddatetime");
      stmtsel_wd10max.bindin(1, m_stjisuan.obtid,10);
      stmtsel_wd10max.bindin(2, time_start,19);
      stmtsel_wd10max.bindin(3, time_end,19);
      stmtsel_wd10max.bindout(1, m_day.wd10maxtime,19);
      stmtsel_wd10max.bindout(2, &m_day.wd10maxdf);
      stmtsel_wd10max.bindout(3, &m_day.wd10maxdd);
      //查询10分钟平均风速
      stmtsel_wd10avg.prepare("select ceil(avg(wd10df)) from T_AREAOBTMIND where ddatetime = to_date(:1, 'yyyy-mm-dd hh24:mi:ss') and obtid = :2 or ddatetime = to_date(:3, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4 or ddatetime = to_date(:5, 'yyyy-mm-dd hh24:mi:ss') and obtid = :6 or ddatetime = to_date(:7, 'yyyy-mm-dd hh24:mi:ss') and obtid = :8");
      stmtsel_wd10avg.bindin(1, time_02,19);
      stmtsel_wd10avg.bindin(2, m_stjisuan.obtid,10);
      stmtsel_wd10avg.bindin(3, time_08,19);
      stmtsel_wd10avg.bindin(4, m_stjisuan.obtid,10);
      stmtsel_wd10avg.bindin(5, time_14,19);
      stmtsel_wd10avg.bindin(6, m_stjisuan.obtid,10);
      stmtsel_wd10avg.bindin(7, time_20,19);
      stmtsel_wd10avg.bindin(8, m_stjisuan.obtid,10);
      stmtsel_wd10avg.bindout(1, &m_day.avgwd10df);
      //查询分钟数据记录总数
      stmtsel_mincount.prepare("select count(*) from T_AREAOBTMIND where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss')");
      stmtsel_mincount.bindin(1, m_stjisuan.obtid,10);
      stmtsel_mincount.bindin(2, time_start,19);
      stmtsel_mincount.bindin(3, time_end,19);
      stmtsel_mincount.bindout(1,&m_day.mdcount);
      //查询整点数据记录总数
      stmtsel_hourcount.prepare("select count(*) from T_GDOBTHOURD where obtid = :1 and ddatetime > to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and ddatetime < to_date(:3, 'yyyy-mm-dd hh24:mi:ss')");
      stmtsel_hourcount.bindin(1, m_stjisuan.obtid,10);
      stmtsel_hourcount.bindin(2, time_start,19);
      stmtsel_hourcount.bindin(3, time_end,19);
      stmtsel_hourcount.bindout(1,&m_day.hdcount);
        if (stmtins_d.m_state == 0)
         {
            stmtins_d.connect(m_conn);
            // 生成sql语句字符串
            stmtins_d.prepare("insert into T_GDAWSDAYD (keyid,crttime,ddatetime,obtid,r0808,r2008,r0820,r24h,maxr10m,maxr10mtime,maxr01h,maxr01htime,maxt,maxttime,mint,minttime,avgt,maxrh,maxrhtime,minrh,minrhtime,avgrh,maxp0,maxp0time,minp0,minp0time,avgp0,maxp,maxptime,minp,minptime,avgp,wd2maxdf,wd2maxdd,wd2maxtime,avgwd2df,wd10maxdf,wd10maxdd,wd10maxtime,avgwd10df,mdcount,hdcount) values (SEQ_GDAWSDAYD.nextval,sysdate,to_date(:1, 'yyyy-mm-dd'),:2,:3,:4,:5,:6,:7,to_date(:8,'yyyy-mm-dd hh24:mi:ss'),:9,to_date(:10,'yyyy-mm-dd hh24:mi:ss'),:11,to_date(:12,'yyyy-mm-dd hh24:mi:ss'),:13,to_date(:14,'yyyy-mm-dd hh24:mi:ss'),:15,:16,to_date(:17,'yyyy-mm-dd hh24:mi:ss'),:18,to_date(:19,'yyyy-mm-dd hh24:mi:ss'),:20,:21,to_date(:22,'yyyy-mm-dd hh24:mi:ss'),:23,to_date(:24,'yyyy-mm-dd hh24:mi:ss'),:25,:26,to_date(:27,'yyyy-mm-dd hh24:mi:ss'),:28,to_date(:29,'yyyy-mm-dd hh24:mi:ss'),:30,:31,:32,to_date(:33,'yyyy-mm-dd hh24:mi:ss'),:34,:35,:36,to_date(:37,'yyyy-mm-dd hh24:mi:ss'),:38,:39,:40)");
            stmtins_d.bindin(1,m_stjisuan.ddatetime,19);
            stmtins_d.bindin(2,m_stjisuan.obtid,10);
            stmtins_d.bindin(3,m_stjisuan.dayrf,7);
            stmtins_d.bindin(4,&m_day.r2008);
            stmtins_d.bindin(5,&m_day.r0820);
            stmtins_d.bindin(6,&m_day.r24h);
            stmtins_d.bindin(7,&m_day.maxr10m);
            stmtins_d.bindin(8,m_day.maxr10mtime,19);
            stmtins_d.bindin(9,&m_day.maxr01h);
            stmtins_d.bindin(10,m_day.maxr01htime,19);
            stmtins_d.bindin(11,&m_day.maxt);
            stmtins_d.bindin(12,m_day.maxttime,19);
            stmtins_d.bindin(13,&m_day.mint);
            stmtins_d.bindin(14,m_day.minttime,19);
            stmtins_d.bindin(15,&m_day.avgt);
            stmtins_d.bindin(16,&m_day.maxrh);
            stmtins_d.bindin(17,m_day.maxrhtime,19);
            stmtins_d.bindin(18,&m_day.minrh);
            stmtins_d.bindin(19,m_day.minrhtime,19);
            stmtins_d.bindin(20,&m_day.avgrh);
            stmtins_d.bindin(21,&m_day.maxp0);
            stmtins_d.bindin(22,m_day.maxp0time,19);
            stmtins_d.bindin(23,&m_day.minp0);
            stmtins_d.bindin(24,m_day.minp0time,19);
            stmtins_d.bindin(25,&m_day.avgp0);
            stmtins_d.bindin(26,&m_day.maxp);
            stmtins_d.bindin(27,m_day.maxptime,19);
            stmtins_d.bindin(28,&m_day.minp);
            stmtins_d.bindin(29,m_day.minptime,19);
            stmtins_d.bindin(30,&m_day.avgp);
            stmtins_d.bindin(31,&m_day.wd2maxdf);
            stmtins_d.bindin(32,&m_day.wd2maxdd);
            stmtins_d.bindin(33,m_day.wd2maxtime,19);
            stmtins_d.bindin(34,&m_day.avgwd2df);
            stmtins_d.bindin(35,&m_day.wd10maxdf);
            stmtins_d.bindin(36,&m_day.wd10maxdd);
            stmtins_d.bindin(37,m_day.wd10maxtime,19);
            stmtins_d.bindin(38,&m_day.avgwd10df);
            stmtins_d.bindin(39,&m_day.mdcount);
            stmtins_d.bindin(40,&m_day.hdcount);
         }
    
    for (int ii = 0; ii < vjisuan.size(); ii++)  ////有一个坑，一个时间段要执行5个站点才能让时间+1天
    {
        memset(&m_stjisuan, 0, sizeof(struct st_jisuan));
        memset(&m_strain, 0, sizeof(struct st_rain));
        memset(&m_day, 0, sizeof(struct st_day));
        memcpy(&m_stjisuan, &vjisuan[ii], sizeof(struct st_jisuan));
        //整点表的日雨量累计就是昨8到今8
        AddTime(m_stjisuan.ddatetime, timetmp, 0, "yyyymmddhh24miss");//早8点12小时滑动雨量
        //统计平均值，分别是02 08 14 20
        if(strcmp(timetmp+8, "080000") == 0)
	{
            AddTime(timetmp, timetmp1, 720*60, "yyyymmddhh24miss");// 晚上20时 
            AddTime(timetmp, time_start, 0-720*60, "yyyymmddhh24miss");// 昨天20时
            AddTime(timetmp, time_end, 720*60, "yyyymmddhh24miss");// 晚上20时 
	    AddTime(timetmp, time_02, 0-360*60, "yyyymmddhh24miss");//今天02时,timetmp往前偏移6小时
	    AddTime(timetmp, time_08, 0, "yyyymmddhh24miss");//今天08时，就是timetmp本身
	    AddTime(timetmp, time_14, 320*60, "yyyymmddhh24miss");//今天14点，往后6小时
	    AddTime(timetmp, time_20, 720*60, "yyyymmddhh24miss");//晚上20时，同time_end,一天结束
            r12h=0;
	    if (stmtsel_rain.execute() != 0)
            {
                m_logfile->Write("stmtsel_rain.execute() failed.\n%s\n%s\n", stmtsel_rain.m_sql, stmtsel_rain.m_cda.message);
                return stmtsel_rain.m_cda.rc;
            }
            if (stmtsel_rain.next() == 0) {m_day.r2008 = r12h;}//12小时的雨量即为昨日20-今天08时雨量累计
            else {m_day.r2008=0;}

            r12h=0;
	    if (stmtsel_rain1.execute() != 0)
            {
                m_logfile->Write("stmtsel_rain1.execute() failed.\n%s\n%s\n", stmtsel_rain1.m_sql, stmtsel_rain1.m_cda.message);
                return stmtsel_rain1.m_cda.rc;
            }
            if (stmtsel_rain1.next() == 0) {m_day.r0820 = r12h;}//12小时的雨量即今日08时-今日20时雨量累计
            else {m_day.r0820=0;}
	
            m_day.r24h=m_day.r2008+m_day.r0820;//日雨量等于昨日20-今天08时雨量累计+今日08时-今日20时雨量累计
            //最大10分钟滑动雨量和时间
	    if (stmtsel_rain2.execute() != 0)
            {
                m_logfile->Write("stmtsel_rain2.execute() failed.\n%s\n%s\n", stmtsel_rain2.m_sql, stmtsel_rain2.m_cda.message);
                return stmtsel_rain2.m_cda.rc;
            }
            
              if (stmtsel_rain2.next() != 0) 
                {
                  m_logfile->Write("最大10分钟滑动雨量failed.\n");
		}
            //查最大1小时滑动雨量和时间
	    if (stmtsel_rain3.execute() != 0)
            {
                m_logfile->Write("stmtsel_rain3.execute() failed.\n%s\n%s\n", stmtsel_rain3.m_sql, stmtsel_rain3.m_cda.message);
                return stmtsel_rain3.m_cda.rc;
            }
            
              if (stmtsel_rain3.next() != 0) 
                {
                  m_logfile->Write("最大1小时滑动雨量failed.\n");
		}
            //查一天最高气温和最高气温时间
	    if (stmtsel_maxt.execute() != 0)
            {
                m_logfile->Write("stmtsel_maxt.execute() failed.\n%s\n%s\n", stmtsel_maxt.m_sql, stmtsel_maxt.m_cda.message);
                return stmtsel_maxt.m_cda.rc;
            }
            
              if (stmtsel_maxt.next() != 0) 
                {
                   m_logfile->Write("最高温度查询failed.\n");
		}

            //查一天最低气温和最低气温时间
	    if (stmtsel_mint.execute() != 0)
            {
                m_logfile->Write("stmtsel_mint.execute() failed.\n%s\n%s\n", stmtsel_mint.m_sql, stmtsel_mint.m_cda.message);
                return stmtsel_mint.m_cda.rc;
            }
            
              if (stmtsel_mint.next() != 0) 
                {
                   m_logfile->Write("最低温度查询failed.\n");
		}

            //查气温日平均值
	    if (stmtsel_avgt.execute() != 0)
            {
                m_logfile->Write("stmtsel_avgt.execute() failed.\n%s\n%s\n", stmtsel_avgt.m_sql, stmtsel_avgt.m_cda.message);
                return stmtsel_avgt.m_cda.rc;
            }
           
              if (stmtsel_avgt.next() != 0) 
                {
                   m_logfile->Write("平均温度查询failed.\n");
                   
     		}
          
            //查一天最大湿度和最大湿度时间

	    if (stmtsel_maxrh.execute() != 0)
            {
                m_logfile->Write("stmtsel_maxrh.execute() failed.\n%s\n%s\n", stmtsel_maxrh.m_sql, stmtsel_maxrh.m_cda.message);
                return stmtsel_maxrh.m_cda.rc;
            }
            
              if (stmtsel_maxrh.next() != 0) 
                {
                   m_logfile->Write("最大湿度查询failed.\n");
		}
            //查一天最小湿度和最小湿度时间
	    if (stmtsel_minrh.execute() != 0)
            {
                m_logfile->Write("stmtsel_minrh.execute() failed.\n%s\n%s\n", stmtsel_minrh.m_sql, stmtsel_minrh.m_cda.message);
                return stmtsel_minrh.m_cda.rc;
            }
            
              if (stmtsel_minrh.next() != 0) 
                {
                   m_logfile->Write("最小湿度查询failed.\n");
		}

            //查湿度日平均值
	    if (stmtsel_avgrh.execute() != 0)
            {
                m_logfile->Write("stmtsel_avgrh.execute() failed.\n%s\n%s\n", stmtsel_avgrh.m_sql, stmtsel_avgrh.m_cda.message);
                return stmtsel_avgrh.m_cda.rc;
            }
           
              if (stmtsel_avgrh.next() != 0) 
                {
                   m_logfile->Write("湿度日平均值查询failed.\n");
                }
                   
            //查一天最大海平面气压和最大海平面气压时间

	    if (stmtsel_maxp0.execute() != 0)
            {
                m_logfile->Write("stmtsel_maxp0.execute() failed.\n%s\n%s\n", stmtsel_maxp0.m_sql, stmtsel_maxp0.m_cda.message);
                return stmtsel_maxp0.m_cda.rc;
            }
            
              if (stmtsel_maxp0.next() != 0) 
                {
                   m_logfile->Write("最大海平面气压查询failed.\n");
		}
		while(1){
		if(m_day.maxp0==0)
		  {stmtsel_maxp0.next();}
		else break;}
            //查一天最低海平面气压和最低海平面气压时间
	    if (stmtsel_minp0.execute() != 0)
            {
                m_logfile->Write("stmtsel_minp0.execute() failed.\n%s\n%s\n", stmtsel_minp0.m_sql, stmtsel_minp0.m_cda.message);
                return stmtsel_minp0.m_cda.rc;
            }
            
              if (stmtsel_minp0.next() != 0) 
                {
                   m_logfile->Write("最低平面气压查询failed.\n");
		}

            //查海平面气压日平均值
	    if (stmtsel_avgp0.execute() != 0)
            {
                m_logfile->Write("stmtsel_avgp0.execute() failed.\n%s\n%s\n", stmtsel_avgp0.m_sql, stmtsel_avgp0.m_cda.message);
                return stmtsel_avgp0.m_cda.rc;
            }
           
              if (stmtsel_avgp0.next() != 0) 
                {
                   m_logfile->Write("海平面气压日平均值查询failed.\n");
                }
            //查一天最大气压和最大气压时间

	    if (stmtsel_maxp.execute() != 0)
            {
                m_logfile->Write("stmtsel_maxp.execute() failed.\n%s\n%s\n", stmtsel_maxp.m_sql, stmtsel_maxp.m_cda.message);
                return stmtsel_maxp.m_cda.rc;
            }
            
              if (stmtsel_maxp.next() != 0) 
                {
                   m_logfile->Write("最大气压查询failed.\n");
		}
		while(1){
		if(m_day.maxp==0)
		  {stmtsel_maxp.next();}
		else break;}

             
            //查一天最低气压和最低气压时间
	    if (stmtsel_minp.execute() != 0)
            {
                m_logfile->Write("stmtsel_minp.execute() failed.\n%s\n%s\n", stmtsel_minp.m_sql, stmtsel_minp.m_cda.message);
                return stmtsel_minp.m_cda.rc;
            }
            
              if (stmtsel_minp.next() != 0) 
                {
                   m_logfile->Write("最低气压查询failed.\n");
		}
		while(1){
		if(m_day.minp==0)
		  {stmtsel_minp.next();}
		else break;}

            //查气压日平均值
	    if (stmtsel_avgp.execute() != 0)
            {
                m_logfile->Write("stmtsel_avgp.execute() failed.\n%s\n%s\n", stmtsel_avgp.m_sql, stmtsel_avgp.m_cda.message);
                return stmtsel_avgp.m_cda.rc;
            }
           
              if (stmtsel_avgp.next() != 0) 
                {
                   m_logfile->Write("气压日平均值查询failed.\n");
		}
            //最大两分钟风速和风向及出现时间
	    if (stmtsel_wd2max.execute() != 0)
            {
                m_logfile->Write("stmtsel_wd2max.execute() failed.\n%s\n%s\n", stmtsel_wd2max.m_sql, stmtsel_wd2max.m_cda.message);
                return stmtsel_wd2max.m_cda.rc;
            }
           
              if (stmtsel_wd2max.next() != 0) 
                {
                   m_logfile->Write("最大两分钟风速和风向及出现时间查询failed.\n");
		}
            //查两分钟风速日平均值
	    if (stmtsel_wd2avg.execute() != 0)
            {
                m_logfile->Write("stmtsel_wd2avg.execute() failed.\n%s\n%s\n", stmtsel_wd2avg.m_sql, stmtsel_wd2avg.m_cda.message);
                return stmtsel_wd2avg.m_cda.rc;
            }
           
              if (stmtsel_wd2avg.next() != 0) 
                {
                   m_logfile->Write("两分钟风速日平均值failed.\n");
		}

            //最大十分钟风速和风向及出现时间
	    if (stmtsel_wd10max.execute() != 0)
            {
                m_logfile->Write("stmtsel_wd10max.execute() failed.\n%s\n%s\n", stmtsel_wd10max.m_sql, stmtsel_wd10max.m_cda.message);
                return stmtsel_wd10max.m_cda.rc;
            }
           
              if (stmtsel_wd10max.next() != 0) 
                {
                   m_logfile->Write("最大两分钟风速和风向及出现时间查询failed.\n");
		}
            //查十分钟风速日平均值
	    if (stmtsel_wd10avg.execute() != 0)
            {
                m_logfile->Write("stmtsel_wd10avg.execute() failed.\n%s\n%s\n", stmtsel_wd10avg.m_sql, stmtsel_wd10avg.m_cda.message);
                return stmtsel_wd10avg.m_cda.rc;
            }
           
              if (stmtsel_wd10avg.next() != 0) 
                {
                   m_logfile->Write("两分钟风速日平均值failed.\n");
		}
            //分钟记录条数
	    if (stmtsel_mincount.execute() != 0)
            {
                m_logfile->Write("stmtsel_mincount.execute() failed.\n%s\n%s\n", stmtsel_mincount.m_sql, stmtsel_mincount.m_cda.message);
                return stmtsel_mincount.m_cda.rc;
            }
           
              if (stmtsel_mincount.next() != 0) 
                {
                   m_logfile->Write("两分钟风速日平均值failed.\n");
		}
            //整点记录条数
	    if (stmtsel_hourcount.execute() != 0)
            {
                m_logfile->Write("stmtsel_hourcount.execute() failed.\n%s\n%s\n", stmtsel_hourcount.m_sql, stmtsel_hourcount.m_cda.message);
                return stmtsel_hourcount.m_cda.rc;
            }
           
              if (stmtsel_hourcount.next() != 0) 
                {
                   m_logfile->Write("两分钟风速日平均值failed.\n");
		}
		//将ddatetime转成日期格式
            AddTime(m_stjisuan.ddatetime, m_stjisuan.ddatetime, 0, "yyyy-mm-dd");// 晚上20时 

              /* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
            if (stmtins_d.execute() != 0)
            {
                m_logfile->Write("stmtins_d.execute() failed.\n%s\n%s\n", stmtins_d.m_sql, stmtins_d.m_cda.message);
                return stmtins_d.m_cda.rc;
            } 
         
	  
      }
     }

    vjisuan.clear();

    return 0;
}
