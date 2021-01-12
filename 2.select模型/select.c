//ʱ�䣺2019��7��30��16:32:25
/*
selectģ��
1.�������c / sģ���У�accept recvɵ�ȵ�����   1�����ɵ������
											   2�������ִ������ send recv accept��ִ�й����ж��������ģ�����ע�⣬selectģ���ǽ��ɵ�ȵ�����ģ�������⼸�������������������
2.ʵ�ֶ���ͻ������ӣ������ͻ��˷ֱ�ͨ��
3.���ڷ��������ͻ��˾Ͳ�������ˣ���Ϊֻ��һ��socket   �ͻ���recv�ȴ��ĵ�ʱ��Ҳ����send ��ֻ��Ҫ��������һ���̣߳�recv���߳���

selectģ���߼�
1.ÿ���ͻ��˶���socket, ������Ҳ���Լ���socket, �����е�socketװ��һ�����ݽṹ�������
2.ͨ��select����������1�е�socket���飬��ĳ��socket����Ӧ��select�ͻ�ͨ������� / ����ֵ��������
3.����Ӧ����   1�������⵽���Ƿ�����socket, �Ǿ����пͻ������ӣ�����accept    
			   2�������⵽���ǿͻ���socket, �Ǿ��ǿͻ�������ͨ�ţ�����send��recv
*/


//#define FD_SETSIZE 128     //FD_SETSIZEĬ����64��������winsock2.hǰ��������꣬�Ϳ�����selectģ�ʹ�������socket,��Ҫ����selectԭ����Ǳ�����⣬Խ��϶�Ч��Խ�ͣ��ӳ�Խ��selectģ��Ӧ�þ���С�û���������
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>    //ͷ�ļ�
#pragma comment(lib, "ws2_32.lib")   //���ļ�

fd_set allSockets;   
//�ص�������ϵͳ�������ǵĺ���
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		//�ͷ�����socket
		for (u_int i = 0; i < allSockets.fd_count; i++)   //���Ҫ����allSockets���Զ����ȫ�ֵ�
		{
			closesocket(allSockets.fd_array[i]);
		}
		//���������
		WSACleanup();
	}
	return TRUE;
}

int main()
{
	//����̨���ڵ�X�˳����ò���ϵͳ�������ͷ�����   ������Ͷ��һ������
	SetConsoleCtrlHandler(fun, TRUE);   //(fun true):����ϵͳִ��Ĭ�ϵĺ�������Ĭ�ϵĶ�����Ȼ�����fun�����������ǵĶ�������������ͷ�    ���fun��ΪNULL,ϵͳ����һЩĬ�ϵ���
	                                    //��fun false��:����ϵͳִ��Ĭ�ϵĺ�������Ĭ�ϵĶ���,������fun����
	//�������
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("������ʧ��!\n");
		return 0;
	}

	//У��汾��
	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
	{
		printf("�汾������!\n");
		if (SOCKET_ERROR == WSACleanup())
			printf("�����룺%d\n", WSAGetLastError());
		return 0;
	}

	//����������socket
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);   //��ַ���ͣ��׽������ͣ�Э������
	if (INVALID_SOCKET == socketServer)
	{
		printf("������socket����ʧ��,�����룺%d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	//��IP��ַ�Ͷ˿�
	struct sockaddr_in socketMsg;
	socketMsg.sin_family = AF_INET;			//Э��
	socketMsg.sin_port = htons(12345);		//�˿�
	socketMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");  //��ַ
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*)&socketMsg, sizeof(socketMsg)))
	{
		printf("��ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//��ʼ�����������ͻ�����������
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))   //����2�ǹ������Ӷ��е���󳤶ȣ�SOMAXCONN����ϵͳ�Զ�ѡ������ʵĸ���
	{
		printf("����ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	/*
		select
		��һ��������һ��װ�ͻ���socket�ṹ
		fd_set allSockets;
		
		FD_ZERO(&allSockets); ��������

		FD_SET(socketServer, &allSockets);  �������һ��Ԫ�� ��socket��������64�Ҵ�socket�����ڵ�ʱ��

		FD_CLR(socketServer, &clientSockets);  ������ɾ��ָ����socket    ����Ҫ�ֶ��ͷ�closesocket

		int a = FD_ISSET(socketServer, &clientSockets);   �ж�һ��socket�Ƿ��ڼ�����  ���ڷ���0���ڷ��ط�0

		�ڶ�����select
		���ã�����socket���ϣ����ĳ��socket�����¼������ӻ����շ����ݣ���ͨ������ֵ�Լ�������������
		select();      ����2����accept��recvɵ�����⣬����select�ṹ�Ȼ���ģ�͵��������ã�����3��4һ�����ΪNULL
		����1. ���ԣ��˴���0��Ϊ�˼���berkeley sockets ���¼��ݾɱ�׼
		����2.��1������Ƿ��пɶ���socket�����ͻ��˷�����Ϣ�ˣ���socket�ͻᱻ����
			  ��2��&setRead   ��ʼ��Ϊ���е�socket,ͨ��selectͶ�Ÿ�ϵͳ��ϵͳ�����¼�������socket�ٸ�ֵ���������ú����������ֻʣ���������socket

		����3.��1������Ƿ��п�д��socket�����ǿ��Ը���Щ�ͻ����׽��ַ���Ϣ����send��ֻҪ���ӳɹ������������Ǹÿͻ����׽��־��ǿ�д��
			  ��2��&setWrite  ��ʼ��Ϊ���е�socket��ͨ��selectͶ�Ÿ�ϵͳ��ϵͳ����д��socket�ٸ�ֵ���������ú������������װ�ſ��Ա�send���ݵĿͻ���socket.һ�����Ǿ�ֱ��send�ˣ�������������߼��ϣ��õĲ��Ƿǳ���

		����4.��1������׽����ϵ��쳣�����÷���2.3һ���������쳣������׽�������װ����������������
			  ��2���õ��쳣�׽����ϵľ��������  getsockopt

		����5.��1�����ȴ�ʱ��  ���統�ͻ���û������ʱ����ôselect�������Ե�һ�ᣬһ��ʱ�����û�У��ͼ���ִ��select�������䣬����о�����ִ����������
			  ��2��TIMEVAL  ������Ա:tv_sec �룬tv_usec ΢��   �����0 0 ���Ƿ�����״̬����������   ���3 4 �����޿ͻ�����Ӧ������µȴ�3��4΢��
		      ��3��NULL select��ȫ����  ֱ���ͻ����з�Ӧ���ż���

		����ֵ��1��0 �ͻ����ٵȴ�ʱ����û�з�Ӧ ���� continue������  ��2��>0   �пͻ�����������  ��3��SOCKET_ERROR �����˴��� WSAGetLastError()�õ�������
	*/
	

	//fd_set allSockets;
	FD_ZERO(&allSockets);
	FD_SET(socketServer, &allSockets);   //������װ��ȥ

	while (1)
	{
		//��ʱsocket���ϣ�����select������ֻ��������Ӧ��socket,�����е�socket�ͱ��ˣ����Բ���ֱ��ʹ�ñ�������socket��allsockets
		fd_set readSockets = allSockets;		//select�������2���ɶ���socket����
		fd_set writeSockets = allSockets;		//select�������3����д��socket����
		//FD_CLR(socketServer, &writeSockets);  //���Բ���2ʱ���ɰѷ�����socket��ɾ����Ҳ��û�У���ȥɾ��
		fd_set errorSockets = allSockets;		//select�������4���쳣���󼯺�

		//ʱ���
		struct timeval st;
		st.tv_sec = 3;
		st.tv_usec = 0;

		//select
		int nRes = select(0, &readSockets, &writeSockets, &errorSockets, &st);   //���ԣ�����2.3.4�ֱ���ԣ����Բ���3ʱֻҪ�пͻ������ӷ�������select��������ͻ����0��ѭ��ִ�д���0�ķ�֧

		if (0 == nRes)		//û����Ӧ
		{
			continue;
		}
		else if (nRes > 0)   //����Ӧ
		{
			//�����쳣���󼯺�
			for (u_int i = 0; i < errorSockets.fd_count; i++)   
			{
				char str[100] = { 0 };
				int len = 99;
				if (SOCKET_ERROR == getsockopt(errorSockets.fd_array[i], SOL_SOCKET, SO_ERROR, str, &len))
				{
					printf("�޷��õ�������Ϣ\n");
				}
				printf("%s\n", str);
			}
			//�����д����
			for (u_int i = 0; i < writeSockets.fd_count; i++)   
			{
				//printf("������%d,%d:��д\n", socketServer, writeSockets.fd_array[i]);    //printf���ԣ�����printf�����ӡwriteSockets�еķ�����socket
				if (SOCKET_ERROR == send(writeSockets.fd_array[i], "ok", 2, 0))		//����з�����Ϣsend����ֵֻ�д���0��SOCKET_ERROR����������ͻ��������˳���ǿ���˳�ʱ�˴�send������SOCKET_ERROR
				{																	//���ʲô��������send����0
					int a = WSAGetLastError();
				}
			}
			//����ɶ�����,�ͻ��˹ر��˳�Ҳ���⴦��
			for (u_int i = 0; i < readSockets.fd_count; i++)  
			{
				if (readSockets.fd_array[i] == socketServer)  
				{
					//accept
					SOCKET socketClient = accept(socketServer, NULL, NULL);
					if (INVALID_SOCKET == socketClient)
					{
						//���ӳ���
						continue;
					}
					FD_SET(socketClient, &allSockets);
					printf("���ӳɹ�\n");
					//send
				}
				else
				{
					//�ͻ���
					char strBuf[1500] = { 0 };
					int nRecv = recv(readSockets.fd_array[i], strBuf, 1500, 0);
					if (0 == nRecv)
					{
						//�ͻ���������
						//�Ӽ������õ�
						SOCKET socketTemp = readSockets.fd_array[i];
						FD_CLR(readSockets.fd_array[i], &allSockets);
						//�ͷ�
						closesocket(socketTemp);
					}
					else if (nRecv > 0)
					{
						//���յ�����Ϣ
						printf(strBuf);
					}
					else  //SOCK_ERROR
					{
						//ǿ������Ҳ�ǳ��� 10054
						int a = WSAGetLastError();
						switch (a)
						{
						case 10054:
							{
								SOCKET socketTemp = readSockets.fd_array[i];
								FD_CLR(readSockets.fd_array[i], &allSockets);
								closesocket(socketTemp);
								break;
							}
						}
					}
				}
			}
		}
		else
		{
			//����������
			//���ݾ���Ĵ�������д�����������whileѭ�������ߵȴ�����continue��һ��ѭ��
			break;
		}
	}

	//�ͷ�����socket
	for (u_int i = 0; i < allSockets.fd_count; i++)
	{
		closesocket(allSockets.fd_array[i]);
	}
	//���������
	WSACleanup();
	system("pause");
	return 0;    //�����رտ���̨����
}



/*
�����ܽ�    
socket����   select�ж���û����Ӧ��	1.����0��û�У�������
									2.����>0:����Ӧ��   ��1���ɶ��ģ�accept, recv�� 
														��2����д�� send ���ǷǵôӴ˴�����send  
														��3���쳣�� getsockopt
									3.SOCKET_ERROR  ��������ִ�д���

ִ������
���緢��1500���ֽڣ�send��1500���ֽڸ��Ƶ�Э�黺���������ƵĹ����ǿ����ģ����������;�пͻ�����Ӧ�����ˣ�������ͻ���send��Ϣֻ��һ����send, 
�����Լ�д�ĺ���Ҳ��ִ�����������ǵ���Ƶ�ʵͣ�Ҳ�����д��������ݸ��ƣ�send / recv����Ƶ�ʸߣ����ܵ��µȴ�ʱ��̫��

select��������  1.���ȴ�    ִ������ + ʱ�������״̬	����5ʱ������Ϊ0���������������һ��ѭ�������Ǳ�������ʱ���ܸɱ��  ɵ��  û��Ŀ��ĵȴ�
				2.��ȴ�	ִ������ + ������			����5ʱ������Ϊ��Ϊ0������3s�������������ٵȴ�3s					ɵ��  û��Ŀ��ĵȴ�
				3.ȫ�ȴ�	ִ������ + Ӳ����			����5ʱ������ΪNULL, ֱ���пͻ��˷���Ϣ������						����  �ȵ����õ�Ŀ����Ӧ
*/


