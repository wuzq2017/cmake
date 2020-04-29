
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include "../types.h"
#define UDP_SOCKET_NUM		32

//设备操作结构
typedef struct
{
	//描述符
	int socket_fd;
	//远端地址记忆 用于主动发送到客户端
	struct sockaddr_in remoteaddr;
	//接收回调函数
	uint16 (*recv_callback)(uint8 *, uint16, uint8 *);
}udp_socket_t;
//定义客户端存储结构 因为要主动发送
static udp_socket_t udp_sockets[UDP_SOCKET_NUM];



//***********************************************************************************************************************
//函数作用:udp接收线程
//参数说明:
//注意事项:
//返回说明:无
//***********************************************************************************************************************
static void *udp_recv_thread(void *param)
{
	udp_socket_t *udp_socket_p=(udp_socket_t *)param;//指针转换
	uint8 recv_buf[1024];//接收缓冲区
	uint8 send_buf[1024];//发送缓冲区
	//接收地址长度
	socklen_t socklen = sizeof(struct sockaddr_in);
	
	printf("udp recv thread start\r\n");
	
	while(1)
	{
		//读取socket
		int length=recvfrom(udp_socket_p->socket_fd,recv_buf,sizeof(recv_buf),0,(struct sockaddr *)&(udp_socket_p->remoteaddr),&socklen);
		//判断是否有数据
		if(length>0)
		{
			if(udp_socket_p->recv_callback)
			{
				length=udp_socket_p->recv_callback(recv_buf,length,send_buf);
				//判断是否要发送应答
				if(length>0)
				{
					sendto(udp_socket_p->socket_fd,send_buf,length,0,(struct sockaddr *)&(udp_socket_p->remoteaddr),socklen);
				}
			}
		}
	}
	
	return NULL;
}

//***********************************************************************************************************************
//函数作用: udp初始化
//参数说明:
//注意事项:
//返回说明:如果成功返回>=0的数 失败返回-1
//***********************************************************************************************************************
int udp_init(int local_port, uint16(*recv_callback)(uint8 *, uint16, uint8 *))
{
	//客户端索引 用于查找存储的空位置
	int socket_index=0;
	//指向客户端设备结构
	udp_socket_t * udp_socket_p;
	//本地地址结构
	struct sockaddr_in localaddr;
	//接收线程句柄
	pthread_t recv_thread;
		
	
	//寻找空位置
	while(socket_index<(UDP_SOCKET_NUM-1))
	{
		//找到以后指向他
		if(udp_sockets[socket_index].socket_fd<=0)
		{
			//指向设备文件结构
			udp_socket_p=&udp_sockets[socket_index];
			break;
		}
		socket_index++;
	}
	//判断存储空间满了
	if(socket_index>=(UDP_SOCKET_NUM-1))
	{
		printf("udp is full \r\n");
		exit(0);
	}
		
	//创建UDP套接字
	if((udp_socket_p->socket_fd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		printf("Can't create udp \r\n");
		exit(1);
	}

	//允许发送广播包
	{
        int param=1;
        setsockopt(udp_socket_p->socket_fd,SOL_SOCKET,SO_BROADCAST,&param,sizeof(param));
	}

	//填充本地IP和端口
	if(local_port)
	{
		memset(&localaddr,0,sizeof(struct sockaddr_in));
		localaddr.sin_family=AF_INET;
		localaddr.sin_addr.s_addr=htonl(INADDR_ANY);
		localaddr.sin_port=htons(local_port);
		//if(inet_pton(AF_INET,"192.168.1.113", &localaddr.sin_addr)<=0)
		//{
		//	printf("Wrong source IP address!\n");
		//	exit(0);
		//}
		if(bind(udp_socket_p->socket_fd, (struct sockaddr *)&localaddr,sizeof(struct sockaddr_in))<0)
		{
			printf("Can't bind port %d \r\n",local_port);
			exit(1);
		}	
	}
	
	//设置回调函数
	udp_socket_p->recv_callback=recv_callback;
	
	//创建接收线程 并且把socket属性传递
	pthread_create(&recv_thread,NULL,udp_recv_thread,(void *)udp_socket_p);
	usleep(1000);
	
	//返回索引号 用于应用层发送时候的传递
	return socket_index;
}

//***********************************************************************************************************************
//函数作用:发送数据
//参数说明:
//注意事项:
//返回说明:如果成功返回0
//***********************************************************************************************************************
void udp_send(int socket_index, char * remote_ip, int remote_port, uint8 *buf, uint16 length)
{
	//指向sockets结构
	udp_socket_t * udp_socket_p;
	//远端地址结构
	struct sockaddr_in remoteaddr;
	
	
	//判断超过数量
	if(socket_index>=UDP_SOCKET_NUM)
	{
		printf("socket_index too large\r\n");
		exit(0);
	}
	
	//指向设备文件结构
	udp_socket_p=&udp_sockets[socket_index];

	//判断有效性
	if(udp_socket_p->socket_fd<=0)
	{
		printf("socket_index is not occupyed\r\n");
		exit(0);
	}
		
	//填充对方IP和端口
	if(remote_port)
	{
		memset(&remoteaddr,0,sizeof(struct sockaddr_in));
		remoteaddr.sin_family=AF_INET;
		remoteaddr.sin_port=htons(remote_port);
		if(inet_pton(AF_INET,remote_ip,&remoteaddr.sin_addr)<=0)	
		{
			printf("Wrong dest IP address!\n");
			exit(0);
		}
	}
	//填充远端地址记忆 用于主动发送到客户端
	else
	{
		memcpy(&remoteaddr,&udp_socket_p->remoteaddr,sizeof(struct sockaddr_in));
	}
	
	//发送数据
	sendto(udp_socket_p->socket_fd,buf,length,0,(struct sockaddr *)&remoteaddr,sizeof(struct sockaddr_in));
}
