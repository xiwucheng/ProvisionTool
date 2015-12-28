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



CDeviceManager::CDeviceManager()
{
	InitializeCriticalSection(&m_cs);
}

CDeviceManager::~CDeviceManager(void)
{
	EnterCriticalSection(&m_cs);
	LeaveCriticalSection(&m_cs);
	m_list.RemoveAll();
	DeleteCriticalSection(&m_cs);
}


BOOL CDeviceManager::hasAndroidDevices() {

	CString device_descriptor;
	HDEVINFO hDevInfoAndroid = NULL, hDevInfoGadget = NULL;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i = 0;

	LPTSTR buffer = NULL;
	DWORD buffersize = 0;
	DWORD DataT = 0;

	CStringList parents;

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
			goto __end;
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
__end:
	if (hDevInfoAndroid)
		SetupDiDestroyDeviceInfoList(hDevInfoAndroid);

	if (buffer) {
		LocalFree(buffer);
		buffer=NULL;
		buffersize = 0;
	}
	return TRUE;
}

CString CDeviceManager::getAndroidDeviceSerialNumber() {
	CString device_descriptor;
	HDEVINFO hDevInfoAndroid = NULL, hDevInfoGadget = NULL;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i = 0;

	LPTSTR buffer = NULL;
	DWORD buffersize = 0;
	DWORD DataT = 0;

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
		while (!SetupDiGetDeviceInstanceId(hDevInfoAndroid,&DeviceInfoData,buffer,buffersize,&buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buffer) LocalFree(buffer);
				buffer = (TCHAR *)LocalAlloc(LPTR,buffersize * 2);
			} else {
				break;
			}
		}

		CString tmpString(buffer);
		device_descriptor=tmpString.Mid(tmpString.ReverseFind('\\')+1);
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


CString CDeviceManager::getAndroidDeviceVendor()
{
	CString device_descriptor;
	HDEVINFO hDevInfoAndroid = NULL, hDevInfoGadget = NULL;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i = 0;

	LPTSTR buffer = NULL;
	DWORD buffersize = 0;
	DWORD DataT = 0;

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
		while (!SetupDiGetDeviceInstanceId(hDevInfoAndroid,&DeviceInfoData,buffer,buffersize,&buffersize))
		{
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buffer) LocalFree(buffer);
				buffer = (TCHAR *)LocalAlloc(LPTR,buffersize * 2);
			} else {
				break;
			}
		}

		CString tmpString(buffer);
		device_descriptor=tmpString.Mid(tmpString.Find('\\')+1);
		device_descriptor=device_descriptor.Left(device_descriptor.Find('&'));
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

CString CDeviceManager::getAndroidDevices() {

	CString device_descriptor;
	HDEVINFO hDevInfoAndroid = NULL, hDevInfoGadget = NULL;
	SP_DEVINFO_DATA DeviceInfoData;
	DWORD i = 0;

	LPTSTR buffer = NULL;
	DWORD buffersize = 0;
	DWORD DataT = 0;

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
				
			} 
			else 
			{
				adbProcess();
				Sleep(10);
				POSITION pos = m_list.GetHeadPosition();  
				while( pos!= NULL ) 
				{  
					CString line = m_list.GetNext(pos);
					if (-1 != line.Find(_T("daemon not running"))) {
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

void CDeviceManager::Split(const CString& src, CString delimit, int num, CString nullSubst) 
{ 
	ASSERT( !src.IsEmpty() && !delimit.IsEmpty() ); 
	if(num==1)   
	{ 
		m_list.AddTail(src);  
		return;  
	}  
	int deliLen = delimit.GetLength();  
	long index = -1, lastSearchPosition = 0, cnt = 0;  
	while( (index=src.Find(delimit, lastSearchPosition))!=-1 )  
	{  
		if(index==lastSearchPosition)  
			m_list.AddTail(nullSubst); 
		else 
			m_list.AddTail(src.Mid(lastSearchPosition, index-lastSearchPosition));  
		lastSearchPosition = index + deliLen; 
		if(num) 
		{     
			++cnt;  
			if(cnt+1==num) break;  
		}  
	}  
	CString lastOne = src.Mid(lastSearchPosition);
	m_list.AddTail( lastOne.IsEmpty()? nullSubst:lastOne);
}  


BOOL CDeviceManager::adbProcess() 
{
	return RemoteCmd(_T("fastboot.exe"), _T(" devices"));
}


BOOL CDeviceManager::RemoteCmd(LPTSTR lpCmd, LPTSTR lpCmdline) {

	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL,path,MAX_PATH);
	CString p(path);
	int nPos = p.ReverseFind('\\');
	p = p.Left(nPos);
	BOOL bRetVal = FALSE;
	DWORD exCode = 0xFFFFFFFF;

	EnterCriticalSection(&m_cs);//mutex for preventing recurse

	PROCESS_INFORMATION pi={0};
	STARTUPINFO si={0};
	SECURITY_ATTRIBUTES sa={0};
	HANDLE hReadOutPipe,hWriteOutPipe;
	HANDLE hReadInPipe,hWriteInPipe;
	sa.bInheritHandle=TRUE;
	sa.nLength=sizeof (SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor=NULL;

	if (!CreatePipe(&hReadInPipe,&hWriteInPipe,&sa,0))
	{
		goto __end;
	}
	SetHandleInformation(hWriteInPipe, HANDLE_FLAG_INHERIT, 0);//communication with console, like kbd input.

	if (!CreatePipe(&hReadOutPipe,&hWriteOutPipe,&sa,0))
	{
		goto __end;
	}
	SetHandleInformation(hReadOutPipe, HANDLE_FLAG_INHERIT, 0);//communication with console, read from process via createprocess.

	si.cb=sizeof (STARTUPINFO);
	si.wShowWindow=SW_HIDE;
	si.dwFlags=STARTF_USESHOWWINDOW|STARTF_USESTDHANDLES;
	si.hStdOutput = si.hStdError = hWriteOutPipe;
	si.hStdInput = hReadInPipe;
    m_hReadOutPipe = hReadOutPipe;

	m_hThread = CreateThread(NULL, 0, ThreadProcForCmd,(void*)this, 0, NULL); 

	LPWSTR filePath = (LPWSTR)p.GetBuffer();
	p.ReleaseBuffer();

	if(CreateProcess(lpCmd, lpCmdline, 0, 0, TRUE, 0, 0, filePath, &si, &pi)) 
	{
		int ret = WaitForSingleObject(pi.hProcess,5000);
		if(ret == WAIT_TIMEOUT)
		{
			TerminateProcess(pi.hProcess, 4);
	    }

		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		char* buffer = "ExitThread";
		DWORD BytesRead = 0;
		BOOL flag = WriteFile(hWriteOutPipe, buffer, (DWORD)strlen(buffer), &BytesRead, NULL);

	} 
	else
	{
		goto __end;
    }

	int ret = WaitForSingleObject(m_hThread, 2000);
	if(ret == WAIT_TIMEOUT) 
	{
		TerminateThread(m_hThread, 5);
		CloseHandle(m_hThread);
	}
	GetExitCodeThread(m_hThread,&exCode);
	bRetVal = (exCode == 0);

__end:
	if(hWriteOutPipe != NULL) 
	{
		CloseHandle(hWriteOutPipe);
		hWriteOutPipe = NULL;
	}
	if(hReadOutPipe != NULL) 
	{
		CloseHandle(hReadOutPipe);
		m_hReadOutPipe = hReadOutPipe = NULL;
	}
	if(hWriteInPipe != NULL) 
	{
		CloseHandle(hWriteInPipe);
		hWriteInPipe = NULL;
	}
	if(hReadInPipe != NULL)
	{
		CloseHandle(hReadInPipe);
		hReadInPipe = NULL;
	}

	LeaveCriticalSection(&m_cs);

	return bRetVal;

}

DWORD WINAPI CDeviceManager::ThreadProcForCmd (LPVOID lpParam) {
	CDeviceManager *pdm = (CDeviceManager *)lpParam;
	DWORD dwRetVal = 0xFFFFFFFF;
	pdm->m_list.RemoveAll();
	pdm->m_Result = _T("");
	//OUTPUT
	while(1) 
	{
		char buffer[BUFSIZE] = {0};
		DWORD BytesRead = 0;;
		BOOL flag = ReadFile(pdm->m_hReadOutPipe, buffer, BUFSIZE, &BytesRead, NULL);

		CString buf(buffer);
		//sucess
		pdm->Split(buf, _T("\n"), 0, _T("<null>"));
		
		if (-1 != buf.Find(_T("usage")))
		{
			dwRetVal = 0xFFFFFFFF;
			break;
		}
		else if (-1 != buf.Find(_T("error")) || -1 != buf.Find(_T("FAILED")) || -1 != buf.Find(_T("Error")) || -1 != buf.Find(_T("Failed")) || -1 != buf.Find(_T("cannot")) || -1 != buf.Find(_T("unsupported"))  || -1 != buf.Find(_T("Unsupported"))  || -1 != buf.Find(_T("No such file")) || -1 != buf.Find(_T("not found")) || -1 !=buf.Find(_T("Permission denied"))) 
		{
			pdm->m_Result += buf;
			dwRetVal = 0xFFFFFFFF;
			break; 
		}	
		else if(-1 != buf.Find(_T("finished")) || -1 != buf.Find(_T("KB/s")) || -1 != buf.Find(_T("Successful")) ||  -1 != buf.Find(_T("Passed")) || -1 != buf.Find(_T("...successful")))
		{ 
			pdm->m_Result += buf;
			dwRetVal = 0;
			break;
		}
		else if (-1 != buf.Find(_T("fastboot"))) 
		{ 
			pdm->m_Result += buf;
			dwRetVal = 0;
			break;
		}
		else if(-1 != buf.Find(_T("ExitThread"))) 
		{
			break;
		} 
		else if (!flag || (BytesRead == 0)) 
		{ 
			break; 
		}
		else
		{
			pdm->m_Result += buf;
		}
	}
	return dwRetVal;

}
