// Drawing_Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "Drawing_Client.h"
#include "StreamQueue.h"

// ���� ����:
HINSTANCE hInst;								// ���� �ν��Ͻ��Դϴ�.
SOCKET sock;
CAyaStreamSQ SendQ;
CAyaStreamSQ RecvQ;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL ConnectProc();
void CloseProc();
void ReadProc();
void WriteProc();
void CompleteRecvPacket();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: ���⿡ �ڵ带 �Է��մϴ�.
	MSG msg;
	WNDCLASSEX wcex;
	HWND hWnd;
	int retval;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAWING_CLIENT));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_DRAWING_CLIENT);
	wcex.lpszClassName = L"DrawingClient";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	RegisterClassEx(&wcex);

	hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	//IP Dialog �����
	hWnd = CreateWindow(L"DrawingClient", L"Drawing", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	//���� ����
	sock = socket(AF_INET, SOCK_STREAM, 0);

	WSAAsyncSelect(sock, hWnd, UM_SOCKET, FD_CONNECT | FD_CLOSE | FD_READ | FD_WRITE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// �⺻ �޽��� �����Դϴ�.
	while (GetMessage(&msg, NULL, 0, 0))
	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	}

	return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case UM_SOCKET :
		if (WSAGETASYNCERROR(lParam))
			//����ó��

		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT :
			if (!ConnectProc())
				return;
			break;

		case FD_CLOSE :
			CloseProc();
			break;

		case FD_READ :
			ReadProc();
			break;

		case FD_WRITE :
			WriteProc();
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: ���⿡ �׸��� �ڵ带 �߰��մϴ�.
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL ConnectProc()
{
	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	InetPton(AF_INET, L"127.0.0.1", &sockaddr.sin_addr);
	sockaddr.sin_port = htons(25000);

	int retval = connect(sock, (SOCKADDR *)&sockaddr, sizeof(sockaddr));

	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			return FALSE;
	}

	return TRUE;
}

void CloseProc()
{
	closesocket(sock);
	WSACleanup();
}

void ReadProc()
{
	int retval;
	retval = recv(sock, RecvQ.GetWriteBufferPtr(), RecvQ.GetNotBrokenPutSize(), 0);

	if (retval == 0)
		//��������
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
		}//����ó��
	}

	RecvQ.MoveWritePos(retval);
	CompleteRecvPacket();
}

void WriteProc()
{

}

void CompleteRecvPacket()
{
	while (1)
	{
		WORD Header;
		if (RecvQ.GetUseSize() <= sizeof(Header))
			return;

		RecvQ.Peek((char *)&Header, sizeof(Header));
		if (RecvQ.GetUseSize() < Header + sizeof(Header))
			return;

		RecvQ.RemoveData(sizeof(Header));
		
		Packet packet;
		packet.wLength = Header;
		//x,y ����
		RecvQ.Get((char *)&packet, sizeof(packet));
	}
}