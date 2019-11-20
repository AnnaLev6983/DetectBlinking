
// DetectBlinkingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DetectBlinking.h"
#include "DetectBlinkingDlg.h"
#include "afxdialogex.h"
#include <fcntl.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString strcode[][2] =		{{"a","1211"},
							{ "b","1121" },
							{ "c","1112" },
							{ "d","1234" },
							{ "e","1243" },
							{ "f","1324" },
							{ "g","1342" },
							{ "h","1432" },
							{ "i","1423" },
							{ "j","2211" },
							{ "k","2121" },
							{ "l","2112" },
							{ "m","2234" },
							{ "n","2243" },
							{ "n","2243" },
							{ "o","2324" },
							{ "p","2342" },
							{ "q","2432" },
							{ "r","3211" },
							{ "s","3121" },
							{ "t","3112" },
							{ "u","3234" },
							{ "v","3243" },
							{ "w","3324" },
							{ "x","3342" },
							{ "y","3432" },
							{ "z","3423" } };

std::string strSignalBuff;
unsigned int nCntOfZero;

// CDetectBlinkingDlg dialog

CDetectBlinkingDlg::CDetectBlinkingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DETECTBLINKING_DIALOG, pParent)
	, m_strFile(_T(""))
	, m_chkCamera(FALSE)
	, m_nCamNo(0)
	, m_nRateTime(0)
	, m_strOutput(_T(""))
	, m_nDelayTime(0)
	, m_strCode(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pGraph = NULL;
}

void CDetectBlinkingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strFile);
	DDX_Control(pDX, IDC_PROGRESSBAR, m_pBar);
	DDX_Check(pDX, IDC_CHECK1, m_chkCamera);
	DDX_Text(pDX, IDC_EDIT2, m_nCamNo);
	DDV_MinMaxInt(pDX, m_nCamNo, 0, 10);
	DDX_CBIndex(pDX, IDC_RATETIME, m_nRateTime);
	DDX_Control(pDX, IDC_RATETIME, m_cmbRateTime);
	DDX_Text(pDX, IDC_OUTPUT, m_strOutput);
	DDX_Control(pDX, IDC_DELAYTIME, m_cmbDelayTime);
	DDX_CBIndex(pDX, IDC_DELAYTIME, m_nDelayTime);
	DDX_Text(pDX, IDC_EDIT4, m_strCode);
}

BEGIN_MESSAGE_MAP(CDetectBlinkingDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CDetectBlinkingDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDetectBlinkingDlg::OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CDetectBlinkingDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CDetectBlinkingDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDCANCEL, &CDetectBlinkingDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK1, &CDetectBlinkingDlg::OnBnClickedCheck1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &CDetectBlinkingDlg::OnDeltaposSpin1)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_RATETIME, &CDetectBlinkingDlg::OnCbnSelchangeRatetime)
END_MESSAGE_MAP()


// CDetectBlinkingDlg message handlers

BOOL CDetectBlinkingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	{
		cv::namedWindow("INPUT", 0);
		cv::resizeWindow("INPUT", 480, 290);
		HWND hWnd = (HWND)cvGetWindowHandle("INPUT");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_PLAYWND)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);
	}

	{
		cv::namedWindow("MASK", 0);
		cv::resizeWindow("MASK", 480, 290);
		HWND hWnd = (HWND)cvGetWindowHandle("MASK");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_MASKWND)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);
	}

	{
		cv::namedWindow("BACK", 0);
		cv::resizeWindow("BACK", 480, 290);
		HWND hWnd = (HWND)cvGetWindowHandle("BACK");
		HWND hParent = ::GetParent(hWnd);
		::SetParent(hWnd, GetDlgItem(IDC_BACKWND)->m_hWnd);
		::ShowWindow(hParent, SW_HIDE);
	}

	CString str;

	for (int i = 0; i < 27; i++)
	{
		str += strcode[i][0];
		str += "\t";
		str += strcode[i][1];
		str += "\t";

		if(i % 2 == 1)
			str += "\r\n";
	}

	GetDlgItem(IDC_EDIT5)->SetWindowText(str);

	m_cmbDelayTime.AddString("0ms");
	m_cmbDelayTime.AddString("5ms");
	m_cmbDelayTime.AddString("10ms");
	m_cmbDelayTime.AddString("15ms");
	m_nDelayTime = 1;

	m_cmbRateTime.AddString("100ms");
	m_cmbRateTime.AddString("500ms");
	m_cmbRateTime.AddString("1s");

	m_nRateTime = 2;
	UpdateData(0);
	OnCbnSelchangeRatetime();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDetectBlinkingDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDetectBlinkingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDetectBlinkingDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
	m_chkCamera = !m_chkCamera;

	GetDlgItem(IDC_SPIN1)->EnableWindow(m_chkCamera);
	GetDlgItem(IDOK)->EnableWindow(!m_chkCamera);
}


void CDetectBlinkingDlg::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	m_nCamNo -= pNMUpDown->iDelta;
	if (m_nCamNo < 0)
		m_nCamNo = 0;
	if (m_nCamNo > 9)
		m_nCamNo = 9;
	UpdateData(0);
	*pResult = 0;
}


void CDetectBlinkingDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	delete m_pGraph;
}


void CDetectBlinkingDlg::OnCbnSelchangeRatetime()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if (m_pGraph)
		delete m_pGraph;
	m_pGraph = new CChart();
	m_pGraph->Create(WS_VISIBLE | WS_CHILD, CRect(0, 0, 485, 140), GetDlgItem(IDC_GRAPH), 12000);

	m_pGraph->SetChartTitle("Serial Signal (x : frame)");
	m_pGraph->SetChartLabel("", "On/Off");
	m_pGraph->m_BGColor = RGB(0, 0, 0);
	m_pGraph->m_GridColor = RGB(0, 128, 0);
	m_pGraph->SetAxisStyle(0);
	m_pGraph->nSerieCount = 2;

	m_pGraph->mpSerie[0].m_plotColor = RGB(255, 0, 0);
	m_pGraph->mpSerie[1].m_plotColor = RGB(0, 0, 255);

	switch (m_nRateTime)
	{
	case 0:	// 100ms
		m_pGraph->AllocSerie(60);

		m_pGraph->SetRange(0, 60, 0, 2);
		m_pGraph->SetGridXYNumber(20, 1);
		break;
	case 1:	// 500ms
		m_pGraph->AllocSerie(300);

		m_pGraph->SetRange(0, 300, 0, 2);
		m_pGraph->SetGridXYNumber(20, 1);
		break;
	default: // 1000ms
		m_pGraph->AllocSerie(600);

		m_pGraph->SetRange(0, 600, 0, 2);
		m_pGraph->SetGridXYNumber(20, 1);
		break;
	}
}


void CDetectBlinkingDlg::DoEvents()
{
	MSG msg;
	BOOL result;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		result = ::GetMessage(&msg, NULL, 0, 0);
		if (result == 0) // WM_QUIT
		{
			::PostQuitMessage(msg.wParam);
			break;
		}
		else if (result == -1)
		{
			// Handle errors/exit application, etc.
		}
		else
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}
}

void CDetectBlinkingDlg::OnBnClickedButtonStart()
{
	// TODO: Add your control notification handler code here

	UpdateData();

	if (m_chkCamera)
	{
		capture.open(m_nCamNo);
		if (!capture.isOpened()) {
			AfxMessageBox(_T("Error opening video stream"), MB_OK | MB_ICONERROR);
			return;
		}
	}
	else
	{
		capture.open((string)m_strFile);
		if (!capture.isOpened()) {
			AfxMessageBox(_T("Error opening video file"), MB_OK | MB_ICONERROR);
			return;
		}
	}

	m_pBar.SetRange32(0, (int)capture.get(CV_CAP_PROP_FRAME_COUNT));

	GetDlgItem(IDC_BUTTON_START)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow();
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow();
	GetDlgItem(IDC_SPIN1)->EnableWindow(false);
	GetDlgItem(IDOK)->EnableWindow(false);
	GetDlgItem(IDCANCEL)->EnableWindow(false);
	m_cmbDelayTime.EnableWindow(FALSE);
	m_cmbRateTime.EnableWindow(FALSE);

	Play();
}


void CDetectBlinkingDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	m_nPlayStatus = 0;
	m_pBar.SetPos(0);

	GetDlgItem(IDC_BUTTON_START)->EnableWindow();
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(false);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(false);
	GetDlgItem(IDC_SPIN1)->EnableWindow(m_chkCamera);
	GetDlgItem(IDOK)->EnableWindow(!m_chkCamera);
	GetDlgItem(IDCANCEL)->EnableWindow();
	m_cmbDelayTime.EnableWindow();
	m_cmbRateTime.EnableWindow();
}


void CDetectBlinkingDlg::OnBnClickedButtonPause()
{
	// TODO: Add your control notification handler code here
	if (m_nPlayStatus != 2)
	{
		m_nPlayStatus = 2;
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText("Continue");
	}
	else
	{
		m_nPlayStatus = 1;
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText("Pause");
	}
}

void CDetectBlinkingDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CDetectBlinkingDlg::OnBnClickedOk()
{
	TCHAR szFilters[] = _T("Video Files (*.mp4)|*.mp4|All Files (*.*)|*.*||");

	CFileDialog fileDlg(TRUE, _T("Open Video file"), _T("*.mp4"),
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (fileDlg.DoModal() == IDOK)
	{
		m_strFile = fileDlg.GetPathName();

		UpdateData(0);
	}
}

void CDetectBlinkingDlg::Play()
{
	m_nPlayStatus = 1;
	imgBack.release();
	m_strOutput = "";

	nCapFPS = (int)capture.get(CV_CAP_PROP_FPS);
	//int delay = 1000 / nCapFPS;

	switch (m_nRateTime)
	{
	case 0: // 100ms  : 0'scount of break time is ...  fps / 10, 30fps->3
		nCntOfZero = (int)ceil(nCapFPS / 10);
		break;
	case 1: // 500ms;  : 0'scount of break time is ...  fps / 2, 30fps->15
		nCntOfZero = (int)ceil(nCapFPS / 2) - 6; // delta 6->200ms
		break;
	default: // 1000ms;  : 0'scount of break time is ...  fps / 1, 30fps->30
		nCntOfZero = nCapFPS - 12; // delta 6->400ms
		break;
	}

	float nCurFPS = 0;

	Mat imgInput;

	capture.read(imgInput);

	int nCurFrameIndex = 1;
	int nOldFrameIndex = 0;

	clock_t nCurFPSTime = clock();

	int nCntFrame = 0;
	clock_t startTime;

	// signal detect parameters
	bStartSiganl = false;
	nStartFrame = 0;
	m_pGraph->ClearChart();

	while (1)
	{
		startTime = clock();

		if (m_nPlayStatus == 2)
		{
			nCurFPSTime = clock();
			nCntFrame = 0;

			DoEvents();
			continue;
		}

		if (!m_nPlayStatus || imgInput.empty())
			break;

		nCurFrameIndex = (int)capture.get(CV_CAP_PROP_POS_FRAMES);
		m_pBar.SetPos(nCurFrameIndex);

		if (nCurFrameIndex != nOldFrameIndex)
		{
			nOldFrameIndex = nCurFrameIndex;
			CheckSignal(nCurFrameIndex, DetectBlink(imgInput));
		}

		DoEvents();

		int nDifTime = clock() - nCurFPSTime;

		if (nDifTime >= 1000)
		{
			nCurFPS = (nCntFrame) * 1000.0f / nDifTime;

			nCurFPSTime = clock();
			nCntFrame = 0;
		}

		putText(imgInput, std::_Floating_to_string("FPS: %.1f", nCurFPS), cvPoint(10, 30), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 255, 255), 2);
		imshow("INPUT", imgInput);

		capture.read(imgInput);
		nCntFrame++;

		startTime = clock();
		while (clock() - startTime < m_nDelayTime * 5) {
			waitKey(1);
			DoEvents();
		}
		/*while (clock() - startTime <= delay) {
			waitKey(1);
			DoEvents();
		}*/
	}

	OnBnClickedButtonStop();
}

size_t posTemp;

char CDetectBlinkingDlg::SignalToStr(BOOL nCntDetect)
{
	strSignalBuff += nCntDetect ? "1" : "0";

	if (strSignalBuff.length() < nCntOfZero)
		return 0;

	// In future must insert strSignalBuff filter's function   Ex: 00010000 -> 00000000


	std::string strbreak;
	
	for (int i = 0; i < nCntOfZero; i++)
		strbreak += "0";

	size_t pos = 0;
	int atFrames[4];
	int ncnt = 0;
	
	while (pos < strSignalBuff.length() && (pos = strSignalBuff.find(strbreak, pos)) != std::string::npos)
	{
		atFrames[ncnt] = pos;
		if (++ncnt >= 4)
			break;
		pos += nCntOfZero;
		pos = strSignalBuff.find("1", pos);
	}

	if (ncnt == 4)
	{
		posTemp = pos;

		atFrames[1] = atFrames[1] - 1 - strSignalBuff.find_last_of("0", atFrames[1] - 1);
		atFrames[2] = atFrames[2] - 1 - strSignalBuff.find_last_of("0", atFrames[2] - 1);
		atFrames[3] = atFrames[3] - 1 - strSignalBuff.find_last_of("0", atFrames[3] - 1);

		CString curcode;

		for (int i = 0; i < 4; i++)
		{
			atFrames[i] = (int)round((float)atFrames[i] / nCapFPS);
			CString str;
			str.Format("%d", atFrames[i]);
			curcode += str;
		}

		m_strCode = curcode;

		for (int i = 0; i < 27; i++)
		{
			if (strcode[i][1] == curcode)
			{
				m_strOutput += strcode[i][0];
				m_strOutput += "\t";
				m_strOutput += curcode;
				m_strOutput += "\r\n";

				return strcode[i][0].GetAt(0);
			}
		}

		m_strOutput += "?";
		m_strOutput += "\t";
		m_strOutput += curcode;
		m_strOutput += "\r\n";

		return '?';
	}

	strbreak = "";

	for (int i = 0; i < nCntOfZero * 2; i++)
		strbreak += "0";

	pos = strSignalBuff.find(strbreak);	// word break;

	if (pos != std::string::npos)
	{
		posTemp = pos;
		return '~';
	}

	return 0;
}

int CDetectBlinkingDlg::CheckSignal(int nCurFrameIndex, BOOL nCntDetect)
{
	if (!bStartSiganl && nCntDetect)
	{
		bStartSiganl = TRUE;
		nStartFrame = nCurFrameIndex;

		strSignalBuff = "";
	}

	if (bStartSiganl)
	{
		char ch = SignalToStr(nCntDetect);

		UpdateData(0);

		if (ch == '~' || ch != 0) // detect string
		{
			bStartSiganl = FALSE;
		}

		if (bStartSiganl == FALSE)
		{
			int nDelta = strSignalBuff.length() - posTemp;

			for (int i = m_pGraph->nPlotIndex[1] - nDelta - 1, nDelta = m_pGraph->nPlotIndex[1] - 1; i < nDelta; i++)
				m_pGraph->SetXYValue(i, 1.2f, i, 1);
		}
	}
	
	{
		m_pGraph->AddYValue(nCntDetect > 0 ? 1 : 0, 0);
		m_pGraph->AddYValue(bStartSiganl > 0 ? 1.7f : 1.2f, 1);
		m_pGraph->Invalidate();
	}

	return 0;
}

int CDetectBlinkingDlg::DetectBlink(Mat &img)
{
	Mat imgGray, imgSrc;
	int count = 0;

	resize(img, imgSrc, Size(320, img.rows * 320 / img.cols));
	cvtColor(imgSrc, imgGray, CV_BGR2GRAY);
	GaussianBlur(imgGray, imgGray, Size(5, 5), 2, 2);
	//medianBlur(imgGray, imgGray, 3);

	if (!imgBack.empty())
	{
		Mat imgMask, imgDst;

		absdiff(imgGray, imgBack, imgMask);
		threshold(imgMask, imgMask, 30, 255, CV_THRESH_BINARY);
		dilate(imgMask, imgMask, Mat(), Point(-1, -1), 2, 1, 1);

		vector< vector <Point> > contours; // Vector for storing contour
		vector< Vec4i > hierarchy;

		imgDst = imgMask.clone();
		findContours(imgDst, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Find the contours in the image

		for (unsigned int i = 0; i < contours.size(); i = hierarchy[i][0]) // iterate through each contour.
		{
			Rect r = boundingRect(contours[i]);
			
			if (hierarchy[i][2] < 0) 
			{
				float fRate = r.width * 1.0f / r.height;

				if (fRate < 0.8 || fRate > 1.2)
					continue;

				if (r.width < 5 || r.height < 5)
					continue;

				float cx = r.width * 0.3f;
				float cy = r.height * 0.3f;

				r.x += (int)(cx / 2);
				r.y += (int)(cy / 2);

				r.width -= (int)cx;
				r.height -= (int)cy;

				Mat sub = imgMask(r);
				
				int meanval = (int)mean(sub).val[0];

				if (meanval < 250)
					continue;

				//putText(imgMask, std::to_string(meanval), cvPoint(10, 50), FONT_HERSHEY_SIMPLEX, 1, cvScalar(255, 255, 255), 2);

				rectangle(imgMask, Point(r.x, r.y), Point(r.x + r.width, r.y + r.height), Scalar(128, 128, 128), 2, 2, 0);
				count++;
			}
		}

		/*vector<Vec3f> circles;
		HoughCircles(imgMask, circles, CV_HOUGH_GRADIENT,
			2, imgMask.rows / 2, 30, 30, 5);

		for (size_t i = 0; i < circles.size(); i++)
		{
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);
			circle(imgMask, center, radius, Scalar(128, 0, 128), 2, 2, 0);
		}*/

		imshow("MASK", imgMask); 
	}

	if (/*count == 0 || */imgBack.empty())
	{
		imgBack.release();
		imgBack = imgGray.clone();
		imshow("BACK", imgBack);
	}

	return count;
}
