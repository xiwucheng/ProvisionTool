/*
 * Copyright (c) 2012 Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "StdAfx.h"
#include "DeviceManager.h"

#include "setupapi.h"

#include "cfgmgr32.h"

#define REGSTR_KEY_USB TEXT("USB")
#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#define BUFSIZE 4096


using namespace std;



DeviceManager::DeviceManager()
{
	
}

DeviceManager::~DeviceManager(void)
{
}


BOOL DeviceManager::hasAndroidDevices() {

	CString device_descriptor, device_instance_path;
	CString device_android_address;
	CString phone_serial(_T(""));
	HDEVINFO hDevInfoAndroid = NULL, hDevInfoGadget = NULL;
	SP_DEVINFO_DATA DeviceInfoData, DeviceInfoDataGadget;
	DWORD i = 0, k = 0;
	//CONFIGRET ConfigRet;
	int ibuf = 1024;
	int hub_port = 0;
	DEVINST parentInst, childInst;

	LPTSTR buffer = NULL;
	DWORD buffersize = 0;
	DWORD DataT = 0;

	CStringList parents;

	DEVINST devInstParent;
	CONFIGRET ret_status;
	TCHAR szDeviceInstanceID [MAX_DEVICE_ID_LEN];

	// Andoid GUI
	GUID usbGuidAndroid;
	usbGuidAndroid.Data1 = 0x3F966BD9;
	usbGuidAndroid.Data2 = 0xFA04 ;
	usbGuidAndroid.Data3 = 0x4ec5;
	usbGuidAndroid.Data4[0] = 0x99;
	usbGuidAndroid.Data4[1] = 0x1C;
	usbGuidAndroid.Data4[2] = 0xD3;
	usbGuidAndroid.Data4[3] = 0x26;
	usbGuidAndroid.Data4[4] = 0x97;
	usbGuidAndroid.Data4[5] = 0x3B;
	usbGuidAndroid.Data4[6] = 0x51;
	usbGuidAndroid.Data4[7] = 0x28;

	GUID usbGuidGadget;
	usbGuidGadget.Data1 = 0x36fc9e60;
	usbGuidGadget.Data2 = 0xc465 ;
	usbGuidGadget.Data3 = 0x11cf;
	usbGuidGadget.Data4[0] = 0x80;
	usbGuidGadget.Data4[1] = 0x56;
	usbGuidGadget.Data4[2] = 0x44;
	usbGuidGadget.Data4[3] = 0x45;
	usbGuidGadget.Data4[4] = 0x53;
	usbGuidGadget.Data4[5] = 0x54;
	usbGuidGadget.Data4[6] = 0x00;
	usbGuidGadget.Data4[7] = 0x00;

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// Get the list of the android devices (MOS or POS mode)
	hDevInfoAndroid = SetupDiGetClassDevs(&usbGuidAndroid, REGSTR_KEY_USB, 0, DIGCF_PRESENT);

	if (hDevInfoAndroid == INVALID_HANDLE_VALUE) {
		SetupDiDestroyDeviceInfoList(hDevInfoAndroid);
		return FALSE;
	}

	// examine all the android devices
	for (i=0; SetupDiEnumDeviceInfo(hDevInfoAndroid, i, &DeviceInfoData); i++) {

		// Get device descriptor
		if (buffer) {
			LocalFree(buffer);
			buffer=NULL;
			buffersize = 0;
		}
		while (!SetupDiGetDeviceRegistryProperty(hDevInfoAndroid, &DeviceInfoData, SPDRP_DEVICEDESC, &DataT, (PBYTE)buffer, buffersize, &buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buffer) LocalFree(buffer);
				buffer = (TCHAR *)LocalAlloc(LPTR,buffersize * 2);
			} else {
				break;
			}
		}

		CString tmpString(buffer);
		device_descriptor=tmpString.MakeLower();

		// handle only the device for which the device descriptor contains the "android" key word
		if(device_descriptor.Find(_T("adb interface")) != -1 ) {
			return TRUE;
		} 
	} // end for SetupDiEnumDeviceInfo Android


	if (hDevInfoAndroid)
		SetupDiDestroyDeviceInfoList(hDevInfoAndroid);

	if (buffer) {
		LocalFree(buffer);
		buffer=NULL;
		buffersize = 0;
	}
	return FALSE;
}

CString DeviceManager::getAndroidDevices() {

	CString device_descriptor, device_instance_path;
	CString device_android_address;
	CString phone_serial(_T(""));
	HDEVINFO hDevInfoAndroid = NULL, hDevInfoGadget = NULL;
	SP_DEVINFO_DATA DeviceInfoData, DeviceInfoDataGadget;
	DWORD i = 0, k = 0;
	//CONFIGRET ConfigRet;
	int ibuf = 1024;
	int hub_port = 0;
	DEVINST parentInst, childInst;

	LPTSTR buffer = NULL;
	DWORD buffersize = 0;
	DWORD DataT = 0;

	CStringList parents;

	DEVINST devInstParent;
	CONFIGRET ret_status;
	TCHAR szDeviceInstanceID [MAX_DEVICE_ID_LEN];

	// Andoid GUI
	GUID usbGuidAndroid;
	usbGuidAndroid.Data1 = 0x3F966BD9;
	usbGuidAndroid.Data2 = 0xFA04 ;
	usbGuidAndroid.Data3 = 0x4ec5;
	usbGuidAndroid.Data4[0] = 0x99;
	usbGuidAndroid.Data4[1] = 0x1C;
	usbGuidAndroid.Data4[2] = 0xD3;
	usbGuidAndroid.Data4[3] = 0x26;
	usbGuidAndroid.Data4[4] = 0x97;
	usbGuidAndroid.Data4[5] = 0x3B;
	usbGuidAndroid.Data4[6] = 0x51;
	usbGuidAndroid.Data4[7] = 0x28;

	GUID usbGuidGadget;
	usbGuidGadget.Data1 = 0x36fc9e60;
	usbGuidGadget.Data2 = 0xc465 ;
	usbGuidGadget.Data3 = 0x11cf;
	usbGuidGadget.Data4[0] = 0x80;
	usbGuidGadget.Data4[1] = 0x56;
	usbGuidGadget.Data4[2] = 0x44;
	usbGuidGadget.Data4[3] = 0x45;
	usbGuidGadget.Data4[4] = 0x53;
	usbGuidGadget.Data4[5] = 0x54;
	usbGuidGadget.Data4[6] = 0x00;
	usbGuidGadget.Data4[7] = 0x00;

	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

	// Get the list of the android devices (MOS or POS mode)
	hDevInfoAndroid = SetupDiGetClassDevs(&usbGuidAndroid, REGSTR_KEY_USB, 0, DIGCF_PRESENT);

	if (hDevInfoAndroid == INVALID_HANDLE_VALUE) {
		SetupDiDestroyDeviceInfoList(hDevInfoAndroid);
		return _T("");
	}

	// examine all the android devices
	for (i=0; SetupDiEnumDeviceInfo(hDevInfoAndroid, i, &DeviceInfoData); i++) {

		// Get device descriptor
		if (buffer) {
			LocalFree(buffer);
			buffer=NULL;
			buffersize = 0;
		}
		while (!SetupDiGetDeviceRegistryProperty(hDevInfoAndroid, &DeviceInfoData, SPDRP_DEVICEDESC, &DataT, (PBYTE)buffer, buffersize, &buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buffer) LocalFree(buffer);
				buffer = (TCHAR *)LocalAlloc(LPTR,buffersize * 2);
			} else {
				break;
			}
		}

		CString tmpString(buffer);
		device_descriptor=tmpString.MakeLower();
		if (device_descriptor.Trim() == _T("android")) {
			device_descriptor = _T("");
		}
		// handle only the device for which the device descriptor contains the "android" key word
		if(device_descriptor.Find(_T("intel android")) != -1 ) {
			
			if(device_descriptor.Find(_T("adb interface")) != -1 ) {
				
			} else {
				adbProcess();
				Sleep(10);
				POSITION pos = m_list.GetHeadPosition();  
				while( pos!= NULL ) 
				{  
					CString line = m_list.GetNext(pos);
					if (-1 != line.Find(_T("daemon not running"))) {
						 //need reconnect
						//CDeviceListener::isRemoved = TRUE;
					}
					
					if ((-1 != line.Find(_T("devices")))||(-1 != line.Find(_T("fastboot")))) {
						continue;
					}
					if ((-1 != line.Find(_T("device")))||(-1 != line.Find(_T("fastboot")))) {
						device_descriptor = _T("composite");
						break;
					}
				}  
			}
			break;
			
			//AfxMessageBox(_T("device_descriptor  ")+device_descriptor, MB_OK| MB_ICONINFORMATION);
		} 
	} // end for SetupDiEnumDeviceInfo Android


	if (hDevInfoAndroid)
		SetupDiDestroyDeviceInfoList(hDevInfoAndroid);

	if (buffer) {
		LocalFree(buffer);
		buffer = NULL;
		buffersize = 0;
	}
	return device_descriptor;
}

void DeviceManager::Split(const CString& src, CString delimit, CStringList* pOutList, int num, CString nullSubst) 
{ 
	ASSERT( !src.IsEmpty() && !delimit.IsEmpty() ); 
	if(num==1)   
	{ 
		pOutList->AddTail(src);  
		return;  
	}  
	int deliLen = delimit.GetLength();  
	long index = -1, lastSearchPosition = 0, cnt = 0;  
	while( (index=src.Find(delimit, lastSearchPosition))!=-1 )  
	{  
		if(index==lastSearchPosition)  
			pOutList->AddTail(nullSubst); 
		else 
			pOutList->AddTail(src.Mid(lastSearchPosition, index-lastSearchPosition));  
		lastSearchPosition = index + deliLen; 
		if(num) 
		{     
			++cnt;  
			if(cnt+1==num) break;  
		}  
	}  
	CString lastOne = src.Mid(lastSearchPosition);
	pOutList->AddTail( lastOne.IsEmpty()? nullSubst:lastOne);
}  

DWORD WINAPI DeviceManager::ThreadProcRorProcess (LPVOID lpParam) {
	DeviceManager *pdm = (DeviceManager *)lpParam;
	//OUTPUT
	while(1) {
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead = 0;;
		BOOL flag = ReadFile(pdm->m_ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL);

		if (!flag || (BytesRead == 0)) break; 
		CString buf(buffer);
		//sucess
		pdm->m_list.RemoveAll();
		pdm->Split(buf, _T("\n"), &pdm->m_list, 0, _T("<null>"));
		
		if (-1 != buf.Find(_T("device"))) {
			break;
		}
		else if (-1 != buf.Find(_T("fastboot"))) { //add the fastboot retunr
			break;
		}
		
		//fail
		if (-1 != buf.Find(_T("not found")) || -1 != buf.Find(_T("error")) || -1 !=buf.Find(_T("Permission denied"))) {
			break;


		}
	}
	return 0;

}


void DeviceManager::adbProcess() {

	CString str;
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL,path,MAX_PATH);
	CString p(path);
	int nPos = p.ReverseFind('\\');
	p = p.Left(nPos+1) + _T("fastboot.exe");


    // one way to start process, but can read return result of process via pipe
	// create pipe
	SECURITY_ATTRIBUTES securityAttr = {0};
	securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttr.bInheritHandle = TRUE;
	securityAttr.lpSecurityDescriptor = NULL;

	HANDLE ChildIn_Read, ChildIn_Write;
	if (!CreatePipe(&ChildIn_Read, &ChildIn_Write, &securityAttr, 0)) {
		MessageBoxA(NULL, "Error CreatePipe 1", "device", MB_OK);
	}

	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	HANDLE ChildOut_Read, ChildOut_Write;

	if (!CreatePipe(&ChildOut_Read, &ChildOut_Write, &securityAttr, 0)) {
		MessageBoxA(NULL, "Error CreatePipe 2", "device", MB_OK);
	}
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);


	DWORD dwThreadID = 0;
	CreateThread(NULL, 0, ThreadProcRorProcess,(void*)this, 0, &dwThreadID); 

	// create procss
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo) ; //Only compulsory field
	StartupInfo.hStdError = ChildOut_Write;
	StartupInfo.hStdOutput = ChildOut_Write;
	StartupInfo.hStdInput = ChildIn_Read;

	StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	LPWSTR filePath = (LPWSTR)p.GetBuffer();
	LPWSTR out;
	out = _T(" devices");

	p.ReleaseBuffer();
    m_ChildOut_Read = ChildOut_Read ;


	if(CreateProcess(filePath, out, NULL, NULL, TRUE, NULL, NULL, NULL, &StartupInfo, &ProcessInfo)) {
		int ret = WaitForSingleObject(ProcessInfo.hProcess,7000);
		if(ret == WAIT_TIMEOUT) {

	        str.Format(_T("%d"), ret);
			::TerminateProcess(ProcessInfo.hProcess, 4 );

	    }

		Sleep(100);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	} else {
		
    }

	if(ChildIn_Read != NULL) {
		CloseHandle(ChildIn_Read);
		ChildIn_Read = NULL;
	}
	if(ChildIn_Write != NULL) {
		CloseHandle(ChildIn_Write);
		ChildIn_Write = NULL;
	}
	if(ChildOut_Write != NULL) {
		CloseHandle(ChildOut_Write);
		ChildOut_Write = NULL;
	}
	if(ChildOut_Read != NULL) {
		CloseHandle(ChildOut_Read);
		ChildOut_Read = NULL;
		m_ChildOut_Read = NULL;
	}
	
	return;

}


BOOL DeviceManager::ftServerIsStarted()
{
	m_Scuccess = FALSE;
	adbShellProcess(_T(" shell ps | grep 'ft'"));
	//adbShellProcess(_T(" shell /system/bin/ft"));
	Sleep(30);
	return m_Scuccess;
}

BOOL DeviceManager::ftExecuableFileExsits()
{
	m_Scuccess = FALSE;
	adbShellProcess(_T(" shell ls /system/bin/ft"));
	Sleep(10);
	return m_Scuccess;
}

BOOL DeviceManager::TXEExecutableFileExsits()
{
	m_Scuccess = FALSE;
	adbShellProcess(_T(" shell ls /system/bin/TXEI_SEC_TOOLS"));
	Sleep(10);
	return m_Scuccess;
}

BOOL DeviceManager::CC6_UMIP_ACCESS_APPExecutableFileExsits()
{
	m_Scuccess = FALSE;
	adbShellProcess(_T(" shell ls /system/bin/CC6_UMIP_ACCESS_APP"));
	Sleep(10);
	return m_Scuccess;
}


CString DeviceManager::adbPushToSdcard(LPCWSTR cmd)
{
	m_Scuccess = FALSE;
	m_Result = _T("");
	adbShellProcess((LPWSTR)cmd);
	Sleep(10);
	return m_Result;
}


DWORD WINAPI DeviceManager::ThreadProcRorProcess2 (LPVOID lpParam) {
	DeviceManager *pdm = (DeviceManager *)lpParam;
	//OUTPUT
	int runCount = 0;
	int test_result= 0;
	while(1) {
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead = 0;
		BOOL flag = ReadFile(pdm->m_ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL);

		CString buf(buffer);
		
		//fail
		if (-1 != buf.Find(_T("not contain")) || -1 != buf.Find(_T("failed")) || -1 != buf.Find(_T("No such file")) || -1 != buf.Find(_T("not found")) || -1 != buf.Find(_T("error")) || -1 !=buf.Find(_T("Permission denied"))) {
			pdm->m_Scuccess = FALSE;
			test_result=1;
			pdm->m_Result += buf;
			break;

		}	
		if(-1 != buf.Find(_T("KB/s")) || -1 != buf.Find(_T("completed successfully")) || -1 != buf.Find(_T("keybox.dat")) || -1 != buf.Find(_T("TXEI_SEC_TOOLS"))|| buf.GetLength() <= 2) {
			pdm->m_Scuccess = TRUE;
			pdm->m_Result += buf;
			break;
		}
		//sucess
		//nohup: appending output to nohup.out
		
		if (-1 != buf.Find(_T("ft"))) {//AfxMessageBox(_T(" need to start server  buf ")+buf, MB_OK| MB_ICONINFORMATION);
			if (-1 != buf.Find(_T("/system/bin/ft"))) {
				pdm->m_Scuccess = TRUE;
				pdm->m_Result += buf;
				break;
			} else {
				char * phrase = NULL; 
				phrase = strtok(buffer, " ");
				while(phrase) {
					if(!strncmp(phrase, "ft", 2)) {
						pdm->m_Scuccess = TRUE;
						pdm->m_Result += buf;
						break;
					}
					phrase = strtok(NULL, " ");
				}
			}

		}

		if(-1 != buf.Find(_T("ExitThread"))) {
			if((runCount <= 1)||(test_result == 0)) {
				pdm->m_Scuccess = TRUE;
			} else {
			}
			break;
		} else {
			pdm->m_Result += buf;
		}
		if (!flag || (BytesRead == 0)) break;
		runCount ++;
	}
	return 100;

}


void DeviceManager::adbShellProcess(LPWSTR pStr) {

	CString str;
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL,path,MAX_PATH);
	CString p(path);
	int nPos = p.ReverseFind('\\');
	p = p.Left(nPos+1) + _T("fastboot.exe");


    // one way to start process, but can read return result of process via pipe
	// create pipe
	SECURITY_ATTRIBUTES securityAttr = {0};
	securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttr.bInheritHandle = TRUE;
	securityAttr.lpSecurityDescriptor = NULL;

	HANDLE ChildIn_Read, ChildIn_Write;
	if (!CreatePipe(&ChildIn_Read, &ChildIn_Write, &securityAttr, 0)) {
		MessageBoxA(NULL, "Error CreatePipe 1", "shell", MB_OK);
	}

	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	HANDLE ChildOut_Read, ChildOut_Write;

	if (!CreatePipe(&ChildOut_Read, &ChildOut_Write, &securityAttr, 0)) {
		MessageBoxA(NULL, "Error CreatePipe 2", "shell", MB_OK);
	}
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);


	DWORD dwThreadID = 0;
	HANDLE thread2 = CreateThread(NULL, 0, ThreadProcRorProcess2,(void*)this, 0, &dwThreadID); 

	// create procss
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo) ; //Only compulsory field
	StartupInfo.hStdError = ChildOut_Write;
	StartupInfo.hStdOutput = ChildOut_Write;
	StartupInfo.hStdInput = ChildIn_Read;

	StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	LPWSTR filePath = (LPWSTR)p.GetBuffer();
	LPWSTR out;

	out = pStr;

	p.ReleaseBuffer();
    m_ChildOut_Read = ChildOut_Read ;

	if(CreateProcess(filePath, out, NULL, NULL, TRUE, NULL, NULL, NULL, &StartupInfo, &ProcessInfo)) {
		int ret = WaitForSingleObject(ProcessInfo.hProcess,3000);
		if(ret == WAIT_TIMEOUT) {

	        str.Format(_T("%d"), ret);
			::TerminateProcess(ProcessInfo.hProcess, 4);
	    }

		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
		char* buffer = "ExitThread";
		DWORD BytesRead = 0;
		BOOL flag = WriteFile(ChildOut_Write, buffer, strlen(buffer), &BytesRead, NULL);

	} else {
		
    }
	int ret = WaitForSingleObject(thread2, 2000);
	if(ret == WAIT_TIMEOUT) {
		::TerminateThread(thread2, 5);
		CloseHandle(thread2);
	}
	if(ChildIn_Read != NULL) {
		CloseHandle(ChildIn_Read);
		ChildIn_Read = NULL;
	}
	if(ChildIn_Write != NULL) {
		CloseHandle(ChildIn_Write);
		ChildIn_Write = NULL;
	}
	if(ChildOut_Write != NULL) {
		CloseHandle(ChildOut_Write);
		ChildOut_Write = NULL;
	}
	if(ChildOut_Read != NULL) {
		CloseHandle(ChildOut_Read);
		ChildOut_Read = NULL;
		m_ChildOut_Read = NULL;
	}
	return;

}

void DeviceManager::adbShellTXEProcess(LPWSTR pStr) {

	CString str;
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL,path,MAX_PATH);
	CString p(path);
	int nPos = p.ReverseFind('\\');
	p = p.Left(nPos+1) + _T("fastboot.exe");


    // one way to start process, but can read return result of process via pipe
	// create pipe
	SECURITY_ATTRIBUTES securityAttr = {0};
	securityAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttr.bInheritHandle = TRUE;
	securityAttr.lpSecurityDescriptor = NULL;

	HANDLE ChildIn_Read, ChildIn_Write;
	if (!CreatePipe(&ChildIn_Read, &ChildIn_Write, &securityAttr, 0)) {
		MessageBoxA(NULL, "Error CreatePipe 1", "shell", MB_OK);
	}

	SetHandleInformation(ChildIn_Write, HANDLE_FLAG_INHERIT, 0);
	HANDLE ChildOut_Read, ChildOut_Write;

	if (!CreatePipe(&ChildOut_Read, &ChildOut_Write, &securityAttr, 0)) {
		MessageBoxA(NULL, "Error CreatePipe 2", "shell", MB_OK);
	}
	SetHandleInformation(ChildOut_Read, HANDLE_FLAG_INHERIT, 0);


	DWORD dwThreadID = 0;
	HANDLE thread2 = CreateThread(NULL, 0, TXEThreadRorProcess,(void*)this, 0, &dwThreadID); 
	
	// create procss
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo) ; //Only compulsory field
	StartupInfo.hStdError = ChildOut_Write;
	StartupInfo.hStdOutput = ChildOut_Write;
	StartupInfo.hStdInput = ChildIn_Read;

	StartupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	LPWSTR filePath = (LPWSTR)p.GetBuffer();
	LPWSTR out;

	out = pStr;

	p.ReleaseBuffer();
    m_ChildOut_Read = ChildOut_Read ;

	if(CreateProcess(filePath, out, NULL, NULL, TRUE, NULL, NULL, NULL, &StartupInfo, &ProcessInfo)) {
		int ret = WaitForSingleObject(ProcessInfo.hProcess,120000);
		if(ret == WAIT_TIMEOUT) {

	        str.Format(_T("%d"), ret);
			::TerminateProcess(ProcessInfo.hProcess, 4 );
	    }
		
	/*	DWORD BytesRead2 = 0;
		int ret = WaitForSingleObject(ProcessInfo.hProcess, 2000);
		char* writeBuffer = "y\r\n";
		int runCount = 0;
		while(ret == WAIT_TIMEOUT) {
			if(runCount > 6) {
				::TerminateProcess(ProcessInfo.hProcess, 4 );
				break;
			}

			BOOL l = WriteFile(ChildIn_Write, writeBuffer, strlen(writeBuffer), &BytesRead2, NULL);
			ret = WaitForSingleObject(ProcessInfo.hProcess, 1000);
			runCount++;
		}*/
		
		DWORD BytesRead = 0;
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
		char* buffer = "ExitThread";
		BOOL flag = WriteFile(ChildOut_Write, buffer, strlen(buffer), &BytesRead, NULL);
	} else {
		
    }
	int ret = WaitForSingleObject(thread2, 2000);
	if(ret == WAIT_TIMEOUT) {
		::TerminateThread(thread2, 4);
		CloseHandle(thread2);
	}
	if(ChildIn_Read != NULL) {
		CloseHandle(ChildIn_Read);
		ChildIn_Read = NULL;
	}
	if(ChildIn_Write != NULL) {
		CloseHandle(ChildIn_Write);
		ChildIn_Write = NULL;
	}
	if(ChildOut_Write != NULL) {
		CloseHandle(ChildOut_Write);
		ChildOut_Write = NULL;
	}
	if(ChildOut_Read != NULL) {
		CloseHandle(ChildOut_Read);
		ChildOut_Read = NULL;
		m_ChildOut_Read = NULL;
	}
	
	return;

}


DWORD WINAPI DeviceManager::TXEThreadRorProcess (LPVOID lpParam) {
	DeviceManager *pdm = (DeviceManager *)lpParam;
	//OUTPUT
	int runCount = 0;
	while(1) {
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead = 0;
		BOOL flag = ReadFile(pdm->m_ChildOut_Read, buffer, BUFSIZE, &BytesRead, NULL);
		CString buf(buffer);
		//AfxMessageBox(_T(" need to start server  buf ")+buf, MB_OK| MB_ICONINFORMATION);
		//fail
		if (-1 != buf.Find(_T("error")) || -1 != buf.Find(_T("FAILED")) || -1 != buf.Find(_T("Error")) || -1 != buf.Find(_T("Failed")) || -1 != buf.Find(_T("cannot")) || -1 != buf.Find(_T("unsupported"))  || -1 != buf.Find(_T("Unsupported"))  || -1 != buf.Find(_T("No such file")) || -1 != buf.Find(_T("not found")) || -1 !=buf.Find(_T("Permission denied"))) {
			pdm->m_Scuccess = FALSE; 
			break; 

		}	
		if(-1 != buf.Find(_T("KB/s")) || -1 != buf.Find(_T("Successful")) ||  -1 != buf.Find(_T("Passed")) || -1 != buf.Find(_T("...successful"))) { 
			pdm->m_Scuccess = TRUE;
			break;
		}
		if(-1 != buf.Find(_T("ExitThread"))) {//AfxMessageBox(_T(" need to start server  buf ")+buf, MB_OK| MB_ICONINFORMATION);
			if(runCount == 0) {
				pdm->m_Scuccess = TRUE;
			} else {
			}
			break;
		} else {
		}
		if (!flag || (BytesRead == 0)) { 
			break; 
		}
		
		runCount++;
	}
	return 100;

}

void DeviceManager::adbShellTXE(LPCWSTR cmd)
{
	m_Scuccess = TRUE;
	adbShellTXEProcess((LPWSTR)cmd);	
}