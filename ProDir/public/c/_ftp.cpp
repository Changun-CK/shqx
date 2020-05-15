////////////////////////////////////////////////////////
// 本软件由神农开源网提供，开源软件需要大家的努力。
// 技术交流请加QQ群701117364，我们一起把开源软件做得更好。
//
// 软件官方遵守协议约定
//
// 1) 禁止用于技术培训、出版等商业用途。
// 2) 禁止任何侵权行为，包括软件的源码程序、资料文档、文件信息。
// 3) 禁止软件产品插入与软件官方无关的任何广告信息，包括文字、图像、媒体信息。
// 4) 禁止软件产品内置与软件官方程序应用无关的任何第三方插件程序、组件。
// 5) 禁止软件程序存在任何不经过用户许可的远程响应执行的控制程序。
// 6) 禁止软件程序使用任何手段收集用户隐私信息与用户储存的数据。
//
//
//  用户遵守协议约定
//  1) 禁止用户修改软件官方任何的版权说明信息，包括程序的版权协议说明、软件官方链接、文字说明、图像标志、媒体信息。
//  2) 禁止用户通过任何方式破坏、侵犯软件官方的正常发展运营，包括软件官方程序的正常运行与商业授权机制。
//
////////////////////////////////////////////////////////

#include "_ftp.h"

Cftp::Cftp()
{
  m_ftpconn=0;

  initdata();

  FtpInit();

  m_connectfailed=false;
  m_loginfailed=false;
  m_optionfailed=false;
}

Cftp::~Cftp()
{
  logout();
}

void Cftp::initdata()
{
  m_size=0;

  memset(m_mtime,0,sizeof(m_mtime));
}

bool Cftp::login(const char *in_host,const char *in_username,const char *in_password,const int in_mode)
{
  if (m_ftpconn != 0) { FtpQuit(m_ftpconn); m_ftpconn=0; }

  m_connectfailed=m_loginfailed=m_optionfailed=false;

  if (FtpConnect(in_host,&m_ftpconn) == false)  { m_connectfailed=true; return false; }

  if (FtpLogin(in_username,in_password,m_ftpconn) == false)  { m_loginfailed=true; return false; }

  if (FtpOptions(FTPLIB_CONNMODE,(long)in_mode,m_ftpconn) == false) { m_optionfailed=true; return false; }

  return true;
}

bool Cftp::logout()
{
  if (m_ftpconn == 0) return false;

  FtpQuit(m_ftpconn);

  m_ftpconn=0;

  return true;
}

bool Cftp::get(const char *in_remotefilename,const char *in_localfilename,const bool bCheckMTime)
{
  if (m_ftpconn == 0) return false;

  // 创建本地文件目录
  MKDIR(in_localfilename);

  char strlocalfilenametmp[301];
  memset(strlocalfilenametmp,0,sizeof(strlocalfilenametmp));
  snprintf(strlocalfilenametmp,300,"%s.tmp",in_localfilename);

  // 获取远程服务器的文件的时间
  if (mtime(in_remotefilename) == false) return false;

  // 取文件
  if (FtpGet(strlocalfilenametmp,in_remotefilename,FTPLIB_IMAGE,m_ftpconn) == false) return false;
  
  // 判断文件获取前和获取后的时间，如果时间不同，表示文件已改变，返回失败
  if (bCheckMTime==false)
  {
    char strmtime[21];
    strcpy(strmtime,m_mtime);

    if (mtime(in_remotefilename) == false) return false;

    if (strcmp(m_mtime,strmtime) != 0) return false;
  }

  // 重置文件时间
  UTime(strlocalfilenametmp,m_mtime);

  // 改为正式的文件
  if (rename(strlocalfilenametmp,in_localfilename) != 0) return false; 

  // 获取文件的大小
  m_size=FileSize(in_localfilename);

  return true;
}

bool Cftp::mtime(const char *in_remotefilename)
{
  if (m_ftpconn == 0) return false;
  
  memset(m_mtime,0,sizeof(m_mtime));
  
  char strmtime[21];
  memset(strmtime,0,sizeof(strmtime));

  if (FtpModDate(in_remotefilename,strmtime,14,m_ftpconn) == false) return false;

  AddTime(strmtime,m_mtime,0+8*60*60);

  return true;
}

bool Cftp::size(const char *in_remotefilename)
{
  if (m_ftpconn == 0) return false;

  m_size=0;
  
  if (FtpSize(in_remotefilename,&m_size,FTPLIB_IMAGE,m_ftpconn) == false) return false;

  return true;
}

bool Cftp::site(const char *in_command)
{
  if (m_ftpconn == 0) return false;
  
  if (FtpSite(in_command,m_ftpconn) == false) return false;

  return true;
}

bool Cftp::chdir(const char *in_remotedir)
{
  if (m_ftpconn == 0) return false;
  
  // 此处是问题代码, 详细可见ftplib.c的FtpChdir函数
  if (FtpChdir(in_remotedir,m_ftpconn) == 0) return false;

  return true;
}

bool Cftp::mkdir(const char *in_remotedir)
{
  if (m_ftpconn == 0) return false;
  
  if (FtpMkdir(in_remotedir,m_ftpconn) == false) return false;

  return true;
}

bool Cftp::rmdir(const char *in_remotedir)
{
  if (m_ftpconn == 0) return false;
  
  if (FtpRmdir(in_remotedir,m_ftpconn) == false) return false;

  return true;
}

bool Cftp::dir(const char *in_remotedir,const char *out_listfilename)
{
  if (m_ftpconn == 0) return false;
  
  if (FtpDir(out_listfilename,in_remotedir,m_ftpconn) == false) return false;

  return true;
}

bool Cftp::nlist(const char *in_remotedir,const char *out_listfilename)
{
  if (m_ftpconn == 0) return false;

  // 创建本地文件目录
  MKDIR(out_listfilename);
  
  if (FtpNlst(out_listfilename,in_remotedir,m_ftpconn) == false) return false;

  return true;
}

bool Cftp::put(const char *in_localfilename,const char *in_remotefilename,const bool bCheckSize)
{
  if (m_ftpconn == 0) return false;

  char strremotefilenametmp[301];
  memset(strremotefilenametmp,0,sizeof(strremotefilenametmp));
  snprintf(strremotefilenametmp,300,"%s.tmp",in_remotefilename);

  if (FtpPut(in_localfilename,strremotefilenametmp,FTPLIB_IMAGE,m_ftpconn) == false) return false;

  if (FtpRename(strremotefilenametmp,in_remotefilename,m_ftpconn) == false) return false;

  // 判断已上传的文件的大小与本地文件是否相同，确保上传成功。
  if (bCheckSize==true)
  {
    if (size(in_remotefilename) == false) return false;

    if (m_size != FileSize(in_localfilename)) return false; 
  }

  return true;
}

bool Cftp::ftpdelete(const char *in_remotefilename)
{
  if (m_ftpconn == 0) return false;

  if (FtpDelete(in_remotefilename,m_ftpconn) == false) return false;
  
  return true;
}

bool Cftp::ftprename(const char *in_srcremotefilename,const char *in_dstremotefilename)
{
  if (m_ftpconn == 0) return false;

  if (FtpRename(in_srcremotefilename,in_dstremotefilename,m_ftpconn) == false) return false;
  
  return true;
}

char *Cftp::response()
{
  if (m_ftpconn == 0) return 0;

  return FtpLastResponse(m_ftpconn);
}

