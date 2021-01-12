//ʱ�䣺2019��8��8��17:18:52
//�ص�I/O

/*
�ص�IO���ܣ�
���壺�ص�IO��windows�ṩ��һ���첽��д�ļ��Ļ���
���ã�	1.������д�ļ���socket���ʾ����ļ�������,��recv����������,��Э�黺�����е�����ȫ�����ƽ�buffer������Ž��������ظ��Ƶĸ�����дҲһ����ͬһʱ��ֻ�ܶ�дһ����������ȫ�����ߵ�
		2.�ص�IO���ƶ�д��������ָ���Լ����ǵ�bufferͶ������ϵͳ��Ȼ����ֱ�ӷ��أ�����ϵͳ���������̣߳������ݸ��ƽ����ǵ�Buffer�����ݸ����ڼ䣬���Ǿ�ȥ�������¶����������󣬼���д���̱�����첽������ͬʱͶ�ݶ����д����
		3.������accept, recv, send�Ż������첽���̣���AcceptEx WSARecv WSASend��������     ����˵����ǶԻ���c/sģ�͵�ֱ���Ż�
���ʣ�	1.�ṹ�壺WSAOVERLAPPED  struct _WSAOVERLAPPED
                              {										ǰ�ĸ���Աϵͳʹ�ã����ǲ���Ҫֱ��ʹ��
							       DWORD Internal;                  ��Ա1���������������ṩ�̶���ʹ��   ����������ߣ�
								   DWORD InternalHigh;              ��Ա2���������������ṩ�̶���ʹ��    ���ջ��߷��͵��ֽ���
								   DWORD Offset;                    ��Ա3���������������ṩ�̶���ʹ��
								   DWORD OffsetHigh;                ��Ա4���������������ṩ�̶���ʹ��   һ���Ǵ�����
								   WSAEVENT hEvent;                 ��Ա5���¼�����   ����Ψһ��ע��    ����������ͻᱻ�ó����ź�
							  }
		2.����һ���ýṹ��ı�����socket��
ʹ�ã�	1.�첽ѡ��ģ�Ͱ���Ϣ��socket��һ��Ȼ��ϵͳ����Ϣ���ƴ�����
		2.�¼�ѡ��ģ�Ͱ��¼���socket��һ��Ȼ��ϵͳ���¼����ƴ�����
		3.�ص�IOģ�Ͱ��ص��ṹ��socket��һ��Ȼ��ϵͳ���ص�IO���ƴ�����
�ص�IO�������ַ�ʽ��1.�¼�֪ͨ
					2.�������   �ص�����

�ص�IO�߼���1.�¼�֪ͨ����1������AcceptEx WSARecv WSASendͶ��
                        ��2������ɵĲ������¼��ź��ó����ź�
						��3������WSAWaitForMultipleEvents��ȡ�¼��ź�
		    2.�ص�����/������� ��1������AcceptEx WSARecv WSASendͶ��
			                    ��2����ɺ��Զ����ûص�����
���ܣ�1.�����в��Խ����    20000���ͻ���ͬʱ���������Լ�������Ϣ   ��1��ϵͳCPUʹ��������40%����  ��2����ɶ˿�ֻ��2%����  ��Ʒ
	 2.����ģ��Ҳ�ܴ�����ô��   ����ͬ������������ԭ�򣬵����ӳ�̫��
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

//��һ��,�����¼����飬socket���飬�ص��ṹ������ �±���ͬ�İ󶨳�һ��
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

	//�ڶ����������ص�IOģ��ʹ�õ�socket
/*
		���ã�	1.����һ�������첽������socket
				2.WSASocket windowsר��   (1)WSA�ĺ�������windowsר�õ�   (2)windows socket async ��������֧���첽����
		����1����ַ������  AF_INET
		����2���׽��ֵ�����   TCP��SOCK_STREAM
		����3��Э�������
		����4��	1.�����׽�����ϸ������   ��1��ָ��WSAPROTOCOL_INFO�ṹ��ָ��
										��2�����磺���������Ƿ���Ҫ���ӣ�accept  connect�����Ƿ�֤������������(������ݶ�ʧ�Ƿ�����)������3��0����ôƥ���ĸ�Э�飻���ô�������ֽ����������׽���Ȩ�ޣ����кܶౣ���ֶΣ����Ժ���չʹ��
				2.��ʹ�� NULL

		����5��	1.һ��socket����ID,�������һ�β������socket
				2.�����ģ���ʱ���ã���0
		����6��	1.ָ���׽�������
				2.��дWSA_FLAG_OVERLAPPED   ����һ�����ص�IOģ��ʹ�õ�socket
				3.�����ģ�1��WSA_FLAG_MULTIPOINT_C_ROOT; WSA_FLAG_MULTIPOINT_C_LEAF; WSA_FLAG_MULTIPOINT_D_ROOT; WSA_FLAG_MULTIPOINT_D_LEAF   �⼸�����ڶಥЭ��
						��2��WSA_FLAG_ACCESS_SYSTEM_SECURITY	1���׽��ֲ���Ȩ��/��ϲ���4ʹ��
																2�����壺���Զ��׽���send����Ȩ�ޣ��ͻᴥ����ص�Ȩ�����ã�����
						��3��WSA_FLAG_NO_HANDLE_INHERIT	1���׽��ֲ��ɱ��̳�
														2���ڶ��߳̿����У����̻߳�̳и��̵߳�socket,�����̴߳�����һ��socket,��ô���߳�������ʹ�÷�ʽ �� һ��ֱ���ø���ģ����Ӷ�ʹ����һ��socket--����   �����Ѹ�������socket����һ�ݣ��Լ��ã���������������socket�����Ǳ���һ��--�̳�
		����ֵ��1.�ɹ����ؿ��õ�socket  �����˾�һ��Ҫ�����׽���
				2.ʧ�ܷ���INVALID_SOCKET
*/
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


	//��������Ͷ��AcceptEx
	/*
		1.�������	��1���Կͻ����׽���Ͷ��WSARecv
							1���������  ������Ϣ  �Կͻ����׽���Ͷ��WSARecv   �ظ�����
							2���ӳ����  �Ǿ�ȥѭ������Ϣ
					��2) ��������Կͻ����׽���Ͷ��WSASend
							1���������  ������Ϣ       �ظ�����
							2���ӳ����  �Ǿ�ȥѭ������Ϣ
					��3) �Է������׽��ּ���Ͷ��AcceptEx �ظ�����
		2.�ӳ����  �Ǿ�ȥѭ������Ϣ
	*/
	if (0 != PostAccept())
	{
		Clear();
		WSACleanup();
		return 0;
	}
	
	//ѭ���ȴ��¼�
	/*
		1.���ź� WSAWaitForMultipleEvents
		2.���ź�	��1) ��ȡ�ص��ṹ�ϵ���Ϣ   WSAGetOverlappedResult
					��2���ͻ����Ƴ�       ɾ������˵���Ϣ
					��3����������   Ͷ��AcceptEx  �ϱ�����߼�д���ˣ���װ�ɺ����ͷǳ�����
					��4��������Ϣ   ������Ϣ    �Կͻ����׽���Ͷ��WSARecv
					��5��������Ϣ   ��������Կͻ����׽���Ͷ��WSASend
	*/
	while (1)
	{
		for (int i = 0; i < g_count; i++)
		{
			DWORD nRes = WSAWaitForMultipleEvents(1, &g_allOlp[i].hEvent, FALSE, 0, FALSE);
			if (WSA_WAIT_FAILED == nRes || WSA_WAIT_TIMEOUT == nRes)
			{
				continue;
			}

			//���ź���
			/*
				���ܣ���ȡ��Ӧ��socket�ϵľ������ 
				����1�����źŵ�socket
				����2����Ӧ���ص��ṹ
				����3��	1.�ɷ��ͻ��߽��յ���ʵ���ֽ���
						2. 0 ��ʾ�ͻ�������
				����4��	1.�����ص�����ѡ���˻����¼������֪ͨʱ�����ܽ�fWait��������ΪTRUE
						2.ѡ���¼�֪ͨ����TRUE
				����5��	1.װWSARecv�Ĳ���5 lpflags
						2.������NULL
				����ֵ��1.����ִ�гɹ�����TRUE
						2.ʧ�ܷ���FALSE
			*/
			DWORD dwState;
			DWORD dwFlag;
			BOOL bFlag = WSAGetOverlappedResult(g_allSock[i], &g_allOlp[i], &dwState, TRUE, &dwFlag);

			//�ź��ÿ�
			WSAResetEvent(g_allOlp[i].hEvent);

			if (FALSE == bFlag)
			{
				int a = WSAGetLastError();
				if (10054 == a)    //��Recvһ��
				{
					//�ͻ���ǿ���˳�
					printf("force close\n");
					//�ر�
					closesocket(g_allSock[i]);
					WSACloseEvent(g_allOlp[i].hEvent);
					//��������ɾ��
					g_allSock[i] = g_allSock[g_count - 1];
					g_allOlp[i] = g_allOlp[g_count - 1];
					//ѭ�����Ʊ�����1
					i--;
					//������1
					g_count--;
				}
				continue;
			}
			//�ɹ�
			if (0 == i)
			{
				//�������������
				printf("accept\n");
				PostSend(g_count);
				//Ͷ��Recv
				PostRecv(g_count);
				//�������Ͷ��send
				//�ͻ�������++
				g_count++;
				//Ͷ��accept
				PostAccept();
				continue;
			}

			if (0 == dwState)      //acceptʱdwState��Ȼ��0������Ӧ�����ж�accept���ж��Ƿ���������
			{
				//�ͻ�����������
				printf("close\n");
				//�ر�
				closesocket(g_allSock[i]);
				WSACloseEvent(g_allOlp[i].hEvent);
				//��������ɾ��
				g_allSock[i] = g_allSock[g_count - 1];
				g_allOlp[i] = g_allOlp[g_count - 1];
				//ѭ�����Ʊ�����1
				i--;
				//������1
				g_count--;
				continue;
			}

			if (0 != dwState)
			{
				//���ͻ��߽��ճɹ���
				if (g_strRecv[0] != 0)
				{
					//recv
					//��ӡ��Ϣ
					printf("%s\n", g_strRecv);
					memset(g_strRecv, 0, MAX_RECV_COUNT);
					//�������Ͷ��send
					//���Լ�Ͷ�ݽ���
					PostRecv(i);
				}
				else
				{
					//send
				}
			}
		}
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
/*
	���ܣ�Ͷ�ݷ�����socket,�첽��������
	����1��������socket
	����2�����ӷ������Ŀͻ��˵�socket   1.ע�⣺����Ҫ����WSASocket���Դ���  
										2.AcceptEx���ͻ��˵�IP�˿ںŰ������socket��
    ����3��	1.��������ָ�룬�������������Ϸ��͵ĵ�һ������	��1���ͻ��˵�һ��send,�������������
															��2���ڶ����Ժ����WSARecv����
			2.char str[1024]
			3.��������NULL

	����4��	1.����0  ȡ���˲���3�Ĺ���
			2.���óɲ���3��1024��	��1����ô�ͻ���յ�һ�ε�����
									��2����ʱ���ͻ������Ӳ�������һ����Ϣ���������Ų����ź�  �� ���Ի�������״���������Ӳ��У�����ȿͻ��˷���һ����Ϣ�������������źţ���������0������

    ����5��	1.Ϊ���ص�ַ��Ϣ�������ֽ�������ֵ�������ٱ�ʹ�õĴ���Э�������ַ���ȳ�16���ֽ�   (����ȥ����ͻ��˵�ַ��Ϣ�ռ�Ĵ�С)
			2.sizeof(struct sockaddr_in) + 16

    ����6��	1.ΪԶ�̵�ַ��Ϣ�������ֽ�������ֵ�������ٱ�ʹ�õĴ���Э�������ַ���ȳ�16���ֽڣ�����Ϊ0    �������˼���������ڴ���������ռ�ȥ��ʱ�洢�ͻ��˵ĵ�ַ��Ϣ��    ���ļ���ַ�������ַ��
			2.sizeof(struct sockaddr_in) + 16

	����7��	1.�ú������Խ��յ�һ�οͻ��˷�������Ϣ����������Ϣ�պ��ǵ���ʱ����յ��ˣ�Ҳ���������յ��ˣ��ͻ������ӵ�ͬʱ��������Ϣ������ʱ��װ�Ž��յ����ֽ�������ϲ���3��4ʹ�ã�   
				��1��Ҳ�������������ͬ����ɵ�ʱ����������ᱻװ��
				��2��û����������Ҳ���첽���յ���Ϣ����ʱ���������û��
			2.��ҿ�����д����1����дNULL,����������ֽ���    ��2��Ҳ������дDWORD�����ĵ�ַ�� ���������
	����8�����ǵ��ص��ṹ(������socket��Ӧ���ص��ع�)
	����ֵ��1.TRUE   ��������    ��ִ�о��Ѿ��пͻ��˵���
	       2.FALSE   ����    ��1��int a = WSAGetLatError()
							��2����� a == ERROR_IO_PENDING  �첽�ȴ��� �ͻ��˻�û��
							��3�����������ݴ�������
*/
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
		printf("accept\n");
		PostSend(g_count);
		PostRecv(g_count);
		//�������Ͷ��send
		//�ͻ�������++
		g_count++;
		//Ͷ��accept
		PostAccept();

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
}

int PostRecv(int index)
{
/*
    ���ã�Ͷ���첽������Ϣ
	����1���ͻ���socket
	����2�����պ����Ϣ�洢buffer    WSABUF    struct _WSABUF{ULONG len;  CHAR *buf}    ��Ա1���ֽ���   ��Ա2��ָ���ַ������ָ��
	����3������2�Ǹ�WSABUF�ĸ���  1��
	����4��	1.���ճɹ��Ļ�������װ�ųɹ����յ����ֽ���
			2.����6�ص��ṹ��ΪNULL��ʱ�򣬴˲�����������ΪNULL

	����5��	1.ָ�������޸�WSARecv����������Ϊ�ı�־��ָ��
			2.MSG_PEEK   Э�黺������Ϣ���Ƴ�������ɾ������recv����5һ��
			3.MSG_OOB    ��������
			4.MSG_PUSH_IMMEDIATE        ֪ͨ���;�����ɣ�1�����紫��10M���ݣ�һ��ֻ�ܴ�1M�������Ҫ���10�ݷ��ͣ���һ�ݷ����У�����9��Ҫ�Ŷӵȴ���ָ���������ǣ���ôָʾ�˿�㣬����ˣ���ôû�����͵ľͱ������ˣ��Ӷ���������ݷ���ȱʧ
		                                                ��2���ò������������ڶ����ݵķ���    ���������û���⣬�����ļ�ʲô�ľͲ�������
														��3����ʾϵͳ���촦�������ܼ���һ�����ӳ�
			5.MSG_WAITALL   �������ṩ�Ļ����������������ѹرգ�������ȡ����������        �Ű����ݷ��ͳ�ȥ
			6.MSG_PARTIIAL    �����ģ�sendָ����recv���벻ָ����       ��ʾ���Ǵ˴ν��յ��������ǿͻ��˷�����һ���֣�������������һ����

	����6���ص��ṹ
	����7���ص�����   ���������ʽʹ��    �¼���ʽ���Բ�ʹ��  ����NULL
	����ֵ��1.��������  0   ������Ƚ��У����̾ͷ��ͳ�ȥ��
	       2.SOCKET_ERROR   int a = WSAGetLastError()    a == WSA_IO_PENDING��ʾ���Լ�ȥ�ɰ�
*/

	WSABUF wsabuf;
	wsabuf.buf = g_strRecv;
	wsabuf.len = MAX_RECV_COUNT;

	DWORD dwRecvCount;
	DWORD dwflag = 0;

	int nRes = WSARecv(g_allSock[index], &wsabuf, 1, &dwRecvCount, &dwflag, &g_allOlp[index], NULL);
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


int PostSend(int index)
{
	WSABUF wsabuf;
	wsabuf.buf = "���";
	wsabuf.len = MAX_RECV_COUNT;

	DWORD dwSendCount;    //�ɹ����͵��ֽ���
	DWORD dwflag = 0;     //����������Ϊ�ı�־

	int nRes = WSASend(g_allSock[index], &wsabuf, 1, &dwSendCount, dwflag, &g_allOlp[index], NULL);
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
