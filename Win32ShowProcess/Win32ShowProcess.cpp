// Win32ShowProcess.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Win32ShowProcess.h"

#define MAX_LOADSTRING 100

// ȫ�ֱ���: 
HINSTANCE hInst;								// ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];					// �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];			// ����������
HWND hdlg;                                     //�����ڶԻ���
HWND hPrcsListview;                                //�б�ؼ�
HWND hDLLListview;                                //�б�ؼ�

// �˴���ģ���а����ĺ�����ǰ������: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);//�Ի�����Ϣ����ص�����

void showProcess();
int getSelectPid();
void getDLLInfo();
void DebugPrivilege();
void onBtnPause();
void onBtnRecover();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  �ڴ˷��ô��롣
	MSG msg;
	HACCEL hAccelTable;

	// ��ʼ��ȫ���ַ���
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32SHOWPROCESS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// ִ��Ӧ�ó����ʼ��: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32SHOWPROCESS));

	// ����Ϣѭ��: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  ����:  MyRegisterClass()
//
//  Ŀ��:  ע�ᴰ���ࡣ
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32SHOWPROCESS));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32SHOWPROCESS);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ����:  InitInstance(HINSTANCE, int)
//
//   Ŀ��:  ����ʵ�����������������
//
//   ע��: 
//
//        �ڴ˺����У�������ȫ�ֱ����б���ʵ�������
//        ��������ʾ�����򴰿ڡ�
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ��ʵ������洢��ȫ�ֱ�����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 585,370 , NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   //��ʾ�Ի���

   hdlg = CreateDialog(hInst//Ӧ�ó���ʵ���������дWinMain�����е�hInstance ����
	   , MAKEINTRESOURCE(IDD_FORMVIEW)//�������ڵ���Դģ�档��MAKEINTRESOURCE(�����ԴID)����
	   , hWnd//��ǰ���ڵľ��
	   , (DLGPROC)DlgProc);//�������ĶԻ�����Ϣ������
   ShowWindow(hdlg, SW_SHOWNA);

   hPrcsListview = GetDlgItem(hdlg, IDC_LIST1);//��ȡlistview���ھ��
   hDLLListview = GetDlgItem(hdlg, IDC_LIST2);//��ȡlistview���ھ��

   LVCOLUMN vcl;
   vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
   // ��һ��  
   vcl.pszText = L"������";//�б���  
   vcl.cx = 168;//�п�  
   vcl.iSubItem = 0;//�������� 
   ListView_InsertColumn(hPrcsListview, 0, &vcl);
   vcl.pszText = L"����ID";//�б���  
   vcl.cx = 80;//�п�  
   vcl.iSubItem = 1;//�������� 
   ListView_InsertColumn(hPrcsListview, 1, &vcl);

   vcl.pszText = L"DLL��";//�б���  
   vcl.cx = 110;//�п�  
   vcl.iSubItem = 0;//�������� 
   ListView_InsertColumn(hDLLListview, 0, &vcl);
   vcl.pszText = L"DLL·��";//�б���  
   vcl.cx = 128;//�п�  
   vcl.iSubItem = 1;//�������� 
   ListView_InsertColumn(hDLLListview, 1, &vcl);

   return TRUE;
}

//
//  ����:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  Ŀ��:    ���������ڵ���Ϣ��
//
//  WM_COMMAND	- ����Ӧ�ó���˵�
//  WM_PAINT	- ����������
//  WM_DESTROY	- �����˳���Ϣ������
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��: 
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  �ڴ���������ͼ����...
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

// �����ڡ������Ϣ�������
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

//TODO���Ի�����Ϣ����
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam){
	UNREFERENCED_PARAMETER(lParam);
	switch (msg)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON4:
			PostQuitMessage(0);
			break;
		case IDC_BUTTON2:
			showProcess();
			break;
		case IDC_BUTTON3:
			onBtnPause();
			break;
		case IDC_BUTTON5:
			getDLLInfo();
			break;
		case IDC_BUTTON6:
			onBtnRecover();
			break;
		default:
			break;
		}
		break;
	}
	default:
		//MessageBox(hdlg, _T("δ֪����Ϣ"), L"GG", MB_OK);
		break;
	}
	return (INT_PTR)FALSE;
}

void showProcess(){
	//ö�ٽ�����Ҫ��������API����
	/*CreateToolhelp32Snapshot��Process32First��Process32Next*/
	ListView_DeleteAllItems(hPrcsListview);//����б�

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//�������̿���
	if (hSnap == INVALID_HANDLE_VALUE){
		MessageBox(hdlg, _T("CreateToolhelp32Snapshot Error"), L"GG", MB_OK);
		return;
	}
	PROCESSENTRY32 Pe32 = { 0 };
	Pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bRet = Process32First(hSnap, &Pe32);
	CString str;
	int index = 0;
	//��ȡ���̿����е�ÿһ�������Ϣ
	while (bRet)
	{
		LVITEM vitem;
		vitem.mask = LVIF_TEXT;
		vitem.pszText = Pe32.szExeFile;
		vitem.iItem = index;
		vitem.iSubItem = 0;
		ListView_InsertItem(hPrcsListview, &vitem);//�������
		str.Format(_T("%d"), Pe32.th32ProcessID);
		vitem.pszText = (LPWSTR)(LPCTSTR)str;
		vitem.iSubItem = 1;
		ListView_SetItem(hPrcsListview, &vitem);//��������
		index++;
		bRet = Process32Next(hSnap, &Pe32);
	}
	CloseHandle(hSnap);
}

//��������Ȩ��
void DebugPrivilege(){
	HANDLE hToken = NULL;
	//�򿪵�ǰ���̷�������
	BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (bRet){
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		//ȡ������Ȩ�޵�LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//�����������Ƶ�Ȩ��
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
}

int getSelectPid(){
	DWORD selected = ListView_GetSelectionMark(hPrcsListview);
	if (selected == -1){
		MessageBox(hdlg, _T("��ѡ��һ������"), L"GG", MB_OK);
		return 0;
	}
	LVITEM vitem;
	wchar_t tmp[10];
	vitem.mask = LVIF_TEXT;
	vitem.iItem = selected;
	vitem.iSubItem = 1;
	vitem.pszText = tmp;
	vitem.cchTextMax = sizeof(tmp);
	if (!ListView_GetItem(hPrcsListview, &vitem)){
		MessageBox(hdlg, _T("����PIDʧ��"), L"GG", MB_OK);
		return 0;
	}
	//��wchat_tת��Ϊint
	selected = wcstod(tmp, L'\0');
	return selected;
}

void onBtnPause(){
	int pid = getSelectPid();
	//IDΪ0�򷵻�
	if (pid == 0)
		return;
	//�����߳̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	if (hSnap == INVALID_HANDLE_VALUE){
		pid = GetLastError();
		CString erc;
		erc.Format(L"error code:%d", pid);
		MessageBox(NULL, erc, L"�߳̿��մ�������", MB_OK);
		return;
	}
	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);
	//��ȡ���̿����е�ÿһ�������Ϣ
	while (bRet)
	{
		//�õ�����ѡ�н��̵��߳�
		if (Te32.th32OwnerProcessID == pid){
			//���߳�
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
			if (hThread == INVALID_HANDLE_VALUE){
				MessageBox(NULL, L"GG", L"�߳̾���򿪴���", MB_OK);
				return;
			}
			//��ͣ�߳�
			SuspendThread(hThread);
			CloseHandle(hThread);
		}
		bRet = Thread32Next(hSnap, &Te32);
	}
	CloseHandle(hSnap);
}

void onBtnRecover(){
	int pid = getSelectPid();
	//IDΪ0�򷵻�
	if (pid == 0)
		return;
	//�����߳̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	if (hSnap == INVALID_HANDLE_VALUE){
		pid = GetLastError();
		CString erc;
		erc.Format(L"error code:%d", pid);
		MessageBox(NULL, erc, L"�߳̿��մ�������", MB_OK);
		return;
	}
	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);
	//��ȡ���̿����е�ÿһ�������Ϣ
	while (bRet)
	{
		//�õ�����ѡ�н��̵��߳�
		if (Te32.th32OwnerProcessID == pid){
			//���߳�
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
			if (hThread == INVALID_HANDLE_VALUE){
				MessageBox(NULL, L"GG", L"�߳̾���򿪴���", MB_OK);
				return;
			}
			//�ָ��߳�
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
		bRet = Thread32Next(hSnap, &Te32);
	}
	CloseHandle(hSnap);
}

void getDLLInfo(){
	ListView_DeleteAllItems(hDLLListview);//����б�
	//��ȡѡ�еĽ��̵�PID
	int selected = getSelectPid();
	if (selected==0)
		return;
	//����Ȩ��
	DebugPrivilege();

	MODULEENTRY32 Me32 = { 0 };
	Me32.dwSize = sizeof(MODULEENTRY32);
	//����ģ�����
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, selected);
	if (hSnap == INVALID_HANDLE_VALUE){
		//MessageBox(hdlg, _T("CreateToolhelp32Snapshot Error"), L"GG", MB_OK);
		selected = GetLastError();
		CString erc;
		erc.Format(L"error code:%d", selected);
		MessageBox(NULL, erc, L"ģ����մ�������", MB_OK);
		return;
	}
	BOOL bRet = Module32First(hSnap, &Me32);
	int index = 0;
	//��ȡģ������е�ÿһ�������Ϣ
	while (bRet)
	{
		LVITEM vitem;
		vitem.mask = LVIF_TEXT;
		vitem.pszText = Me32.szModule;
		vitem.iItem = index;
		vitem.iSubItem = 0;
		ListView_InsertItem(hDLLListview, &vitem);//�������
		vitem.pszText = Me32.szExePath;
		vitem.iSubItem = 1;
		ListView_SetItem(hDLLListview, &vitem);//��������
		index++;
		bRet = Module32Next(hSnap, &Me32);
	}
	CloseHandle(hSnap);
}
