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

#ifndef __FTP_H
#define __FTP_H

#include "_public.h"
#include "ftplib.h"

class Cftp
{
public:
  // ftp连接句柄
  netbuf *m_ftpconn;

  // 文件的大小
  unsigned int m_size;

  // 文件的时间modifytime
  char m_mtime[21];

  Cftp();
 ~Cftp();

  // 存放login()方法登录失败的原因
  bool m_connectfailed;
  bool m_loginfailed;
  bool m_optionfailed;

  void initdata();

  // 登录ftp服务器
  // in_host 服务器地址和端口，中间用":"分隔，如"192.168.1.1:21"
  // in_username ftp用户名
  // in_password ftp的密码
  // in_mode 传输模式，FTPLIB_PASSIVE是被动模式，FTPLIB_PORT是主动模式
  bool login(const char *in_host,const char *in_username,const char *in_password,const int in_mode=FTPLIB_PASSIVE);
  
  // 注销
  bool logout();

  // 获取ftp服务器上文件的时间
  bool mtime(const char *in_remotefilename);

  // 获取ftp服务器上文件的大小
  bool size(const char *in_remotefilename);

  // 向服务端发送site命令
  bool site(const char *in_command);

  // 改变ftp远程目录
  bool chdir(const char *in_remotedir);

  // 在ftp服务器上创建目录
  bool mkdir(const char *in_remotedir);

  // 删除ftp服务器上的目录
  bool rmdir(const char *in_remotedir);

  // 发送list命令列出ftp服务器目录中的文件，结果保存到本地文件中
  // 如果是列出当前目录，in_remotedir用"","*","."都行。
  bool nlist(const char *in_remotedir,const char *out_listfilename);

  // 发送dir命令列出ftp服务器目录中的文件，结果保存到本地文件中
  bool dir(const char *in_remotedir,const char *out_listfilename);

  // 从ftp服务器上获取文件
  // in_remotefilename 待获取的远程文件名
  // in_localfilename  本地文件名，可以与in_remotefilename不同
  // bCheckMTime 文件传输完成后，是否核对文件传输前后的时间，保证文件的完整性
  // 注意，文件在传输的过程中，采用临时文件命名的方法，即在in_localfilename后加".tmp"，在传输
  // 完成后才正式改为in_localfilename
  bool get(const char *in_remotefilename,const char *in_localfilename,const bool bCheckMTime=true);

  // 向ftp服务器发送文件
  // in_localfilename 本地待发送的文件名
  // in_remotefilename 远程文件名
  // bCheckSize 文件传输完成后，是否核对本地和远程文件的大小，保证文件的完整性
  // 注意，文件在传输的过程中，采用临时文件命名的方法，即在in_remotefilename后加".tmp"，在传输
  // 完成后才正式改为in_remotefilename
  bool put(const char *in_localfilename,const char *in_remotefilename,const bool bCheckSize=true);

  // 删除ftp服务器上的文件
  bool ftpdelete(const char *in_remotefilename);

  // 把ftp服务器上的文件改名
  bool ftprename(const char *in_srcremotefilename,const char *in_dstremotefilename);

  // 获取服务器返回信息的最后一条 return a pointer to the last response received
  char *response();
};

#endif
