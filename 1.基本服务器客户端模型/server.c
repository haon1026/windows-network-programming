//ʱ�䣺2019��7��27��16:55:47
//server

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>					//�򿪸��ĵ��ɿ����汾��Ϣ
#pragma comment(lib, "ws2_32.lib")		//windows socket�ڶ���32λ��lib��Դ�ļ�����õĶ������ļ�
//#include <WinSock.h>    ��Ӧ�� wsock32.lib 


int main(void)
{
	//1.�������
	WORD wdVersion = MAKEWORD(2, 2);				//�����汾�ؼ��֣����ֽڷŸ��汾�ţ����ֽڷ����汾��
	//int a = *((char *)&wdVersion);				//���2��ȡwdVersion�ĵ͵�ַλ
	//int b = *((char *)&wdVersion + 1);			//���1��ȡwdVersion�ĸߵ�ַλ

	WSADATA wdSockMsg;									//��һ��д������������ֱ��ȡ��ַ
	//LPWSADATA lpw = malloc(sizeof(WSADATA));			//�ڶ���д��������ָ�����ָ��̬����WSADATA��Ҫfree,LPWSADATA������WSADATA* 

	int nRes = WSAStartup(wdVersion, &wdSockMsg);			//�����������������һ������������İ汾�ؼ��֣��ڶ���������ָ��WSADATA�ṹ��ĵ�ַ��������������Ϣ����������ṹ����
														/*
															MAKEWORD(2, 2),������İ汾�����ڣ�
															��1������1.3��2.3 �����汾��û�и��汾  �õ������汾����󸱰汾1.1��2.2��ʹ��
															��2������3.1��3.3 �������汾�� ʹ��ϵͳ���ṩ�����İ汾2.2
															��3������0.0 0.1 0.1 ���汾��0 ������ʧ�ܣ���֧��������׽��ְ汾

															typedef struct WSAData {
															WORD                    wVersion;		����Ҫʹ�õİ汾
															WORD                    wHighVersion;	ϵͳ�ṩ�����ǵ���߰汾
															#ifdef _WIN64
															unsigned short          iMaxSockets;	���ؿ��õ�socket��������2�汾֮���û����
															unsigned short          iMaxUdpDg;		UDP���ݱ���Ϣ�Ĵ�С��2�汾֮���û����
															char FAR *              lpVendorInfo;	��Ӧ���ض�����Ϣ��2�汾֮���û����
															char                    szDescription[WSADESCRIPTION_LEN+1];   ��ǰ�汾���������Ϣ
															char                    szSystemStatus[WSASYS_STATUS_LEN+1];   ��ǰ״̬
															#else
															......
															} WSADATA, FAR * LPWSADATA;
														*/
	//if (WSAStarup(wdVerison, &wdSockMsg) != 0)
	if (0 != nRes)					//��������򿪳ɹ�������ֵ��0
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
		if (SOCKET_ERROR == WSACleanup())			//��������0
		{
			int a = WSAGetLastError();				//����ʧ�ܿ��øú�����ȡ�����룬���ô�����ɲ��Ҵ�����Ϣ
		}
		return 0;
	}

	//3.����SOCKET
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);		//SOCKET��һ���������ͣ�һ��������������Ψһ�ģ���ʶ�ŵ�ǰ��Ӧ�ó���Э���ص����Ϣ��ID,���ƺ�   SOCKET����=IP��ַ���˿�
																			//ÿһ���ͻ�����һ��SOCKET����������һ��SOCKET��ͨ��ʱ������Ҫ���SOCKET����������˭ͨ�žʹ���˭��SOCKET
																			//socket():����1����ַ�����ͣ�����2���׽��ֵ����ͣ�����3��Э�������
	int a = WSAGetLastError();					//����Ϊ0������᷵�ش�����
	if (INVALID_SOCKET == socketServer)			//����ʧ��
	{
		//��ȡ�����룬
		int a = WSAGetLastError();				//WSAGetLastError��ȡ����ĺ����Ĵ�����
		//���������
		WSACleanup();
		return 0;
	}

	//4.�󶨵�ַ��˿�
	struct sockaddr_in si;
	si.sin_family = AF_INET;								//������socketʱ�ĵ�ַ���Ͷ�Ӧ ipv4:AF_INET ipv6:AF_INET6
	si.sin_port = htons(12345);								//���������ת����unsigned short  �����ֽ���ת�������ֽ���  (���۷�Χ0-65535)  (0-1023Ϊϵͳ����ռ�ö˿ں� )   //netstat -ano�鿴�˿�ʹ�����  //netstat -aon|findstr "12345"�鿴�˿��Ƿ�ʹ��
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");		//127.0.0.1���ػػ���ַ�����͵�ַ    //ipconfig
	/*si.sin_addr.S_un.S_un_b.s_b1 = 192;
	si.sin_addr.S_un.S_un_b.s_b2 = 168;
	si.sin_addr.S_un.S_un_b.s_b3 = 0;
	si.sin_addr.S_un.S_un_b.s_b4 = 1;*/

	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr*)&si, sizeof(si)))		//����2������ָ��const struct sockaddr�ṹ���ָ��,sockaddr����д���ַ�˿� ->д��sockaddr_in�ṹ����ǿת���ڴ�ṹ��һ����
	{																						//sockaddr :  u_short sa_family; CHAR sa_data[14];
		//������																			//sockaddr_in :short sin_family; USHORT sin_port; IN_ADDR sin_addr; CHAR sin_zero[8];
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);				//���ͷ�socket�ٹر�����⣬����ᱨ��
		//���������
		WSACleanup();
		return 0;
	}

	//5.��ʼ������ �����ͻ�����������     ���׽����������������������ӵ�״̬
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))			//����2�ǹ������Ӷ��е���󳤶ȣ�������100���û��������󣬵���ϵͳһ��ֻ�ܴ���20����ʣ��80�����ܲ����˼ң�����ϵͳ�ʹ��������м�¼��Щ��ʱ���ܴ���һ�ᴦ��������������Ⱥ�˳����   
	{																//SOMAXCONN����ϵͳ�Զ�ѡ������ʵĸ���
		//������
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}

	//6.�����ͻ���socket      listen�����ͻ����������ӣ����ͻ��˵���Ϣ�󶨵�һ��socket�ϣ�Ҳ���Ǹ��ͻ��˴���һ��socket,ͨ������ֵ���ظ����ǿͻ��˵�socket
	struct sockaddr_in clientMsg;				//������ǲ�����д��ϵͳ���������д��Ҳ����ַ����
	int len = sizeof(clientMsg);
	SOCKET socketClient = accept(socketServer, (struct sockaddr *)&clientMsg, &len);    //һ��ֻ�ܴ���һ�����м����ͻ������ӣ���Ҫ���ü��Σ� ����������û�пͻ������ӻ�һֱ�����⣬����
																						/*
																							����2��ϵͳ�����Ǽ����ſͻ��˵Ķ�̬���϶����¼�ͻ��˵���Ϣ��Ҳ����IP��ַ�Ͷ˿ںţ���ͨ������ṹ���¼
																							����3������2�Ĵ�С��
																							����2��3Ҳ���ó�NULL��accept������ֱ�ӵõ��ͻ��˵ĵ�ַ���˿ںţ�������һ���ͻ�����Ϣ�ͻḲ�ǵ���ǰ����Ϣ

																							�����ʱ�����ܣ�Ҳ���ڽ���������getpeername������ÿͻ�����Ϣ
																							SOCKET socketClient = accept(socketServer, NULL, NULL);
																							getpeername(socketClient, (struct sockaddr *)&clientMsg, &len);
																							�õ����ط�������Ϣ��getsockname()
																							getsockname(socketServer, (struct sockaddr *)&clientMsg, &len);   //��ʹ�˴���Ŀͻ�����Ϣ����ȡ��Ҳ�Ǳ��ط���������Ϣ
																						*/														
	if (INVALID_SOCKET == socketClient)                                                
	{                                                                                    
		printf("�ͻ�������ʧ��");                                                       
		//������
		int a = WSAGetLastError();
		//�ͷ�
		closesocket(socketServer);
		//���������
		WSACleanup();
		return 0;
	}
	printf("�ͻ������ӳɹ�\n");

	if (SOCKET_ERROR == send(socketClient, "���Ƿ����������յ��������Ϣ", sizeof("���Ƿ����������յ��������Ϣ"), 0))
	{
		//������
		int a = WSAGetLastError();
		//����ʵ��������� ���ݴ�������Ϣ����Ӧ��������/�ȴ�/�������
	}

	while(1)
	{
		//7.����˽��տͻ�����Ϣ:�õ�ָ���ͻ��ˣ�����1����������Ϣ
		//ԭ�������Ǹ��ƣ�1.���ݵĽ��ܶ�����Э�鱾�����ģ�Ҳ����socket�ĵײ����ģ�ϵͳ��һ�λ��������洢�Ž��ܵ������� 2.������ߵ���recv�����ã�����ͨ��socket�ҵ�������������������ݸ��ƽ�����2�����Ʋ���3��
		char buf[1500] = { 0 };
		int res = recv(socketClient, buf, 1499, 0);          
																	/*
																		����1���ͻ��˵�socket��ÿ���ͻ��˶�ӦΨһ��socket
																		����2���ͻ�����Ϣ�Ĵ洢�ռ䣬Ҳ�����ַ�����      һ��1500�ֽڣ����紫������Ԫ��1500�ֽڣ�Ҳ���ǿͻ��˷����������ݣ�һ��������1500�ֽڣ����Э��涨���ܽ������ֵ�����Կͻ���һ�������1500�ֽڣ����������1500��һ�ξ͹���
																		����3����Ҫ��ȡ���ֽ�����һ���ǲ���2���ֽ���-1����\0�ַ�����β�������� %s��ֱ�����
																		����4�����ݵĶ�ȡ��ʽ��һ����0		�����߼���˵�������߼���0����1.���Ǵ�ϵͳ�����������ݶ���buf��ϵͳ�������ı����ľ�Ӧ�ñ�ɾ�����ˣ���ȻҲ���˷ѿռ䣬�Ͼ�ͨ��ʱ�䳤�Ļ����Ǿͱ�ը�ˡ�
																																		 2.���Ǵ�ϵͳ�����������ݶ���buf��������Ҫ������Ӧ�����ݣ��������ǿɿصģ�ϵͳ���������������������
																																		 3.��������ɾ�Ļ����кܶ�ô� 1.ϵͳ���������������ݣ������ǵ�buf�࣬��ô���Ƕ������ģ�ϵͳɾ�����Ӷ����ǾͿ������ΰ����е����ݶ��꣬�����ɾ��ÿ�ζ��Ǵ�ͷ������ѭ�������ÿ�ζ���ĳ���ַ�������硰abcd��ֻ�������ĸ�   2.���Լ����յ��˶����ֽڣ� ��Ϊ����ֵ���Ǳ��ζ�����������
																											����4��MSG_PEEK   1.���Ӵ�������ݣ����ݽ����Ƶ��������У����Ӳ�������������ɾ��  2.������ʹ�ã������ݲ���/�޷�����
																											����4��MSG_OOB   �������� ���壺���Ǵ���һ�����ݣ������һ��������������ݣ��൱��С��BB   ʵ�ʣ�������ʹ�� 1.TPCЭ��淶��RFC793����OOB��ԭʼ������������Ҫ�󡱹淶ȡ����RFC1122��,����������������RFC793 OOBʵ�� 2.��Ȼ�������ݣ���ȫ����send���Σ���һ��recv���Σ���Ȼ��Ҫ���⴦���������
																											����4��MSG_WAITALL ֱ��ϵͳ�������ֽ����������3��������ֽ������ſ�ʼ��ȡ
																		����ֵ�Ƕ��������ֽ�����С   ��û�ˣ���recv�������ţ����ſͻ��˷������ݣ���������ͬ��
																	*/
		if (0 == res)							//�ͻ������ߣ����ͷſͻ���socket����˷���0��        
		{
			printf("�����жϣ��ͻ�������\n");
		}
		else if (SOCKET_ERROR == res)			//ִ��ʧ�ܣ�����SOCKET_ERROR
		{
			//������
			int a = WSAGetLastError();
			//����ʵ��������� ���ݴ�������Ϣ����Ӧ��������/�ȴ�/�������
		}
		else
		{
			printf("%d  %s\n", res, buf);
		}

		//7.����˸��ͻ��˷�������
		//���ã���Ŀ�귢������   ���ʣ�send���������ǵ����ݸ���ճ����ϵͳ��Э�鷢�ͻ�������������Ż����ͳ�ȥ������䵥Ԫ��1500�ֽ�
		scanf("%s", buf);
		int b = send(socketClient, buf, strlen(buf), 0);			
																/*
																	����1��Ŀ���socket��ÿ���ͻ��˶�ӦΨһ��socket
																	����2����Ŀ�귢���ֽڴ�
																		��1�������Ҫ����1500�ֽ�:
																			1��.����ʱ��Э��Ҫ���а�װ������Э����Ϣ��Ҳ��Э��ͷ�����߽а�ͷ��
																			2��.�����С��ͬ��Э�鲻һ������·��14�ֽڣ�ipͷ20�ֽڣ�tcpͷ20�ֽڣ����ݽ�β��Ҫ��״̬ȷ�ϣ�������Ҳ��ʮ���ֽڣ�����ʵ�����ǵ�����λ����д1500����Ҫ���������Ǿ�1024�������1400�Ͳ����
																		��2������1500���ֽ�ϵͳ���Ƭ����
																			1��.����2000���ֽ�,ϵͳ��ֳ��������������ͷ100�ֽ� ��һ��1400+��ͷ ==1500�� �ڶ���600+��ͷ==700�������η��ͳ�ȥ
																			2��.�����1.ϵͳҪ�����Ƿְ��ٴ�����ٷ��ͣ��ͻ��˽��յ��˻��ò����������ݣ��Ӷ�������ϵͳ�ù���������Ч��   2.�е�Э�飬�Ͱѷ�Ƭ��ö���ֱ�Ӷ���
																	����3. �ֽڸ���
																	����4. ���͹���һ��д0     1.MSG_OOBͬrecv,   2.MSG_DONTROUTE  ָ�����ݲ�Ӧ��·�����ƣ�windows�׽��ַ����ṩ������Ժ��Դ˱�־
																	����ֵ���ɹ�����д����ֽ�����ִ��ʧ�ܷ���SOCKET_ERROR
																*/
																		
		if (SOCKET_ERROR == b)
		{
			//������
			int a = WSAGetLastError();
			//����ʵ��������� ���ݴ�������Ϣ����Ӧ��������/�ȴ�/�������
		}

	}
	closesocket(socketClient);
	closesocket(socketServer); //WSAGetLastError��ȡ����ĺ����Ĵ�����
	//���������
	WSACleanup();
	system("pause");
	return 0;
}


/*
int WSAAPI WSAStartup(_In_ WORD wVersionRequested, _Out_ LPWSADATA lpWSAData)
SOCKET WSAAPI socket(_In_ int af, _In_ int type, _In_ int protocol)
int WSAAPI bind(_In_ SOCKET s, _In_reads_bytes_(namelen) const struct sockaddr FAR * name, _In_ int namelen)
int WSAAPI listen(_In_ SOCKET s, _In_ int backlog)
SOCKET WSAAPI accept(_In_ SOCKET s, _Out_writes_bytes_opt_(*addrlen) struct sockaddr FAR * addr, _Inout_opt_ int FAR * addrlen)
int WSAAPI recv(_In_ SOCKET s, _Out_writes_bytes_to_(len, return) __out_data_source(NETWORK) char FAR * buf, _In_ int len, _In_ int flags)
int WSAAPI send(_In_ SOCKET s, _In_reads_bytes_(len) const char FAR * buf, _In_ int len, _In_ int flags)

WSAAPI ����Լ�� ���Ժ��Ը�ϵͳ���ģ��������޹�
����Լ�������ˣ�1.�������ֵı��뷽ʽ 2.��������ջ˳�� 3.�����ĵ���ʱ��
*/

/*
��C/C++д��������ʱ�������������ֽڵ�����˳�������˳������⡣���ǾͿ����õ�htons(), ntohl(), ntohs()��htons()��4��������
�����ֽ�˳���뱾���ֽ�˳��֮���ת��������

htonl()--"Host to Network Long"
ntohl()--"Network to Host Long"
htons()--"Host to Network Short"
ntohs()--"Network to Host Short"

֮������Ҫ��Щ��������Ϊ��������ݱ�ʾ���������ֽ�˳��NBO��HBO

�����ֽ�˳��NBO(Network Byte Order): ���Ӹߵ��͵�˳��洢����������ʹ��ͳһ�������ֽ�˳�򣬿��Ա�����������⡣

�����ֽ�˳��(HBO��Host Byte Order): ��ͬ�Ļ���HBO����ͬ����CPU����йأ����ݵ�˳������cpu������,�������ϵͳ�޹ء�
�� Intel x86�ṹ��, short����0x1234��ʾΪ34 12, int����0x12345678��ʾΪ78 56 34 12
�� IBM power PC�ṹ��, short����0x1234��ʾΪ12 34, int����0x12345678��ʾΪ12 34 56 78
*/