//ʱ�䣺2019��8��10��13:32:39
//�������

/*
���ʣ�	1.Ϊ���ǵ�socket,�ص��ṹ��һ�����������첽�������ʱ��ϵͳ�첽�Զ��������������send��һ����recv��һ������ɾ͵��ø��Եĺ������Զ�����  ��1��Ȼ�������ٺ����ڲ�����Ӧ�Ĳ��� ��2��ע�⣺�첽���֮����е�֪ͨ
		2.�¼�֪ͨ  ��Ҫ���ǵ���WSAGetOverlappedResult�õ������Ȼ������߼����Լ����࣬������߼�̫�࣬����Լ�˼��
		3.��������  ���෽ʽ�ϣ�����������ܸ��ã�ϵͳ�Զ�����

�����߼���	1.�����¼�����,socket���飬�ص��ṹ������    �±���ͬ�İ󶨳�һ��
			2.�����ص�IOģ��ʹ�õ�socket    WSASocket
			3.Ͷ��AcceptEx   (1)������ɣ�   1���Կͻ����׽���Ͷ��WSARecv   
													1.�������
													2.�ص�������  ������Ϣ     �Կͻ����׽���Ͷ��WSARecv
											2����������Կͻ����׽���Ͷ��WSASend    
													1.�������
										            2.�ص�����
											3���Է������׽��ּ���Ͷ��AcceptEx   �ظ�����
							(2)�ӳ����    �Ǿ�ȥѭ�����ź�
		4.ѭ�����ź�	��1�����ź�   WSAWaitForMultipleEvents
						(2)���ź�    ��������   Ͷ��AcceptEx
						(3)��������Կͻ����׽���Ͷ��WSASend   �ص�����
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <mswsock.h>      //Ҫ����winsock2.h���棬��Ȼ�ᱨ��
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#define MAX_COUNT 1024
#define MAX_RECV_COUNT 1024

SOCKET g_allSock[MAX_COUNT];      //����socket����     �������¼������ʡ�ԣ��ص��ṹ����������¼���
OVERLAPPED g_allOlp[MAX_COUNT];   //�����ص��ṹ������    �±���ͬ�İ󶨳�һ�飬���ú���ʱ���Զ��󶨵�һ��
int g_count;

//���ջ�����
char g_strRecv[MAX_RECV_COUNT] = { 0 };

int PostAccept();
int PostRecv(int index);
int PostSend(int index);
void Clear()
{
	for (int i = 0; i < g_count; i++)
	{
		closesocket(g_allSock[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
	}
}

//�ص�����  (�����̨�˳�)
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		Clear();
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
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("������ʧ��\n");
		return 0;
	}

	//У��汾
	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
	{
		printf("�汾����\n");
		WSACleanup();
		return 0;
	}

	//�����ص�IOģ��ʹ�õ�socket
	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		printf("������socket����ʧ��,�����룺%d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	//��IP�Ͷ˿�
	struct sockaddr_in socketServerMsg;
	socketServerMsg.sin_family = AF_INET;
	socketServerMsg.sin_port = htons(12345);
	socketServerMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr *)&socketServerMsg, sizeof(socketServerMsg)))
	{
		printf("��IP�Ͷ˿�ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);

		WSACleanup();
		return 0;
	}

	//����
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		printf("����ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	g_allSock[g_count] = socketServer;
	g_allOlp[g_count].hEvent = WSACreateEvent();
	g_count++;


	//Ͷ��AcceptEx
	if (0 != PostAccept())
	{
		Clear();
		WSACleanup();
		return 0;
	}

	//ѭ���ȴ��¼�
	while (1)
	{
		DWORD nRes = WSAWaitForMultipleEvents(1, &g_allOlp[0].hEvent, FALSE, WSA_INFINITE, TRUE);    //����5�� WSA_INFINITE �۾͵�һ�������Ե����ź����߾���
																									//����6������TRUE  ���壺1.���ȴ��¼�������������̻��ƽ����һ��  
																														//2.ʵ�ֵȴ��¼�������������̺������첽ִ�У�ִ���겢���ȴ��¼������ź�     �ȴ��¼���������WSA_WAIT_IO_COMPLETION(һ���������ִ�����ˣ��ͻ᷵�����ֵ)
																														//3.��WSAWaitForMultipleEvents�����ܻ�ȡ�¼����ź�֪ͨ�����ܻ�ȡ������̵�ִ��֪ͨ
		if (WSA_WAIT_FAILED == nRes || WSA_WAIT_IO_COMPLETION == nRes)   
		{
			continue;
		}

		//���ź���
		//�ź��ÿ�
		WSAResetEvent(g_allOlp[0].hEvent);

		//�������������
		//PostSend(g_count);
		printf("accept\n");
		//Ͷ��Recv
		PostRecv(g_count);
		//�������Ͷ��send
		//�ͻ�������++
		g_count++;
		//Ͷ��accept
		PostAccept();
	}


	Clear();
	//���������
	WSACleanup();

	system("pause");
	return 0;
}

//Ͷ��AcceptEx
int PostAccept()
{
	//Ͷ���첽������������
	while (1)   //ѭ������
	{
		//�����ͻ���socket����Ӧ���¼�   ������2��Ҫ��
		g_allSock[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
		g_allOlp[g_count].hEvent = WSACreateEvent();

		//�����ַ�����   ������3��Ҫ��
		char str[1024] = { 0 };

		DWORD dwRecvcount;   //����7��Ҫ

		//Ͷ��AcceptEx
		BOOL bRes = AcceptEx(g_allSock[0], g_allSock[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &dwRecvcount, &g_allOlp[0]);
		if (TRUE == bRes)
		{
			//���������
			//Ͷ��Recv
			PostRecv(g_count);
			//�������Ͷ��send
			//�ͻ�������++
			g_count++;
			//Ͷ��accept,�ݹ鷽��, �ݹ������ᱬջ
			//PostAccept();
			continue;
		}
		else
		{
			int a = WSAGetLastError();
			if (ERROR_IO_PENDING == a)
			{
				//�ӳٴ���
				break;
			}
			else
			{
				break;
			}
		}
	}
	return 0;
}

//�ص�����
/*
	����ֵvoid
	����Լ�� CALLBACK
	�������� ����
	����1��������
	����2�����ͻ���յ����ֽ���
	����3���ص��ṹ
	����4������ִ�еķ�ʽ  ����WSARecv����5
	�������� 1.dwError == 10054  ǿ���˳�   ��1�����ж�   ��2��ɾ���ͻ���
			2.cbTransferred == 0   �����˳�     
			3. else   ��������
	��ӦWSAGetOverlapperResult����
*/
void CALLBACK RecvCall(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	//����һ�����ֻ�ȡ�±�ķ���Ч��̫��
	//int i = 0;
	//for (i; i < g_count; i++)
	//{
	//	if (lpOverlapped->hEvent == g_allOlp[i].hEvent)
	//	{
	//		break;
	//	}
	//}
	//��������
	int i = lpOverlapped - &g_allOlp[0];

	if (10054 == dwError || 0 == cbTransferred)
	{
		//ɾ���ͻ���
		printf("close\n");
		//�ر�
		closesocket(g_allSock[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
		//��������ɾ��
		g_allSock[i] = g_allSock[g_count - 1];
		g_allOlp[i] = g_allOlp[g_count - 1];
		//������1
		g_count--;
	}
	else
	{
		printf("%s\n", g_strRecv);
		memset(g_strRecv, 0, MAX_RECV_COUNT);
		//�������Ͷ��send
		//���Լ�Ͷ�ݽ���
		PostRecv(i);
	}
}

int PostRecv(int index)
{
	//Ͷ���첽������Ϣ

	WSABUF wsabuf;
	wsabuf.buf = g_strRecv;
	wsabuf.len = MAX_RECV_COUNT;

	DWORD dwRecvCount;
	DWORD dwflag = 0;

	int nRes = WSARecv(g_allSock[index], &wsabuf, 1, &dwRecvCount, &dwflag, &g_allOlp[index], RecvCall);
	if (0 == nRes)
	{
		//�������
		//��ӡ��Ϣ
		printf("%s\n", wsabuf.buf);
		memset(g_strRecv, 0, MAX_RECV_COUNT);
		//�������Ͷ��send
		//���Լ�Ͷ�ݽ���
		PostRecv(index);
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//�ӳٴ���
			return 0;
		}
		else
		{
			return a;
		}
	}

	return 0;
}

void CALLBACK SendCall(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	printf("send over\n");
}

int PostSend(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = "���";
	wsabuf.len = MAX_RECV_COUNT;

	DWORD dwSendCount;
	DWORD dwflag = 0;

	int nRes = WSASend(g_allSock[index], &wsabuf, 1, &dwSendCount, dwflag, &g_allOlp[index], SendCall);
	if (0 == nRes)
	{
		//�������
		//��ӡ��Ϣ
		printf("send�ɹ�\n");
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			//�ӳٴ���
			return 0;
		}
		else
		{
			return a;
		}
	}

	return 0;
}


/*
�Ա��¼�֪ͨ�� 1.�¼�֪ͨ�������Լ���������	��1������waitfor,����˳���ܱ�֤
											��2��ѭ�������࣬�±�Խ��Ŀͻ����ӳٻ�Խ��
			 2.�������������д�����������  ϵͳ���ݾ����¼��Զ��������ǵĴ��룬�Զ�����
			 3.����������̱��¼�֪ͨ�������Ժ�һЩ
���⣺�����¶ϵ�ʱ����һ�δ�������У��ͻ��˲������send���������������ν�����Ϣ����һ�ν�����Ϣ������������Ϣ
*/