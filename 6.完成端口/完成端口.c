//ʱ�䣺2019��8��13��20:23:23
//��ɶ˿�

/*
��ɶ˿�Ҳ��windows��һ�ֻ���   ���ص�IOģ�ͻ����ϵ��Ż�


�ص�IO��������     1.�¼�֪ͨ   ��1������
								��2��ѭ��ѯ��  �ӳٸߣ����˺ܶ����ù�
								��3�����ö��߳� ������Ѻ��� ����̫��

				   2.�������   ��1���߳�����̫��  ÿ���ͻ��˶���һ���߳�ȥ���ûص�����

				   3.�̹߳���   ��1���л��߳����Ĵ���CPU��Դ/ʱ��
								��2��Ϊʲô			CPU�����ж��߳�ִ��״��	1����һ��ʱ��Ƭ�����ڣ�ÿ���߳�ִ��һ����ʱ��
																			2������һ��ʱ��Ƭ������1ms����100���߳��ڵ��������У���ô��Щ�߳�һ��ʱ�������ڷֵ�0.01msִ��ʱ�䣬ʱ�䵽������ִ�е�ʲôλ�ã�
																				�����л���һ���߳�ִ�У���ˣ���Щ�߳̾��ڲ�ͣ���л���ִ�У������ٶ�̫�죬�����˷ֱ治���������Ծ�չ�ֳ�ͬʱ���е�Ч��      ��ҵ��Կ�̫������Ῠ�����������һ��ԭ��  ������α���߳� ƭ�˵�
													����ͬһ��Ӧ�ó����ڵ�����ִ�ж���̣߳������ϻ��ǵ��̵߳�ִ��Ч�ʣ�����һ���߳�ִ����Ҫ1s����ô10���̣߳��ܹ�ִ������Ҫ10s������������10������Ҫ�����߳��л���ʱ�䣬���Դ���10s
													����ж��̵߳�ִ��״��  1������˵��̣߳��ﵽ��������ͬʱ����
																			2�����Լ���һ̨������8��CPU����ôһ�����򴴽�8���̣߳�����ϵͳ���Զ���8���̷ָ߳�8���ˣ��Ӷ��ﵽ����ߵ�Ч��
													��������£����������źܶ��������м�ǧ���߳�ͬʱ����    ϵͳ�����������Ϊ��λ�����̷߳���ִ�е�
				  4.�����߳����Ƕ��٣�  ��1�������ϸ�CPU����һ������õ�
										��2�������Ǽ���   1�������м��ִ�   CPU����  CPU����*2  CPU����*2+2
														  2��CPU�и���Ҫ��������n��m�߳�    ����i7 700K   4��8�߳�  4��ָ�����ĺˣ�Ҳ����CPU��ʵ����    ��������ѵ��߳�����4�����������ض��ڴ�ʹ���˳��̼߳�����ʹ�������˿���ģ���˫�˵����ܣ�������8��������������Ӧ���и�8��ûɶ����   ��Ӧ�þ���CPU����*2������
																							    i7 9700K  8��8�߳�  �������û��ʹ�ó��̼߳��������˵��߳�   �����߳���=CPU����
										��3��CPU����*2+2  ����������Ǹ���ʵ��Ӧ���еľ������  �̺߳����п��ܻ����Sleep(),WSAWaitForMultipleEvents()���ຯ������ʹ�̹߳��𣨲�ռCPUʱ��Ƭ�����Ӷ�ʹ��CPUĳ���˿����ˣ���Ͳ����ˣ�����һ��ཨ�����������Խ�������������CPU��ͣЪ���Ӷ��������ϱ�֤�����ִ��Ч��
										��4���������     1��ĳһ��������CPUһ�����ڷֵ�ʱ��Խ�ࣨ�������̸߳��ࣩ���Ǿ�Խ��    �����̷ֵ߳�ʱ���Խ����
														  2�������л�ʱ��   �����ͻ��и�ƿ��    ϵͳ���ܷ�������
														  3�����ݳ����㲻ͬ������Ӧ�������߳�
��ɶ˿�		1.ģ����Ϣ���У�����һ��֪ͨ���У�ϵͳ����   ��֤���򣬲������ù�
				2.��������������߳�   �������CPU������


�����߼�	 ԭ��	1.���ص��׽��֣��ͻ���+����������һ����ɶ˿ڣ�һ�����͵ı���������һ��
					2.ʹ��AcceptEx, WSARecv, WSASendͶ������
					3.��ϵͳ�첽������󣬾ͻ��֪ͨ���һ�����У����Ǿͽ���֪ͨ���У��ö����ɲ���ϵͳ������ά��
					4.��ɶ˿ڿ������Ϊ������е�ͷ������ͨ��GetQueueCompletionStatus�Ӷ���ͷ�����ã�һ��һ������

			 ���룺	1.������ɶ˿�  CreateIoCompletionPort
					2.����ɶ˿���socket��   CreateIoCompletionPort
					3.����ָ����Ŀ���߳�   ��1��CPU����һ��   CreateThread
										   ��2���߳��ڲ�   1��GetQueueCompletionStatus  2�����ദ��
					4.ʹ��AcceptEx, WSARecv, WSASendͶ������
					5.���߳����� ���������Ļ������ִ�����ˣ�����͹ر��ˣ�

*/





#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <mswsock.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mswsock.lib")

#define MAX_COUNT 1024
#define MAX_RECV_COUNT 1500

SOCKET g_allsockets[MAX_COUNT];
OVERLAPPED g_allOlp[MAX_COUNT];
int g_count;
//int g_newsocketID;
HANDLE hPort;
HANDLE *pThread;
int nProcessorsCount;


//���ջ�����
char g_strrecv[1500] = {0};

int PostAccept(void);    //C���Լ�void��ʾ�����ܲ����� ���Ϊ�գ���ʾ����������ȷ��
int PostRecv(int index);
int PostSend(int index);
void Clear(void)
{
	for (int i = 0; i < g_count; i++)
	{
		if (0 == g_allsockets[i])
			continue;
		closesocket(g_allsockets[i]);
		WSACloseEvent(g_allOlp[i].hEvent);
	}
}

BOOL g_flag = TRUE;

//�̺߳���
DWORD WINAPI ThreadProc(LPVOID lpPrameter);

//�ص�����  (�����̨�˳�)
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		CloseHandle(hPort);
		Clear();

		g_flag = FALSE;    //����̨����˳�ʱ�ر����߳�

		//�ͷ��߳̾��
		for (int i = 0; i < nProcessorsCount; i++)
		{
			//TerminateThread();
			CloseHandle(pThread[i]);
		}
		free(pThread);

		break;
	}

	return TRUE;
}

int main()
{
	//����̨�˳� ������Ͷ��һ������
	SetConsoleCtrlHandler(fun, TRUE);

	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("������ʧ��\n");
		return -1;
	}

	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
	{
		printf("�汾����\n");
		WSACleanup();
		return -1;
	}

	SOCKET socketServer = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == socketServer)
	{
		printf("����������socketʧ�ܣ�������:%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	struct sockaddr_in socketServerMsg;
	socketServerMsg.sin_family = AF_INET;
	socketServerMsg.sin_port = htons(12345);
	socketServerMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketServer, (struct sockaddr *)&socketServerMsg, sizeof(socketServerMsg)))
	{
		printf("��IP�Ͷ˿�ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}

	
/*
	���ܣ�1.����һ����ɶ˿�    2.����ɶ˿���SOCKET����һ��
	����1��	1.������ɶ˿�    INVALID_HANDLE_VALUE	(1)��ʱ����2ΪNULL
													(2)����3����  ���0
			2.�󶨵Ļ�   ������socket
	����2��1.������ɶ˿�    NULL
		  2.�󶨵Ļ�    ��ɶ˿ڱ���
	����3��1.������ɶ˿�   0
		  2.�󶨵Ļ�    ��1���ٴδ���socketServer    Ҳ���Դ���һ���±꣬�����
						��2����ϵͳ���յ��Ķ�Ӧ�����ݹ�����һ��
	����4��1.������ɶ˿�   ��1������˶˿������ͬʱ���е��߳�����
						   ��2����CPU�ĺ�������    �����Լ���ȡ    GetSystemInfo
						   (3)����д0   ��ʾĬ����CPU����
		  2.�󶨵Ļ�     ��1�����Դ˲�������0������
						��2����˵���Ե�ʱ��������ɶ����û�����õģ�����һ����0
	����ֵ��1.�ɹ�  ��1��������2ΪNULL,   ����һ���µ���ɶ˿�
					��2��������2��ΪNULL    �����Լ�
					��3��������1Ϊsocket    ������socket�󶨺�Ķ˿�
		   2.ʧ�ܷ���0    (1)GetLastError()
						 (2)��ɶ˿�Ҳ��windows��һ�ֻ���      ��������ר�����ļ�����������
*/

	g_allsockets[g_count] = socketServer;
	g_allOlp[g_count].hEvent = WSACreateEvent();
	g_count++;

	//������ɶ˿�
	hPort =  CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (0 == hPort)
	{
		int a = GetLastError();
		printf("%d\n", a);
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}

	//��
	HANDLE hPort1 = CreateIoCompletionPort((HANDLE)socketServer, hPort, 0, 0);
	if (hPort1 != hPort)
	{
		int a = GetLastError();
		printf("%d\n", a);
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}

	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		printf("����ʧ�ܣ������룺%d\n", WSAGetLastError());
		CloseHandle(hPort);
		closesocket(socketServer);
		WSACleanup();
		return -1;
	}


	if (0 != PostAccept())
	{
		Clear();
		WSACleanup();
		return -1;
	}

	//�����߳���������
	SYSTEM_INFO SystemProcessorsCount;
	GetSystemInfo(&SystemProcessorsCount);
	nProcessorsCount = SystemProcessorsCount.dwNumberOfProcessors;
	//����
/*
	���ܣ�����һ���߳�
	����1��1.�߳̾���Ƿ񱻼̳�    NULL���̳�
		  2.ָ���̵߳�ִ��Ȩ��     NULLĬ�ϵ�
	����2��1.�߳�ջ��С      ��0��ϵͳʹ��Ĭ�ϴ�С    1M
		  2.�ֽ�Ϊ��λ
	����3���̺߳�����ַ     ��1��DWORD WINAPI ThreadProc(LPVOID lpPrameter)
							(2)����LPVOID lpPrameter�����ǲ���4���ݽ���������
	����4���ⲿ���̴߳�������
	����5��1. 0     �߳�����ִ��
		  2.  CREATE_SUSPENDED    �̴߳��������״̬������ResumeThread��������
		  3.  STACK_SIZE_PARAM_IS_A_RESERVATION    (1)�����ˣ�����2����ջ������С    �����ڴ���ջ�Ĵ�С     1M
												   ��2��δ���ã�����ջ�ύ��С    �����ڴ���ջ�Ĵ�С     4KB
	����6���߳�ID    ������NULL
	����ֵ��1.�ɹ�   �����߳̾��    ��1���ں˶���
									��2�����Ҫ�ͷ�   CloseHandle
		   2.ʧ��    NULL    GetLastError   �õ�������
*/
	pThread = (HANDLE)malloc(sizeof(HANDLE) * nProcessorsCount);
	for (int i = 0; i < nProcessorsCount; i++)
	{
		pThread[i] = CreateThread(NULL, 0, ThreadProc, hPort, 0, NULL);
		if (NULL == pThread[i])
		{
			int a = GetLastError();
			printf("%d\n", a);
			CloseHandle(hPort);
			closesocket(socketServer);
			WSACleanup();
			return -1;
		}
	}

	//����
	while (1)
	{
		Sleep(1000);    //���̹߳���1s����ռ��CPUִ�����ڣ��ڳ�1S��CPU,���������̣߳���CPU�Ѻ�
	}

	//�ͷ��߳̾��
	for (int i = 0; i < nProcessorsCount; i++)
	{
		CloseHandle(pThread[i]);
	}
	free(pThread);

	CloseHandle(hPort);
	Clear();
	WSACleanup();
	system("pause");
	return 0;
}

//�̺߳���
DWORD WINAPI ThreadProc(LPVOID lpPrameter)
{
/*
	��֪ͨʱ���̹߳��𣬲�ռ��CPU��ʱ�䣬�ǳ���
	���ܣ����Դ�ָ����I/O��ɶ˿�ȡI/O������ݰ�
	����1����ɶ˿�    ����Ҫ��������������
	����2�����ջ��߷��͵��ֽ���
	����3����ɶ˿ں�������3��������
	����4���ص��ṹ
	����5��	1.�ȴ�ʱ��    ��û�пͻ�����Ӧʱ��֪ͨ������ʲô��û�У���������Ҳget����ʲô��������ô��һ�ỹ��һֱ��
			2.INFINITE    һֱ�ȣ�����Ҳ������
	����ֵ��1.�ɹ�����TRUE
	2.ʧ�ܷ���FALSE   GetLastError
	continue
*/
	HANDLE port = (HANDLE)lpPrameter;
	DWORD NumberOfBytes;
	ULONG_PTR index;
	LPOVERLAPPED lpOverlapped;

	while (g_flag)
	{
		BOOL bFlag = GetQueuedCompletionStatus(port, &NumberOfBytes, &index, &lpOverlapped, INFINITE);
		if (FALSE == bFlag)
		{
			int a = GetLastError();
			if (64 == a)
			{
				printf("force close\n");
			}
			printf("�����룺%d\n", a);
			continue;
		}

		//����
		//accept
		if (0 == index)
		{
			printf("accept\n");
			//�󶨵���ɶ˿�
			HANDLE hPort1 = CreateIoCompletionPort((HANDLE)g_allsockets[g_count], hPort, g_count, 0);
			if (hPort1 != hPort)
			{
				int a = GetLastError();
				printf("%d\n", a);
				closesocket(g_allsockets[g_count]);
				continue;
			}
			PostSend(g_count);
			//�¿ͻ���Ͷ��recv
			PostRecv(g_count);
			g_count++;
			PostAccept();
		}
		else
		{
			if (0 == NumberOfBytes)
			{
				//�ͻ�������
				printf("close\n");
				//�ر�
				closesocket(g_allsockets[index]);
				WSACloseEvent(g_allOlp[index].hEvent);

				g_allsockets[index] = 0;       //socket���±���ˣ�����ֱ�Ӱ����һ��socket��ֵ��������������ֵ0��֮���ͷ�0�ͱ��ͷ���
				g_allOlp[index].hEvent = NULL;
			}
			else
			{
				if (g_strrecv[0] != 0)
				{
					//�յ�recv
					printf("%s\n", g_strrecv);
					memset(g_strrecv, 0, sizeof(g_strrecv));
					//Ͷ��
					PostRecv(index);

				}
				else
				{
					//send
					printf("send ok\n");
				}
			}
		}
	}
	return 0;
}

int PostAccept()
{
	g_allsockets[g_count] = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_allOlp[g_count].hEvent = WSACreateEvent();
	//g_newsocketID = g_count;     //PostRecv��PostSendӦ���Ǵ�������һ��socket��������socket��Ч����,��Ϊɾ��socket�Ե�����Ч����g_count�仯����ʱ�ٴ���g_count�����       ���л���������

	char str[1500] = {0};
	DWORD recv_count;
	BOOL bFlag = AcceptEx(g_allsockets[0], g_allsockets[g_count], str, 0, sizeof(struct sockaddr_in) + 16, sizeof(struct sockaddr_in) + 16, &recv_count, &g_allOlp[0]);
	
	//��ɶ˿ڲ���Ҫ����������ɻ����첽��ɣ�ֻҪ��ɾͻ����֪ͨ��֪ͨ�ͻ�װ��֪ͨ���У��߳̾���ȡ��֪ͨ
	//if (TRUE == bFlag)
	//{
	//	HANDLE hPort1 = CreateIoCompletionPort((HANDLE)g_allsockets[g_count], hPort, g_count, 0);
	//	if (hPort1 != hPort)
	//	{
	//		int a = GetLastError();
	//		printf("%d\n", a);
	//		closesocket(g_allsockets[g_count]);
	//		//continue;
	//	}

	//	printf("accept");
	//	PostRecv(g_count);
	//	PostSend(g_count);
	//	g_count++;
	//	PostAccept();
	//	return 0;
	//}
	//else
	//{
	//	int a = WSAGetLastError();
	//	if (ERROR_IO_PENDING == a)
	//	{
	//		return 0;
	//	}
	//	else
	//	{
	//		return a;
	//	}
	//}
	int a = WSAGetLastError();
	if (ERROR_IO_PENDING != a)
	{
		//����ִ�г���
		return -1;
	}
	return 0;
}

int PostRecv(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = g_strrecv;
	wsabuf.len = MAX_RECV_COUNT;

	DWORD read_count;
	DWORD flag = 0;
	int nRes = WSARecv(g_allsockets[index], &wsabuf, 1, &read_count, &flag, &g_allOlp[index], NULL);

	//��ɶ˿ڲ���Ҫ����������ɻ����첽��ɣ�ֻҪ��ɾͻ����֪ͨ��֪ͨ�ͻ�װ��֪ͨ���У��߳̾���ȡ��֪ͨ
	/*if (0 == nRes)
	{
		printf("%s\n", g_strrecv);
		memset(g_strrecv, 0, MAX_RECV_COUNT);
		PostRecv(index);
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			return 0;
		}
		else
		{
			return a;
		}
	}*/
	int a = WSAGetLastError(); 
	if (ERROR_IO_PENDING != a)    //�����⣺WSASend����ִ���������nRes=0, ��ʱȡ������a=0�� a != ERROR_IO_PENDINGҲ�����if�ڲ���
	{
		//����ִ�г���
		return -1;
	}
	return 0;
}

int PostSend(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = "���ӳɹ�";
	wsabuf.len = MAX_RECV_COUNT;

	DWORD send_count;
	DWORD flag = 0;
	int nRes = WSASend(g_allsockets[index], &wsabuf, 1, &send_count, flag, &g_allOlp[index], NULL);

	//��ɶ˿ڲ���Ҫ����������ɻ����첽��ɣ�ֻҪ��ɾͻ����֪ͨ��֪ͨ�ͻ�װ��֪ͨ���У��߳̾���ȡ��֪ͨ
	/*if (0 == nRes)
	{
		printf("send�ɹ�\n");
		return 0;
	}
	else
	{
		int a = WSAGetLastError();
		if (ERROR_IO_PENDING == a)
		{
			return 0;
		}
		else
		{
			return a;
		}
	}*/
	int a = WSAGetLastError();       //�����⣺WSASend����ִ���������nRes=0, ��ʱȡ������a=0�� a != ERROR_IO_PENDINGҲ�����if�ڲ���
	if (ERROR_IO_PENDING != a)
	{
		//�ӳٴ���
		//����ִ�г���
		return -1;
	}
	return 0;
}