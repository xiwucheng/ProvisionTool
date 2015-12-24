// ProvisionToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ProvisionTool.h"
#include "ProvisionToolDlg.h"
#include "DeviceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


/* ---------------------------------------------------------------*/
/*                        DEFINE                                  */
/* ---------------------------------------------------------------*/
/* size of buffer */
#define BUFFERSIZE				1024/* 1K */
/* server port number */
#define TIN_HELP				"help"
#define TIN_EXIT				"exit"
#define TIN_CONNECT             "PressDisconnected"
#define TIN_CLEAR				"clear"
#define TIN_PRINTF				printf
#define COMBOBOX_ITEMS_MAX		200
#define SPACENUMBER				59
#define CONNECT_DEVICE_ERROR	111
#define CONNECT_FASTBOOT_ERROR	222
#define CONNECT_ADB_ERROR		333
#define CONNECT_FT_SEVER_ERROR	444
#define CONNECT_FT_FILE_ERROR	555
#define FTM_COLUMN_DEFAULT		1
#define FTM_ROW_DEFAULT			1
#define FTM_ROW_MAX				100
#define TMP_FILE_NAME			"./ftmTmp.txt"
#define LOG_FILE_NAME			"./ftmLog.txt"

// CProvisionToolDlg dialog

WCHAR* wszSNCmd[4] = {
	_T("flash txefileset ./writeSNFile"),
	_T("oem txe TXEI_SEC_TOOLS -acd-write 18 /logs/tmp"),
	_T("oem txe TXEI_SEC_TOOLS -acd-read 18 /logs/readSNFile"),
	_T("oem txe cat /logs/readSNFile")
};

#import "msxml3.dll"
using namespace MSXML2;

CProvisionToolDlg::CProvisionToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProvisionToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nRunIndex = 0;
	m_bRun = FALSE;
}

void CProvisionToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProvisionToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BROWSE, &CProvisionToolDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_WRITE, &CProvisionToolDlg::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_READ, &CProvisionToolDlg::OnBnClickedRead)
END_MESSAGE_MAP()


// CProvisionToolDlg message handlers

BOOL CProvisionToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	GUID MfDevice = {0xa5dcbf10, 0x6530, 0x11d2, {0x90, 0x1f, 0x0, 0xc0, 0x4f, 0xb9, 0x51, 0xed}};
	DEV_BROADCAST_DEVICEINTERFACE   dbi;
	ZeroMemory(&dbi,sizeof(dbi));
	dbi.dbcc_size=sizeof(dbi);
	dbi.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	dbi.dbcc_reserved = 0;
	dbi.dbcc_classguid = MfDevice;
	m_hMountBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_MOUNT));
	m_hDeleteBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_DELETE));
	m_hNABitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_NA));
	m_hSuccessBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_SUCESS));
	m_hFailureBitmap = LoadBitmap(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDB_FAILURE));
	m_hDevNotify = RegisterDeviceNotification(m_hWnd, &dbi, DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);//DEVICE_NOTIFY_ALL_INTERFACE_CLASSES
	if (Connect_device() == 0)
	{
		m_bConnected = TRUE;
		((CStatic*)GetDlgItem(IDC_STATUS))->SetBitmap(m_hMountBitmap);
	}
	else
	{
		m_bConnected = FALSE;
		((CStatic*)GetDlgItem(IDC_STATUS))->SetBitmap(m_hDeleteBitmap);
	}
	CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_BURNPRG);
	pCtrl->SetRange(0,100);
	((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hNABitmap);

	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	m_szPathName = path;
	int pos = m_szPathName.ReverseFind('\\');
	m_szPathName = m_szPathName.Left(pos);
	SetCurrentDirectory(m_szPathName);
	SetWindowPos(&CWnd::wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProvisionToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProvisionToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CProvisionToolDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_DEVICECHANGE) {
		DEV_BROADCAST_DEVICEINTERFACE* pdbd = (DEV_BROADCAST_DEVICEINTERFACE*)lParam;

		switch (wParam) {
			case DBT_DEVICEARRIVAL:
				if (pdbd->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
					PDEV_BROADCAST_DEVICEINTERFACE pDb = (PDEV_BROADCAST_DEVICEINTERFACE)pdbd;
					CString devName = pDb->dbcc_name;
					devName.MakeLower();
					if (devName.Find(_T("vid_8087")) != -1) {
						if (Connect_device() == 0)
						{
							m_bConnected = TRUE;
							((CStatic*)GetDlgItem(IDC_STATUS))->SetBitmap(m_hMountBitmap);
						}
					}	
				} 
				break;
			case DBT_DEVICEREMOVECOMPLETE:
				if (pdbd->dbcc_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
					PDEV_BROADCAST_DEVICEINTERFACE pDb = (PDEV_BROADCAST_DEVICEINTERFACE)pdbd;
					CString devName = pDb->dbcc_name;
					devName.MakeLower();
					if (devName.Find(_T("vid_8087")) != -1) {
						m_bConnected = FALSE;
						((CStatic*)GetDlgItem(IDC_STATUS))->SetBitmap(m_hDeleteBitmap);
					}	
				} 
				break;
			default:
				break;
		}
	}

	return CDialog::DefWindowProc(message, wParam, lParam);
}

int CProvisionToolDlg::Connect_device(void)
{
	DeviceManager * pdm = new DeviceManager();
	CString deviceName = pdm->getAndroidDevices();
	int retryCount = 10;
	while(deviceName.IsEmpty() && retryCount != 0) {
		deviceName = pdm->getAndroidDevices();
		retryCount--;
		Sleep(200);
	}

	if (deviceName.IsEmpty()) {
		delete pdm;
		return CONNECT_DEVICE_ERROR;
	} else if (-1 != deviceName.Find(_T("composite"))) {//adb device
		delete pdm;
		return CONNECT_FASTBOOT_ERROR;

	} else if (-1 != deviceName.Find(_T("adb interface"))) {//fastboot device 
		if(System(_T("fastboot.exe"), _T(" devices"), SW_HIDE, FALSE) == -1)
		{
			delete pdm;
			return CONNECT_FASTBOOT_ERROR;
		}
		Sleep(200);
		deviceName = pdm->getAndroidDevices();
		if (-1 != deviceName.Find(_T("composite"))) {//adb device
			if(System(_T("adb.exe"), _T(" forward tcp:8686 tcp:8686"), SW_HIDE, FALSE) == -1) {
				delete pdm;
				return CONNECT_ADB_ERROR;
			}
			// for fastboot
			delete pdm;
			return CONNECT_ADB_ERROR;
		}
		else if (-1 != deviceName.Find(_T("adb interface"))) {//fastboot device
				if(System(_T("fastboot.exe"), _T(" forward tcp:8686 tcp:8686"), SW_HIDE, FALSE) == -1) {
					delete pdm;
					return CONNECT_ADB_ERROR;
			}
		}		
		
	}
	delete pdm;
	return 0;
}

int CProvisionToolDlg::System(LPCWSTR cmd, LPCWSTR param, int nShow, BOOL isInfinite)
{
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, path, MAX_PATH);
	CString filePath(path);
	int pos = filePath.ReverseFind('\\');
	filePath = filePath.Left(pos + 1) + cmd;
	int ret = 0;
	if (isInfinite) 
	{
		// create procss
		PROCESS_INFORMATION ProcessInfo;
		STARTUPINFO StartupInfo; //This is an [in] parameter
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(StartupInfo) ; //Only compulsory field

		StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		LPWSTR LFilePath = (LPWSTR)filePath.GetBuffer();
		filePath.ReleaseBuffer();

		if(CreateProcess(LFilePath, (LPWSTR)param, NULL, NULL, TRUE, NULL, NULL, NULL, &StartupInfo, &ProcessInfo)) 
		{
			ret = WaitForSingleObject(ProcessInfo.hProcess,360000000);//timeout INFINITE
			if(ret == WAIT_TIMEOUT) {
				::TerminateProcess(ProcessInfo.hProcess, 4 );
			}
			CloseHandle(ProcessInfo.hThread);
			CloseHandle(ProcessInfo.hProcess);
		}
	} 
	else 
	{
		// create procss
		PROCESS_INFORMATION ProcessInfo;
		STARTUPINFO StartupInfo; //This is an [in] parameter
		ZeroMemory(&StartupInfo, sizeof(StartupInfo));
		StartupInfo.cb = sizeof(StartupInfo) ; //Only compulsory field

		StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		StartupInfo.wShowWindow = nShow;
		LPWSTR LFilePath = (LPWSTR)filePath.GetBuffer();
		filePath.ReleaseBuffer();

		if(CreateProcess(LFilePath, (LPWSTR)param, NULL, NULL, TRUE, NULL, NULL, NULL, &StartupInfo, &ProcessInfo)) {
			ret = WaitForSingleObject(ProcessInfo.hProcess,5000);//timeout 5s
			if(ret == WAIT_TIMEOUT) {
				::TerminateProcess(ProcessInfo.hProcess, 4 );
				return -1;
			}
			CloseHandle(ProcessInfo.hThread);
			CloseHandle(ProcessInfo.hProcess);
		}
#if 0
		SHELLEXECUTEINFO ShExecInfo = {0};
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = (LPCWSTR)filePath.GetBuffer();
		filePath.ReleaseBuffer();
		ShExecInfo.lpParameters = param;
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = nShow;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteEx(&ShExecInfo);
		ret = WaitForSingleObject(ShExecInfo.hProcess,5000);//timeout 5s

		if (ret == WAIT_TIMEOUT) {
			::TerminateProcess(ShExecInfo.hProcess, 4);
			return -1;
		}
#endif
	}
	return 0;
}

int CProvisionToolDlg::BuildSN(CString snStr)
{
	CStdioFile writeSNFile;
	if(writeSNFile.Open(_T("./writeSNFile") ,CFile::modeReadWrite | CFile::modeCreate)) {
		char * snchar = String2MultiChar(snStr);
		writeSNFile.Write(snchar, strlen(snchar));
		writeSNFile.Close();
		delete[] snchar;
		return 0;
	}
	return -1;
}

int CProvisionToolDlg::WriteSN(int len)
{
	CFileFind fileFind;
	CString tmpStr;
	CString szSNFile =_T("./writeSNFile");
	BOOL bFinshed = fileFind.FindFile(szSNFile);
	CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_BURNPRG);

	if (!bFinshed) 
	{
		AddLog(_T("SN 文件不存在!"));
		return -1;
	}
	DeviceManager* pdm = new DeviceManager(); 
	
	AddLog(pdm->adbPushToSdcard(_T(" flash txefileset ./writeSNFile")));
	if (!pdm->m_Scuccess)
	{
		goto __end;
	}
	tmpStr.Format(_T("%s %d %d"), _T(" oem txe TXEI_SEC_TOOLS -acd-write 18 /logs/tmp"), len, 100);
	AddLog(pdm->adbPushToSdcard(tmpStr));
	if (!pdm->m_Scuccess)
	{
		goto __end;
	}
	AddLog(pdm->adbPushToSdcard(_T(" oem txe TXEI_SEC_TOOLS -acd-read 18 /logs/readSNFile")));
	if (!pdm->m_Scuccess)
	{
		goto __end;
	}
	AddLog(pdm->adbPushToSdcard(_T(" oem txe cat /logs/readSNFile")));
	if (!pdm->m_Scuccess)
	{
		goto __end;
	}

	//AddLog(_T("SN 烧写成功!\r\n"));
	delete pdm;
	return 0;

__end:
	//AddLog(_T("SN 烧写失败!\r\n"));
	delete pdm;
	return -1;
}

int CProvisionToolDlg::WriteKeyBox(void)
{
	//provision key box
	CFileFind fileFind;
	CString szKeyFile = _T("./key.dat");
	CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_BURNPRG);
	BOOL bFinshed = fileFind.FindFile(szKeyFile);
	if (!bFinshed) 
	{
		AddLog(_T("Keybox 文件不存在!"));
		return -1;
	}

	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	DeviceManager* pdm = new DeviceManager(); 
	
	if(!pdm->TXEExecutableFileExsits())
	{
		AddLog(_T("TXEI_SEC_TOOLS 文件不存在!"));
		delete pdm;
		return -1;
	}
	AddLog(pdm->adbPushToSdcard(_T(" flash txefileset ./key.dat")));
	if(!pdm->m_Scuccess) 
	{
		goto __end;
	}
	AddLog(pdm->adbPushToSdcard(_T(" oem txe TXEI_SEC_TOOLS -acd-write 1 /logs/tmp 128 128")));
	if(!pdm->m_Scuccess) 
	{
		goto __end;
	}

	//AddLog(_T("Keybox 烧写成功!"));
	delete pdm;
	return 0;

__end:
	//AddLog(_T("Keybox 烧写失败!"));
	delete pdm;
	return -1;
}

int CProvisionToolDlg::BuildKeyBox(CString filePath)
{
	// TODO: Add your control notification handler code here
	::CoInitialize(NULL);
	MSXML2::IXMLDOMDocumentPtr pXmlDoc;
	MSXML2::IXMLDOMElementPtr pXmlElement;
	MSXML2::IXMLDOMElementPtr pXmlRoot;
	MSXML2::IXMLDOMNodeListPtr pXmlNodeList;
	MSXML2::IXMLDOMNamedNodeMapPtr pXmlNamedNodeMap;
	MSXML2::IXMLDOMNodePtr pXmlNode;
	HRESULT hR = pXmlDoc.CreateInstance(_uuidof(MSXML2::DOMDocument30));
	if(!SUCCEEDED(hR)) {
		return -1;
	}

	CString tmpXml = filePath;
	char* keyboxFile = String2MultiChar(tmpXml);
	
	if(!pXmlDoc->load(keyboxFile)) {
		AddLog(_T("文件解析错误，请检查keybox文件内容是否合法!"));
		//MessageBox(_T("Error in parsing ") + fileName + _T(" file, please check the contents of the keybox file"), _T("Error"), MB_ICONERROR);
		delete[] keyboxFile;
		return -1;
	}
	delete[] keyboxFile;


	//root attribute
	pXmlRoot = pXmlDoc->GetdocumentElement();
	pXmlRoot->get_attributes(&pXmlNamedNodeMap);

	long NamedNodeMapSize;
	pXmlNamedNodeMap->get_length(&NamedNodeMapSize);
	if(NamedNodeMapSize < 0) {
		AddLog(_T("DeviceID 不存在，请确认keybox文件是否正确 "));
		pXmlRoot.Release();
		pXmlNamedNodeMap.Release();
		return -1;
	}
	pXmlNamedNodeMap->get_item(0, &pXmlNode);

	CString rootAttsName((char*)(_bstr_t)pXmlNode->nodeName);
	if(rootAttsName != _T("DeviceID")) {
		AddLog(_T("DeviceID 不存在，请确认keybox文件是否正确 "));
		//MessageBox(_T("DeviceID attribute is required in ") + fileName , _T("Error"), MB_ICONERROR);
		pXmlRoot.Release();
		pXmlNamedNodeMap.Release();
		return -1;

	}
	CString rootAttsText((char*)(_bstr_t)pXmlNode->text);
	//save dat
	//CStdioFile widevine_File;
	CStdioFile widevine_File;
	CString szKeyFile;
	szKeyFile = _T("./key.dat");
	
	if(!widevine_File.Open(szKeyFile ,CFile::modeReadWrite | CFile::typeBinary | CFile::modeCreate)) {
		pXmlRoot.Release();
		pXmlNamedNodeMap.Release();
		return -1;
	}
	widevine_File.Write((char*)(_bstr_t)pXmlNode->text, strlen((char*)(_bstr_t)pXmlNode->text));
		
	int nullTerminatedCount = 32 - strlen((char*)(_bstr_t)pXmlNode->text);
	if(nullTerminatedCount > 0) 
	{
		for(int i = 0; i < nullTerminatedCount ; i++)
		{
			char nullChar = (char)0;
			widevine_File.Write(&nullChar, 1);
		}
	}

	long xmlNodesNum;
	pXmlRoot->get_childNodes(&pXmlNodeList);
	pXmlNodeList->get_length(&xmlNodesNum);
	for(int i = 0; i < xmlNodesNum; i++) 
	{
		pXmlNodeList->get_item(i, &pXmlNode);
		CString childNodeName((char*)(_bstr_t)pXmlNode->nodeName);
		CString childNodeValue((char*)(_bstr_t)pXmlNode->text);
		size_t len = 0;
		char* str = String2MultiChar(childNodeValue);
		BYTE* buf2 = HexStr2BinBuf(str,&len);
		widevine_File.Write(buf2, len);
		free(buf2);
		delete str;
	}
	widevine_File.Close();
	pXmlRoot.Release();
	pXmlNodeList.Release();
	pXmlNamedNodeMap.Release();
	pXmlNode.Release();
	pXmlDoc.Release();
	::CoUninitialize();

	return 0;
}

BYTE* CProvisionToolDlg::HexStr2BinBuf(const char* hex_str, size_t* buf_size)
{
	if(!hex_str)
	{
		*buf_size = 0;
		return NULL;
	}
	//1.偶数个
	size_t len = strlen(hex_str);
	

	*buf_size = len/2;
	BYTE* buf = (BYTE*)malloc(*buf_size);
	
	for(size_t i = 0,j = 0; i<len; i+=2,++j)
	{
		BYTE value = (BYTE)HexStr2Int(hex_str+i,2);
		buf[j] = value;
	}
	return buf;
}

size_t CProvisionToolDlg::HexStr2Int(const char* hex_str, size_t length)
{
	size_t sum = 0;
	for(size_t i = 0;i < length; ++i)
	{
		int asc = (int)hex_str[i];
		size_t r1 =(asc & 0x40)?(asc&0x0F)+0x9:(asc & 0x0F);
		sum+=(r1*(int)pow((double)16,(double)(length-i-1)));
	}
	return sum;
}

char* CProvisionToolDlg::String2MultiChar(CString& str)
{
	char *ptr;     
	LONG len;    
	len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);   
	ptr = new char [len+1];    
	memset(ptr,0,len + 1);    
	WideCharToMultiByte(CP_ACP, 0, str, -1, ptr, len + 1, NULL, NULL);    
	return ptr;
}

void CProvisionToolDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	if (!m_bConnected)
	{
		MessageBox(_T("设备未连接，请先连接设备！"),_T("错误"),MB_ICONWARNING);
		return;
	}
	if (!m_bRun)
	{
		m_bRun = TRUE;
	}
	else
	{
		return; //wait thread complete
	}
	m_nRunIndex = 1;
	GetDlgItem(IDC_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_READ)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_DISABLED);
	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoThread,this,0,NULL));
}

void CProvisionToolDlg::OnBnClickedBrowse()
{
	// TODO: Add your control notification handler code here
	TCHAR path[MAX_PATH];
	CString WvPath;
	ZeroMemory(path,sizeof(path));
	BROWSEINFO binfo;
	binfo.hwndOwner = m_hWnd;
	binfo.pidlRoot = NULL;
	binfo.pszDisplayName = path;
	binfo.lpszTitle = _T("请选择keybox文件所在目录:");
	binfo.ulFlags = 0;
	binfo.lpfn = NULL;
	binfo.iImage = 0;
	LPITEMIDLIST lpList = SHBrowseForFolder(&binfo);
	if(lpList && SHGetPathFromIDList(lpList, path)) 
	{
		WvPath.Format(_T("%s"), path);
		GetDlgItem(IDC_KEYPATH)->SetWindowTextW(WvPath);
	} 
}

void CProvisionToolDlg::OnBnClickedWrite()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CProvisionToolDlg::OnBnClickedRead()
{
	// TODO: Add your control notification handler code here
	if (!m_bConnected)
	{
		MessageBox(_T("设备未连接，请先连接设备！"),_T("错误"),MB_ICONWARNING);
		return;
	}
	if (!m_bRun)
	{
		m_bRun = TRUE;
	}
	else
	{
		return; //wait thread complete
	}
	m_nRunIndex = 2;
	GetDlgItem(IDC_WRITE)->EnableWindow(FALSE);
	GetDlgItem(IDC_READ)->EnableWindow(FALSE);
	GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	EnableMenuItem(::GetSystemMenu(m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_DISABLED);
	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)DoThread,this,0,NULL));
}

void CProvisionToolDlg::AddLog(CString log)
{
	CString buff,tmp;
	GetDlgItem(IDC_CMDLOG)->GetWindowText(buff);
	tmp=log;
	//tmp+=TEXT("\r\n");
	buff+=tmp;
	CEdit* output=(CEdit*)GetDlgItem(IDC_CMDLOG);
	output->SetWindowText(buff);
	output->LineScroll(output->GetLineCount());
}

void CProvisionToolDlg::ReadDevice(void)
{
	CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_BURNPRG);
	((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hNABitmap);
	pCtrl->SetPos(0);
	//SetDlgItemText(IDC_CMDLOG,_T(""));

	DeviceManager* pdm = new DeviceManager(); 
	BOOL isCC6Exist = pdm->CC6_UMIP_ACCESS_APPExecutableFileExsits();
	pCtrl->SetPos(30);
	if(!isCC6Exist) 
	{
		AddLog(pdm->adbPushToSdcard(_T(" oem txe TXEI_SEC_TOOLS -acd-read 1 /logs/test.dat")));

	} 
	else 
	{
		AddLog(pdm->adbPushToSdcard(_T(" oem txe TXEI_SEC_TOOLS -acd-read 1 /logs/test.dat")));
	}
	pCtrl->SetPos(70);
	
	if(pdm->m_Scuccess) 
	{
		if(!isCC6Exist)
		{
			AddLog(pdm->adbPushToSdcard(_T(" oem txe cat /logs/test.dat")));
		} 
		else 
		{
			AddLog(pdm->adbPushToSdcard(_T(" oem txe cat /logs/test.dat")));
		}

	}
	pCtrl->SetPos(100);
	//read key box
	if(pdm->m_Result != _T("")) {
		//MessageBox(_T("Read keybox succeeds") , _T("Info"), MB_ICONINFORMATION);
		((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hSuccessBitmap);
	}
	else
	{
		pCtrl->SetPos(0);
		((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hFailureBitmap);
	}
	delete pdm;
}

int CProvisionToolDlg::WriteDevice(void)
{
	CString filePath;
	TCHAR path[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, path);
	GetDlgItem(IDC_KEYPATH)->GetWindowTextW(filePath);
	CProgressCtrl* pCtrl = (CProgressCtrl*)GetDlgItem(IDC_BURNPRG);
	((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hNABitmap);
	pCtrl->SetPos(0);
	if(filePath == _T("")) {
		MessageBox(_T("请指定keybox所在文件夹"), _T("警告"), MB_ICONWARNING);
		return -1;
	}

	CString snStr;
	GetDlgItemText(IDC_SN,snStr);
	if (snStr.GetLength() == 0)
	{
		MessageBox(_T("请输入SN号码"), _T("警告"), MB_ICONWARNING);
		return -1;
	}

	SetDlgItemText(IDC_CMDLOG,_T(""));
	char* szSNStr = String2MultiChar(snStr);
	//browse the directory named filePath
	CFileFind fileFind;
	CString szXmlPath;
	GetDlgItemText(IDC_KEYPATH,szXmlPath);
	CString tmpXml = szXmlPath + _T("\\*.xml");
	BOOL bFinshed = fileFind.FindFile(tmpXml);
	CString fileName = _T("");
	CFile fp;
	while(bFinshed)
	{
		bFinshed = fileFind.FindNextFile();
		if(!fileFind.IsDirectory()) 
		{
			fileName = fileFind.GetFilePath();
			if (fp.Open(fileName,CFile::modeRead|CFile::typeBinary))
			{
				DWORD len = fp.GetLength();
				char* tmpBuff = new char[len+1];
				memset(tmpBuff,0,len+1);
				fp.Read(tmpBuff,len);
				fp.Close();
				char* token = strstr(tmpBuff,szSNStr);
				if (token != NULL && *(token + strlen(szSNStr)) == '\"')
				{
					delete tmpBuff;
					break;
				}
				delete tmpBuff;
			}
		}
		fileName = _T("");
	}
	fileFind.Close();
	delete szSNStr;
	if(fileName == _T("")) {
		MessageBox(_T("没有找到可用的keybox文件"), _T("错误"), MB_ICONERROR);
		return -1;
	}

	int pos,pos2 = fileName.ReverseFind(_T('\\'));
	CString outDir=fileName.Left(pos2+1)+_T("backup");
	DeviceManager* pdm = new DeviceManager(); 
	BOOL isCC6Exist;

	if (BuildSN(snStr))
	{
		goto __err;
	}
	pCtrl->SetPos(20);
	if (BuildKeyBox(fileName))
	{
		goto __err;
	}
	pCtrl->SetPos(40);
	if (WriteSN(snStr.GetLength()))
	{
		goto __err;
	}
	pCtrl->SetPos(60);
	if (WriteKeyBox())
	{
		goto __err;
	}
	pCtrl->SetPos(80);


	isCC6Exist = pdm->CC6_UMIP_ACCESS_APPExecutableFileExsits();
	if(!isCC6Exist) 
	{
		AddLog(pdm->adbPushToSdcard(_T(" oem txe TXEI_SEC_TOOLS -acd-read 1 /logs/test.dat")));

	} 
	else 
	{
		AddLog(pdm->adbPushToSdcard(_T(" oem txe TXEI_SEC_TOOLS -acd-read 1 /logs/test.dat")));
	}
	
	if(pdm->m_Scuccess) 
	{
		if(!isCC6Exist)
		{
			AddLog(pdm->adbPushToSdcard(_T(" oem txe cat /logs/test.dat")));
		} 
		else 
		{
			AddLog(pdm->adbPushToSdcard(_T(" oem txe cat /logs/test.dat")));
		}

	}
	pCtrl->SetPos(100);
	//read key box
	pos=pdm->m_Result.Find(snStr);
	if(pdm->m_Result != _T("") && pos != -1) 
	{
		//MessageBox(_T("Read keybox succeeds") , _T("Info"), MB_ICONINFORMATION);
		((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hSuccessBitmap);
	}
	else
	{
		goto __err;
	}


	//((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hSuccessBitmap);
	CreateDirectory(outDir,NULL);
	outDir = outDir+fileName.Right(fileName.GetLength()-pos2);
	MoveFileEx(fileName,outDir,MOVEFILE_REPLACE_EXISTING);
	delete pdm;
	return 0;
__err:
	pCtrl->SetPos(0);
	delete pdm;
	((CStatic*)GetDlgItem(IDC_STATUS2))->SetBitmap(m_hFailureBitmap);
	return -1;
}

UINT CProvisionToolDlg::DoThread(LPVOID lParam)
{
	CProvisionToolDlg* pThis = (CProvisionToolDlg*)lParam;
	if (pThis->m_nRunIndex == 1)
	{
		pThis->WriteDevice();
	}
	else if (pThis->m_nRunIndex == 2)
	{
		pThis->ReadDevice();
	}
	pThis->GetDlgItem(IDC_WRITE)->EnableWindow();
	pThis->GetDlgItem(IDC_READ)->EnableWindow();
	pThis->GetDlgItem(IDC_BROWSE)->EnableWindow();
	EnableMenuItem(::GetSystemMenu(pThis->m_hWnd,FALSE),SC_CLOSE,MF_BYCOMMAND|MF_ENABLED);
	pThis->m_bRun = FALSE;
	return 0;
}
