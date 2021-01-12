//ʱ�䣺2019��8��5��23:33:27
//�¼�ѡ��ģ��

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//�����¼������Լ�socket���ϵĽṹ��
struct fd_es_set
{
	unsigned short count;
	SOCKET sockall[WSA_MAXIMUM_WAIT_EVENTS];
	WSAEVENT eventall[WSA_MAXIMUM_WAIT_EVENTS];
};

//һ��һ��ѯ��,����ɽṹ������
struct fd_es_set esSet[20];    //�ṹ������

//�ص�����
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		for (int j = 0; j < 20; j++)
		{
			for (int i = 0; i < esSet[j].count; i++)
			{
				closesocket(esSet[j].sockall[i]);
				WSACloseEvent(esSet[j].eventall[i]);
			}
		}
		break;
	}

	return TRUE;
}


int main()
{
	//����̨�˳� ������Ͷ��һ������
	SetConsoleCtrlHandler(fun, TRUE);

	//�������
	WSADATA wsadata;
	int nRes = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (0 != nRes)
	{
		printf("����ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
		return 0;
	}

	//У��汾
	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
	{
		printf("�汾����\n");
		WSACleanup();
		return 0;
	}

	//����������Socket
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketServer)
	{
		printf("����������Socketʧ��\n");
		WSACleanup();
		return 0;
	}

	//��IP��ַ�Ͷ˿�
	struct sockaddr_in socketMsg;
	socketMsg.sin_family = AF_INET;
	socketMsg.sin_port = htons(12345);
	socketMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*)&socketMsg, sizeof(socketMsg)))
	{
		printf("��IP��ַ�Ͷ˿ڳ��ִ��󣬴�����Ϊ%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//����
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		printf("����ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//һ��һ��ѯ��,����ɽṹ������
	//struct fd_es_set esSet[20];    //�ṹ������
	//memset(esSet, 0, sizeof(struct fd_es_set)*20);    //memset����esSet��ǰλ�ú����sizeof(struct fd_es_set)*20���ֽ���0�滻��������esSet

	//�����¼�                                                      
	WSAEVENT eventServer = WSACreateEvent();   

	if (WSA_INVALID_EVENT == eventServer)  
	{
		printf("�����¼�ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	
	//�󶨲�Ͷ�� WSAEventSelect  
	if (SOCKET_ERROR == WSAEventSelect(socketServer, eventServer, FD_ACCEPT))
	{
		printf("���¼�ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
		//�ͷ��¼����
		WSACloseEvent(eventServer);
		//�ͷ�socket
		closesocket(socketServer);
		//���������
		WSACleanup();
	}

	//װ��ȥ
	esSet[0].eventall[esSet[0].count] = eventServer;
	esSet[0].sockall[esSet[0].count] = socketServer;
	esSet[0].count++;

	//һ��һ�鴦����1�����̣߳�һ��һ��˳����  ��2����������̣߳�ÿ���̴߳���һ���¼������64��������socketֻ��һ����accept���ڵ�һ���̣߳������̴߳���read,write,close����

	while (1)
	{
		//ѯ���¼�
		for (int j = 0; j < 20; j++)     //���̣߳�һ��һ��˳����
		{
			if (0 == esSet[j].count)   //������Ч�¼����Ϊ0��������һ��ѭ��
			{
				continue;
			}
			DWORD nRes = WSAWaitForMultipleEvents(esSet[j].count, esSet[j].eventall, FALSE, 0, FALSE);

			if (WSA_WAIT_FAILED == nRes)
			{
				printf("ѯ���¼�ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
				break;
			}
			if (WSA_WAIT_TIMEOUT == nRes)
			{
				continue;
			}
			DWORD nIndex = nRes - WSA_WAIT_EVENT_0;   //��ʱ�õ��������ź��¼�������ֵ��С��

			//��������2��//������ֵǰ���¼��������źŵģ�����ֱ�ӴӴ˴���ʼѭ���������������¼����������źŵ��¼����Ż�2���Ż�1����һ��
			for (int i = nIndex; i < esSet[j].count; i++)
			{
				DWORD nRes = WSAWaitForMultipleEvents(1, &esSet[j].eventall[i], FALSE, 0, FALSE);
				if (WSA_WAIT_FAILED == nRes)
				{
					printf("ѯ���¼�ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
					continue;
				}
				//�������4д���峬ʱ�������ʱ�¼�Ҫ���ϳ�ʱ�ж�
				if (WSA_WAIT_TIMEOUT == nRes)
				{
					continue;
				}

				//�о��¼�,�õ��±��Ӧ�ľ������
				WSANETWORKEVENTS  NetworkEvents;
				if (SOCKET_ERROR == WSAEnumNetworkEvents(esSet[j].sockall[i], esSet[j].eventall[i], &NetworkEvents))
				{
					printf("�õ��±��Ӧ�ľ������ʧ�ܣ������룺%d\n", WSAGetLastError());
					break;
				}

				if (NetworkEvents.lNetworkEvents & FD_ACCEPT) 
				{
					if (0 == NetworkEvents.iErrorCode[FD_ACCEPT_BIT])
					{
						//��������
						SOCKET socketClient = accept(esSet[j].sockall[i], NULL, NULL);
						if (INVALID_SOCKET == socketClient)
						{
							continue;
						}

						//�����¼�����
						WSAEVENT wsaClientEvent = WSACreateEvent();
						if (WSA_INVALID_EVENT == wsaClientEvent)
						{
							closesocket(socketClient);
							continue;
						}

						//Ͷ�ݸ�ϵͳ
						if (SOCKET_ERROR == WSAEventSelect(socketClient, wsaClientEvent, FD_READ | FD_CLOSE | FD_WRITE))
						{
							closesocket(socketClient);
							WSACloseEvent(wsaClientEvent);
							continue;
						}

						//ÿ��ֻ��װ64��������64���Ļ�Ҫװ����һ��
						for (int m = 0; m < 20; m++)
						{
							if (esSet[m].count < 64)
							{
								//װ���ṹ��
								esSet[m].sockall[esSet[m].count] = socketClient;
								esSet[m].eventall[esSet[m].count] = wsaClientEvent;
								esSet[m].count++;
								break;
							}
						}
						printf("accept event\n");
					}
					else
					{
						continue;
					}
				}

				if (NetworkEvents.lNetworkEvents & FD_WRITE)    
				{
					if (0 == NetworkEvents.iErrorCode[FD_WRITE_BIT])
					{
						if (SOCKET_ERROR == send(esSet[j].sockall[i], "connect success", strlen("connect success"), 0)) 
						{
							printf("sendʧ�ܣ�������Ϊ��%d\n", WSAGetLastError());  
							continue;
						}
						printf("write event\n");
					}
					else
					{
						printf("socket error�׽��ִ��󣬴�����Ϊ��%d\n", NetworkEvents.iErrorCode[FD_WRITE_BIT]);
					}
				}

				if (NetworkEvents.lNetworkEvents & FD_READ)
				{
					if (0 == NetworkEvents.iErrorCode[FD_READ_BIT])
					{
						char strRecv[1500] = { 0 };
						if (SOCKET_ERROR == recv(esSet[j].sockall[i], strRecv, 1499, 0))
						{
							printf("recvʧ�ܣ�������Ϊ��%d\n", WSAGetLastError());
							continue;
						}
						printf("���յ����ݣ�%s\n", strRecv);
					}
					else
					{
						printf("socket error�׽��ִ��󣬴�����Ϊ��%d\n", NetworkEvents.iErrorCode[FD_READ_BIT]);
						continue;
					}
				}

				if (NetworkEvents.lNetworkEvents & FD_CLOSE)
				{
					//��ӡ
					printf("client close\n");
					printf("client force out�ͻ���ǿ���˳��������룺%d\n", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
					//�������ߵĿͻ��� �׽��� �¼�
					closesocket(esSet[j].sockall[i]);
					esSet[j].sockall[i] = esSet[j].sockall[esSet[j].count - 1];
					WSACloseEvent(esSet[j].eventall[i]);
					esSet[j].eventall[i] = esSet[j].eventall[esSet[j].count - 1];
					esSet[j].count--;
				}
			}
		}
	}


	//�ͷ�socket,�ͷ��¼����
	for (int j = 0; j < 20; j++)
	{
		for (int i = 0; i < esSet[j].count; i++)
		{
			closesocket(esSet[j].sockall[i]);
			WSACloseEvent(esSet[j].eventall[i]);
		}
	}
	//���������
	WSACleanup();
	system("pause");
	return 0;
}
*/

/*
->�¼�����  �¼�ѡ��ģ��  ->�ص�I/Oģ��
C/S -> select
->��Ϣ����  �첽ѡ��ģ��  ->��ɶ˿�ģ��
*/
