#include "_shqx.h"

/* CSURFDATA类 */
CSURFDATA::CSURFDATA(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CSURFDATA::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_stsurfdata, 0, sizeof(struct st_surfdata));
}

CSURFDATA::~CSURFDATA() {}

/* 把用逗号分隔的记录拆分到m_stsurfdata结构中 */
bool CSURFDATA::SplitBuffer(const char *strBuffer)
{
	m_total++;
	memset(&m_stsurfdata, 0, sizeof(struct st_surfdata));

	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, ",");
	if (CmdStr.CmdCount() != 9) { m_discarded++; return false; }

	double ttmp;
	CmdStr.GetValue(0,  m_stsurfdata.obtid);                      // 站点代码
	CmdStr.GetValue(1,  m_stsurfdata.ddatetime);                  // 数据时间: 格式yyyy-mm-dd hh:mi:ss
	CmdStr.GetValue(2, &ttmp); m_stsurfdata.t   = (int)(ttmp*10); // 气温: 单位, 0.1摄氏度
	CmdStr.GetValue(3, &ttmp); m_stsurfdata.p   = (int)(ttmp*10); // 气压: 单位: 0.1百帕
	CmdStr.GetValue(4, &m_stsurfdata.u);                          // 相对湿度, 0-100之间的值
	CmdStr.GetValue(5, &m_stsurfdata.wd);                         // 风向, 0-360之间的值
	CmdStr.GetValue(6, &ttmp); m_stsurfdata.wf  = (int)(ttmp*10); // 风速: 单位: 0.1m/s
	CmdStr.GetValue(7, &ttmp); m_stsurfdata.r   = (int)(ttmp*10); // 降雨量: 0.1mm
	CmdStr.GetValue(8, &ttmp); m_stsurfdata.vis = (int)(ttmp*10); // 能见度: 0.1m

	return true;
}

/* 把XML格式的记录拆分到m_stsurfdata结构中 */
bool CSURFDATA::SplitXmlstr(const char *strBuffer)
{
	m_total++;
	memset(&m_stsurfdata, 0, sizeof(struct st_surfdata));
	
	double ttmp;
	if (GetXMLBuffer(strBuffer, "obtid",     m_stsurfdata.obtid)     == false) { m_discarded++; return false; }
	if (GetXMLBuffer(strBuffer, "ddatetime", m_stsurfdata.ddatetime) == false) { m_discarded++; return false; }
	if (GetXMLBuffer(strBuffer, "t",        &ttmp)                   == false) { m_discarded++; return false; }
	m_stsurfdata.t   = (int)(ttmp*10);
	if (GetXMLBuffer(strBuffer, "p",        &ttmp)                   == false) { m_discarded++; return false; }
	if (GetXMLBuffer(strBuffer, "u",        &m_stsurfdata.u)         == false) { m_discarded++; return false; }
	if (GetXMLBuffer(strBuffer, "wd",       &m_stsurfdata.wd)        == false) { m_discarded++; return false; }
	m_stsurfdata.wf  = (int)(ttmp*10);
	if (GetXMLBuffer(strBuffer, "wf",       &ttmp)                   == false) { m_discarded++; return false; }
	m_stsurfdata.wf  = (int)(ttmp*10);
	if (GetXMLBuffer(strBuffer, "r",        &ttmp)                   == false) { m_discarded++; return false; }
	m_stsurfdata.r   = (int)(ttmp*10);
	if (GetXMLBuffer(strBuffer, "vis",      &ttmp)                   == false) { m_discarded++; return false; }
	m_stsurfdata.vis = (int)(ttmp*10);

	return true;
}

long CSURFDATA::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from t_surfdata where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
    	stmtsel.bindin(1,  m_stsurfdata.obtid, 5);
    	stmtsel.bindin(2,  m_stsurfdata.ddatetime, 19);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update t_surfdata set crttime=sysdate, t=:1, p=:2, u=:3, wd=:4, wf=:5, r=:6, vis=:7 where obtid=:8 and ddatetime=to_date(:9, 'yyyy-mm-dd hh24:mi:ss')");
        stmtupd.bindin(1, &m_stsurfdata.t);
        stmtupd.bindin(2, &m_stsurfdata.p);
        stmtupd.bindin(3, &m_stsurfdata.u);
        stmtupd.bindin(4, &m_stsurfdata.wd);
        stmtupd.bindin(5, &m_stsurfdata.wf);
        stmtupd.bindin(6, &m_stsurfdata.r);
        stmtupd.bindin(7, &m_stsurfdata.vis);
    	stmtupd.bindin(8,  m_stsurfdata.obtid, 5);
    	stmtupd.bindin(9,  m_stsurfdata.ddatetime, 19);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into t_surfdata(keyid, crttime, obtid, ddatetime, t, p, u, wd, wf, r, vis) values(SEQ_SURFDATA.nextval, sysdate, :1, to_date(:2, 'yyyy-mm-dd hh24:mi:ss'), :3, :4, :5, :6, :7, :8, :9)");
    	stmtins.bindin(1,  m_stsurfdata.obtid, 5);
    	stmtins.bindin(2,  m_stsurfdata.ddatetime, 19);
    	stmtins.bindin(3, &m_stsurfdata.t);
    	stmtins.bindin(4, &m_stsurfdata.p);
    	stmtins.bindin(5, &m_stsurfdata.u);
    	stmtins.bindin(6, &m_stsurfdata.wd);
    	stmtins.bindin(7, &m_stsurfdata.wf);
    	stmtins.bindin(8, &m_stsurfdata.r);
    	stmtins.bindin(9, &m_stsurfdata.vis);
	}

    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    if (stmtsel.execute() != 0)
    {
		m_discarded++;
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount == 0)
	{
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
    	}
		m_inserted++;
	}

	if (iccount > 0)
	{
    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtupd.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
			return stmtupd.m_cda.rc;
    	}
		m_updated++;
	}

	return 0;
}

/* CSIGNALLOG类 */
CSIGNALLOG::CSIGNALLOG(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CSIGNALLOG::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_stsignallog, 0, sizeof(struct st_signallog));
	vsignallog.clear();
}

CSIGNALLOG::~CSIGNALLOG() {}

/* 把用空格分隔的记录拆分到vsignallog容器中 */
bool CSIGNALLOG::SplitBuffer(const char *strBuffer)
{
	vsignallog.clear();
	memset(&m_stsignallog, 0, sizeof(struct st_signallog));

	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, " ", true);
	if (CmdStr.CmdCount() < 3) { m_discarded++; return false; }

	/* 处理时间
	time_t titmp=0;
	char timetmp[21]; memset(timetmp, 0, sizeof(timetmp));
	CmdStr.GetValue(0, timetmp, 12);
	strcat(timetmp, "00");  // 加入0秒, 方便转化.
	// 先将字符串时间转换成数字, 再加上8小时(8*60*60), 再转换成字符串时间(yyyy-mm-dd hh:mi:ss).
	strtotime(timetmp, &titmp, "yyyymmddhhmiss");
	timetostr(titmp+8*60*60, m_stsignallog.ddatetime);
	*/

	CmdStr.GetValue(0, m_stsignallog.ddatetime, 12);
	strcat(m_stsignallog.ddatetime, "00");  // 加入0秒, 方便转化.
	AddTime(m_stsignallog.ddatetime, m_stsignallog.ddatetime, 8*60*60, "yyyy-mm-dd hh:mi:ss");

	// 处理街道代码
	CmdStr.GetValue(1, m_stsignallog.obtid, 4);

	// 处理信号名和颜色
	char strtmp[11];
	for (int ii=3; ii<=CmdStr.CmdCount(); ii++)
	{
		memset(strtmp, 0, sizeof(strtmp));
		CmdStr.GetValue(ii-1, strtmp, 5);
		m_stsignallog.signalname[0]=strtmp[0];
		m_stsignallog.signalcolor[0]=strtmp[1];
		vsignallog.push_back(m_stsignallog);
		m_total++;
	}

	return true;
}

/* 把容器vsignallog更新到数据库中 */
long CSIGNALLOG::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_SIGNALLOG where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and signalname = :3");
    	stmtsel.bindin(1, m_stsignallog.obtid, 4);
    	stmtsel.bindin(2, m_stsignallog.ddatetime, 19);
    	stmtsel.bindin(3, m_stsignallog.signalname, 1);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_SIGNALLOG set crttime=sysdate, signalcolor=:4 where obtid=:1 and ddatetime=to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and signalname=:3");
        stmtupd.bindin(1, m_stsignallog.obtid, 4);
        stmtupd.bindin(2, m_stsignallog.ddatetime, 19);
        stmtupd.bindin(3, m_stsignallog.signalname, 1);
        stmtupd.bindin(4, m_stsignallog.signalcolor, 1);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_SIGNALLOG(keyid, crttime, obtid, ddatetime, signalname, signalcolor) values(SEQ_SIGNALLOG.nextval, sysdate, :1, to_date(:2, 'yyyy-mm-dd hh24:mi:ss'), :3, :4)");
        stmtins.bindin(1, m_stsignallog.obtid, 4);
        stmtins.bindin(2, m_stsignallog.ddatetime, 19);
        stmtins.bindin(3, m_stsignallog.signalname, 1);
        stmtins.bindin(4, m_stsignallog.signalcolor, 1);
	}

	for (int ii=0;ii<vsignallog.size();ii++)
	{
		memcpy(&m_stsignallog, &vsignallog[ii], sizeof(struct st_signallog));
    	/* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    	if (stmtsel.execute() != 0)
    	{
			m_discarded++;
    	    m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
			return stmtsel.m_cda.rc;
    	}

    	iccount = 0;
    	stmtsel.next();

    	if (iccount == 0)
		{
    		/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    		if (stmtins.execute() != 0)
    		{
				m_discarded++;
    	        m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
				return stmtins.m_cda.rc;
    		}
			m_inserted++;
		}

		if (iccount > 0)
		{
    		/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    		if (stmtupd.execute() != 0)
    		{
				m_discarded++;
    	        m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
				return stmtupd.m_cda.rc;
    		}
			m_updated++;
		}
	}

	return 0;
}

/* CTYPHOONBEST类 */
CTYPHOONBEST::CTYPHOONBEST(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CTYPHOONBEST::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_sttyphoonbest, 0, sizeof(struct st_typhoonbest));
}

CTYPHOONBEST::~CTYPHOONBEST(){}

/* 把用空格分隔的头部记录拆分到m_sttyphoonbest结构中 */
bool CTYPHOONBEST::SplithdBuffer(const char *strBuffer)
{
	memset(&m_sttyphoonbest, 0, sizeof(struct st_typhoonbest));

	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, " ");
	//if (CmdStr.CmdCount() != 9) { m_discarded++; return false; }

	CmdStr.GetValue(0,  m_sttyphoonbest.classmk);    // 分类标志
	CmdStr.GetValue(1,  m_sttyphoonbest.innaid);     // 国际编号
	CmdStr.GetValue(2, &m_sttyphoonbest.pathno);     // 路径数据记录的行数
	CmdStr.GetValue(3,  m_sttyphoonbest.cycno);      // 气旋编号
	CmdStr.GetValue(4,  m_sttyphoonbest.cycid);      // 国内编号
	CmdStr.GetValue(5, &m_sttyphoonbest.endsign);    // 终结标志
	CmdStr.GetValue(6, &m_sttyphoonbest.intervals);  // 数据的间隔时间, 小时, 目前均为6小时
	CmdStr.GetValue(7,  m_sttyphoonbest.egname);     // 英文名称
	CmdStr.GetValue(8,  m_sttyphoonbest.fmtdate);    // 数据集成的日期
	strcat(m_sttyphoonbest.fmtdate, "000000");       // 加入0时0分0秒, 方便转化.

	return true;
}

/* 把用空格分隔的中间记录拆分到m_sttyphoonbest结构中 */
bool CTYPHOONBEST::SplitinBuffer(const char *strBuffer)
{
	m_total++;
	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, " ");

	CmdStr.GetValue(0,  m_sttyphoonbest.ddatetime, 10);        // 数据时间
	strcat(m_sttyphoonbest.ddatetime, "0000");  // 加入0分0秒, 方便转化.
	AddTime(m_sttyphoonbest.ddatetime, m_sttyphoonbest.ddatetime, 8*60*60, "yyyy-mm-dd hh:mi:ss");

	CmdStr.GetValue(1, &m_sttyphoonbest.intensitysign); // 强度标记
	CmdStr.GetValue(2, &m_sttyphoonbest.lat);           // 纬度
	CmdStr.GetValue(3, &m_sttyphoonbest.lon);           // 经度
	CmdStr.GetValue(4, &m_sttyphoonbest.pressure);      // 中心最低气压(hPa); 百帕
	CmdStr.GetValue(5, &m_sttyphoonbest.wdmaxspeed);    // 2分钟平均近中心最大风速(MSW, m/s)
	if (CmdStr.CmdCount() == 7)
	CmdStr.GetValue(6,  m_sttyphoonbest.wdavespeed);    // 2分钟平均风速(m/s)

	return true;
}

/* 将结构体更新到表中 */
long CTYPHOONBEST::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_TYPHOON_BEST where cycno = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and egname = :3");
    	stmtsel.bindin(1,  m_sttyphoonbest.cycno, 4);
    	stmtsel.bindin(2,  m_sttyphoonbest.ddatetime, 19);
    	stmtsel.bindin(3,  m_sttyphoonbest.egname, 20);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_TYPHOON_BEST set crttime=sysdate, innaid=:1, classmk=:4, fmtdate=to_date(:5, 'yyyy-mm-dd hh24:mi:ss'), pathno=:6, intervals=:7, endsign=:8, intensitysign=:9, lat=:10, lon=:11, pressure=:12, wdmaxspeed=:13, wdavespeed=:14 cycid=:15 where cycno=:3 and egname=:2 and ddatetime=to_date(:16, 'yyyy-mm-dd hh24:mi:ss')");

        stmtupd.bindin(1,  m_sttyphoonbest.innaid, 4);
        stmtupd.bindin(2,  m_sttyphoonbest.egname, 20);
        stmtupd.bindin(3,  m_sttyphoonbest.cycno, 4);
        stmtupd.bindin(4,  m_sttyphoonbest.classmk, 5);
        stmtupd.bindin(5,  m_sttyphoonbest.fmtdate, 19);
        stmtupd.bindin(6, &m_sttyphoonbest.pathno);
        stmtupd.bindin(7, &m_sttyphoonbest.intervals);
    	stmtupd.bindin(8, &m_sttyphoonbest.endsign);
    	stmtupd.bindin(9, &m_sttyphoonbest.intensitysign);
    	stmtupd.bindin(10,&m_sttyphoonbest.lat);
    	stmtupd.bindin(11,&m_sttyphoonbest.lon);
    	stmtupd.bindin(12,&m_sttyphoonbest.pressure);
    	stmtupd.bindin(13,&m_sttyphoonbest.wdmaxspeed);
    	stmtupd.bindin(14, m_sttyphoonbest.wdavespeed, 3);
    	stmtupd.bindin(15, m_sttyphoonbest.cycid, 4);
    	stmtupd.bindin(16, m_sttyphoonbest.ddatetime, 19);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_TYPHOON_BEST(keyid, crttime, innaid, egname, cycno, classmk, fmtdate, pathno, intervals, endsign, intensitysign, lat, lon, pressure, wdmaxspeed, wdavespeed, cycid, ddatetime) values(SEQ_TYPHOON_BEST.nextval, sysdate, :1, :2, :3, :4, to_date(:5, 'yyyy-mm-dd hh24:mi:ss'), :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, to_date(:16, 'yyyy-mm-dd hh24:mi:ss'))");
		
        stmtins.bindin(1,  m_sttyphoonbest.innaid, 4);
        stmtins.bindin(2,  m_sttyphoonbest.egname, 20);
        stmtins.bindin(3,  m_sttyphoonbest.cycno, 4);
        stmtins.bindin(4,  m_sttyphoonbest.classmk, 5);
        stmtins.bindin(5,  m_sttyphoonbest.fmtdate, 19);
        stmtins.bindin(6, &m_sttyphoonbest.pathno);
        stmtins.bindin(7, &m_sttyphoonbest.intervals);
    	stmtins.bindin(8, &m_sttyphoonbest.endsign);
    	stmtins.bindin(9, &m_sttyphoonbest.intensitysign);
    	stmtins.bindin(10,&m_sttyphoonbest.lat);
    	stmtins.bindin(11,&m_sttyphoonbest.lon);
    	stmtins.bindin(12,&m_sttyphoonbest.pressure);
    	stmtins.bindin(13,&m_sttyphoonbest.wdmaxspeed);
    	stmtins.bindin(14, m_sttyphoonbest.wdavespeed, 3);
    	stmtins.bindin(15, m_sttyphoonbest.cycid, 4);
    	stmtins.bindin(16, m_sttyphoonbest.ddatetime, 19);
	}

    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    if (stmtsel.execute() != 0)
    {
		m_discarded++;
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount == 0)
	{
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
    	}
		m_inserted++;
	}

	if (iccount > 0)
	{
    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtupd.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
			return stmtupd.m_cda.rc;
    	}
		m_updated++;
	}

	return 0;
}

/* CCLOUDOBT类 */
CCLOUDOBT::CCLOUDOBT(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CCLOUDOBT::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_stcloudobt, 0, sizeof(struct st_cloudobt));
}

CCLOUDOBT::~CCLOUDOBT() {}

/* 把用逗号分隔的记录拆分到m_stcloudobt结构中 */
bool CCLOUDOBT::SplitBuffer(const char *strBuffer)
{
	m_total++;
	memset(&m_stcloudobt, 0, sizeof(struct st_cloudobt));

	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, ",");
	int no1, no2;

	CmdStr.GetValue(1,  m_stcloudobt.statno);
	CmdStr.GetValue(2,  m_stcloudobt.servtype);
	CmdStr.GetValue(3,  m_stcloudobt.devflag);
	CmdStr.GetValue(4,  m_stcloudobt.devid);
	CmdStr.GetValue(5,  m_stcloudobt.ddatetime);
	CmdStr.GetValue(6,  m_stcloudobt.framelabel);
	CmdStr.GetValue(7,  m_stcloudobt.obfeatures);
	CmdStr.GetValue(8,  m_stcloudobt.devstatus);

	no1 = atoi(m_stcloudobt.obfeatures); // 观测要素变量数
	no2 = atoi(m_stcloudobt.devstatus);  // 设备状态变量数
	if (CmdStr.CmdCount() != (no1 + no2) * 2 + 12) { m_discarded++; return false; }

	char strtmp1[31]; char strtmp2[31];
	for (int ii=0; ii<no1; ii++)
	{
		memset(strtmp1, 0, sizeof(strtmp1)); memset(strtmp2, 0, sizeof(strtmp2));
		sprintf(strtmp1, "%s%d", "m_stcloudobt.fname", ii+1);
		sprintf(strtmp2, "%s%d", "m_stcloudobt.fvalue", ii+1);

		if (strcmp(strtmp1, "m_stcloudobt.fname1")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname1);
		if (strcmp(strtmp1, "m_stcloudobt.fname2")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname2);
		if (strcmp(strtmp1, "m_stcloudobt.fname3")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname3);
		if (strcmp(strtmp1, "m_stcloudobt.fname4")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname4);
		if (strcmp(strtmp1, "m_stcloudobt.fname5")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname5);
		if (strcmp(strtmp1, "m_stcloudobt.fname6")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname6);
		if (strcmp(strtmp1, "m_stcloudobt.fname7")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname7);
		if (strcmp(strtmp1, "m_stcloudobt.fname8")  == 0) CmdStr.GetValue((2*ii+9),  m_stcloudobt.fname8);
		
		if (strcmp(strtmp2, "m_stcloudobt.fvalue1") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue1);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue2") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue2);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue3") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue3);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue4") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue4);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue5") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue5);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue6") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue6);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue7") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue7);
		if (strcmp(strtmp2, "m_stcloudobt.fvalue8") == 0) CmdStr.GetValue((2*ii+10), m_stcloudobt.fvalue8);
	}

	CmdStr.GetValue((2*no1+9), m_stcloudobt.qualitybit);

	for (int ii=0; ii<no2; ii++)
	{
		memset(strtmp1, 0, sizeof(strtmp1)); memset(strtmp2, 0, sizeof(strtmp2));
		sprintf(strtmp1, "%s%d", "m_stcloudobt.sname", ii+1);
		sprintf(strtmp2, "%s%d", "m_stcloudobt.svalue", ii+1);

		if (strcmp(strtmp1, "m_stcloudobt.sname1")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname1);
		if (strcmp(strtmp1, "m_stcloudobt.sname2")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname2);
		if (strcmp(strtmp1, "m_stcloudobt.sname3")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname3);
		if (strcmp(strtmp1, "m_stcloudobt.sname4")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname4);
		if (strcmp(strtmp1, "m_stcloudobt.sname5")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname5);
		if (strcmp(strtmp1, "m_stcloudobt.sname6")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname6);
		if (strcmp(strtmp1, "m_stcloudobt.sname7")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname7);
		if (strcmp(strtmp1, "m_stcloudobt.sname8")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname8);
		if (strcmp(strtmp1, "m_stcloudobt.sname9")  == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname9);
		if (strcmp(strtmp1, "m_stcloudobt.sname10") == 0) CmdStr.GetValue((2*(ii+no1)+10),  m_stcloudobt.sname10);
		
		if (strcmp(strtmp2, "m_stcloudobt.svalue1") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue1);
		if (strcmp(strtmp2, "m_stcloudobt.svalue2") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue2);
		if (strcmp(strtmp2, "m_stcloudobt.svalue3") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue3);
		if (strcmp(strtmp2, "m_stcloudobt.svalue4") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue4);
		if (strcmp(strtmp2, "m_stcloudobt.svalue5") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue5);
		if (strcmp(strtmp2, "m_stcloudobt.svalue6") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue6);
		if (strcmp(strtmp2, "m_stcloudobt.svalue7") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue7);
		if (strcmp(strtmp2, "m_stcloudobt.svalue8") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue8);
		if (strcmp(strtmp2, "m_stcloudobt.svalue9") == 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue9);
		if (strcmp(strtmp2, "m_stcloudobt.svalue10")== 0) CmdStr.GetValue((2*(ii+no1)+11),  m_stcloudobt.svalue10);
	}
	
	CmdStr.GetValue(2*(no1+no2)+10, m_stcloudobt.checkcode);

	return true;
}

long CCLOUDOBT::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_CLOUDOBT where statno = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
    	stmtsel.bindin(1,  m_stcloudobt.statno, 6);
    	stmtsel.bindin(2,  m_stcloudobt.ddatetime, 19);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_CLOUDOBT set crttime=sysdate, servtype=:2, \
devflag=:3, devid=:4, framelabel=:6, obfeatures=:7, devstatus=:8, fname1=:9, \
fvalue1=:10, fname2=:11, fvalue2=:12, fname3=:13, fvalue3=:14, fname4=:15, \
fvalue4=:16, fname5=:17, fvalue5=:18, fname6=:19, fvalue6=:20, fname7=:21, \
fvalue7=:22, fname8=:23, fvalue8=:24, qualitybit=:25, sname1=:26 svalue1=:27, \
sname2=:28, svalue2=:29, sname3=:30, svalue3=:31, sname4=:32, svalue4=:33, \
sname5=:34, svalue5=:35, sname6=:36, svalue6=:37, sname7=:38, svalue7=:39, \
sname8=:40, svalue8=:41, sname9=:42, svalue9=:43, sname10=:44, svalue10=:45, \
checkcode=:46 where statno=:1 and ddatetime=to_date(:5, 'yyyy-mm-dd hh24:mi:ss')");
        stmtupd.bindin(1, m_stcloudobt.statno, 6);
        stmtupd.bindin(2, m_stcloudobt.servtype, 2);
        stmtupd.bindin(3, m_stcloudobt.devflag, 4);
        stmtupd.bindin(4, m_stcloudobt.devid, 3);
        stmtupd.bindin(5, m_stcloudobt.ddatetime, 19);
        stmtupd.bindin(6, m_stcloudobt.framelabel, 2);
        stmtupd.bindin(7, m_stcloudobt.obfeatures, 2);
    	stmtupd.bindin(8, m_stcloudobt.devstatus, 2);

    	stmtupd.bindin(9, m_stcloudobt.fname1, 5);
    	stmtupd.bindin(10, m_stcloudobt.fvalue1, 4);
    	stmtupd.bindin(11, m_stcloudobt.fname2, 5);
    	stmtupd.bindin(12, m_stcloudobt.fvalue2, 4);
    	stmtupd.bindin(13, m_stcloudobt.fname3, 5);
    	stmtupd.bindin(14, m_stcloudobt.fvalue3, 4);
    	stmtupd.bindin(15, m_stcloudobt.fname4, 5);
    	stmtupd.bindin(16, m_stcloudobt.fvalue4, 4);
    	stmtupd.bindin(17, m_stcloudobt.fname5, 5);
    	stmtupd.bindin(18, m_stcloudobt.fvalue5, 4);
    	stmtupd.bindin(19, m_stcloudobt.fname6, 5);
    	stmtupd.bindin(20, m_stcloudobt.fvalue6, 4);
    	stmtupd.bindin(21, m_stcloudobt.fname7, 5);
    	stmtupd.bindin(22, m_stcloudobt.fvalue7, 4);
    	stmtupd.bindin(23, m_stcloudobt.fname8, 5);
    	stmtupd.bindin(24, m_stcloudobt.fvalue8, 4);

    	stmtupd.bindin(25, m_stcloudobt.qualitybit, 10);

    	stmtupd.bindin(26, m_stcloudobt.sname1, 5);
    	stmtupd.bindin(27, m_stcloudobt.svalue1, 1);
    	stmtupd.bindin(28, m_stcloudobt.sname2, 5);
    	stmtupd.bindin(29, m_stcloudobt.svalue2, 1);
    	stmtupd.bindin(30, m_stcloudobt.sname3, 5);
    	stmtupd.bindin(31, m_stcloudobt.svalue3, 1);
    	stmtupd.bindin(32, m_stcloudobt.sname4, 5);
    	stmtupd.bindin(33, m_stcloudobt.svalue4, 1);
    	stmtupd.bindin(34, m_stcloudobt.sname5, 5);
    	stmtupd.bindin(35, m_stcloudobt.svalue5, 1);
    	stmtupd.bindin(36, m_stcloudobt.sname6, 5);
    	stmtupd.bindin(37, m_stcloudobt.svalue6, 1);
    	stmtupd.bindin(38, m_stcloudobt.sname7, 5);
    	stmtupd.bindin(39, m_stcloudobt.svalue7, 1);
    	stmtupd.bindin(40, m_stcloudobt.sname8, 5);
    	stmtupd.bindin(41, m_stcloudobt.svalue8, 1);
    	stmtupd.bindin(42, m_stcloudobt.sname9, 5);
    	stmtupd.bindin(43, m_stcloudobt.svalue9, 1);
    	stmtupd.bindin(44, m_stcloudobt.sname10, 5);
    	stmtupd.bindin(45, m_stcloudobt.svalue10, 1);

    	stmtupd.bindin(46, m_stcloudobt.checkcode, 4);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_CLOUDOBT(keyid, crttime, statno, servtype, \
devflag, devid, ddatetime, framelabel, obfeatures, devstatus, fname1, fvalue1, \
fname2, fvalue2, fname3, fvalue3, fname4, fvalue4, fname5, fvalue5, fname6, \
fvalue6, fname7, fvalue7, fname8, fvalue8, qualitybit, sname1, svalue1, \
sname2, svalue2, sname3, svalue3, sname4, svalue4, sname5, svalue5, sname6, svalue6,\
 sname7, svalue7, sname8, svalue8, sname9, svalue9, sname10, svalue10, checkcode) \
values(SEQ_CLOUDOBT.nextval, sysdate, :1, :2, :3, :4, \
to_date(:5, 'yyyy-mm-dd hh24:mi:ss'), :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, \
:16, :17, :18, :19, :20, :21, :22, :23, :24, :25, :26, :27, :28, :29, :30, :31, :32, \
:33, :34, :35, :36, :37, :38, :39, :40, :41, :42, :43, :44, :45, :46)");
        stmtins.bindin(1, m_stcloudobt.statno, 6);
        stmtins.bindin(2, m_stcloudobt.servtype, 2);
        stmtins.bindin(3, m_stcloudobt.devflag, 4);
        stmtins.bindin(4, m_stcloudobt.devid, 3);
        stmtins.bindin(5, m_stcloudobt.ddatetime, 19);
        stmtins.bindin(6, m_stcloudobt.framelabel, 2);
        stmtins.bindin(7, m_stcloudobt.obfeatures, 2);
    	stmtins.bindin(8, m_stcloudobt.devstatus, 2);

    	stmtins.bindin(9, m_stcloudobt.fname1, 5);
    	stmtins.bindin(10, m_stcloudobt.fvalue1, 4);
    	stmtins.bindin(11, m_stcloudobt.fname2, 5);
    	stmtins.bindin(12, m_stcloudobt.fvalue2, 4);
    	stmtins.bindin(13, m_stcloudobt.fname3, 5);
    	stmtins.bindin(14, m_stcloudobt.fvalue3, 4);
    	stmtins.bindin(15, m_stcloudobt.fname4, 5);
    	stmtins.bindin(16, m_stcloudobt.fvalue4, 4);
    	stmtins.bindin(17, m_stcloudobt.fname5, 5);
    	stmtins.bindin(18, m_stcloudobt.fvalue5, 4);
    	stmtins.bindin(19, m_stcloudobt.fname6, 5);
    	stmtins.bindin(20, m_stcloudobt.fvalue6, 4);
    	stmtins.bindin(21, m_stcloudobt.fname7, 5);
    	stmtins.bindin(22, m_stcloudobt.fvalue7, 4);
    	stmtins.bindin(23, m_stcloudobt.fname8, 5);
    	stmtins.bindin(24, m_stcloudobt.fvalue8, 4);

    	stmtins.bindin(25, m_stcloudobt.qualitybit, 10);

    	stmtins.bindin(26, m_stcloudobt.sname1, 5);
    	stmtins.bindin(27, m_stcloudobt.svalue1, 1);
    	stmtins.bindin(28, m_stcloudobt.sname2, 5);
    	stmtins.bindin(29, m_stcloudobt.svalue2, 1);
    	stmtins.bindin(30, m_stcloudobt.sname3, 5);
    	stmtins.bindin(31, m_stcloudobt.svalue3, 1);
    	stmtins.bindin(32, m_stcloudobt.sname4, 5);
    	stmtins.bindin(33, m_stcloudobt.svalue4, 1);
    	stmtins.bindin(34, m_stcloudobt.sname5, 5);
    	stmtins.bindin(35, m_stcloudobt.svalue5, 1);
    	stmtins.bindin(36, m_stcloudobt.sname6, 5);
    	stmtins.bindin(37, m_stcloudobt.svalue6, 1);
    	stmtins.bindin(38, m_stcloudobt.sname7, 5);
    	stmtins.bindin(39, m_stcloudobt.svalue7, 1);
    	stmtins.bindin(40, m_stcloudobt.sname8, 5);
    	stmtins.bindin(41, m_stcloudobt.svalue8, 1);
    	stmtins.bindin(42, m_stcloudobt.sname9, 5);
    	stmtins.bindin(43, m_stcloudobt.svalue9, 1);
    	stmtins.bindin(44, m_stcloudobt.sname10, 5);
    	stmtins.bindin(45, m_stcloudobt.svalue10, 1);

    	stmtins.bindin(46, m_stcloudobt.checkcode, 4);
	}

    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    if (stmtsel.execute() != 0)
    {
		m_discarded++;
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount == 0)
	{
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
    	}
		m_inserted++;
	}

	if (iccount > 0)
	{
    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtupd.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
			return stmtupd.m_cda.rc;
    	}
		m_updated++;
	}

	return 0;
}

/* CPREFINEDATA类 */
CPREFINEDATA::CPREFINEDATA(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CPREFINEDATA::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_strefinedata1, 0, sizeof(struct st_refinedata1));
	memset(&m_strefinedata2, 0, sizeof(struct st_refinedata2));
	memset(&m_strefinedata3, 0, sizeof(struct st_refinedata3));
}

CPREFINEDATA::~CPREFINEDATA(){}

/* 把用空格分隔的记录拆分到m_strefinedata结构中 */
bool CPREFINEDATA::SplitBuffer(const char *strBuffer)
{
	int count;

	CCmdStr CmdStr;
	CmdStr.SplitToCmd(strBuffer, " ");
	count = CmdStr.CmdCount(); // 分段数

	if (!((count == 1) || (count == 2) || (count == 6) || (count == 22))) { m_discarded++; return false; }

	if (count == 1)
	{
		if (strlen(strBuffer) > 5) return false;
		strcpy(m_strefinedata1.totalstat, strBuffer);
	}

	if (count == 2)
	{
		CmdStr.GetValue(0, m_strefinedata1.prodcode);
		CmdStr.GetValue(1, m_strefinedata1.ddatetime);
		strcat(m_strefinedata1.ddatetime, "0000");  // 加入0分0秒, 方便转化.
		AddTime(m_strefinedata1.ddatetime, m_strefinedata1.ddatetime, 8*60*60, "yyyy-mm-dd hh:mi:ss");
		memset(&m_strefinedata3, 0, sizeof(struct st_refinedata3));
		memset(&m_strefinedata2, 0, sizeof(struct st_refinedata2));
	}

	if (count == 6)
	{
		CmdStr.GetValue(0, m_strefinedata2.obtid);
		CmdStr.GetValue(1, m_strefinedata2.lon);
		CmdStr.GetValue(2, m_strefinedata2.lat);
		CmdStr.GetValue(3, m_strefinedata2.height);
		CmdStr.GetValue(4, m_strefinedata2.agings);
		CmdStr.GetValue(5, m_strefinedata2.forprods);
		memset(&m_strefinedata3, 0, sizeof(struct st_refinedata3));
	}

	if (count == 22)
	{
		CmdStr.GetValue(0,  m_strefinedata3.timestep);
		CmdStr.GetValue(1,  m_strefinedata3.temperature);
		CmdStr.GetValue(2,  m_strefinedata3.rehumid);
		CmdStr.GetValue(3,  m_strefinedata3.wddirect);
		CmdStr.GetValue(4,  m_strefinedata3.wdspeed);
		CmdStr.GetValue(5,  m_strefinedata3.pressure);
		CmdStr.GetValue(6,  m_strefinedata3.precipit);
		CmdStr.GetValue(7,  m_strefinedata3.totalcloud);
		CmdStr.GetValue(8,  m_strefinedata3.lowcloud);
		CmdStr.GetValue(9,  m_strefinedata3.pweather);
		CmdStr.GetValue(10, m_strefinedata3.visibility);
		CmdStr.GetValue(11, m_strefinedata3.maxtemper);
		CmdStr.GetValue(12, m_strefinedata3.mintemper);
		CmdStr.GetValue(13, m_strefinedata3.maxrehumid);
		CmdStr.GetValue(14, m_strefinedata3.minrehumid);
		CmdStr.GetValue(15, m_strefinedata3.hcp24);
		CmdStr.GetValue(16, m_strefinedata3.hcp12);
		CmdStr.GetValue(17, m_strefinedata3.tcloud12);
		CmdStr.GetValue(18, m_strefinedata3.lcloud12);
		CmdStr.GetValue(19, m_strefinedata3.pweath12);
		CmdStr.GetValue(20, m_strefinedata3.wddir12);
		CmdStr.GetValue(21, m_strefinedata3.wdspeed12);
		m_total++;
	}

	return true;
}

/* 将结构体更新到表中 */
long CPREFINEDATA::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_REFINEDATA where prodcode = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and obtid = :3 and timestep = :4");
    	stmtsel.bindin(1,  m_strefinedata1.prodcode, 6);
    	stmtsel.bindin(2,  m_strefinedata1.ddatetime, 19);
    	stmtsel.bindin(3,  m_strefinedata2.obtid, 6);
    	stmtsel.bindin(4,  m_strefinedata3.timestep, 3);

    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_REFINEDATA set crttime=sysdate, totalstat=:3, lon=:5, lat=:6, height=:7, agings=:8, forprods=:9, temperature=:11, rehumid=:12, wddirect=:13, wdspeed=:14, pressure=:15, precipit=:16, totalcloud=:17, lowcloud=:18, pweather=:19, visibility=:20, maxtemper=:21, mintemper=:22, maxrehumid=:23, minrehumid=:24, hcp24=:25, hcp12=:26, tcloud12=:27, lcloud12=:28, pweath12=:29, wddir12=:30, wdspeed12=:31 where prodcode=:1 and ddatetime=to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and obtid=:4, timestep=:10");
		stmtupd.bindin(1,  m_strefinedata1.prodcode, 6);
		stmtupd.bindin(2,  m_strefinedata1.ddatetime, 19);
		stmtupd.bindin(3,  m_strefinedata1.totalstat, 5);

		stmtupd.bindin(4,  m_strefinedata2.obtid, 6);
		stmtupd.bindin(5,  m_strefinedata2.lon, 6);
		stmtupd.bindin(6,  m_strefinedata2.lat, 5);
		stmtupd.bindin(7,  m_strefinedata2.height, 6);
		stmtupd.bindin(8,  m_strefinedata2.agings, 2);
		stmtupd.bindin(9,  m_strefinedata2.forprods, 2);

		stmtupd.bindin(10, m_strefinedata3.timestep, 3);
		stmtupd.bindin(11, m_strefinedata3.temperature, 6);
		stmtupd.bindin(12, m_strefinedata3.rehumid, 6);
		stmtupd.bindin(13, m_strefinedata3.wddirect, 6);
		stmtupd.bindin(14, m_strefinedata3.wdspeed, 6);
		stmtupd.bindin(15, m_strefinedata3.pressure, 6);
		stmtupd.bindin(16, m_strefinedata3.precipit, 6);
		stmtupd.bindin(17, m_strefinedata3.totalcloud, 6);
		stmtupd.bindin(18, m_strefinedata3.lowcloud, 6);
		stmtupd.bindin(19, m_strefinedata3.pweather, 6);
		stmtupd.bindin(20, m_strefinedata3.visibility, 6);
		stmtupd.bindin(21, m_strefinedata3.maxtemper, 6);
		stmtupd.bindin(22, m_strefinedata3.mintemper, 6);
		stmtupd.bindin(23, m_strefinedata3.maxrehumid, 6);
		stmtupd.bindin(24, m_strefinedata3.minrehumid, 6);
		stmtupd.bindin(25, m_strefinedata3.hcp24, 6);
		stmtupd.bindin(26, m_strefinedata3.hcp12, 6);
		stmtupd.bindin(27, m_strefinedata3.tcloud12, 6);
		stmtupd.bindin(28, m_strefinedata3.lcloud12, 6);
		stmtupd.bindin(29, m_strefinedata3.pweath12, 6);
		stmtupd.bindin(30, m_strefinedata3.wddir12, 6);
		stmtupd.bindin(31, m_strefinedata3.wdspeed12, 6);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_REFINEDATA(keyid, crttime, prodcode, ddatetime, totalstat, obtid, lon, lat, height, agings, forprods, timestep, temperature, rehumid, wddirect, wdspeed, pressure, precipit, totalcloud, lowcloud, pweather, visibility, maxtemper, mintemper, maxrehumid, minrehumid, hcp24, hcp12, tcloud12, lcloud12, pweath12, wddir12, wdspeed12) values(SEQ_REFINEDATA.nextval, sysdate, :1, to_date(:2,'yyyy-mm-dd hh24:mi:ss'), :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, :16, :17, :18, :19, :20, :21, :22, :23, :24, :25, :26, :27, :28, :29, :30, :31)");
		stmtins.bindin(1,  m_strefinedata1.prodcode, 6);
		stmtins.bindin(2,  m_strefinedata1.ddatetime, 19);
		stmtins.bindin(3,  m_strefinedata1.totalstat, 5);

		stmtins.bindin(4,  m_strefinedata2.obtid, 6);
		stmtins.bindin(5,  m_strefinedata2.lon, 6);
		stmtins.bindin(6,  m_strefinedata2.lat, 5);
		stmtins.bindin(7,  m_strefinedata2.height, 6);
		stmtins.bindin(8,  m_strefinedata2.agings, 2);
		stmtins.bindin(9,  m_strefinedata2.forprods, 2);

		stmtins.bindin(10, m_strefinedata3.timestep, 3);
		stmtins.bindin(11, m_strefinedata3.temperature, 6);
		stmtins.bindin(12, m_strefinedata3.rehumid, 6);
		stmtins.bindin(13, m_strefinedata3.wddirect, 6);
		stmtins.bindin(14, m_strefinedata3.wdspeed, 6);
		stmtins.bindin(15, m_strefinedata3.pressure, 6);
		stmtins.bindin(16, m_strefinedata3.precipit, 6);
		stmtins.bindin(17, m_strefinedata3.totalcloud, 6);
		stmtins.bindin(18, m_strefinedata3.lowcloud, 6);
		stmtins.bindin(19, m_strefinedata3.pweather, 6);
		stmtins.bindin(20, m_strefinedata3.visibility, 6);
		stmtins.bindin(21, m_strefinedata3.maxtemper, 6);
		stmtins.bindin(22, m_strefinedata3.mintemper, 6);
		stmtins.bindin(23, m_strefinedata3.maxrehumid, 6);
		stmtins.bindin(24, m_strefinedata3.minrehumid, 6);
		stmtins.bindin(25, m_strefinedata3.hcp24, 6);
		stmtins.bindin(26, m_strefinedata3.hcp12, 6);
		stmtins.bindin(27, m_strefinedata3.tcloud12, 6);
		stmtins.bindin(28, m_strefinedata3.lcloud12, 6);
		stmtins.bindin(29, m_strefinedata3.pweath12, 6);
		stmtins.bindin(30, m_strefinedata3.wddir12, 6);
		stmtins.bindin(31, m_strefinedata3.wdspeed12, 6);
	}

    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    if (stmtsel.execute() != 0)
    {
		m_discarded++;
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount == 0)
	{
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
    	}
		m_inserted++;
	}

	if (iccount > 0)
	{
    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtupd.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
			return stmtupd.m_cda.rc;
    	}
		m_updated++;
	}

	return 0;
}

/* CACIDOBT类 */
CACIDOBT::CACIDOBT(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CACIDOBT::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_stacidobt, 0, sizeof(struct st_acidobt));
}

CACIDOBT::~CACIDOBT() {}

/* 把用空格分隔的头部记录拆分到vsignallog容器中 */
bool CACIDOBT::SplithdBuffer(const char *strBuffer)
{
	memset(&m_stacidobt, 0, sizeof(struct st_acidobt));

	CCmdStr CmdStr;
	char strtemp[8];

	CmdStr.SplitToCmd(strBuffer, " ", true);
	if (CmdStr.CmdCount() != 5) { m_discarded++; return false; }

	CmdStr.GetValue(0, m_stacidobt.acidobt);

	// 处理纬度信息
	CmdStr.GetValue(1, m_stacidobt.lat);
	memset(strtemp, 0, sizeof(strtemp));
	strncpy(strtemp, m_stacidobt.lat, 2);
	strcat(strtemp, "d");
	strcat(strtemp, m_stacidobt.lat+2);
	strncpy(m_stacidobt.lat, sizeof(m_stacidobt.lat), strtemp, 5);
	strcat(m_stacidobt.lat, "\'");
	strcat(m_stacidobt.lat, strtemp + 5);
	DeleteLChar(m_stacidobt.lat, '0');
	if (m_stacidobt.lat[0] == 'd') LPad(m_stacidobt.lat, '0', 1+strlen(m_stacidobt.lat));


	// 处理经度信息
	CmdStr.GetValue(2, m_stacidobt.lon);
	memset(strtemp, 0, sizeof(strtemp));
	strncpy(strtemp, m_stacidobt.lon, 3);
	strcat(strtemp, "d");
	strcat(strtemp, m_stacidobt.lon+3);
	strncpy(m_stacidobt.lon, sizeof(m_stacidobt.lon), strtemp, 6);
	strcat(m_stacidobt.lon, "\'");
	strcat(m_stacidobt.lon, strtemp + 6);
	DeleteLChar(m_stacidobt.lon, '0');
	if (m_stacidobt.lon[0] == 'd') LPad(m_stacidobt.lon, '0', 1+strlen(m_stacidobt.lon));

	CmdStr.GetValue(3, m_stacidobt.height);
	CmdStr.GetValue(4, m_stacidobt.obmethod);

	return true;
}

/* 把用空格分隔的中间记录拆分到vsignallog容器中 */
bool CACIDOBT::SplitinBuffer(const char *strBuffer)
{
	m_total++;
	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, " ", true);
	if (CmdStr.CmdCount() != 28) { m_discarded++; return false; }

	char strtmp[20];
	memset(strtmp, 0, sizeof(strtmp));
	CmdStr.GetValue(0, m_stacidobt.ddatetime, 14);
	strncpy(m_stacidobt.tbegin, m_stacidobt.ddatetime, 6);
	CmdStr.GetValue(1, strtmp);
	strcat(m_stacidobt.tbegin, strtmp);
	CmdStr.GetValue(2, strtmp);
	strncpy(m_stacidobt.tend, m_stacidobt.ddatetime, 6);
	strcat(m_stacidobt.tend, strtmp);

	CmdStr.GetValue(3,  m_stacidobt.prept);
	CmdStr.GetValue(4,  m_stacidobt.t);
	CmdStr.GetValue(5,  m_stacidobt.PH1);
	CmdStr.GetValue(6,  m_stacidobt.PH2);
	CmdStr.GetValue(7,  m_stacidobt.PH3);
	CmdStr.GetValue(8,  m_stacidobt.PHarv);
	CmdStr.GetValue(9,  m_stacidobt.K1);
	CmdStr.GetValue(10, m_stacidobt.K2);
	CmdStr.GetValue(11, m_stacidobt.K3);
	CmdStr.GetValue(12, m_stacidobt.Karv);
	CmdStr.GetValue(13, m_stacidobt.ret);
	CmdStr.GetValue(14, m_stacidobt.rePH1);
	CmdStr.GetValue(15, m_stacidobt.rePH2);
	CmdStr.GetValue(16, m_stacidobt.rePH3);
	CmdStr.GetValue(17, m_stacidobt.rePHarv);
	CmdStr.GetValue(18, m_stacidobt.reK1);
	CmdStr.GetValue(19, m_stacidobt.reK2);
	CmdStr.GetValue(20, m_stacidobt.reK3);
	CmdStr.GetValue(21, m_stacidobt.reKarv);

	CmdStr.GetValue(22, strtmp);
	strncpy(m_stacidobt.wddir14, strtmp, 3);
	DeleteLChar(m_stacidobt.wddir14,'P');
	strcpy(m_stacidobt.wdsp14, strtmp + 3);

	CmdStr.GetValue(23, strtmp);
	strncpy(m_stacidobt.wddir20, sizeof(m_stacidobt.wddir20), strtmp, 3);
	DeleteLChar(m_stacidobt.wddir20,'P');
	strcpy(m_stacidobt.wdsp20, strtmp + 3);

	CmdStr.GetValue(24, strtmp);
	strncpy(m_stacidobt.wddir02, sizeof(m_stacidobt.wddir02), strtmp, 3);
	DeleteLChar(m_stacidobt.wddir02,'P');
	strcpy(m_stacidobt.wdsp02, strtmp + 3);

	CmdStr.GetValue(25, strtmp);
	strncpy(m_stacidobt.wddir08, sizeof(m_stacidobt.wddir08), strtmp, 3);
	DeleteLChar(m_stacidobt.wddir08,'P');
	strcpy(m_stacidobt.wdsp08, strtmp + 3);

	memset(strtmp, 0, sizeof(strtmp));
	CmdStr.GetValue(26, strtmp);
	strncpy(m_stacidobt.wtph1, strtmp, 2);
	strncpy(m_stacidobt.wtph2, sizeof(m_stacidobt.wtph2), strtmp+2, 2);
	strncpy(m_stacidobt.wtph3, sizeof(m_stacidobt.wtph3), strtmp+4, 2);
	strncpy(m_stacidobt.wtph4, sizeof(m_stacidobt.wtph4), strtmp+6, 2);

	/*
	memset(strtmp, 0, sizeof(strtmp));
	CmdStr.GetValue(27, strtmp);
	strncpy(m_stacidobt.acidnote, strtmp, 5);
	*/
	CmdStr.GetValue(27, m_stacidobt.acidnote);
	DeleteRChar(m_stacidobt.acidnote,'=');

	return true;
}

/* 把结构体m_stacidobt更新到数据库中 */
long CACIDOBT::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_ACIDOBT where acidobt = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
    	stmtsel.bindin(1, m_stacidobt.acidobt, 5);
    	stmtsel.bindin(2, m_stacidobt.ddatetime, 19);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_ACIDOBT set crttime=sysdate, lat=:2, lon=:3, height=:4, obmethod=:5, tbegin=to_date(:7, 'yyyy-mm-dd hh24:mi:ss'), tend=to_date(:8, 'yyyy-mm-dd hh24:mi:ss'), prept=:9, t=:10, PH1=:11, PH2=:12, PH3=:13, PHarv=:14, K1=:15, K2=:16, K3=:17, Karv=:18, ret=:19, rePH1=:20, rePH2=:21, rePH3=:22, rePHarv=:23, reK1=:24, reK2=:25, reK3=:26, reKarv=:27, wddir14=:28, wdsp14=:29, wddir20=:30, wdsp20=:31, wddir02=:32, wdsp02=:33, wddir08=:34, wdsp08=:35, wtph1=:36, wtph2=:37, wtph3=:38, wtph4=:39, acidnote=:40 where acidobt=:1 and ddatetime=to_date(:6, 'yyyy-mm-dd hh24:mi:ss')");
        stmtupd.bindin(1, m_stacidobt.acidobt, 5);
        stmtupd.bindin(2, m_stacidobt.lat, 7);
        stmtupd.bindin(3, m_stacidobt.lon, 8);
        stmtupd.bindin(4, m_stacidobt.height, 6);
        stmtupd.bindin(5, m_stacidobt.obmethod, 1);
        stmtupd.bindin(6, m_stacidobt.ddatetime, 19);
        stmtupd.bindin(7, m_stacidobt.tbegin, 19);
        stmtupd.bindin(8, m_stacidobt.tend, 19);
        stmtupd.bindin(9, m_stacidobt.prept, 5);
        stmtupd.bindin(10, m_stacidobt.t, 3);
        stmtupd.bindin(11, m_stacidobt.PH1, 4);
        stmtupd.bindin(12, m_stacidobt.PH2, 4);
        stmtupd.bindin(13, m_stacidobt.PH3, 4);
        stmtupd.bindin(14, m_stacidobt.PHarv, 4);
        stmtupd.bindin(15, m_stacidobt.K1, 5);
        stmtupd.bindin(16, m_stacidobt.K2, 5);
        stmtupd.bindin(17, m_stacidobt.K3, 5);
        stmtupd.bindin(18, m_stacidobt.Karv, 5);
        stmtupd.bindin(19, m_stacidobt.ret, 3);
        stmtupd.bindin(20, m_stacidobt.rePH1, 4);
        stmtupd.bindin(21, m_stacidobt.rePH2, 4);
        stmtupd.bindin(22, m_stacidobt.rePH3, 4);
        stmtupd.bindin(23, m_stacidobt.rePHarv, 4);
        stmtupd.bindin(24, m_stacidobt.reK1, 5);
        stmtupd.bindin(25, m_stacidobt.reK2, 5);
        stmtupd.bindin(26, m_stacidobt.reK3, 5);
        stmtupd.bindin(27, m_stacidobt.reKarv, 5);
        stmtupd.bindin(28, m_stacidobt.wddir14, 3);
        stmtupd.bindin(29, m_stacidobt.wdsp14, 3);
        stmtupd.bindin(30, m_stacidobt.wddir20, 3);
        stmtupd.bindin(31, m_stacidobt.wdsp20, 3);
        stmtupd.bindin(32, m_stacidobt.wddir02, 3);
        stmtupd.bindin(33, m_stacidobt.wdsp02, 3);
        stmtupd.bindin(34, m_stacidobt.wddir08, 3);
        stmtupd.bindin(35, m_stacidobt.wdsp08, 3);
        stmtupd.bindin(36, m_stacidobt.wtph1, 2);
        stmtupd.bindin(37, m_stacidobt.wtph2, 2);
        stmtupd.bindin(38, m_stacidobt.wtph3, 2);
        stmtupd.bindin(39, m_stacidobt.wtph4, 2);
        stmtupd.bindin(40, m_stacidobt.acidnote, 5);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_ACIDOBT(keyid, crttime, acidobt, lat, lon, height, obmethod, ddatetime, tbegin, tend, prept, t, PH1, PH2, PH3, PHarv, K1, K2, K3, Karv, ret, rePH1, rePH2, rePH3, rePHarv, reK1, reK2, reK3, reKarv, wddir14, wdsp14, wddir20, wdsp20, wddir02, wdsp02, wddir08, wdsp08, wtph1, wtph2, wtph3, wtph4, acidnote) values(SEQ_ACIDOBT.nextval, sysdate, :1, :2, :3, :4, :5, to_date(:6, 'yyyy-mm-dd hh24:mi:ss'), to_date(:7, 'yyyy-mm-dd hh24:mi:ss'), to_date(:8, 'yyyy-mm-dd hh24:mi:ss'), :9, :10, :11, :12, :13, :14, :15, :16, :17, :18, :19, :20, :21, :22, :23, :24, :25, :26, :27, :28, :29, :30, :31, :32, :33, :34, :35, :36, :37, :38, :39, :40)");
        stmtins.bindin(1, m_stacidobt.acidobt, 5);
        stmtins.bindin(2, m_stacidobt.lat, 7);
        stmtins.bindin(3, m_stacidobt.lon, 8);
        stmtins.bindin(4, m_stacidobt.height, 6);
        stmtins.bindin(5, m_stacidobt.obmethod, 1);
        stmtins.bindin(6, m_stacidobt.ddatetime, 19);
        stmtins.bindin(7, m_stacidobt.tbegin, 19);
        stmtins.bindin(8, m_stacidobt.tend, 19);
        stmtins.bindin(9, m_stacidobt.prept, 5);
        stmtins.bindin(10, m_stacidobt.t, 3);
        stmtins.bindin(11, m_stacidobt.PH1, 4);
        stmtins.bindin(12, m_stacidobt.PH2, 4);
        stmtins.bindin(13, m_stacidobt.PH3, 4);
        stmtins.bindin(14, m_stacidobt.PHarv, 4);
        stmtins.bindin(15, m_stacidobt.K1, 5);
        stmtins.bindin(16, m_stacidobt.K2, 5);
        stmtins.bindin(17, m_stacidobt.K3, 5);
        stmtins.bindin(18, m_stacidobt.Karv, 5);
        stmtins.bindin(19, m_stacidobt.ret, 3);
        stmtins.bindin(20, m_stacidobt.rePH1, 4);
        stmtins.bindin(21, m_stacidobt.rePH2, 4);
        stmtins.bindin(22, m_stacidobt.rePH3, 4);
        stmtins.bindin(23, m_stacidobt.rePHarv, 4);
        stmtins.bindin(24, m_stacidobt.reK1, 5);
        stmtins.bindin(25, m_stacidobt.reK2, 5);
        stmtins.bindin(26, m_stacidobt.reK3, 5);
        stmtins.bindin(27, m_stacidobt.reKarv, 5);
        stmtins.bindin(28, m_stacidobt.wddir14, 3);
        stmtins.bindin(29, m_stacidobt.wdsp14, 3);
        stmtins.bindin(30, m_stacidobt.wddir20, 3);
        stmtins.bindin(31, m_stacidobt.wdsp20, 3);
        stmtins.bindin(32, m_stacidobt.wddir02, 3);
        stmtins.bindin(33, m_stacidobt.wdsp02, 3);
        stmtins.bindin(34, m_stacidobt.wddir08, 3);
        stmtins.bindin(35, m_stacidobt.wdsp08, 3);
        stmtins.bindin(36, m_stacidobt.wtph1, 2);
        stmtins.bindin(37, m_stacidobt.wtph2, 2);
        stmtins.bindin(38, m_stacidobt.wtph3, 2);
        stmtins.bindin(39, m_stacidobt.wtph4, 2);
        stmtins.bindin(40, m_stacidobt.acidnote, 5);
	}

    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    if (stmtsel.execute() != 0)
    {
		m_discarded++;
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount == 0)
	{
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
    	}
		m_inserted++;
	}

	if (iccount > 0)
	{
    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtupd.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
			return stmtupd.m_cda.rc;
    	}
		m_updated++;
	}

	return 0;
}

/* CAUTOSTVIS类 */
CAUTOSTVIS::CAUTOSTVIS(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CAUTOSTVIS::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_stautostvis1, 0, sizeof(struct st_autostvis1));
	memset(&m_stautostvis2, 0, sizeof(struct st_autostvis2));
}

CAUTOSTVIS::~CAUTOSTVIS(){}

/* 把用空格分隔的头部记录拆分到m_stautostvis1结构中 */
bool CAUTOSTVIS::SplithdBuffer(const char *strBuffer)
{
	CCmdStr CmdStr;
	char strtemp[300];
	memset(strtemp, 0, sizeof(strtemp));

	CmdStr.SplitToCmd(strBuffer, " ");
	int counts = 0; counts = CmdStr.CmdCount();
	CmdStr.GetValue(0, strtemp);

	if ( (counts != 3) || (strlen(strtemp) != 28) ) return false;

	memset(&m_stautostvis1, 0, sizeof(struct st_autostvis1));
	memset(&m_stautostvis2, 0, sizeof(struct st_autostvis2));
	strncpy(m_stautostvis1.statid, strtemp, 5);
	strncpy(m_stautostvis1.ddatetime, sizeof(m_stautostvis1.ddatetime), strtemp+5, 8);

	return true;
}

/* 把用空格分隔的中间记录拆分到m_stautostvis2结构中 */
bool CAUTOSTVIS::SplitinBuffer(const char *strBuffer)
{
	m_total++;
	memset(&m_stautostvis2, 0, sizeof(struct st_autostvis2));

	char strtmp[50];
	if (strlen(strBuffer) != 66) { m_discarded++; return false; }

	//strncpy(m_stautostvis2.hourminu, strBuffer, 4);
	memset(strtmp, 0, sizeof(strtmp));
	strncpy(strtmp, sizeof(strtmp), strBuffer, 4);
	strncpy(m_stautostvis2.hourminu, strtmp, 2);
	strcat(m_stautostvis2.hourminu, ":");
	strcat(m_stautostvis2.hourminu, strtmp+2);

	strncpy(m_stautostvis2.minuvis, sizeof(m_stautostvis2.minuvis), strBuffer+4, 6);

	strncpy(m_stautostvis2.minumaxvis, sizeof(m_stautostvis2.minumaxvis), strBuffer+10, 6);

	//strncpy(m_stautostvis2.minumaxvist, sizeof(m_stautostvis2.minumaxvist), strBuffer+16, 4);
	memset(strtmp, 0, sizeof(strtmp));
	strncpy(strtmp, sizeof(strtmp), strBuffer+16, 4);
	strncpy(m_stautostvis2.minumaxvist, strtmp, 2);
	strcat(m_stautostvis2.minumaxvist, ":");
	strcat(m_stautostvis2.minumaxvist, strtmp+2);

	strncpy(m_stautostvis2.minuminvis, sizeof(m_stautostvis2.minuminvis), strBuffer+20, 6);

	//strncpy(m_stautostvis2.minuminvist, sizeof(m_stautostvis2.minuminvist), strBuffer+26, 4);
	memset(strtmp, 0, sizeof(strtmp));
	strncpy(strtmp, sizeof(strtmp), strBuffer+26, 4);
	strncpy(m_stautostvis2.minuminvist, strtmp, 2);
	strcat(m_stautostvis2.minuminvist, ":");
	strcat(m_stautostvis2.minuminvist, strtmp+2);

	strncpy(m_stautostvis2.minuvis10, sizeof(m_stautostvis2.minuvis10), strBuffer+30, 6);
	strncpy(m_stautostvis2.minumaxvis10, sizeof(m_stautostvis2.minumaxvis10), strBuffer+36, 6);

	//strncpy(m_stautostvis2.minumaxvist10, sizeof(m_stautostvis2.minumaxvist10), strBuffer+42, 4);
	memset(strtmp, 0, sizeof(strtmp));
	strncpy(strtmp, sizeof(strtmp), strBuffer+42, 4);
	strncpy(m_stautostvis2.minumaxvist10, strtmp, 2);
	strcat(m_stautostvis2.minumaxvist10, ":");
	strcat(m_stautostvis2.minumaxvist10, strtmp+2);

	strncpy(m_stautostvis2.minuminvis10, sizeof(m_stautostvis2.minuminvis10), strBuffer+46, 6);

	//strncpy(m_stautostvis2.minuminvist10, sizeof(m_stautostvis2.minuminvist10), strBuffer+52, 4);
	memset(strtmp, 0, sizeof(strtmp));
	strncpy(strtmp, sizeof(strtmp), strBuffer+52, 4);
	strncpy(m_stautostvis2.minuminvist10, strtmp, 2);
	strcat(m_stautostvis2.minuminvist10, ":");
	strcat(m_stautostvis2.minuminvist10, strtmp+2);

	strncpy(m_stautostvis2.qccode, sizeof(m_stautostvis2.qccode), strBuffer+56, 10);

	return true;
}

/* 将结构体更新到表中 */
long CAUTOSTVIS::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_AUTOSTVIS where statid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd') and hourminu = :3");
    	stmtsel.bindin(1,  m_stautostvis1.statid, 5);
    	stmtsel.bindin(2,  m_stautostvis1.ddatetime, 10);
    	stmtsel.bindin(3,  m_stautostvis2.hourminu, 5);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_AUTOSTVIS set crttime=sysdate, minuvis=:4, minumaxvis=:5, minumaxvist=:6, minuminvis=:7, minuminvist=:8, minuvis10=:9, minumaxvis10=:10, minumaxvist10=:11, minuminvis10=:12, minuminvist10=:13, qccode=:14 where statid=:1 and ddatetime=to_date(:2, 'yyyy-mm-dd') and hourminu=:3");

        stmtupd.bindin(1,  m_stautostvis1.statid, 5);
        stmtupd.bindin(2,  m_stautostvis1.ddatetime, 10);
        stmtupd.bindin(3,  m_stautostvis2.hourminu, 5);
        stmtupd.bindin(4,  m_stautostvis2.minuvis, 6);
        stmtupd.bindin(5,  m_stautostvis2.minumaxvis, 6);
        stmtupd.bindin(6,  m_stautostvis2.minumaxvist, 5);
        stmtupd.bindin(7,  m_stautostvis2.minuminvis, 6);
    	stmtupd.bindin(8,  m_stautostvis2.minuminvist, 5);
    	stmtupd.bindin(9,  m_stautostvis2.minuvis10, 6);
    	stmtupd.bindin(10, m_stautostvis2.minumaxvis10, 6);
    	stmtupd.bindin(11, m_stautostvis2.minumaxvist10, 5);
    	stmtupd.bindin(12, m_stautostvis2.minuminvis10, 6);
    	stmtupd.bindin(13, m_stautostvis2.minuminvist10, 5);
    	stmtupd.bindin(14, m_stautostvis2.qccode, 10);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_AUTOSTVIS (keyid, crttime, statid, ddatetime, hourminu, minuvis, minumaxvis, minumaxvist, minuminvis, minuminvist, minuvis10, minumaxvis10, minumaxvist10, minuminvis10, minuminvist10, qccode) values(SEQ_AUTOSTVIS.nextval, sysdate, :1, to_date(:2, 'yyyy-mm-dd'), :3, :4, :5, :6, :7, :8, :9, :10, :11, :12, :13, :14)");
		
        stmtins.bindin(1,  m_stautostvis1.statid, 5);
        stmtins.bindin(2,  m_stautostvis1.ddatetime, 10);
        stmtins.bindin(3,  m_stautostvis2.hourminu, 5);
        stmtins.bindin(4,  m_stautostvis2.minuvis, 6);
        stmtins.bindin(5,  m_stautostvis2.minumaxvis, 6);
        stmtins.bindin(6,  m_stautostvis2.minumaxvist, 5);
        stmtins.bindin(7,  m_stautostvis2.minuminvis, 6);
    	stmtins.bindin(8,  m_stautostvis2.minuminvist, 5);
    	stmtins.bindin(9,  m_stautostvis2.minuvis10, 6);
    	stmtins.bindin(10, m_stautostvis2.minumaxvis10, 6);
    	stmtins.bindin(11, m_stautostvis2.minumaxvist10, 5);
    	stmtins.bindin(12, m_stautostvis2.minuminvis10, 6);
    	stmtins.bindin(13, m_stautostvis2.minuminvist10, 5);
    	stmtins.bindin(14, m_stautostvis2.qccode, 10);
	}

    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    if (stmtsel.execute() != 0)
    {
		m_discarded++;
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount == 0)
	{
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
    	}
		m_inserted++;
	}

	if (iccount > 0)
	{
    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtupd.execute() != 0)
    	{
			m_discarded++;
            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
			return stmtupd.m_cda.rc;
    	}
		m_updated++;
	}

	return 0;
}

///*
///* CSURFZFILES类 */
//CSURFZFILES::CSURFZFILES(connection *conn, CLogFile *logfile)
//{
//	initdata();
//	m_conn = conn; m_logfile = logfile;
//}
//
//void CSURFZFILES::initdata()
//{
//	m_total = m_inserted = m_updated = m_discarded = 0;
//	m_conn = 0; m_logfile = 0;
//	memset(&m_stsurfzfiles, 0, sizeof(struct st_surfzfiles));
//}
//
//CSURFZFILES::~CSURFZFILES(){}
//
///* 把用空格分隔的头部记录拆分到m_stsurfzfiles结构中 */
//bool CSURFZFILES::SplitBuffer(const char *strBuffer)
//{
//	memset(&m_stsurfzfiles, 0, sizeof(struct st_surfzfiles));
//
//	CCmdStr CmdStr;
//
//	CmdStr.SplitToCmd(strBuffer, " ");
//	//if (CmdStr.CmdCount() != 9) { m_discarded++; return false; }
//
//	CmdStr.GetValue(0,  m_stsurfzfiles.classmk);    // 分类标志
//	CmdStr.GetValue(1,  m_stsurfzfiles.innaid);     // 国际编号
//	CmdStr.GetValue(2, &m_stsurfzfiles.pathno);     // 路径数据记录的行数
//	CmdStr.GetValue(3,  m_stsurfzfiles.cycno);      // 气旋编号
//	CmdStr.GetValue(4,  m_stsurfzfiles.cycid);      // 国内编号
//	CmdStr.GetValue(5, &m_stsurfzfiles.endsign);    // 终结标志
//	CmdStr.GetValue(6, &m_stsurfzfiles.intervals);  // 数据的间隔时间, 小时, 目前均为6小时
//	CmdStr.GetValue(7,  m_stsurfzfiles.egname);     // 英文名称
//	CmdStr.GetValue(8,  m_stsurfzfiles.fmtdate);    // 数据集成的日期
//	strcat(m_stsurfzfiles.fmtdate, "000000");       // 加入0时0分0秒, 方便转化.
//
//	return true;
//}

///* 将结构体更新到表中 */
//long CSURFZFILES::InsertTable()
//{
//    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
//	if (stmtsel.m_state == 0)
//	{
//		stmtsel.connect(m_conn);
//    	stmtsel.prepare("select count(*) from T_SURFZFILES where cycno = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss') and egname = :3");
//    	stmtsel.bindin(1,  m_stsurfzfiles.cycno, 4);
//    	stmtsel.bindin(2,  m_stsurfzfiles.ddatetime, 19);
//    	stmtsel.bindin(3,  m_stsurfzfiles.egname, 20);
//    	stmtsel.bindout(1, &iccount);
//	}
//
//	if (stmtupd.m_state == 0)
//	{
//		stmtupd.connect(m_conn);
//		stmtupd.prepare("update T_SURFZFILES set crttime=sysdate, innaid=:1, classmk=:4, fmtdate=to_date(:5, 'yyyy-mm-dd hh24:mi:ss'), pathno=:6, intervals=:7, endsign=:8, intensitysign=:9, lat=:10, lon=:11, pressure=:12, wdmaxspeed=:13, wdavespeed=:14 cycid=:15 where cycno=:3 and egname=:2 and ddatetime=to_date(:16, 'yyyy-mm-dd hh24:mi:ss')");
//
//        stmtupd.bindin(1,  m_stsurfzfiles.innaid, 4);
//        stmtupd.bindin(2,  m_stsurfzfiles.egname, 20);
//        stmtupd.bindin(3,  m_stsurfzfiles.cycno, 4);
//        stmtupd.bindin(4,  m_stsurfzfiles.classmk, 5);
//        stmtupd.bindin(5,  m_stsurfzfiles.fmtdate, 19);
//        stmtupd.bindin(6, &m_stsurfzfiles.pathno);
//        stmtupd.bindin(7, &m_stsurfzfiles.intervals);
//    	stmtupd.bindin(8, &m_stsurfzfiles.endsign);
//    	stmtupd.bindin(9, &m_stsurfzfiles.intensitysign);
//    	stmtupd.bindin(10,&m_stsurfzfiles.lat);
//    	stmtupd.bindin(11,&m_stsurfzfiles.lon);
//    	stmtupd.bindin(12,&m_stsurfzfiles.pressure);
//    	stmtupd.bindin(13,&m_stsurfzfiles.wdmaxspeed);
//    	stmtupd.bindin(14, m_stsurfzfiles.wdavespeed, 3);
//    	stmtupd.bindin(15, m_stsurfzfiles.cycid, 4);
//    	stmtupd.bindin(16, m_stsurfzfiles.ddatetime, 19);
//	}
//
//	if (stmtins.m_state == 0)
//	{
//    	stmtins.connect(m_conn);
//    	stmtins.prepare("insert into T_SURFZFILES(keyid, crttime, innaid, egname, cycno, classmk, fmtdate, pathno, intervals, endsign, intensitysign, lat, lon, pressure, wdmaxspeed, wdavespeed, cycid, ddatetime) values(SEQ_SURFZFILES.nextval, sysdate, :1, :2, :3, :4, to_date(:5, 'yyyy-mm-dd hh24:mi:ss'), :6, :7, :8, :9, :10, :11, :12, :13, :14, :15, to_date(:16, 'yyyy-mm-dd hh24:mi:ss'))");
//		
//        stmtins.bindin(1,  m_stsurfzfiles.innaid, 4);
//        stmtins.bindin(2,  m_stsurfzfiles.egname, 20);
//        stmtins.bindin(3,  m_stsurfzfiles.cycno, 4);
//        stmtins.bindin(4,  m_stsurfzfiles.classmk, 5);
//        stmtins.bindin(5,  m_stsurfzfiles.fmtdate, 19);
//        stmtins.bindin(6, &m_stsurfzfiles.pathno);
//        stmtins.bindin(7, &m_stsurfzfiles.intervals);
//    	stmtins.bindin(8, &m_stsurfzfiles.endsign);
//    	stmtins.bindin(9, &m_stsurfzfiles.intensitysign);
//    	stmtins.bindin(10,&m_stsurfzfiles.lat);
//    	stmtins.bindin(11,&m_stsurfzfiles.lon);
//    	stmtins.bindin(12,&m_stsurfzfiles.pressure);
//    	stmtins.bindin(13,&m_stsurfzfiles.wdmaxspeed);
//    	stmtins.bindin(14, m_stsurfzfiles.wdavespeed, 3);
//    	stmtins.bindin(15, m_stsurfzfiles.cycid, 4);
//    	stmtins.bindin(16, m_stsurfzfiles.ddatetime, 19);
//	}
//
//    /* 判断记录是否重复, 若重复则修改列值, 否则插入 */
//    if (stmtsel.execute() != 0)
//    {
//		m_discarded++;
//        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
//		return stmtsel.m_cda.rc;
//    }
//
//    iccount = 0;
//    stmtsel.next();
//
//    if (iccount == 0)
//	{
//    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
//    	if (stmtins.execute() != 0)
//    	{
//			m_discarded++;
//            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
//			return stmtins.m_cda.rc;
//    	}
//		m_inserted++;
//	}
//
//	if (iccount > 0)
//	{
//    	/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
//    	if (stmtupd.execute() != 0)
//    	{
//			m_discarded++;
//            m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
//			return stmtupd.m_cda.rc;
//    	}
//		m_updated++;
//	}
//
//	return 0;
//}

/* Cquyujisuan类 */
Cquyujisuan::Cquyujisuan(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void Cquyujisuan::initdata()
{
	m_conn = 0; m_logfile = 0;
	memset(&m_stquyujisuan, 0, sizeof(struct st_quyujisuan));
	memset(&m_straind, 0, sizeof(struct st_raind));
	vquyujisuan.clear();
}

Cquyujisuan::~Cquyujisuan()
{
	vquyujisuan.clear();
}

bool Cquyujisuan::pstjisuan(const char *cols, const char *tname, const char *andstr)
{
    char strsql[4096];
    memset(strsql, 0, sizeof(strsql));
    sprintf(strsql, "select %s from %s where 1 = 1 %s order by obtid, ddatetime asc", cols, tname, andstr);

	sqlstatement stmtsel(m_conn);
    stmtsel.prepare(strsql);
    memset(&m_stquyujisuan,0,sizeof(struct st_quyujisuan));
    stmtsel.bindout(1,m_stquyujisuan.ddatetime,19);
    stmtsel.bindout(2,m_stquyujisuan.obtid,10);
    stmtsel.bindout(3,m_stquyujisuan.station,2);
    stmtsel.bindout(4,m_stquyujisuan.baoformat,1);
    stmtsel.bindout(5,m_stquyujisuan.bps,6);
    stmtsel.bindout(6,m_stquyujisuan.ys,8);
    stmtsel.bindout(7,m_stquyujisuan.w3a,6);
    stmtsel.bindout(8,m_stquyujisuan.wdidf,6);
    stmtsel.bindout(9,m_stquyujisuan.wdidd,6);
    stmtsel.bindout(10,m_stquyujisuan.wd2df,6);
    stmtsel.bindout(11,m_stquyujisuan.wd2dd,6);
    stmtsel.bindout(12,m_stquyujisuan.wd10df,6);
    stmtsel.bindout(13,m_stquyujisuan.wd10dd,6);
    stmtsel.bindout(14,m_stquyujisuan.wd3smaxdf,6);
    stmtsel.bindout(15,m_stquyujisuan.wd3smaxdd,6);
    stmtsel.bindout(16,m_stquyujisuan.wd3smaxtime,2);
    stmtsel.bindout(17,m_stquyujisuan.wd10maxdf,6);
    stmtsel.bindout(18,m_stquyujisuan.wd10maxdd,6);
    stmtsel.bindout(19,m_stquyujisuan.wd10maxtime,2);
    stmtsel.bindout(20,m_stquyujisuan.wd3daymax,6);
    stmtsel.bindout(21,m_stquyujisuan.wf3daymax,6);
    stmtsel.bindout(22,m_stquyujisuan.wd3daymaxtime,4);
    stmtsel.bindout(23,m_stquyujisuan.wd10daymax,6);
    stmtsel.bindout(24,m_stquyujisuan.wf10daymax,6);
    stmtsel.bindout(25,m_stquyujisuan.wd10daymaxtime,4);
    stmtsel.bindout(26,m_stquyujisuan.t,6);
    stmtsel.bindout(27,m_stquyujisuan.maxt,6);
    stmtsel.bindout(28,m_stquyujisuan.maxttime,2);
    stmtsel.bindout(29,m_stquyujisuan.mint,6);
    stmtsel.bindout(30,m_stquyujisuan.minttime,2);
    stmtsel.bindout(31,m_stquyujisuan.daymaxt,6);
	stmtsel.bindout(32,m_stquyujisuan.daymaxttime,4);
    stmtsel.bindout(33,m_stquyujisuan.daymint,6);
    stmtsel.bindout(34,m_stquyujisuan.dayminttime,4);
    stmtsel.bindout(35,m_stquyujisuan.rh,6);
    stmtsel.bindout(36,m_stquyujisuan.maxrh,6);
    stmtsel.bindout(37,m_stquyujisuan.maxrhtime,2);
    stmtsel.bindout(38,m_stquyujisuan.minrh,6);
    stmtsel.bindout(39,m_stquyujisuan.minrhtime,2);
    stmtsel.bindout(40,m_stquyujisuan.dp,6);
    stmtsel.bindout(41,m_stquyujisuan.p,6);
    stmtsel.bindout(42,m_stquyujisuan.maxp,6);
    stmtsel.bindout(43,m_stquyujisuan.maxptime,2);
    stmtsel.bindout(44,m_stquyujisuan.minp,6);
    stmtsel.bindout(45,m_stquyujisuan.minptime,2);
    stmtsel.bindout(46,m_stquyujisuan.p0,6);
    stmtsel.bindout(47,m_stquyujisuan.hourrf,6);
    stmtsel.bindout(48,m_stquyujisuan.dayrf,6);
    stmtsel.bindout(49,m_stquyujisuan.rfmark1,6);
    stmtsel.bindout(50,m_stquyujisuan.minrf,50);
    stmtsel.bindout(51,m_stquyujisuan.wpv,6);
    stmtsel.bindout(52,m_stquyujisuan.othfields,2000);
    stmtsel.bindout(53,m_stquyujisuan.rddatetime,2);
    stmtsel.bindout(54,m_stquyujisuan.procsts,1);

    if (stmtsel.execute() != 0)
    {
        m_logfile -> Write("select %s failed.\n%s\n%s\n", tname,stmtsel.m_cda.message,stmtsel.m_sql); return false;
    }

	while (1)
	{
		memset(&m_stquyujisuan, 0, sizeof(struct st_quyujisuan));
		if (stmtsel.next() != 0) break;
		vquyujisuan.push_back(m_stquyujisuan); // 得到所有记录的容器
	}

	if (stmtsel.m_cda.rpc > 0) m_logfile -> Write("本次一共%d条记录。\n",stmtsel.m_cda.rpc);

	return true;
}

long Cquyujisuan::phourd(const char *cols, const char *tname, const char *andstr)
{
	vquyujisuan.clear();
	// 得到所有结果的容器
	pstjisuan(cols, tname, andstr);

	if (stmtins_hour.m_state == 0)
	{
    	stmtins_hour.connect(m_conn);
		// 生成sql语句字符串
        stmtins_hour.prepare("insert into T_GDOBTHOURD (keyid,crttime,ddatetime, obtid, station, baoformat, bps, ys, w3a, wdidf, wdidd, wd2df, wd2dd, wd10df, wd10dd, wd3smaxdf, wd3smaxdd, wd3smaxtime, wd10maxdf, wd10maxdd, wd10maxtime, wd3daymax, wf3daymax, wd3daymaxtime, wd10daymax, wf10daymax, wd10daymaxtime, t, maxt, maxttime, mint, minttime, daymaxt, daymaxttime, daymint, dayminttime, rh, maxrh, maxrhtime, minrh, minrhtime, dp, p, maxp, maxptime, minp, minptime, p0, hourrf, dayrf, rfmark1, minrf, wpv, othfields, rddatetime, procsts) values (SEQ_AREAOBTHOURD.nextval,sysdate,to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44,:45,:46,:47,:48,:49,:50,:51,:52,:53,:54)");
        stmtins_hour.bindin(1,m_stquyujisuan.ddatetime,19);
        stmtins_hour.bindin(2,m_stquyujisuan.obtid,10);
        stmtins_hour.bindin(3,m_stquyujisuan.station,2);
        stmtins_hour.bindin(4,m_stquyujisuan.baoformat,1);
        stmtins_hour.bindin(5,m_stquyujisuan.bps,6);
        stmtins_hour.bindin(6,m_stquyujisuan.ys,8);
        stmtins_hour.bindin(7,m_stquyujisuan.w3a,6);
        stmtins_hour.bindin(8,m_stquyujisuan.wdidf,6);
        stmtins_hour.bindin(9,m_stquyujisuan.wdidd,6);
        stmtins_hour.bindin(10,m_stquyujisuan.wd2df,6);
        stmtins_hour.bindin(11,m_stquyujisuan.wd2dd,6);
        stmtins_hour.bindin(12,m_stquyujisuan.wd10df,6);
        stmtins_hour.bindin(13,m_stquyujisuan.wd10dd,6);
        stmtins_hour.bindin(14,m_stquyujisuan.wd3smaxdf,6);
        stmtins_hour.bindin(15,m_stquyujisuan.wd3smaxdd,6);
        stmtins_hour.bindin(16,m_stquyujisuan.wd3smaxtime,2);
        stmtins_hour.bindin(17,m_stquyujisuan.wd10maxdf,6);
        stmtins_hour.bindin(18,m_stquyujisuan.wd10maxdd,6);
        stmtins_hour.bindin(19,m_stquyujisuan.wd10maxtime,2);
        stmtins_hour.bindin(20,m_stquyujisuan.wd3daymax,6);
        stmtins_hour.bindin(21,m_stquyujisuan.wf3daymax,6);
        stmtins_hour.bindin(22,m_stquyujisuan.wd3daymaxtime,4);
        stmtins_hour.bindin(23,m_stquyujisuan.wd10daymax,6);
        stmtins_hour.bindin(24,m_stquyujisuan.wf10daymax,6);
        stmtins_hour.bindin(25,m_stquyujisuan.wd10daymaxtime,4);
        stmtins_hour.bindin(26,m_stquyujisuan.t,6);
        stmtins_hour.bindin(27,m_stquyujisuan.maxt,6);
		stmtins_hour.bindin(28,m_stquyujisuan.maxttime,2);
        stmtins_hour.bindin(29,m_stquyujisuan.mint,6);
        stmtins_hour.bindin(30,m_stquyujisuan.minttime,2);
        stmtins_hour.bindin(31,m_stquyujisuan.daymaxt,6);
        stmtins_hour.bindin(32,m_stquyujisuan.daymaxttime,4);
        stmtins_hour.bindin(33,m_stquyujisuan.daymint,6);
        stmtins_hour.bindin(34,m_stquyujisuan.dayminttime,4);
        stmtins_hour.bindin(35,m_stquyujisuan.rh,6);
        stmtins_hour.bindin(36,m_stquyujisuan.maxrh,6);
        stmtins_hour.bindin(37,m_stquyujisuan.maxrhtime,2);
        stmtins_hour.bindin(38,m_stquyujisuan.minrh,6);
        stmtins_hour.bindin(39,m_stquyujisuan.minrhtime,2);
        stmtins_hour.bindin(40,m_stquyujisuan.dp,6);
        stmtins_hour.bindin(41,m_stquyujisuan.p,6);
        stmtins_hour.bindin(42,m_stquyujisuan.maxp,6);
        stmtins_hour.bindin(43,m_stquyujisuan.maxptime,2);
        stmtins_hour.bindin(44,m_stquyujisuan.minp,6);
        stmtins_hour.bindin(45,m_stquyujisuan.minptime,2);
        stmtins_hour.bindin(46,m_stquyujisuan.p0,6);
        stmtins_hour.bindin(47,m_stquyujisuan.hourrf,6);
        stmtins_hour.bindin(48,m_stquyujisuan.dayrf,6);
        stmtins_hour.bindin(49,m_stquyujisuan.rfmark1,6);
        stmtins_hour.bindin(50,m_stquyujisuan.minrf,50);
        stmtins_hour.bindin(51,m_stquyujisuan.wpv,6);
        stmtins_hour.bindin(52,m_stquyujisuan.othfields,2000);
        stmtins_hour.bindin(53,m_stquyujisuan.rddatetime,2);
        stmtins_hour.bindin(54,m_stquyujisuan.procsts,1);
	}

	for (int ii = 0; ii < vquyujisuan.size(); ii++)
	{
		memset(&m_stquyujisuan, 0, sizeof(struct st_quyujisuan));
		memcpy(&m_stquyujisuan, &vquyujisuan[ii], sizeof(struct st_quyujisuan));
    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins_hour.execute() != 0)
    	{
            m_logfile -> Write("stmtins_hour.execute() failed.\n%s\n%s\n", stmtins_hour.m_sql, stmtins_hour.m_cda.message);
			return stmtins_hour.m_cda.rc;
    	}
	}

	return 0;
}

long Cquyujisuan::preald(const char *cols, const char *tname, const char *andstr)
{
	vquyujisuan.clear();
	// 得到所有结果的容器
	pstjisuan(cols, tname, andstr);

	if (stmtsel_real.m_state == 0)
	{
		stmtsel_real.connect(m_conn);
		stmtsel_real.prepare("select count(*) from T_GDOBTREALD where obtid = :1");
		stmtsel_real.bindin(1, m_stquyujisuan.obtid, 10);
		stmtsel_real.bindout(1, &iccount);
	}

	if (stmtins_real.m_state == 0)
	{
    	stmtins_real.connect(m_conn);
		// 生成sql语句字符串
        stmtins_real.prepare("insert into T_GDOBTREALD (keyid,crttime,ddatetime, obtid, station, baoformat, bps, ys, w3a, wdidf, wdidd, wd2df, wd2dd, wd10df, wd10dd, wd3smaxdf, wd3smaxdd, wd3smaxtime, wd10maxdf, wd10maxdd, wd10maxtime, wd3daymax, wf3daymax, wd3daymaxtime, wd10daymax, wf10daymax, wd10daymaxtime, t, maxt, maxttime, mint, minttime, daymaxt, daymaxttime, daymint, dayminttime, rh, maxrh, maxrhtime, minrh, minrhtime, dp, p, maxp, maxptime, minp, minptime, p0, hourrf, dayrf, rfmark1, minrf, wpv, othfields, rddatetime, procsts) values (SEQ_AREAOBTHOURD.nextval,sysdate,to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16,:17,:18,:19,:20,:21,:22,:23,:24,:25,:26,:27,:28,:29,:30,:31,:32,:33,:34,:35,:36,:37,:38,:39,:40,:41,:42,:43,:44,:45,:46,:47,:48,:49,:50,:51,:52,:53,:54)");
        stmtins_real.bindin(1,m_stquyujisuan.ddatetime,19);
        stmtins_real.bindin(2,m_stquyujisuan.obtid,10);
        stmtins_real.bindin(3,m_stquyujisuan.station,2);
        stmtins_real.bindin(4,m_stquyujisuan.baoformat,1);
        stmtins_real.bindin(5,m_stquyujisuan.bps,6);
        stmtins_real.bindin(6,m_stquyujisuan.ys,8);
        stmtins_real.bindin(7,m_stquyujisuan.w3a,6);
        stmtins_real.bindin(8,m_stquyujisuan.wdidf,6);
        stmtins_real.bindin(9,m_stquyujisuan.wdidd,6);
        stmtins_real.bindin(10,m_stquyujisuan.wd2df,6);
        stmtins_real.bindin(11,m_stquyujisuan.wd2dd,6);
        stmtins_real.bindin(12,m_stquyujisuan.wd10df,6);
        stmtins_real.bindin(13,m_stquyujisuan.wd10dd,6);
        stmtins_real.bindin(14,m_stquyujisuan.wd3smaxdf,6);
        stmtins_real.bindin(15,m_stquyujisuan.wd3smaxdd,6);
        stmtins_real.bindin(16,m_stquyujisuan.wd3smaxtime,2);
        stmtins_real.bindin(17,m_stquyujisuan.wd10maxdf,6);
        stmtins_real.bindin(18,m_stquyujisuan.wd10maxdd,6);
        stmtins_real.bindin(19,m_stquyujisuan.wd10maxtime,2);
        stmtins_real.bindin(20,m_stquyujisuan.wd3daymax,6);
        stmtins_real.bindin(21,m_stquyujisuan.wf3daymax,6);
        stmtins_real.bindin(22,m_stquyujisuan.wd3daymaxtime,4);
        stmtins_real.bindin(23,m_stquyujisuan.wd10daymax,6);
        stmtins_real.bindin(24,m_stquyujisuan.wf10daymax,6);
        stmtins_real.bindin(25,m_stquyujisuan.wd10daymaxtime,4);
        stmtins_real.bindin(26,m_stquyujisuan.t,6);
        stmtins_real.bindin(27,m_stquyujisuan.maxt,6);
		stmtins_real.bindin(28,m_stquyujisuan.maxttime,2);
        stmtins_real.bindin(29,m_stquyujisuan.mint,6);
        stmtins_real.bindin(30,m_stquyujisuan.minttime,2);
        stmtins_real.bindin(31,m_stquyujisuan.daymaxt,6);
        stmtins_real.bindin(32,m_stquyujisuan.daymaxttime,4);
        stmtins_real.bindin(33,m_stquyujisuan.daymint,6);
        stmtins_real.bindin(34,m_stquyujisuan.dayminttime,4);
        stmtins_real.bindin(35,m_stquyujisuan.rh,6);
        stmtins_real.bindin(36,m_stquyujisuan.maxrh,6);
        stmtins_real.bindin(37,m_stquyujisuan.maxrhtime,2);
        stmtins_real.bindin(38,m_stquyujisuan.minrh,6);
        stmtins_real.bindin(39,m_stquyujisuan.minrhtime,2);
        stmtins_real.bindin(40,m_stquyujisuan.dp,6);
        stmtins_real.bindin(41,m_stquyujisuan.p,6);
        stmtins_real.bindin(42,m_stquyujisuan.maxp,6);
        stmtins_real.bindin(43,m_stquyujisuan.maxptime,2);
        stmtins_real.bindin(44,m_stquyujisuan.minp,6);
        stmtins_real.bindin(45,m_stquyujisuan.minptime,2);
        stmtins_real.bindin(46,m_stquyujisuan.p0,6);
        stmtins_real.bindin(47,m_stquyujisuan.hourrf,6);
        stmtins_real.bindin(48,m_stquyujisuan.dayrf,6);
        stmtins_real.bindin(49,m_stquyujisuan.rfmark1,6);
        stmtins_real.bindin(50,m_stquyujisuan.minrf,50);
        stmtins_real.bindin(51,m_stquyujisuan.wpv,6);
        stmtins_real.bindin(52,m_stquyujisuan.othfields,2000);
        stmtins_real.bindin(53,m_stquyujisuan.rddatetime,2);
        stmtins_real.bindin(54,m_stquyujisuan.procsts,1);
	}

	if (stmtupd_real.m_state == 0)
	{
		stmtupd_real.connect(m_conn);
		stmtupd_real.prepare("update T_GDOBTREALD set crttime=sysdate,ddatetime=to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),station=:2,baoformat=:3,bps=:4,ys=:5,w3a=:6,wdidf=:7,wdidd=:8,wd2df=:9,wd2dd=:10,wd10df=:11,wd10dd=:12,wd3smaxdf=:13,wd3smaxdd=:14,wd3smaxtime=:15,wd10maxdf=:16,wd10maxdd=:17,wd10maxtime=:18,wd3daymax=:19,wf3daymax=:20,wd3daymaxtime=:21,wd10daymax=:22,wf10daymax=:23,wd10daymaxtime=:24,t=:25,maxt=:26,maxttime=:27,mint=:28,minttime=:29,daymaxt=:30,daymaxttime=:31,daymint=:32,dayminttime=:33,rh=:34,maxrh=:35,maxrhtime=:36,minrh=:37,minrhtime=:38,dp=:39,p=:40,maxp=:41,maxptime=:42,minp=:43,minptime=:44,p0=:45,hourrf=:46,dayrf=:47,rfmark1=:48,minrf=:49,wpv=:50,othfields=:51,rddatetime=:52,procsts=:53 where (obtid=:54 and ddatetime<to_date(:55, 'yyyy-mm-dd hh24:mi:ss'))");
        stmtupd_real.bindin(1,m_stquyujisuan.ddatetime,19);
        stmtupd_real.bindin(2,m_stquyujisuan.station,2);
        stmtupd_real.bindin(3,m_stquyujisuan.baoformat,1);
        stmtupd_real.bindin(4,m_stquyujisuan.bps,6);
        stmtupd_real.bindin(5,m_stquyujisuan.ys,8);
        stmtupd_real.bindin(6,m_stquyujisuan.w3a,6);
        stmtupd_real.bindin(7,m_stquyujisuan.wdidf,6);
        stmtupd_real.bindin(8,m_stquyujisuan.wdidd,6);
        stmtupd_real.bindin(9,m_stquyujisuan.wd2df,6);
        stmtupd_real.bindin(10,m_stquyujisuan.wd2dd,6);
        stmtupd_real.bindin(11,m_stquyujisuan.wd10df,6);
        stmtupd_real.bindin(12,m_stquyujisuan.wd10dd,6);
        stmtupd_real.bindin(13,m_stquyujisuan.wd3smaxdf,6);
        stmtupd_real.bindin(14,m_stquyujisuan.wd3smaxdd,6);
        stmtupd_real.bindin(15,m_stquyujisuan.wd3smaxtime,2);
        stmtupd_real.bindin(16,m_stquyujisuan.wd10maxdf,6);
        stmtupd_real.bindin(17,m_stquyujisuan.wd10maxdd,6);
        stmtupd_real.bindin(18,m_stquyujisuan.wd10maxtime,2);
        stmtupd_real.bindin(19,m_stquyujisuan.wd3daymax,6);
        stmtupd_real.bindin(20,m_stquyujisuan.wf3daymax,6);
        stmtupd_real.bindin(21,m_stquyujisuan.wd3daymaxtime,4);
        stmtupd_real.bindin(22,m_stquyujisuan.wd10daymax,6);
        stmtupd_real.bindin(23,m_stquyujisuan.wf10daymax,6);
        stmtupd_real.bindin(24,m_stquyujisuan.wd10daymaxtime,4);
        stmtupd_real.bindin(25,m_stquyujisuan.t,6);
        stmtupd_real.bindin(26,m_stquyujisuan.maxt,6);
		stmtupd_real.bindin(27,m_stquyujisuan.maxttime,2);
        stmtupd_real.bindin(28,m_stquyujisuan.mint,6);
        stmtupd_real.bindin(29,m_stquyujisuan.minttime,2);
        stmtupd_real.bindin(30,m_stquyujisuan.daymaxt,6);
        stmtupd_real.bindin(31,m_stquyujisuan.daymaxttime,4);
        stmtupd_real.bindin(32,m_stquyujisuan.daymint,6);
        stmtupd_real.bindin(33,m_stquyujisuan.dayminttime,4);
        stmtupd_real.bindin(34,m_stquyujisuan.rh,6);
        stmtupd_real.bindin(35,m_stquyujisuan.maxrh,6);
        stmtupd_real.bindin(36,m_stquyujisuan.maxrhtime,2);
        stmtupd_real.bindin(37,m_stquyujisuan.minrh,6);
        stmtupd_real.bindin(38,m_stquyujisuan.minrhtime,2);
        stmtupd_real.bindin(39,m_stquyujisuan.dp,6);
        stmtupd_real.bindin(40,m_stquyujisuan.p,6);
        stmtupd_real.bindin(41,m_stquyujisuan.maxp,6);
        stmtupd_real.bindin(42,m_stquyujisuan.maxptime,2);
        stmtupd_real.bindin(43,m_stquyujisuan.minp,6);
        stmtupd_real.bindin(44,m_stquyujisuan.minptime,2);
        stmtupd_real.bindin(45,m_stquyujisuan.p0,6);
        stmtupd_real.bindin(46,m_stquyujisuan.hourrf,6);
        stmtupd_real.bindin(47,m_stquyujisuan.dayrf,6);
        stmtupd_real.bindin(48,m_stquyujisuan.rfmark1,6);
        stmtupd_real.bindin(49,m_stquyujisuan.minrf,50);
        stmtupd_real.bindin(50,m_stquyujisuan.wpv,6);
        stmtupd_real.bindin(51,m_stquyujisuan.othfields,2000);
        stmtupd_real.bindin(52,m_stquyujisuan.rddatetime,2);
        stmtupd_real.bindin(53,m_stquyujisuan.procsts,1);
        stmtupd_real.bindin(54,m_stquyujisuan.obtid,10);
        stmtupd_real.bindin(55,m_stquyujisuan.ddatetime,19);
	}

	for (int ii = 0; ii < vquyujisuan.size(); ii++)
	{
		memset(&m_stquyujisuan, 0, sizeof(struct st_quyujisuan));
		memcpy(&m_stquyujisuan, &vquyujisuan[ii], sizeof(struct st_quyujisuan));

		if (stmtsel_real.execute() != 0)
		{
			m_logfile->Write("stmtsel_real.execute() failed.\n%s\n%s\n",stmtsel_real.m_sql,stmtsel_real.m_cda.message);
			return stmtsel_real.m_cda.rc;
		}

		iccount = 0;
		stmtsel_real.next();

		if (iccount == 0)
		{
    		/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    		if (stmtins_real.execute() != 0)
    		{
        	    m_logfile -> Write("stmtins_real.execute() failed.\n%s\n%s\n", stmtins_real.m_sql, stmtins_real.m_cda.message);
				return stmtins_real.m_cda.rc;
    		}
		}

		if (iccount > 0)
		{
			/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
            if (stmtupd_real.execute() != 0)
            {
                m_logfile -> Write("stmtupd_real.execute() failed.\n%s\n%s\n", stmtupd_real.m_sql, stmtupd_real.m_cda.message);
                return stmtupd_real.m_cda.rc;
            }
		}
	}

	return 0;
}

long Cquyujisuan::praind()
{
	if (stmtins_rain.m_state == 0)
	{
    	stmtins_rain.connect(m_conn);
		// 生成sql语句字符串
        stmtins_rain.prepare("insert into T_GDOBTRAIND (keyid,crttime,ddatetime, obtid,hourrf,minr,r05m,r10m,r30m,r01h,r02h,r03h,r06h,r12h,r24h,r48h,r72h,hdrbz) values (SEQ_AREAOBTRAIND.nextval,sysdate,to_date(:1, 'yyyy-mm-dd hh24:mi:ss'),:2,:3,:4,:5,:6,:7,:8,:9,:10,:11,:12,:13,:14,:15,:16)");
        stmtins_rain.bindin(1,  m_straind.ddatetime,19);
        stmtins_rain.bindin(2,  m_straind.obtid, 10);
        stmtins_rain.bindin(3, &m_straind.hourrf);
        stmtins_rain.bindin(4, &m_straind.minr);
        stmtins_rain.bindin(5, &m_straind.r05m);
        stmtins_rain.bindin(6, &m_straind.r10m);
        stmtins_rain.bindin(7, &m_straind.r30m);
        stmtins_rain.bindin(8, &m_straind.r01h);
        stmtins_rain.bindin(9, &m_straind.r02h);
        stmtins_rain.bindin(10,&m_straind.r03h);
        stmtins_rain.bindin(11,&m_straind.r06h);
        stmtins_rain.bindin(12,&m_straind.r12h);
        stmtins_rain.bindin(13,&m_straind.r24h);
        stmtins_rain.bindin(14,&m_straind.r48h);
        stmtins_rain.bindin(15,&m_straind.r72h);
        stmtins_rain.bindin(16, m_straind.hdrbz,20);
	}

	// 分别存放当前时间的前5、10、20分钟...前的时间字符串
	char timetmp5[20];
	char timetmp10[20];
	char timetmp20[20];
	char timetmp30[20];
	char timetmp60[20];
	char timetmp120[20];
	char timetmp180[20];
	char timetmp360[20];
	char timetmp720[20];
	char timetmp1440[20];
	char timetmp2880[20];
	int  valrf = 0;
	int  valrf2 = 0;
	sqlstatement stmtsel_rain5(m_conn);
	sqlstatement stmtsel_rain10(m_conn);
	sqlstatement stmtsel_rain30(m_conn);
	sqlstatement stmtsel_rain30_2(m_conn);
	sqlstatement stmtsel_rain60(m_conn);
	sqlstatement stmtsel_rain120(m_conn);
	sqlstatement stmtsel_rain180(m_conn);
	sqlstatement stmtsel_rain360(m_conn);
	sqlstatement stmtsel_rain720(m_conn);
	sqlstatement stmtsel_rain1440(m_conn);
	sqlstatement stmtsel_rain2880(m_conn);
	sqlstatement stmtsel_rain4320(m_conn);
	for (int ii = 0; ii < vquyujisuan.size(); ii++)
	{
		memset(timetmp5, 0, sizeof(timetmp5));
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
		memset(&m_stquyujisuan, 0, sizeof(struct st_quyujisuan));
		memset(&m_straind, 0, sizeof(struct st_raind));
		memcpy(&m_stquyujisuan, &vquyujisuan[ii], sizeof(struct st_quyujisuan));

		/* 以下为计算过程 */
		strcpy(m_straind.ddatetime, m_stquyujisuan.ddatetime);
		strcpy(m_straind.obtid, m_stquyujisuan.obtid);
		m_straind.hourrf = atoi(m_stquyujisuan.hourrf);
		AddTime(m_stquyujisuan.ddatetime, timetmp5, 0 - 5 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp10, 0 - 10 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp20, 0 - 20 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp30, 0 - 30 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp60, 0 - 60 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp120, 0 - 120 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp180, 0 - 180 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp360, 0 - 360 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp720, 0 - 720 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp1440, 0 - 1440 * 60, "yyyymmddhhmiss");
		AddTime(m_stquyujisuan.ddatetime, timetmp2880, 0 - 2880 * 60, "yyyymmddhhmiss");

		if (strcmp(timetmp5+10, "0000") == 0) // 若为5分钟,则5分钟滑动雨量就是该时刻的小时雨量
		{
			m_straind.minr = m_straind.hourrf;
			m_straind.r05m = m_straind.hourrf;
		}
		else
		{
			// 查询前五分钟的小时雨量数据,求5分钟滑动雨量
			stmtsel_rain5.prepare("select hourrf from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
			stmtsel_rain5.bindin(1, m_straind.obtid, 10);
			stmtsel_rain5.bindin(2, timetmp5, 19);
			stmtsel_rain5.bindout(1, &valrf);
			if (stmtsel_rain5.execute() != 0)
			{
				m_logfile -> Write("stmtsel_rain5.execute() failed.\n%s\n%s\n", stmtsel_rain5.m_sql, stmtsel_rain5.m_cda.message);
        	    return stmtsel_rain5.m_cda.rc;
			}

			valrf = 0;
			if (stmtsel_rain5.next() == 0)
			{
				m_straind.minr = m_straind.hourrf - valrf; // 当前5分钟滑动
				m_straind.r05m = m_straind.hourrf - valrf; // 当前5分钟滑动
			}
			else { m_straind.r05m = 0; m_straind.minr = 0; }
		}

		// 查询前五分钟的5分钟滑动雨量,求10分钟滑动雨量
		stmtsel_rain10.prepare("select r05m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
		stmtsel_rain10.bindin(1, m_straind.obtid, 10);
		stmtsel_rain10.bindin(2, timetmp5, 19);
		stmtsel_rain10.bindout(1, &valrf);
		if (stmtsel_rain10.execute() != 0)
		{
			m_logfile -> Write("stmtsel_rain10.execute() failed.\n%s\n%s\n", stmtsel_rain10.m_sql, stmtsel_rain10.m_cda.message);
            return stmtsel_rain10.m_cda.rc;
		}

		valrf = 0;
		if (stmtsel_rain10.next() == 0)
		{
			m_straind.r10m = m_straind.r05m + valrf; // 当前10分钟滑动
		}
		else m_straind.r10m = m_straind.r05m;

		// 查询前10分钟和前20分钟的10分钟滑动雨量,求30分钟的滑动雨量
		stmtsel_rain30.prepare("select r10m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain30.bindin(1, m_straind.obtid, 10);
        stmtsel_rain30.bindin(2, timetmp10, 19);
        stmtsel_rain30.bindout(1, &valrf);
        if (stmtsel_rain30.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain30.execute() failed.\n%s\n%s\n", stmtsel_rain30.m_sql, stmtsel_rain30.m_cda.message);
            return stmtsel_rain30.m_cda.rc;
        }
        valrf = 0;
		if (stmtsel_rain30.next() != 0) valrf = 0;

		stmtsel_rain30_2.prepare("select r10m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain30_2.bindin(1, m_straind.obtid, 10);
        stmtsel_rain30_2.bindin(2, timetmp20, 19);
        stmtsel_rain30_2.bindout(1, &valrf2);
        if (stmtsel_rain30_2.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain30_2.execute() failed.\n%s\n%s\n", stmtsel_rain30_2.m_sql, stmtsel_rain30_2.m_cda.message);
            return stmtsel_rain30_2.m_cda.rc;
        }
        valrf2 = 0;
		if (stmtsel_rain30_2.next() != 0) valrf2 = 0;

		m_straind.r30m = valrf + valrf2 + m_straind.r10m; // 30分钟滑动雨量

		// 查询前30分钟的滑动雨量,求1小时的滑动雨量
		stmtsel_rain60.prepare("select r30m from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain60.bindin(1, m_straind.obtid, 10);
        stmtsel_rain60.bindin(2, timetmp30, 19);
        stmtsel_rain60.bindout(1, &valrf);
        if (stmtsel_rain60.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain60.execute() failed.\n%s\n%s\n", stmtsel_rain60.m_sql, stmtsel_rain60.m_cda.message);
            return stmtsel_rain60.m_cda.rc;
        }
        valrf = 0;

		if (stmtsel_rain60.next() != 0) valrf = 0;
		m_straind.r01h = valrf + m_straind.r30m;  // 1小时滑动雨量

		// 查询前1小时的滑动雨量,求2小时的滑动雨量
		stmtsel_rain120.prepare("select r01h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain120.bindin(1, m_straind.obtid, 10);
        stmtsel_rain120.bindin(2, timetmp60, 19);
        stmtsel_rain120.bindout(1, &valrf);
        if (stmtsel_rain120.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain120.execute() failed.\n%s\n%s\n", stmtsel_rain120.m_sql, stmtsel_rain120.m_cda.message);
            return stmtsel_rain120.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain120.next() != 0) valrf = 0;
        m_straind.r02h = valrf + m_straind.r01h;  // 2小时滑动雨量

		// 查询前1小时前的2小时滑动雨量,求3小时的滑动雨量
		stmtsel_rain180.prepare("select r02h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain180.bindin(1, m_straind.obtid, 10);
        stmtsel_rain180.bindin(2, timetmp60, 19);
        stmtsel_rain180.bindout(1, &valrf);
        if (stmtsel_rain180.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain180.execute() failed.\n%s\n%s\n", stmtsel_rain180.m_sql, stmtsel_rain180.m_cda.message);
            return stmtsel_rain180.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain180.next() != 0) valrf = 0;
        m_straind.r03h = valrf + m_straind.r01h;  // 3小时滑动雨量

		// 查询前3小时前的3小时滑动雨量,求6小时的滑动雨量
		stmtsel_rain360.prepare("select r03h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain360.bindin(1, m_straind.obtid, 10);
        stmtsel_rain360.bindin(2, timetmp180, 19);
        stmtsel_rain360.bindout(1, &valrf);
        if (stmtsel_rain360.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain360.execute() failed.\n%s\n%s\n", stmtsel_rain360.m_sql, stmtsel_rain360.m_cda.message);
            return stmtsel_rain360.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain360.next() != 0) valrf = 0;
        m_straind.r06h = valrf + m_straind.r03h;  // 6小时滑动雨量

		// 查询前6小时前的6小时滑动雨量,求12小时的滑动雨量
		stmtsel_rain720.prepare("select r06h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain720.bindin(1, m_straind.obtid, 10);
        stmtsel_rain720.bindin(2, timetmp360, 19);
        stmtsel_rain720.bindout(1, &valrf);
        if (stmtsel_rain720.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain720.execute() failed.\n%s\n%s\n", stmtsel_rain720.m_sql, stmtsel_rain720.m_cda.message);
            return stmtsel_rain720.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain720.next() != 0) valrf = 0;
        m_straind.r12h = valrf + m_straind.r06h;  // 12小时滑动雨量

		// 查询前12小时前的12小时滑动雨量,求24小时的滑动雨量
		stmtsel_rain1440.prepare("select r12h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain1440.bindin(1, m_straind.obtid, 10);
        stmtsel_rain1440.bindin(2, timetmp720, 19);
        stmtsel_rain1440.bindout(1, &valrf);
        if (stmtsel_rain1440.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain1440.execute() failed.\n%s\n%s\n", stmtsel_rain1440.m_sql, stmtsel_rain1440.m_cda.message);
            return stmtsel_rain1440.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain1440.next() != 0) valrf = 0;
        m_straind.r24h = valrf + m_straind.r12h;  // 24小时滑动雨量

		// 查询前24小时前的24小时滑动雨量,求48小时的滑动雨量
		stmtsel_rain2880.prepare("select r24h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain2880.bindin(1, m_straind.obtid, 10);
        stmtsel_rain2880.bindin(2, timetmp1440, 19);
        stmtsel_rain2880.bindout(1, &valrf);
        if (stmtsel_rain2880.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain2880.execute() failed.\n%s\n%s\n", stmtsel_rain2880.m_sql, stmtsel_rain2880.m_cda.message);
            return stmtsel_rain2880.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain2880.next() != 0) valrf = 0;
        m_straind.r48h = valrf + m_straind.r24h;  // 48小时滑动雨量

		// 查询前48小时前的24小时滑动雨量,求72小时的滑动雨量
		stmtsel_rain4320.prepare("select r24h from T_GDOBTRAIND where obtid = :1 and ddatetime = to_date(:2, 'yyyy-mm-dd hh24:mi:ss')");
        stmtsel_rain4320.bindin(1, m_straind.obtid, 10);
        stmtsel_rain4320.bindin(2, timetmp2880, 19);
        stmtsel_rain4320.bindout(1, &valrf);
        if (stmtsel_rain4320.execute() != 0)
        {
            m_logfile -> Write("stmtsel_rain4320.execute() failed.\n%s\n%s\n", stmtsel_rain4320.m_sql, stmtsel_rain4320.m_cda.message);
            return stmtsel_rain4320.m_cda.rc;
        }
        valrf = 0;

        if (stmtsel_rain4320.next() != 0) valrf = 0;
        m_straind.r72h = valrf + m_straind.r48h;  // 72小时滑动雨量

		strcpy(m_straind.hdrbz, "1");

    	/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    	if (stmtins_rain.execute() != 0)
    	{
            m_logfile -> Write("stmtins_rain.execute() failed.\n%s\n%s\n", stmtins_rain.m_sql, stmtins_rain.m_cda.message);
			return stmtins_rain.m_cda.rc;
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

/* CCOUNTRYSIGNAL类 */
CCOUNTRYSIGNAL::CCOUNTRYSIGNAL(connection *conn, CLogFile *logfile)
{
	initdata();
	m_conn = conn; m_logfile = logfile;
}

void CCOUNTRYSIGNAL::initdata()
{
	m_total = m_inserted = m_updated = m_discarded = 0;
	m_conn = 0; m_logfile = 0;
	memset(&m_stcountry, 0, sizeof(struct st_country));
	m_vsig.clear();
}

CCOUNTRYSIGNAL::~CCOUNTRYSIGNAL()
{
	m_vsig.clear();
}

/* 把JSON文件拆分到m_stcountry结构中 */
bool CCOUNTRYSIGNAL::SplitBuffer(const char *strBuffer)
{
	m_vsig.clear();
	memset(&m_stcountry, 0, sizeof(struct st_country));

	cJSON *strbuffer_json, *w;
	strbuffer_json = cJSON_Parse(strBuffer);

	if (!strbuffer_json)
	{
		m_logfile -> Write("JSON格式错误: %s.\n\n", cJSON_GetErrorPtr()); // 输出json格式错误的信息
		return false;
	}
	if (!cJSON_Print(strbuffer_json)) return false;

	// 获取数组对象
	w = cJSON_GetObjectItem(strbuffer_json, "w");

	// 获取数组对象的个数便于循环
	int arr_size = cJSON_GetArraySize(w); 

	// 获取w数组对象孩子节点
	cJSON *w_child = w -> child; // 子对象

	// 循环获取数组下每个字段的值并加入到结构体m_stcountry中
	for (int ii=0; ii<arr_size-1; ii++)
	{
		memset(&m_stcountry, 0, sizeof(struct st_country));

		strcpy(m_stcountry.province, cJSON_GetObjectItem(w_child, "w1")->valuestring);
		strcpy(m_stcountry.city, cJSON_GetObjectItem(w_child, "w2")->valuestring);
		strcpy(m_stcountry.county, cJSON_GetObjectItem(w_child, "w3")->valuestring);
		strcpy(m_stcountry.categoryid, cJSON_GetObjectItem(w_child, "w4")->valuestring);
		strcpy(m_stcountry.categoryname, cJSON_GetObjectItem(w_child, "w5")->valuestring);
		strcpy(m_stcountry.levelid, cJSON_GetObjectItem(w_child, "w6")->valuestring);
		strcpy(m_stcountry.levelname, cJSON_GetObjectItem(w_child, "w7")->valuestring);
		strcpy(m_stcountry.datetime, cJSON_GetObjectItem(w_child, "w8")->valuestring);
		strcpy(m_stcountry.content, cJSON_GetObjectItem(w_child, "w9")->valuestring);
		strcpy(m_stcountry.warsignalid, cJSON_GetObjectItem(w_child, "w10")->valuestring);
		strcpy(m_stcountry.webhtml, cJSON_GetObjectItem(w_child, "w11")->valuestring);
		m_vsig.push_back(m_stcountry);
	}

	cJSON_Delete(strbuffer_json); // 释放内存, 这里只需要释放strbuffer_json即可, 因为其它的都指向它

	return true;
}

/* 将结构体更新到表中 */
long CCOUNTRYSIGNAL::InsertTable()
{
    //不能在此处int iccount = 0;因为每次调用InsertTable,iccount地址改变
	if (stmtsel.m_state == 0)
	{
		stmtsel.connect(m_conn);
    	stmtsel.prepare("select count(*) from T_COUNTRYSIGNAL where province=:1 and city=:2 and county=:3 and categoryid=:4 and datetime=to_date(:5, 'yyyy-mm-dd hh24:mi:ss')");
    	stmtsel.bindin(1,  m_stcountry.province, 20);
    	stmtsel.bindin(2,  m_stcountry.city, 20);
    	stmtsel.bindin(3,  m_stcountry.county, 20);
    	stmtsel.bindin(4,  m_stcountry.categoryid, 4);
    	stmtsel.bindin(5,  m_stcountry.datetime, 20);
    	stmtsel.bindout(1, &iccount);
	}

	if (stmtupd.m_state == 0)
	{
		stmtupd.connect(m_conn);
		stmtupd.prepare("update T_COUNTRYSIGNAL set crttime=sysdate, province=:1, city=:2, county=:3, categoryid=:4, categoryname=:5, levelid=:6, levelname=:7, datetime=to_date(:8, 'yyyy-mm-dd hh24:mi:ss'), content=:9, warsignalid=:10, webhtml=:11");
    	stmtupd.bindin(1,  m_stcountry.province, 20);
    	stmtupd.bindin(2,  m_stcountry.city, 20);
    	stmtupd.bindin(3,  m_stcountry.county, 20);
    	stmtupd.bindin(4,  m_stcountry.categoryid, 4);
    	stmtupd.bindin(5,  m_stcountry.categoryname, 10);
    	stmtupd.bindin(6,  m_stcountry.levelid, 4);
    	stmtupd.bindin(7,  m_stcountry.levelname, 10);
    	stmtupd.bindin(8,  m_stcountry.datetime, 20);
    	stmtupd.bindin(9,  m_stcountry.content, 1000);
    	stmtupd.bindin(10, m_stcountry.warsignalid, 200);
    	stmtupd.bindin(11, m_stcountry.webhtml, 200);
	}

	if (stmtins.m_state == 0)
	{
    	stmtins.connect(m_conn);
    	stmtins.prepare("insert into T_COUNTRYSIGNAL(keyid, crttime, province, city, county, categoryid, categoryname, levelid, levelname, datetime, content, warsignalid, webhtml) values(SEQ_COUNTRYSIGNAL.nextval, sysdate, :1, :2, :3, :4, :5, :6, :7, to_date(:8, 'yyyy-mm-dd hh24:mi:ss'), :9, :10, :11)");
    	stmtins.bindin(1,  m_stcountry.province, 20);
    	stmtins.bindin(2,  m_stcountry.city, 20);
    	stmtins.bindin(3,  m_stcountry.county, 20);
    	stmtins.bindin(4,  m_stcountry.categoryid, 4);
    	stmtins.bindin(5,  m_stcountry.categoryname, 10);
    	stmtins.bindin(6,  m_stcountry.levelid, 4);
    	stmtins.bindin(7,  m_stcountry.levelname, 10);
    	stmtins.bindin(8,  m_stcountry.datetime, 20);
    	stmtins.bindin(9,  m_stcountry.content, 1000);
    	stmtins.bindin(10, m_stcountry.warsignalid, 200);
    	stmtins.bindin(11, m_stcountry.webhtml, 200);
	}

	for (int ii=0; ii<m_vsig.size(); ii++)
	{
		memcpy(&m_stcountry, &m_vsig[ii], sizeof(struct st_country));
    	/* 判断记录是否重复, 若重复则修改列值, 否则插入 */
    	if (stmtsel.execute() != 0)
    	{
			m_discarded++;
    	    m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
			return stmtsel.m_cda.rc;
    	}

    	iccount = 0;
    	stmtsel.next();

    	if (iccount == 0)
		{
    		/* 执行SQL插入语句, 一定要判断返回值, 0-成功, 其它-失败 */
    		if (stmtins.execute() != 0)
    		{
				m_discarded++;
    	        m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
				return stmtins.m_cda.rc;
    		}
			m_inserted++;
		}

		if (iccount > 0)
		{
    		/* 执行SQL修改语句, 一定要判断返回值, 0-成功, 其它-失败 */
    		if (stmtupd.execute() != 0)
    		{
				m_discarded++;
    	        m_logfile -> Write("stmtupd.execute() failed.\n%s\n%s\n", stmtupd.m_sql, stmtupd.m_cda.message);
				return stmtupd.m_cda.rc;
    		}
			m_updated++;
		}
	}
	m_vsig.clear();

	return 0;
}
