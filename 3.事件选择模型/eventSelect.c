//ʱ�䣺2019��8��3��16:46:15
//�¼�ѡ��ģ��

/*
windows�����û���Ϊ�����ַ�ʽ��  1.��Ϣ���ƣ���������Ϣ����   ������̣����е��û��������������꣬�����̣�������ϵİ�ť�����ȵȣ����в��������ΰ�˳�򱻼�¼��װ��һ������
									������ʹ�ã�				 �ص㣺��1����Ϣ�����ɲ���ϵͳά�����������Ĳ�����Ȼ�����Ϣȡ���������ദ��  
																	   ��2�����Ⱥ�˳��
																  ������win32,MFC���ǻ�����Ϣ���У��첽ѡ��ģ��Ҳ�ǻ��������Ϣ���е�

								2.�¼����ƣ��������¼�����	  ������̣���1��������������Ϊ�û����ض�������һ���¼����¼��������Լ�����API��������Ҫ���ٴ�������
									���ֲ�ʹ�ã�						��2�����¼�Ͷ�ݸ�ϵͳ��ϵͳ�Ͱ����Ǽ����ţ����Բ������޴�����̫��ϵͳ���оͿ���
																	    ��3��������������ˣ������û��������ˣ���ô��Ӧ���¼��ͻᱻ�ó����źţ�Ҳ��������1��2�ˣ��ø������
																		��4������ֱ�ӻ�ȡ�����źŵ��¼���Ȼ����
																  �ص㣺��1�������¼����������Լ�����ģ�ϵͳֻ�ǰ������������źţ����������Լ��ƹܶ��� 
																		��2�������
																  �������¼�ѡ�����Ӧ�����

�¼�ѡ����߼� WSAEventSelect  �����߼���select���,������
��һ��������һ���¼����󣨱�����  WSACreateEvent
�ڶ�����Ϊÿһ���¼�����󶨸�socket�Լ�����accept,read,close��Ͷ�ݸ�ϵͳ   
		(1)Ͷ�ݸ�ϵͳ�����Ǿ���ȫ���ù��ˣ�ϵͳ�Լ���� �����Ǿ�ȥ�������ȥ��  
	   ��2��WSAEventSelect
���������鿴�¼��Ƿ����ź�   WSAWaitForMultipleEvents
���Ĳ������źŵĻ��ͷ��ദ��  WSAEnumNetworkEvents
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//�����¼������Լ�socket���ϵĽṹ��
struct fd_es_set
{
	unsigned short count;
	SOCKET sockall[WSA_MAXIMUM_WAIT_EVENTS];      //WSAWaitForMultipleEvents����ѯ�ʵ�һ���¼�����������WSA_MAXIMUM_WAIT_EVENTS����64�������¼��������Ҳ����64������Ҳѯ�ʲ��ˣ����Կ��Զ�����ѯ�ʻ���һ��ѯ��һ�������������ʽѯ��
	WSAEVENT eventall[WSA_MAXIMUM_WAIT_EVENTS];
};

struct fd_es_set esSet;

//�ص�����
BOOL WINAPI fun(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_CLOSE_EVENT:
		for (int i = 0; i < esSet.count; i++)
		{
			closesocket(esSet.sockall[i]);
			WSACloseEvent(esSet.eventall[i]);
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

	//struct fd_es_set esSet = {0};  �����ȫ�ֱ����Ա�ص�����ʹ��

	//�����¼�                                                      
	WSAEVENT eventServer =  WSACreateEvent();   //�ɹ�����һ���¼�      
												/*
													WSAEVENT->HANDLE->void*   �����1.ID
																					2.�ں˶��� ����ϵͳ��һ�����Ӧ�ó���exe������360��QQ�Լ�����д��EventSelect�������Ϊ�������ڲ���ϵͳ�����и�Ӧ�þ������ڵ��ú�����
																								 ������EventSelect�ڶ���a���൱�ھֲ�����,�ں˶����൱��ϵͳ��ϵͳ�Ŀռ�������ı������൱��ȫ�ֵģ�ֻ���ɲ���ϵͳ�����Լ�����
																					��1����ϵͳ���ں�����
																					��2���ɲ���ϵͳ����
																					��3�����ǲ��ܶ�λ�����ݣ�Ҳ�����޸ģ������ڱ�д��������ȥ�޸�   
																							1��void*��ͨ������ָ�룬��֪����ʲô���ͣ����ܽ�����ȡ���ݣ�ֻ����ǿת�ٽ����ã���֪�����Ͳ����޸�    
																							2�����ں˵ı������Թ���ı������Ӷ�ʹ����ϵͳ����ƽ����Ч�����У���������������
																					��4������ϵͳ��������������ϵͳ�����ͷ�        �������û�е����ͷţ���ô�����ܾ�һֱ�������ںˣ�����ں��ڴ�й©��ռ�õ��ǲ���ϵͳ�Ŀռ䣨ȫ�ֵģ����г������õģ����������������û���ͷŵģ�����ֻ���������� ������QQ����12345�˿ڣ�360�����벻��12345�˿ڣ�
																					��5���ں˶�������Щ��socket��thread,event,file�����ں˶���Kernel Objects��
												*/
	
	if (WSA_INVALID_EVENT == eventServer)   //ʧ�ܷ���WSA_INVALID_EVENT
	{
		printf("�����¼�ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}
	//WSAResetEvent(eventServer);   //��ָ���¼������ó����ź�      ����WSAEventSelect����ʹ�õ��¼�����״̬����ȷ�����ǽ��¼�����ľ�����ݸ�hEventObject�����е�WSAEnumNetworkEvents�������⽫�����¼�������ԭ�ӷ�ʽ�����׽����ϻFD�¼���״̬
	//WSASetEvent(eventServer);   //��ָ���¼������ó����ź�

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
												/*
												���ܣ����¼�����socket������룬��Ͷ�ݸ�����ϵͳ
												����1�����󶨵�socket   ���գ�ÿ��socket�����һ���¼�
												����2���¼�����   �߼����ǽ�����1�����2����һ��
												����3�������¼�:    ��1��FD_ACCEPT		�пͻ�������		�������socket��
																	��2��FD_READ		�пͻ��˷�����Ϣ	��ͻ���socket��  �ɶ�����Բ�����|��WSAEventSelect(,,FD_ACCEPT);  WSAEventSelect(,,FD_READ);���ǰ��������κ�һ�ΰ󶨻Ḳ�ǵ�ǰһ��
																	��3��FD_CLOSE		�ͻ���������		��ͻ���socket��  ����ǿ�����ߣ���������
																	��4��FS_WRITE		���Ը��ͻ��˷���Ϣ  ��ͻ���socket��  ����accept�������������źţ�����˵���ͻ������ӳɹ���������ʱsend
																	��5��FD_CONNECT		�ͻ����¼�ģ��ʹ�ã��ͻ���һ�����������������
																	��6��0				ȡ���¼����ӣ��¼���socket���ǰ󶨵�             WSAEventSelect(,,FD_ACCEPT|FD_READ)�� WSAEventSelect(,,0);  ȡ�����¼��ź�
																	��7��FD_OOB			��������   һ�㲻ʹ��
																	��8��FD_QOS			1���׽��ַ�������״̬�����仯��Ϣ֪ͨ  ���磺�����緢��ӵ��ʱ���û����أ�����Ӱ�����죬����ö������¼�һ����������ô��������������޵ģ�ÿ����Դ���������ݣ���ʱ�������ͻ��Ҫ�������ȣ����罵�����ص��ٶȣ��Ա�֤����Ӱ��������ʱ�����صķ��������ͷ����˱仯�����Ͷ��������¼��ͻ���յ��ź��ˡ�
																						2������ͨ��WSAloctl�õ�����������Ϣ      char strOut[2048] = {0};  DWORD nLen = 2048; WSAloctl(socketServer,SIO_QOS,0,0,strout,nLen,&nLen,NULL,NULL)
																	��9��FD_GROUP_QOS   ����     ��û�ж��丳ֵ�������壬��û����    ��Ҫ�����׽�����Qos���ĵ�֪ͨ
																	��10�������ص�I/Oģ����  FD_ROUTING_INTERFACE_CHANGE    1����Ҫ����ָ��Ŀ���·�ɽӿڸ���֪ͨ
																															2�����ݵ���Է�������������·�ı��ˣ������ڶ�̬·���Ż���ѡ��
																															3��Ҫͨ������WSAloctע��֮�󣬲ſ�ʹ��   �ڶ���������ΪSIO_ROUTING_INTERFACE_CHANGE
																							FD_ADDRESS_LIST_CHANGE			1����Ҫ�����׽��ֵ�ַ��ı��ص�ַ�б����֪ͨ       �����������˺ܶ�ͻ��ˣ��Ƿ������ͼ�¼�����еĿͻ��˵ĵ�ַ��Ϣ��Ҳ�����൱��һ���б�����һ��������һ�������Ǳ仯�ˣ����ܵõ���ص��ź���
																															2��Ҫͨ������WSAloctע��֮�󣬲ſ�ʹ��   �ڶ���������ΪSIO_ADDRESS_LIST_CHANGE
												����ֵ���ɹ�����0��ʧ�ܷ���SOCKET_ERROR
												*/
	

	//װ��ȥ
	esSet.eventall[esSet.count] = eventServer;
	esSet.sockall[esSet.count] = socketServer;
	esSet.count++;


	while (1)
	{
		//ѯ���¼�
		DWORD nRes = WSAWaitForMultipleEvents(esSet.count, esSet.eventall, FALSE, WSA_INFINITE, FALSE);
													/*
														���ã���ȡ�����źŵ��¼�
														����1���¼�����  �����¼��б����飩����     
															��1��WSA_MAXIMUM_WAIT_EVENTS  64�����ú�������1���64��
															��2�����Ա�� �����Ƚϸ��ӣ�����selectģ�ͣ�ֱ�Ӿ��ܱ����Ϊselect������Ǹ����飬Ȼ����������ˣ��Ƚ�ֱ�ӣ�ʱ��ѡ�����첽Ͷ�ţ���ϵͳ������������޸��ˣ�Ҫ��������
														����2���¼��б�
														����3���¼��ȴ���ʽ   
															��1��TRUE   ���е��¼��������źţ��ŷ���
															��2��FALSE   1��.�κ�һ���¼������źţ���������    
																			2��.����ֵ��ȥWSA_WAIT_EVENT_0��ʾ�¼��������������״̬���º�������   
																			3��.����ڵ����ڼ䷢������¼�������źţ��������ź��¼�������������������������ź��¼�������ֵ��С��
														����4����ʱ��� �Ժ���Ϊ��λ����select����5һ��������        
															��1��123���ȴ�123����  �ڼ����ź��������أ������ʱ����WSA_WAIT_TIMEOUT
															��2��0������¼������״̬���������أ�������û���ź�
															��3��WSA_INFINITE  �ȴ�ֱ���¼�����
														����5��
															��1��TRUE	�ص�I/Oʹ��
															��2��FALSE  �¼�ѡ��ģ����дFALSE
														����ֵ����1�������±������ֵ		
																	1������3ΪTRUE ���е��¼������ź�
																	2������3ΪFALSE ����ֵ��ȥWSA_WAIT_EVENT_0 == �������¼����±�
																��2��WSA_WAIT_IO_COMPLETION    ����5ΪTRUE,�Ż᷵�����ֵ
																��3��WSA_WAIT_TIMEOUT     ��ʱ�ˣ�continue����
													*/
		
		
		if (WSA_WAIT_FAILED == nRes)
		{
			//������
			printf("ѯ���¼�ʧ�ܣ�������Ϊ%d\n", WSAGetLastError());
			break;
		}
		//�������4д���峬ʱ�������ʱ�¼�Ҫ���ϳ�ʱ�ж�
		//if (WSA_WAIT_TIMEOUT == nRes)
		//{
		//	continue;
		//}
		DWORD nIndex = nRes - WSA_WAIT_EVENT_0;     

		//�о��¼�,�õ��±��Ӧ�ľ������
		WSANETWORKEVENTS  NetworkEvents;
		if (SOCKET_ERROR == WSAEnumNetworkEvents(esSet.sockall[nIndex], esSet.eventall[nIndex], &NetworkEvents))
		{
			printf("�õ��±��Ӧ�ľ������ʧ�ܣ������룺%d\n", WSAGetLastError());
			break;
		}
											/*
												���ã���ȡ�¼����ͣ������¼��ϵ��ź�����      accept,recv,send�ȵ�
												����1����Ӧ��socket
												����2����Ӧ���¼�
												����3����1���������¼�����������װ��
														��2����һ���ṹ��ָ��   struct_WSANETWORKEVENTS{long lNetworkEvents;  int iErrorCode[FD_MAX_EVENTS];}   
																						��Ա1���������    һ���źſ��ܰ���������Ϣ���԰�λ�����ʽ����
																						��Ա2������������    1��FD_ACCEPT�¼���������FD_ACCEPT_BIT�±���    
																												2��û�д��󣬶�Ӧ�ľ���0
												����ֵ���ɹ�  0  �� ʧ�� SOCKET_ERROR
											*/
		


		/*
		�¼����ദ���߼�                �����ϵ㣬��Ȼ����ס�ˣ������¼�Ͷ�ݸ���ϵͳ���ڼ�ͻ��˷��͸���������ȫ����Ϣ�������¼�����������źŵģ����첽�ģ�  ����ͻ�������send���Σ���������һ��recvȫ�����գ���Ϊϵͳ�ײ�Э��ջ�ճ�����
		1.��if�߼����ã����Ͻ�����Ч�ʵ͵�һ��    
			����while��Ӷϵ㣬�򿪿ͻ��˷�һ����Ϣ��ʵ��ִ��������socket, ��һ��lNetworkEvents = 8, ������socketִ��accept�������ڶ����ǿͻ���socket��lNetworkEvents = 3, ������ִ��write��read
		2.if - else�Ƕ�ѡһ����Сbug, ��bug�������� 
			����while��Ӷϵ㣬�򿪿ͻ��˷�һ����Ϣ��ʵ��ִ��������socket, ��һ���Ƿ�����socket��ִ��accept�������ڶ����ǿͻ���socket��������Ϊif_elseֻѡ������һ���źţ����Է�����ִֻ����write
		3.switch�Ļ�����bug����Ҫ�ܶ�Ľ�  
			����while��Ӷϵ㣬�򿪿ͻ��˷�һ����Ϣ��ʵ��ִ��������socket, ��һ���Ƿ�����socketִ��accept�������ڶ��ι����ǿͻ���socket������write��read�ź���һ�������, lNetworkEvents = 3��write��2��read��1��caseƥ�䲻��3��������һ��ѭ��
		*/
		if (NetworkEvents.lNetworkEvents & FD_ACCEPT)     //if���Ϊ��˵�������������ź���Σ�FD_ACCEPT����ź������źŹ�����
		{
			if (0 == NetworkEvents.iErrorCode[FD_ACCEPT_BIT])
			{
				//��������
				SOCKET socketClient = accept(esSet.sockall[nIndex], NULL, NULL);
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

				//װ���ṹ��
				esSet.sockall[esSet.count] = socketClient;
				esSet.eventall[esSet.count] = wsaClientEvent;
				esSet.count++;

				printf("accept event\n");
			}
			else
			{
				continue;
			}
		}

		if (NetworkEvents.lNetworkEvents & FD_WRITE)       //�¼�ѡ��ģ�������Ż���ֻ��aceept������Ų���һ�Σ��ɹ����Ӻ���ʱ������send������Ҫһֱ�����¼�����selectģ��send��һֱ��Ч�ģ�����Ӧ��
		{
			if (0 == NetworkEvents.iErrorCode[FD_WRITE_BIT])
			{
				//�ͻ������ӷ�������ĵ�һ�ν��г�ʼ����������ʱ������send����һ����Ҫ����if�ж�
				if (SOCKET_ERROR == send(esSet.sockall[nIndex], "connect success", strlen("connect success"), 0))    //strlen��sizeof�õ����ֽ�����һ����'\0'
				{
					printf("sendʧ�ܣ�������Ϊ��%d\n", WSAGetLastError());      //send����ִ�г��ִ���
					continue;
				}
				printf("write event\n");
			}
			else
			{
				printf("socket error�׽��ִ��󣬴�����Ϊ��%d\n", NetworkEvents.iErrorCode[FD_WRITE_BIT]);   //�׽��ֳ��ִ���
			}
		}

		if (NetworkEvents.lNetworkEvents & FD_READ)
		{
			if (0 == NetworkEvents.iErrorCode[FD_READ_BIT])
			{
				char strRecv[1500] = {0};
				if (SOCKET_ERROR == recv(esSet.sockall[nIndex], strRecv, 1499, 0))    //�ͻ����˳���FD_CLOSE��֧����
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
			//if (0 == NetworkEvents.iErrorCode[FD_CLOSE_BIT])
			//{
			//}
			//else
			//{
			//	printf("client force out�ͻ���ǿ���˳��������룺%d\n", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
			//}
			//���ַ���ֵ��WSAENETDOWN    WSAECONNRESET   ����������Ƿ���WSAECONNABORTED�����������˳���ǿ���˳�

			//���ܴ�������0��1��Ҫ����
			//��ӡ
			printf("client close\n");
			printf("client force out�ͻ���ǿ���˳��������룺%d\n", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
			//�������ߵĿͻ��� �׽��� �¼�
			//�׽���
			closesocket(esSet.sockall[nIndex]);
			esSet.sockall[nIndex] = esSet.sockall[esSet.count - 1];     //�����¼�ѡ��ʱ����ģ�����ֱ�Ӱѵ�ǰҪ�رյ�socket�����һ��socket������Ȼ��������1
			//�¼�
			WSACloseEvent(esSet.eventall[nIndex]);
			esSet.eventall[nIndex] = esSet.eventall[esSet.count - 1];
			//������1
			esSet.count--;
		}
	}

	//�ͷ��¼�������ͷ�socket
	for (int i = 0; i < esSet.count; i++)
	{
		closesocket(esSet.sockall[i]);
		WSACloseEvent(esSet.eventall[i]);
	}

	//���������
	WSACleanup();
	system("pause");
	return 0;
}


/*
               ->�¼�����  �¼�ѡ��ģ��  ->�ص�I/Oģ��
C/S -> select 
               ->��Ϣ����  �첽ѡ��ģ��  ->��ɶ˿�ģ��
*/
