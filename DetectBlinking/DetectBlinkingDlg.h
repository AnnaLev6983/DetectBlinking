
// DetectBlinkingDlg.h : header file
//

#pragma once

#include "afxcmn.h"
#include "afxwin.h"

#include "Chart.h"
#include "opencv2/core/core.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

// CDetectBlinkingDlg dialog
class CDetectBlinkingDlg : public CDialogEx
{
// Construction
public:
	CDetectBlinkingDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DETECTBLINKING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CString m_strFile;

	void Play();
	void DoEvents(); 
	int DetectBlink(Mat &);
	int CheckSignal(int nFrame, BOOL nCntDetect);
	char SignalToStr(BOOL nCntDetect);

	afx_msg void OnBnClickedButtonStart();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonPause();
	afx_msg void OnBnClickedCancel();

	CProgressCtrl m_pBar;
	CString m_strFps;
	BOOL m_chkCamera;
	int m_nCamNo;
	int m_nPlayStatus;


	VideoCapture capture;
	int nCapFPS;
	Mat imgBack;


	// signal detect parameters
	CChart* m_pGraph;
	BOOL bStartSiganl;
	int nStartFrame;

	afx_msg void OnBnClickedCheck1();
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	int m_nRateTime;
	CComboBox m_cmbRateTime;
	afx_msg void OnCbnSelchangeRatetime();
	CString m_strOutput;
	CComboBox m_cmbDelayTime;
	int m_nDelayTime;
	CString m_strCode;
};
