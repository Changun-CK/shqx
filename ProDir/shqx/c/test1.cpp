#include "_freecplus.h"

pthread_mutex_t mutex; // 声明一个互斥锁
void *pth_main(void *arg); // 与客户端通信的主函数

CTcpServer TcpServer;
CTcpClient TcpClient;

int main()
{
	// 向服务端发起连接请求
	if (TcpClient.ConnectToServer("39.108.127.31", 5050) == false)
	{
		printf("TcpClient.ConnectToServer(%s) is failed.\n", "39.108.127.31, 5050"); return -1;
	}

	pthread_mutex_init(&mutex, 0); // 创建锁

	// 创建线程对象
	pthread_t pthid1, pthid2;
	// 创建第一个线程
	pthread_create(&pthid1, NULL, pth_main, (void *)1);
	// 创建第二个线程
	pthread_create(&pthid2, NULL, pth_main, (void *)2);

	pthread_join(pthid1, NULL); // 等待线程1退出
	pthread_join(pthid2, NULL); // 等待线程2退出

	pthread_mutex_lock(&mutex); // 销毁锁

	return 0;
}
 
void *pth_main(void *arg) // 与客户端通信的主函数
{
	int pno = (long)arg; // 线程编号

	pthread_detach(pthread_self());

	char strbuffer[1024];

	for (int ii = 0; ii < 3; ii++)
	{
		pthread_mutex_lock(&mutex); // 加锁
		memset(strbuffer, 0, sizeof(strbuffer));
		sprintf(strbuffer, "线程%ld,第%d个超女.\n", pno, ii+1);
		if (TcpClient.Write(strbuffer, strlen(strbuffer)) < 0)
			break;
		printf("发送:%s\n", strbuffer);

		memset(strbuffer, 0, sizeof(strbuffer));
		if (TcpClient.Read(strbuffer, strlen(strbuffer)) < 0)
			break;
		printf("线程%ld接收:%s\n", pno, strbuffer);
		pthread_mutex_unlock(&mutex); // 释放锁
		usleep(100); // 否则其他线程无法获得锁
	}

	pthread_exit(0);
}
