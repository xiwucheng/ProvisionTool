// ProvisionToolDlg.h : header file
//
#include <dbt.h>
#pragma once


// CProvisionToolDlg dialog
class CProvisionToolDlg : public CDialog
{
// Construction
public:
	CProvisionToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROVISIONTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	HDEVNOTIFY m_hDevNotify;
	BOOL m_bConnected, m_bRun;
	UINT m_nRunIndex;
	HBITMAP m_hMountBitmap,m_hDeleteBitmap, m_hSuccessBitmap, m_hFailureBitmap, m_hNABitmap;
	CString m_szPathName;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	int Connect_device(void);
	int System(LPCWSTR cmd, LPCWSTR param, int nShow, BOOL isInfinite);
	int BuildSN(CString sn);
	int WriteSN(int len);
	int WriteKeyBox(void);
	int BuildKeyBox(CString filePath);
	BYTE* HexStr2BinBuf(const char* hex_str, size_t* buf_size);
	size_t HexStr2Int(const char* hex_str, size_t length);
	char* String2MultiChar(CString& str);
protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedRead();
	void AddLog(CString);
	void ReadDevice(void);
	int WriteDevice(void);
	static UINT DoThread(LPVOID lParam);
};
