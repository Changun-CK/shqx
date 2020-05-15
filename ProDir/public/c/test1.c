#include<pwd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

/*取得root账号的识别码和根目录*/
int main()
{
	struct passwd *user;
	user = getpwnam("root");
	printf("name:%s\n",user->pw_name);
	printf("uid:%d\n",user->pw_uid);
	printf("home:%s\n",user->pw_dir);

	return 0;
}
