
#include "framework.h"
#include "WindowsProject3.h"
#include "winsock2.h"
#include "stdio.h"
#include "string.h"
using namespace std;

#pragma comment(lib, "ws2_32")
#pragma warning(disable:4996)

#define MAX_LOADSTRING 100
#define WM_SOCKET WM_USER + 1

HINSTANCE hInst;                                
WCHAR szTitle[MAX_LOADSTRING];                  
WCHAR szWindowClass[MAX_LOADSTRING];           
TCHAR szWindowClassd[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM                MyRegisterClassd(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    WndProcd(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

SOCKET client;
HWND chatPage;
HWND loginPage;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8000);

    int ret = connect(client, (SOCKADDR*)&addr, sizeof(addr));

    if (ret == SOCKET_ERROR)
    {
        ret = WSAGetLastError();
        printf("Loi ket noi %d", ret);
    }

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WORKSTATIONCLASS, szWindowClassd, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    MyRegisterClassd(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

ATOM MyRegisterClassd(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProcd;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_WINDOWSPROJECT3);
    wcex.lpszClassName = szWindowClassd;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}



BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND login, chat;

   hInst = hInstance; // Store instance handle in our global variable

   login = CreateWindow(szWindowClass, L"Login", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 440, 200, nullptr, nullptr, hInstance, nullptr);
   chat = CreateWindow(szWindowClassd, L"Chat Page", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 600, 480, nullptr, nullptr, hInstance, nullptr);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
       10, 30, 400, 28, login, (HMENU)IDC_INPUT, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("Log in"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
       170, 65, 100, 30, login, (HMENU)BUTTON_LOGIN, GetModuleHandle(NULL), NULL);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
       10, 10, 400, 350, chat, (HMENU)IDC_LIST_MESSAGE, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
       420, 10, 150, 350, chat, (HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP,
       10, 360, 400, 40, chat, (HMENU)IDC_EDIT_MESSAGE, GetModuleHandle(NULL), NULL);
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("SEND"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
       420, 360, 150, 40, chat, (HMENU)IDC_BUTTON_SEND, GetModuleHandle(NULL), NULL);

   SendDlgItemMessageA(chat, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)"ALL");

   if (!login || !chat)
   {
      return FALSE;
   }

   loginPage = login;
   chatPage = chat;

   ShowWindow(login, nCmdShow);
   UpdateWindow(login);

   return TRUE;
}


void GetListOfUsers(char * id) {

    char buffer[256];
    send(client, "LIST", 5, 0);

    int length = recv(client, buffer, sizeof(buffer),0);
    sprintf(buffer + length - 1, "%s"," ");
   
    char delim[] = " ";
    char* token = strtok(buffer, delim);

    while (token) {

        if (strcmp(token, "LIST")&& strcmp(token, "OK")&&strcmp(token,id))
        {
            SendDlgItemMessageA(chatPage, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)token);
        }

        token = strtok(NULL, delim);
    }
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case BUTTON_LOGIN:
               
                char id[32];
                char buf[256];

                GetDlgItemTextA(hWnd, IDC_INPUT, id, sizeof(id));
                sprintf(buf, "CONNECT %s", id);
                
                send(client, buf, strlen(buf), 0);

                recv(client, buf, sizeof(buf), 0);

                if (strstr(buf, "CONNECT OK")!= NULL)
                {

                    ShowWindow(loginPage, SW_HIDE);
                    ShowWindow(chatPage, SW_SHOWDEFAULT);
                    WSAAsyncSelect(client, chatPage, WM_SOCKET, FD_READ);

                    GetListOfUsers(id);
                }
                else
                {
                    recv(client, buf, sizeof(buf), 0);
                    MessageBox(0, (LPCWSTR)buf, L"Error", MB_OK);
                }
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            TCHAR infor[] = _T("Enter user id to log in:");
            TextOut(hdc,
                10, 10,
                infor, _tcslen(infor));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProcd(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
    case WM_SOCKET:
    {
        if (WSAGETSELECTERROR(lParam))
        {
            closesocket(wParam);
        }
        else if (WSAGETSELECTEVENT(lParam) == FD_READ)
        {
            char buf[256];
            int ret = recv(wParam, buf, sizeof(buf), 0);
            buf[ret] = 0;

            char cmd[32], id[32];
            sscanf(buf, "%s %s", cmd, id);

            if (strcmp(cmd, "USER_CONNECT")==0)
            {
                SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)id);
            }
            else if (strcmp(cmd, "USER_DISCONNECT") == 0)
            {
                int index = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_FINDSTRINGEXACT, 0, (LPARAM)id);
         
                SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_DELETESTRING, index, (LPARAM)0);
            }
        }
        else if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
        {
            closesocket(wParam);
        }
    }
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code here...
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        send(wParam, "DISCONNECT", 11, 0);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
