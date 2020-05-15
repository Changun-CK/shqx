#include "_shqx.h"

CSURFDATA::CSURFDATA(connection *conn, CLogFile *logfile)
{
	m_conn = conn; m_logfile = logfile;
}

void CSURFDATA::BindConnLog(connection *conn, CLogFile *logfile)
{
	m_conn = conn; m_logfile = logfile;
}

CSURFDATA::~CSURFDATA()
{
	
}

bool CSURFDATA::SplitBuffer(const char *strBuffer)
{
	memset(&m_stsurfdata, 0, sizeof(struct st_surfdata));

	CCmdStr CmdStr;

	CmdStr.SplitToCmd(strBuffer, ",");
	if (CmdStr.CmdCount() != 9) return false;

	double ttmp;
	memset(&m_stsurfdata, 0, sizeof(struct st_surfdata));
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

    /* 判断记录是否重复, 防止主键冲突带来的性能下降 */
    if (stmtsel.execute() != 0)
    {
        m_logfile -> Write("stmtsel.execute() failed.\n%s\n%s\n", stmtsel.m_sql, stmtsel.m_cda.message);
		return stmtsel.m_cda.rc;
    }

    iccount = 0;
    stmtsel.next();

    if (iccount > 0) return 0;

    /* 执行SQL语句, 一定要判断返回值, 0-成功, 其它-失败 */
    if (stmtins.execute() != 0)
    {
        if (stmtins.m_cda.rc != 1)
        {
            m_logfile -> Write("stmtins.execute() failed.\n%s\n%s\n", stmtins.m_sql, stmtins.m_cda.message);
			return stmtins.m_cda.rc;
        }
    }

	return 0;
}
