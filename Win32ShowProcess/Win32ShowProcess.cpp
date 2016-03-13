// Win32ShowProcess.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "Win32ShowProcess.h"

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND hdlg;                                     //主窗口对话框
HWND hPrcsListview;                                //列表控件
HWND hDLLListview;                                //列表控件

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK DlgProc(HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam);//对话框消息处理回调函数

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

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32SHOWPROCESS, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32SHOWPROCESS));

	// 主消息循环: 
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
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
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
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 585,370 , NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   //显示对话框

   hdlg = CreateDialog(hInst//应用程序实例句柄，填写WinMain函数中的hInstance 参数
	   , MAKEINTRESOURCE(IDD_FORMVIEW)//创建窗口的资源模版。用MAKEINTRESOURCE(你的资源ID)生成
	   , hWnd//当前窗口的句柄
	   , (DLGPROC)DlgProc);//所创建的对话框消息处理函数
   ShowWindow(hdlg, SW_SHOWNA);

   hPrcsListview = GetDlgItem(hdlg, IDC_LIST1);//获取listview窗口句柄
   hDLLListview = GetDlgItem(hdlg, IDC_LIST2);//获取listview窗口句柄

   LVCOLUMN vcl;
   vcl.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
   // 第一列  
   vcl.pszText = L"进程名";//列标题  
   vcl.cx = 168;//列宽  
   vcl.iSubItem = 0;//子项索引 
   ListView_InsertColumn(hPrcsListview, 0, &vcl);
   vcl.pszText = L"进程ID";//列标题  
   vcl.cx = 80;//列宽  
   vcl.iSubItem = 1;//子项索引 
   ListView_InsertColumn(hPrcsListview, 1, &vcl);

   vcl.pszText = L"DLL名";//列标题  
   vcl.cx = 110;//列宽  
   vcl.iSubItem = 0;//子项索引 
   ListView_InsertColumn(hDLLListview, 0, &vcl);
   vcl.pszText = L"DLL路径";//列标题  
   vcl.cx = 128;//列宽  
   vcl.iSubItem = 1;//子项索引 
   ListView_InsertColumn(hDLLListview, 1, &vcl);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
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
		// 分析菜单选择: 
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
		// TODO:  在此添加任意绘图代码...
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

// “关于”框的消息处理程序。
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

//TODO：对话框消息处理
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
		//MessageBox(hdlg, _T("未知的消息"), L"GG", MB_OK);
		break;
	}
	return (INT_PTR)FALSE;
}

void showProcess(){
	//枚举进程需要调用三个API函数
	/*CreateToolhelp32Snapshot、Process32First、Process32Next*/
	ListView_DeleteAllItems(hPrcsListview);//清空列表

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);//创建进程快照
	if (hSnap == INVALID_HANDLE_VALUE){
		MessageBox(hdlg, _T("CreateToolhelp32Snapshot Error"), L"GG", MB_OK);
		return;
	}
	PROCESSENTRY32 Pe32 = { 0 };
	Pe32.dwSize = sizeof(PROCESSENTRY32);

	BOOL bRet = Process32First(hSnap, &Pe32);
	CString str;
	int index = 0;
	//获取进程快照中的每一项进程信息
	while (bRet)
	{
		LVITEM vitem;
		vitem.mask = LVIF_TEXT;
		vitem.pszText = Pe32.szExeFile;
		vitem.iItem = index;
		vitem.iSubItem = 0;
		ListView_InsertItem(hPrcsListview, &vitem);//先添加项
		str.Format(_T("%d"), Pe32.th32ProcessID);
		vitem.pszText = (LPWSTR)(LPCTSTR)str;
		vitem.iSubItem = 1;
		ListView_SetItem(hPrcsListview, &vitem);//设置子项
		index++;
		bRet = Process32Next(hSnap, &Pe32);
	}
	CloseHandle(hSnap);
}

//用于提升权限
void DebugPrivilege(){
	HANDLE hToken = NULL;
	//打开当前进程访问令牌
	BOOL bRet = OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken);
	if (bRet){
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		//取得描述权限的LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		//调整访问令牌的权限
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		CloseHandle(hToken);
	}
}

int getSelectPid(){
	DWORD selected = ListView_GetSelectionMark(hPrcsListview);
	if (selected == -1){
		MessageBox(hdlg, _T("请选择一个进程"), L"GG", MB_OK);
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
		MessageBox(hdlg, _T("查找PID失败"), L"GG", MB_OK);
		return 0;
	}
	//将wchat_t转换为int
	selected = wcstod(tmp, L'\0');
	return selected;
}

void onBtnPause(){
	int pid = getSelectPid();
	//ID为0则返回
	if (pid == 0)
		return;
	//创建线程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	if (hSnap == INVALID_HANDLE_VALUE){
		pid = GetLastError();
		CString erc;
		erc.Format(L"error code:%d", pid);
		MessageBox(NULL, erc, L"线程快照创建错误", MB_OK);
		return;
	}
	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);
	//获取进程快照中的每一项进程信息
	while (bRet)
	{
		//得到属于选中进程的线程
		if (Te32.th32OwnerProcessID == pid){
			//打开线程
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
			if (hThread == INVALID_HANDLE_VALUE){
				MessageBox(NULL, L"GG", L"线程句柄打开错误", MB_OK);
				return;
			}
			//暂停线程
			SuspendThread(hThread);
			CloseHandle(hThread);
		}
		bRet = Thread32Next(hSnap, &Te32);
	}
	CloseHandle(hSnap);
}

void onBtnRecover(){
	int pid = getSelectPid();
	//ID为0则返回
	if (pid == 0)
		return;
	//创建线程快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, pid);
	if (hSnap == INVALID_HANDLE_VALUE){
		pid = GetLastError();
		CString erc;
		erc.Format(L"error code:%d", pid);
		MessageBox(NULL, erc, L"线程快照创建错误", MB_OK);
		return;
	}
	THREADENTRY32 Te32 = { 0 };
	Te32.dwSize = sizeof(THREADENTRY32);
	BOOL bRet = Thread32First(hSnap, &Te32);
	//获取进程快照中的每一项进程信息
	while (bRet)
	{
		//得到属于选中进程的线程
		if (Te32.th32OwnerProcessID == pid){
			//打开线程
			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, Te32.th32ThreadID);
			if (hThread == INVALID_HANDLE_VALUE){
				MessageBox(NULL, L"GG", L"线程句柄打开错误", MB_OK);
				return;
			}
			//恢复线程
			ResumeThread(hThread);
			CloseHandle(hThread);
		}
		bRet = Thread32Next(hSnap, &Te32);
	}
	CloseHandle(hSnap);
}

void getDLLInfo(){
	ListView_DeleteAllItems(hDLLListview);//清空列表
	//获取选中的进程的PID
	int selected = getSelectPid();
	if (selected==0)
		return;
	//提升权限
	DebugPrivilege();

	MODULEENTRY32 Me32 = { 0 };
	Me32.dwSize = sizeof(MODULEENTRY32);
	//创建模块快照
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, selected);
	if (hSnap == INVALID_HANDLE_VALUE){
		//MessageBox(hdlg, _T("CreateToolhelp32Snapshot Error"), L"GG", MB_OK);
		selected = GetLastError();
		CString erc;
		erc.Format(L"error code:%d", selected);
		MessageBox(NULL, erc, L"模块快照创建错误", MB_OK);
		return;
	}
	BOOL bRet = Module32First(hSnap, &Me32);
	int index = 0;
	//获取模块快照中的每一项进程信息
	while (bRet)
	{
		LVITEM vitem;
		vitem.mask = LVIF_TEXT;
		vitem.pszText = Me32.szModule;
		vitem.iItem = index;
		vitem.iSubItem = 0;
		ListView_InsertItem(hDLLListview, &vitem);//先添加项
		vitem.pszText = Me32.szExePath;
		vitem.iSubItem = 1;
		ListView_SetItem(hDLLListview, &vitem);//设置子项
		index++;
		bRet = Module32Next(hSnap, &Me32);
	}
	CloseHandle(hSnap);
}
