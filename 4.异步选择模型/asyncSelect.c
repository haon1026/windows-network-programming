//ʱ�䣺2019��8��6��21:38:03
//�첽ѡ��ģ��

/*
	���ģ���Ϣ����  ����ϵͳΪÿ�����ڴ���һ����Ϣ���в���ά������������Ҫʹ����Ϣ���У��Ǿ�Ҫ����һ������
	��һ���������ǵ�socket��������һ����Ϣ�ϣ���Ͷ�ݸ�����ϵͳ WSAAsyncSelect
	�ڶ�����ȡ��Ϣ���ദ��  ����в����ˣ��ͻ�õ�ָ������Ϣ
	��ģ��ֻ������windows����Ҫѧ���ִ���˼��
*/

//#include <windows.h>
#include <TCHAR.H>     //_Tͷ�ļ�
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define UM_ASYNCSELECTMSG WM_USER+1      //WM_USER������ϵͳ����ʹ�õ�����WM_USER֮������Ǹ�����Աʹ�õ�

LRESULT CALLBACK WinBackProc(HWND hWnd, UINT msgID, WPARAM wparam, LPARAM lparam);

#define MAX_SOCK_COUNT 1024 
SOCKET g_sockall[MAX_SOCK_COUNT];  //������Ϣ����ʱ������socket
int g_count = 0;     //������Ч����

/*
	����һ����ǰ������ϵͳ�ϵ�Ӧ�ó����ʵ�����������˵ID����
	��������ǰһ��ʵ�������Ӧ�ó���ɴ򿪶�������ǰһ��ʵ����������������������Ѿ���Ч�ˣ�������ȡ��
	�����������������в�������main������һ��
	�����ģ����ڵ�Ĭ����ʾ��ʽ
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE HPreInstance, LPSTR lpCmdLine, int nShowCmd)
{
	//��һ�����������ڽṹ��
	WNDCLASSEX wc;					//WNDCLASS��WNDCLASSEX���ֽṹ�� ex��ʾ����ģ��༸������
	wc.cbClsExtra = 0;				//����ռ�
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = 0;				//����ռ�
	wc.hbrBackground = NULL;		//NULL����Ĭ�ϰ�ɫ
	wc.hCursor = NULL;				//������̬,
	wc.hIcon = NULL;				//���Ͻǵ�ͼ��
	wc.hIconSm = NULL;				//��С����ͼ�꣬����һ��
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WinBackProc;
	wc.lpszClassName = _T("mywindow");		//��������     //�ַ������ֽ���취������������(L, _T, TEXT("mywindow"))����һ���޸��ַ��������Ƽ����Ӵ���Ƕ��޸�
	wc.lpszMenuName = NULL;					//�˵���
	wc.style = CS_HREDRAW | CS_VREDRAW;     //���

	//�ڶ�����ע��ṹ��     ���������͵Ĵ���Ͷ�ݸ�ϵͳ
	RegisterClassEx(&wc);

	//����������������
	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("mywindow"), _T("�ҵĴ���"), WS_OVERLAPPEDWINDOW, 200, 200, 600, 400, NULL, NULL, hInstance, NULL);
	if (NULL == hWnd)
	{
		return 0;
	}

	//���Ĳ�����ʾ����
	ShowWindow(hWnd, nShowCmd);

	//���´���    ���»��ƴ���   �Ǳ�Ҫ��
	UpdateWindow(hWnd);

	//socket��ʼ��
	//�������
	WSADATA wsadata;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wsadata))
	{
		printf("������ʧ��");
		return 0;
	}

	//����汾
	if (2 != HIBYTE(wsadata.wVersion) || 2 != LOBYTE(wsadata.wVersion))
	{
		printf("�汾����\n");
		WSACleanup();
		return 0;
	}

	//����������socket
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketServer)
	{
		printf("����������socketʧ�ܣ������룺%d\n", WSAGetLastError());
		WSACleanup();
		return 0;
	}

	//��IP�Ͷ˿�
	struct sockaddr_in socketMsg;
	socketMsg.sin_family = AF_INET;
	socketMsg.sin_port = htons(12345);
	socketMsg.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == bind(socketServer, (const struct sockaddr *)&socketMsg, sizeof(socketMsg)))
	{
		printf("��IP�Ͷ˿�ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//�����ͻ�����������
	if (SOCKET_ERROR == listen(socketServer, SOMAXCONN))
	{
		printf("����ʧ�ܣ�������:%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//��Ϣ��������Ϣ��socket����Ͷ�ݳ�ȥ
	//���ã���socket����Ϣ����һ�𣬲�Ͷ�ݸ�����ϵͳ
	//����һ��socket
	//�����������ھ�����󶨵��ĸ�������   ���ʾ��Ǵ��ڵ�ID,���
	//����������Ϣ��ţ��Զ�����Ϣ    ���ʾ���һ����
	//�����ģ���Ϣ���ͣ����¼�ѡ��һ��
	if (SOCKET_ERROR == WSAAsyncSelect(socketServer, hWnd, UM_ASYNCSELECTMSG, FD_ACCEPT))
	{
		printf("��Ϣ����Ͷ��ʧ�ܣ������룺%d\n", WSAGetLastError());
		closesocket(socketServer);
		WSACleanup();
		return 0;
	}

	//װ������socket
	g_sockall[g_count] = socketServer;
	g_count++;

	//���岽����Ϣѭ��:socket����ϢͶ�ݸ�ϵͳ�󣬵�socket�����¼���ͻ���������,��Щ��������Ϣ����ʽ������Ϣ���У�ѭ������Ϣ������������������Ϣ��Ȼ��ַ���Ϣ
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))    //ֻ�йرմ��ڲŻ����ѭ��      //һ��ֻȡһ��
	{
		TranslateMessage(&msg);    //������Ϣ
		DispatchMessage(&msg);     //�ַ���Ϣ
	}

	//�ͷ�socket
	for (int i = 0; i < g_count; i++)
	{
		closesocket(g_sockall[i]);
	}
	//�ر������
	WSACleanup();

	return 0;
}

int x = 0;

//���������ص�������һ��ֻ����һ��
LRESULT CALLBACK WinBackProc(HWND hWnd, UINT msgID, WPARAM wparam, LPARAM lparam)     //�ַ����������    ����һ�����ھ��������������ϢID����������socket�������ģ��������Ϣ
{
	HDC hdc = GetDC(hWnd);   //��ȡ��ǰ���ڵ��豸�������
	switch (msgID)			 //������Ϣ������Ӧ����
	{
	case UM_ASYNCSELECTMSG:
		{   //case�ڲ�����ֱ�Ӷ���������ӻ����ſɽ��
			//������MessageBox(NULL, L"���ź���", L"��ʾ", MB_OK);   //����һ����дNULL�������Ĵ����Ƕ������ڣ���дhWnd�������Ĵ�����hWnd���ڵ��Ӵ���;���������������ڵ����ݣ����������������ڵı��⣻�����ģ����������а�ť������
			//��ȡsocket
			SOCKET sock = (SOCKET)wparam;   //wparam�����socket
			//��ȡ��Ϣ
			if (0 != HIWORD(lparam))		//lparam��λ����ǲ����Ĵ����룬����ͻ��˷�����Ϣ�����͹����г����ˣ���õ������룬��Ϊ0�ʹ����д�����0���ܴ�����Ϣ
			{
				if (WSAECONNABORTED == HIWORD(lparam))    //�رտͻ��˻�ִ�е���
				{
					TextOut(hdc, 0, x, _T("close"), strlen("close"));
					x += 15;
					//�رո�socket�ϵ���Ϣ
					WSAAsyncSelect(sock, hWnd, 0, 0);
					//�ر�socket
					closesocket(sock);
					//��¼������ɾ����socket
					for (int i = 0; i < g_count; i++)
					{
						if (sock == g_sockall[i])
						{
							g_sockall[i] = g_sockall[g_count - 1];
							g_count--;
							break;
						}
					}
				}
				break;
			}
			//������Ϣ
			switch (LOWORD(lparam))    //lparam��λ����Ǿ������Ϣ����
				{
				case FD_ACCEPT:
				{
					//���ڴ�ӡ��Ϣ
					TextOut(hdc, 0, x, _T("accept"), strlen("accept"));    //����һ���������һ���֣��ǿͻ������豸�������; ��������������; �����ģ����ڴ�ӡ����Ϣ; �����壺��Ϣ�ֽڴ�С,����'\0'���Ըպ����
					x += 15;
					//�����ͻ���socket
					SOCKET socketClient = accept(sock, NULL, NULL);
					if (INVALID_SOCKET == socketClient)
					{
						printf("�����ͻ���socket���������룺%d\n", WSAGetLastError());
						break;
					}
					//���ͻ���socketͶ�ݸ���Ϣ����
					if (SOCKET_ERROR == WSAAsyncSelect(socketClient, hWnd, UM_ASYNCSELECTMSG, FD_READ|FD_WRITE|FD_CLOSE))
					{
						printf("��Ϣ����Ͷ��ʧ�ܣ������룺%d\n", WSAGetLastError());
						closesocket(socketClient);
						break;
					}
					//��¼�ͻ���socket��Ϊ�˺����ͷŵ�
					g_sockall[g_count] = socketClient;
					g_count++;
				}
					break;

				case FD_READ:
				{
					//read
					TextOut(hdc, 0, x, _T("read"), strlen("read"));
					char str[1024] = { 0 };
					if (SOCKET_ERROR == recv(sock, str, 1023, 0))
					{
						printf("��ȡ��Ϣʧ��,�����룺%d\n", WSAGetLastError());
						break;
					}
					TextOut(hdc, 50, x, str, strlen(str));     //�˴�����ַ���Ҫ�ĳɶ��ֽ���ʽ����Ϊ�Ƚϸ���ֱ�����ַ����޸ĳɶ��ֽ���ʽ��L����ʹ���ˣ����ĳ�ͨ�õ�_L("")
					x += 15;
				}
					break;

				case FD_WRITE:
					//send
					TextOut(hdc, 0, x, _T("write"), strlen("write"));
					x += 15;
					break;

				case FD_CLOSE:
					TextOut(hdc, 0, x, _T("close"), strlen("close"));
					x += 15;
					//�رո�socket�ϵ���Ϣ
					WSAAsyncSelect(sock, hWnd, 0, 0);
					//�ر�socket
					closesocket(sock);
					//��¼������ɾ����socket
					for (int i = 0; i < g_count; i++)
					{
						if (sock == g_sockall[i])
						{
							g_sockall[i] = g_sockall[g_count - 1];
							g_count--;
							break;
						}
					}
				}
		}
		break;

	case WM_CREATE:    //ֻ�ڴ��ڴ�����ʱ��ִ��һ�Σ���ʼ���������ʼ��Ҳ�ɷ��ڴ˴�
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	ReleaseDC(hWnd, hdc);  //�ͷ�hdc
	return DefWindowProc(hWnd, msgID, wparam, lparam);
}



/*
�Ż���ÿ������ά��һ���ģ�Ȼ�󴴽�����̣߳�ÿ���߳�һ�����ڣ�ÿ������Ͷ��һ�������Ŀͻ���
���⣺��һ�δ�������У��ͻ��˲������send���������������ν�����Ϣ����һ�ν�����Ϣ������������Ϣ   ���ԣ�����Ϣѭ��ǰwhile�Ӷϵ㣬�ͻ��˷��Ͷ����Ϣ��ȡ���ϵ������������ֻ��һ��read,���ǰ�������Ϣ���ݶ���ӡ��
*/
