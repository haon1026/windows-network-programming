//ʱ�䣺2019��7��30��12:59:01
//client

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>   //�򿪸��ĵ��ɿ����汾��Ϣ
#pragma comment(lib, "ws2_32.lib")   //windows socket�ڶ���32λ��lib��Դ�ļ�����õĶ������ļ�
//#include <WinSock.h>    ��Ӧ�� wsock32.lib 


int main(void)
{
	//1.�������
	WORD wdVersion = MAKEWORD(2, 2);			//�����汾�ؼ��֣����ֽڷŸ��汾�ţ����ֽڷ����汾��
	//int a = *((char *)&wdVersion);			//���2��ȡwdVersion�ĵ͵�ַλ
	//int b = *((char *)&wdVersion + 1);		//���1��ȡwdVersion�ĸߵ�ַλ

	WSADATA wdSockMsg;								 //��һ��д������������ֱ��ȡ��ַ
	//LPWSADATA lpw = malloc(sizeof(WSADATA));		 //�ڶ���д��������ָ�����ָ��̬����WSADATA��Ҫfree,LPWSADATA������WSADATA* 

	int nRes = WSAStartup(wdVersion, &wdSockMsg);     //�����������������һ������������İ汾�ؼ��֣��ڶ���������ָ��WSADATA�ṹ��ĵ�ַ��������������Ϣ����������ṹ����
	//if (WSAStarup(wdVerison, &wdSockMsg) != 0)
	if (0 != nRes)    //��������򿪳ɹ�������ֵ��0
	{
		switch (nRes)
		{
		case WSASYSNOTREADY:
			printf("�����µ������ԣ����߼�������");
			break;
		case WSAVERNOTSUPPORTED:
			printf("����������");
			break;
		case WSAEINPROGRESS:
			printf("�������������");
			break;
		case WSAEPROCLIM:
			printf("�볢�Թص�����Ҫ���������Ϊ��ǰ���������ṩ������Դ");
			break;
			//case WSAEFAULT :   //WSAStartup�ڶ���������ָ�����д���ˣ�����Ա���
			//	break;
		}
		return 0;
	}
	//printf("%s %s\n", wdSockMsg.szDescription, wdSockMsg.szSystemStatus);

	//2.У��汾
	if (2 != HIBYTE(wdSockMsg.wVersion) || 2 != LOBYTE(wdSockMsg.wVersion))
	{
		//˵���汾���ԣ�����2.2�汾
		//���������,�п�������ʧ��
		if (SOCKET_ERROR == WSACleanup())		//��������0
		{
			int a = WSAGetLastError();			//����ʧ�ܿ��øú�����ȡ�����룬���ô�����ɲ��Ҵ�����Ϣ
		}
		return 0;
	}

	//3.����SOCKET,��������socket���ͻ�������ȥ���ӷ��������ޱ�ĵط������ӿͻ���
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);				//SOCKET��һ���������ͣ�һ��������������Ψһ�ģ���ʶ�ŵ�ǰ��Ӧ�ó���Э���ص����Ϣ��ID,���ƺ�
																					//socket():����1����ַ�����ͣ�����2���׽��ֵ����ͣ�����3��Э�������
	int a = WSAGetLastError();				//����Ϊ0������᷵�ش�����
	if (INVALID_SOCKET == socketServer)		//����ʧ��
	{
		//��ȡ�����룬
		int a = WSAGetLastError();			//WSAGetLastError��ȡ����ĺ����Ĵ�����
		//���������
		WSACleanup();
		return 0;
	}

	//4.���ӷ�����
	//���ã����ӷ��������ѷ�������Ϣ�������socket�󶨵�һ��
	struct sockaddr_in serverMsg;
	serverMsg.sin_family = AF_INET;
	serverMsg.sin_port = htons(12345);		//Ҫ���ӷ��������˿ں�Ҫ����������һ��
	serverMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	//��������ip��ַ

	if (SOCKET_ERROR == connect(socketServer, (struct sockaddr*)&serverMsg, sizeof(serverMsg)))
	{
		int a = WSAGetLastError();
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}

	while (1)
	{
		//5.��������շ���Ϣ
		char buf[1500] = { 0 };
		//int res = recv(socketServer, buf, 1499, 0);

		//if (0 == res)         //�ͻ������ߣ����ͷſͻ���socket����˷���0��        
		//{
		//	printf("�����жϣ��ͻ�������\n");
		//}
		//else if (SOCKET_ERROR == res)    //ִ��ʧ�ܣ�����SOCKET_ERROR
		//{
		//	//������
		//	int a = WSAGetLastError();
		//	//����ʵ��������� ���ݴ�������Ϣ����Ӧ��������/�ȴ�/�������
		//}
		//else
		//{
		//	printf("%d  %s\n", res, buf);
		//}


		//5.��������շ���Ϣ   
		scanf("%s", buf);
		if (0 == strcmp(buf, "0"))
			break;
		if (SOCKET_ERROR == send(socketServer, buf, strlen(buf), 0))
		{
			//������
			int a = WSAGetLastError();
			//����ʵ��������� ���ݴ�������Ϣ����Ӧ��������/�ȴ�/�������
		}
	}
	
	//���������
	closesocket(socketServer);
	WSACleanup();
	system("pause");
	return 0;
}