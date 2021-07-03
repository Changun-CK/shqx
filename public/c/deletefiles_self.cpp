/*
 * 本程序用于删除指定时间前的文件, 参数可配置
 * 作者: 陈锟, 时间: 2020-01-31
 */

#include "_public.h"

/* 程序退出 */
void EXIT(int sig);

char outtime[21];
char MatchStr[301];
float Ddayout;

/* 文件信息类 */
struct st_fileinfo
{
	char fullfilename[301];
	char mtime[21];
	time_t time;
};

/* 存放符合条件的文件名的容器 */
vector<struct st_fileinfo> vremovefile;

/* 显示程序帮助 */
void Help(char *argv[]);

/* 主函数 */
bool _deletefiles(const char *path, float dayout, const char *matchstr);

int main(int argc, char *argv[])
{
	if ( !((argc == 3) || (argc == 4)) ) { Help(argv); return -1; }

	/* 关闭全部的信号 */
	//CloseIOAndSignal();

	/* 处理程序退出的信号 */
	signal(SIGINT, EXIT); signal(SIGTERM, EXIT);

	memset(outtime, 0, sizeof(outtime));
	memset(MatchStr, 0, sizeof(MatchStr));

	if (argc == 3) strcpy(MatchStr, "*");
	else strcpy(MatchStr, argv[3]);

	Ddayout = atof(argv[2]);

	LocalTime(outtime, "yyyy-mm-dd hh:mi:ss", 0 - (int)(Ddayout*24*60*60));

	/* 主函数 */
	_deletefiles(argv[1], Ddayout, MatchStr);

	return 0;
}

/* 程序退出 */
void EXIT(int sig)
{
	printf("程序退出, sig = %d\n", sig);
	exit(0);
}

/* 显示程序帮助 */
void Help(char *argv[])
{
	printf("\n本程序用于删除指定时间前的文件, 参数可配置.\n");
	printf("\nUsing:/ProDir/public/bin/deletefiles pathname dayout [matchstr]\n");
	printf("\nSample:/ProDir/public/bin/deletefiles /data/shqx/ftp/surfdata 0.1 SURF_ZH*.TXT\n\n");
	printf("pathname:   待删除文件的目录.\n");
	printf("dayout:     待删除文件的保留天数, 可用小数.\n");
	printf("[matchstr]: 待删除文件的匹配规则, 这是一个可选参数, 默认为所有文件.\n");
}

/* 主函数 */
bool _deletefiles(const char *path, float dayout, const char *matchstr)
{
	CDir Dir;
	struct st_fileinfo stfileinfo;
	time_t tmptime;
	vremovefile.clear();

	/* 扫描数据文件存放的目录, 包括子目录 */
	if (Dir.OpenDir(path, MatchStr, 100000, true) == false) { printf("Dir.OpenDir(%s) failed.\n", path); return false; }

	while (1)
	{
		memset(&stfileinfo, 0, sizeof(struct st_fileinfo));
		/* 读取文件 */
		if (Dir.ReadDir() == false) break;
		/* 获取文件信息 */
    	sprintf(stfileinfo.fullfilename, "%s", Dir.m_FullFileName);
    	sprintf(stfileinfo.mtime, "%s", Dir.m_ModifyTime);
    	strtotime(stfileinfo.mtime, &stfileinfo.time);
    	strtotime(outtime, &tmptime);

		/* 如果文件时间大于或等于指定时间, 则放弃删除
		 * 应注意此时等于指定时间我们仍保留该文件 */
		if (tmptime <= stfileinfo.time) continue;

		/* 如果文件在outday之前那么将结构体文件加入容器中 */
		vremovefile.push_back(stfileinfo);
	}

	/* 删除文件 */
	if (vremovefile.empty() == true)
	{
 		printf("没有找到需要删除的文件\n"); return false; 
	}

	printf("容器不为空\n");

	for (int ii=0; ii<vremovefile.size(); ii++)
	{
		if (remove(vremovefile[ii].fullfilename) == 0) { printf("已删除文件:%s\n", vremovefile[ii].fullfilename); continue; }
		printf("删除文件失败:%s\n", vremovefile[ii].fullfilename);
	}

	return true;
}
