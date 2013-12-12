// FollowMEDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FollowME.h"
#include "FollowMEDlg.h"
#include "Cvvimage.h"
#include "math.h"
#include <windows.h> 
#define PI 3.14159265
#define SQRT2 1.414

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NO_CONTROL -32768
#define M_PWM 0
#define M_POSITION 1
#define M_VELOCITY 2
#define cFULL_COUNT 32767
#define cWHOLE_RANGE 1200

#define TIMER_FRAME 1

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CFollowMEDlg dialog




CFollowMEDlg::CFollowMEDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFollowMEDlg::IDD, pParent)
	, is_connected(false)
	, m_ir0(0)
	, m_ir1(0)
	, m_ir2(0)
	, m_ir3(0)
	, m_ir4(0)
	, m_ir5(0)
	, m_ir6(0)
	, m_sonar0(0)
	, m_sonar1(0)
	, m_sonar2(0)
	, m_motion_direction(0)
	, m_encoder0(0)
	, m_encoder1(0)
	, m_speed(0)
	, m_direction(0)
	, pedestrain_thread_param(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFollowMEDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VITAMINCTRL1, m_VitCtrl);
	DDX_Control(pDX, IDC_BUTTON_CONNECT, m_connect);
	DDX_Control(pDX, IDC_EDIT_USER, m_user);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_password);
	DDX_Control(pDX, IDC_EDIT_PORT, m_port);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Control(pDX, IDC_DRROBOTSDKCONTROLCTRL1, m_MOTSDK);
	DDX_Text(pDX, IDC_EDIT_IR0, m_ir0);
	DDX_Text(pDX, IDC_EDIT_IR1, m_ir1);
	DDX_Text(pDX, IDC_EDIT_IR2, m_ir2);
	DDX_Text(pDX, IDC_EDIT_IR3, m_ir3);
	DDX_Text(pDX, IDC_EDIT_IR4, m_ir4);
	DDX_Text(pDX, IDC_EDIT_IR5, m_ir5);
	DDX_Text(pDX, IDC_EDIT_IR6, m_ir6);
	DDX_Text(pDX, IDC_EDIT_SONAR0, m_sonar0);
	DDX_Text(pDX, IDC_EDIT_SONAR1, m_sonar1);
	DDX_Text(pDX, IDC_EDIT_SONAR2, m_sonar2);
	DDX_Control(pDX, IDC_SLIDER_SPEED, m_slider_speed);
	DDX_Control(pDX, IDC_SLIDER_DIRECTION, m_slider_direction);
	DDX_Control(pDX, IDC_EDIT_DIRECTION, m_edit_speed);
	DDX_Control(pDX, IDC_EDIT_SPEED, m_edit_direction);
}

BEGIN_MESSAGE_MAP(CFollowMEDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CFollowMEDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDOK, &CFollowMEDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_FORWARD, &CFollowMEDlg::OnBnClickedButtonForward)
	ON_BN_CLICKED(IDC_BUTTON_LEFT, &CFollowMEDlg::OnBnClickedButtonLeft)
	ON_BN_CLICKED(IDC_BUTTON_RIGHT, &CFollowMEDlg::OnBnClickedButtonRight)
	ON_BN_CLICKED(IDC_BUTTON_BACKWARD, &CFollowMEDlg::OnBnClickedButtonBackward)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CFollowMEDlg::OnBnClickedButtonStop)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_MOVE, &CFollowMEDlg::OnBnClickedButtonMove)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_STN_CLICKED(IDC_Image_View, &CFollowMEDlg::OnStnClickedImageView)
END_MESSAGE_MAP()


// CFollowMEDlg message handlers

BOOL CFollowMEDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// set up the camera configurations
	is_connected=false;
	m_user.SetWindowTextA("root");
	m_password.SetWindowTextA("drrobot");
	m_port.SetWindowTextA("8081");
	m_ip.SetWindowTextA("192.168.0.199");
	ConnectCamera();

	// set the robot and robot parameters
	m_MOTSDK.connectRobot ("DrRobotMotion");
	m_slider_speed.SetRange(0, 100);
	m_speed=10;
	m_slider_speed.SetPos(m_speed);
	char str[80];
	sprintf(str, "%d", m_speed);
	m_edit_speed.SetWindowTextA(str);
	m_slider_direction.SetRange(0, 360);
	m_direction=0;
	m_slider_direction.SetPos(m_direction);
	sprintf(str, "%d", m_direction);
	m_edit_direction.SetWindowTextA(str);

	// for the human detector and tracker
	scanner=new DetectionScanner(HUMAN_height,HUMAN_width,HUMAN_xdiv,HUMAN_ydiv,256,0.8);
	LoadCascade(*scanner);
	pedestrain_thread_param=new PEDESTRAINTHREADPARAM;
	pedestrain_thread_param->scanner=scanner;
	pedestrain_thread_param->is_processing=false;
	pedestrain_thread_param->window=(void *)this;
	track_config=new Config("config.txt");
	pedestrain_thread_param->tracker=new Tracker(* track_config);
	pedestrain_thread_param->counter=0;
	pedestrain_thread_param->is_gesture=false;
	pedestrain_thread_param->is_tracking=false;
	pedestrain_thread_param->config=&config;
	pedestrain_thread_param->gesture=new GestureRecognition("classifier.xml.gz", "dictionary.yml");

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFollowMEDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CFollowMEDlg::OnPaint()
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
HCURSOR CFollowMEDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*
	start the processing of frames
*/
void CFollowMEDlg::OnBnClickedButtonConnect()
{
	// TODO: Add your control notification handler code here
	// stop the processing, if we already starts
	if (is_connected)
	{
		is_connected=false;
		m_connect.SetWindowTextA("Start");
		KillTimer(TIMER_FRAME);
		// stop the robot
		OnBnClickedButtonStop();
	}
	// start the processing, by start the timer
	else
	{
		is_connected=true;
		m_connect.SetWindowTextA("Stop");
		// start the timer, where we capture the frame and run pedestrain detection
		SetTimer(TIMER_FRAME, 100, NULL);
	}
}

/*
	exit the program
*/
void CFollowMEDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	m_VitCtrl.Disconnect();
	OnBnClickedButtonStop();
	OnOK();
}

/*
those functions control the camera location
*/
void CFollowMEDlg::OnBnClickedButtonForward()
{
	// TODO: Add your control notification handler code here
	m_VitCtrl.SendCameraCommand("up", 30000);
}

void CFollowMEDlg::OnBnClickedButtonLeft()
{
	// TODO: Add your control notification handler code here
	m_VitCtrl.SendCameraCommand("left", 30000);
}

void CFollowMEDlg::OnBnClickedButtonRight()
{
	// TODO: Add your control notification handler code here
	m_VitCtrl.SendCameraCommand("right", 30000);
}

void CFollowMEDlg::OnBnClickedButtonBackward()
{
	// TODO: Add your control notification handler code here
	m_VitCtrl.SendCameraCommand("down", 30000);
}

/*
	stop the movement of robots
*/
void CFollowMEDlg::OnBnClickedButtonStop()
{
	// TODO: Add your control notification handler code here
	m_MOTSDK.DisableDcMotor (0);
	m_MOTSDK.DisableDcMotor (1);
}
BEGIN_EVENTSINK_MAP(CFollowMEDlg, CDialog)
	ON_EVENT(CFollowMEDlg, IDC_DRROBOTSDKCONTROLCTRL1, 1, CFollowMEDlg::StandardSensorEventDrrobotsdkcontrolctrl1, VTS_NONE)
	ON_EVENT(CFollowMEDlg, IDC_DRROBOTSDKCONTROLCTRL1, 2, CFollowMEDlg::MotorSensorEventDrrobotsdkcontrolctrl1, VTS_NONE)
	ON_EVENT(CFollowMEDlg, IDC_DRROBOTSDKCONTROLCTRL1, 3, CFollowMEDlg::CustomSensorEventDrrobotsdkcontrolctrl1, VTS_NONE)
	ON_EVENT(CFollowMEDlg, IDC_VITAMINCTRL1, 1, CFollowMEDlg::OnClickVitaminctrl1, VTS_I4 VTS_I4)
END_EVENTSINK_MAP()

/*
	the infrad sensor, we haven't used it yet
*/
void CFollowMEDlg::StandardSensorEventDrrobotsdkcontrolctrl1()
{
	// TODO: Add your message handler code here
	m_sonar0= m_MOTSDK.GetSensorSonar1 ();
	m_sonar1= m_MOTSDK.GetSensorSonar2 ();
	m_sonar2= m_MOTSDK.GetSensorSonar3 ();

	m_ir0 = m_MOTSDK.GetSensorIRRange ();
	UpdateData(false);
}
/*
those sensors don't work properly
*/
void CFollowMEDlg::MotorSensorEventDrrobotsdkcontrolctrl1()
{
	// TODO: Add your message handler code here
	//m_ir1 = m_MOTSDK.GetCustomAD3 ();
	//m_ir2 = m_MOTSDK.GetCustomAD4 ();
	//m_ir3 = m_MOTSDK.GetCustomAD5 ();
	//m_ir4 = m_MOTSDK.GetCustomAD6 ();
	//m_ir5 = m_MOTSDK.GetCustomAD7 ();
	//m_ir6 = m_MOTSDK.GetCustomAD8 ();
	//UpdateData(false);
}

/*
	this function collects the location of wheels of the robot
*/
void CFollowMEDlg::CustomSensorEventDrrobotsdkcontrolctrl1()
{
	// TODO: Add your message handler code here
	m_encoder1 = m_MOTSDK.GetEncoderPulse2 ();
	m_encoder0 = m_MOTSDK.GetEncoderPulse1 ();
}

/*
	on each timer event, a new thread will be started to process the frame
	please refer to "PedestrainThreadFunction" for details of how we process the frame
*/
void CFollowMEDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	// check whether we need to grab a frame or not
	if (nIDEvent==TIMER_FRAME)
	{
		// we process a new frame, when the previous one is finished
		if (!pedestrain_thread_param->is_processing)
		{
			AfxBeginThread(PedestrainThreadFunction, pedestrain_thread_param);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

// // this function converts this VARIANT to IplImage
IplImage * CFollowMEDlg::Variant2IpplImage(VARIANT vData , VARIANT vInfo)
{
	unsigned int width, height;
	width=*((long *)vInfo.parray[0].pvData+2);
	height=*((long *)vInfo.parray[0].pvData+6);
	IplImage* img_tmp;
	img_tmp=cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	IplImage *frame=cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 3);
	memcpy(img_tmp->imageData, (unsigned char *)vData.parray[0].pvData+54, width*height*3);
	cvConvertImage(img_tmp, frame, CV_CVTIMG_FLIP);
	cvReleaseImage(&img_tmp);
	return frame;
}

// this function shows the IplImage in the picture control
void CFollowMEDlg::ShowImage(IplImage* img, UINT ID)
{
	// get the DC
	CDC* pDC = GetDlgItem( ID ) ->GetDC();
	// get HDC
	HDC hDC = pDC ->GetSafeHdc();				
 
	CRect rect;
	GetDlgItem(ID) ->GetClientRect( &rect );
	// for device size
	int rw = rect.right - rect.left;			
	int rh = rect.bottom - rect.top;
	// and the image size
	int iw = img->width;	
	int ih = img->height;
	// center the image
	int tx = (int)(rw - iw)/2;	
	int ty = (int)(rh - ih)/2;
	SetRect( rect, tx, ty, tx+iw, ty+ih );
	// copy the image
	CvvImage cimg;
	cimg.CopyOf( img );
	// start drawing
	cimg.DrawToHDC( hDC, &rect );	
 
	ReleaseDC( pDC );
}

// this function implements the robot moving function
// direction: 0~pi*2
// speed: 0~100
void CFollowMEDlg::RobotMove(int direction, int speed)
{
	// compute the speed for each wheel
	short left=(short) SQRT2*speed*10*sin(direction*PI/180+PI/4);
	short right=(short) SQRT2*speed*10*sin(direction*PI/180-PI/4);
	m_MOTSDK.SetDcMotorControlMode (0,M_VELOCITY);
	m_MOTSDK.SetDcMotorControlMode (1,M_VELOCITY);
	m_MOTSDK.SetDcMotorVelocityControlPID (0, 10, 3, 100);
	m_MOTSDK.SetDcMotorVelocityControlPID (1, 10, 3, 100);
	m_MOTSDK.DcMotorVelocityNonTimeCtrAll (left, right,NO_CONTROL,NO_CONTROL,NO_CONTROL,NO_CONTROL); 
}

/*
	control the movement of robot manually with specific speed and direction
	no time control is provided
*/
void CFollowMEDlg::OnBnClickedButtonMove()
{
	// TODO: Add your control notification handler code here
	// RobotMove(m_direction, m_speed);
	RobotMovePosition(m_speed, m_direction);
}

/*
	control the moving direction of robot
*/
void CFollowMEDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar->GetDlgCtrlID()==IDC_SLIDER_DIRECTION)
	{
		m_direction=m_slider_direction.GetPos();
		char str[80];
		sprintf(str, "%d", m_direction);
		m_edit_direction.SetWindowTextA(str);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

/*
	control the speed of the robot
*/
void CFollowMEDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (pScrollBar->GetDlgCtrlID()==IDC_SLIDER_SPEED)
	{
		m_speed=m_slider_speed.GetPos();
		char str[80];
		sprintf(str, "%d", m_speed);
		m_edit_speed.SetWindowTextA(str);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

// we will take a snapshot once the user have clicked the image view
void CFollowMEDlg::OnStnClickedImageView()
{
	// TODO: Add your control notification handler code here
	CTime theTime; 
	theTime=CTime::GetCurrentTime();
	m_VitCtrl.SaveSnapshot(2, theTime.Format("%Y-%M-%d-%H-%M-%S.bmp"));
}

// this function controls the behavior of when we click the camera
void CFollowMEDlg::OnClickVitaminctrl1(long lX, long lY)
{
	// TODO: Add your message handler code here
}

/*
this function implements pedstraint detection
V2: we use the state machine to handle the logic behind pedestrain detection/tracking and gesture recognition
Note, this function has the risk of memory leak, please check the commented out "cvReleaseImage()"
However, if uncomment this line, there will be run time error reported
*/
UINT PedestrainThreadFunction(LPVOID pParam)
{
	PEDESTRAINTHREADPARAM *param=(PEDESTRAINTHREADPARAM *)pParam;
	CFollowMEDlg *dlg=(CFollowMEDlg *)param->window;
	param->is_processing=true;

	// grab frame from the camera
	VARIANT vData, vInfo;
	if (dlg->m_VitCtrl.GetSnapshot(2, &vData, &vInfo)!=S_OK)
	{
		// failed
		param->is_processing=false;	
		return -1;
	}
	// get the data from the buffer
	// we need to check the structure of variant
	IplImage *frame=dlg->Variant2IpplImage(vData, vInfo);
	// release this buffer
	VariantClear(&vData);    // must release the buffer
	VariantClear(&vInfo);    // must release the buffer

	// perform the detection
	bool detected=false;
	std::vector<CPedestrainRect> results;
	if (param->counter==0)
	{
		// for the first frame of each cycle, we apply the detector
		results=DetectHuman(frame,*(param->scanner));
		// to intialize the model
		if (results.size()>0)
		{
			detected=true;
			// we already initialize before, just reset it
			if (param->tracker->IsInitialised())
			{
				param->tracker->Reset();
			}
			// we do an initialization
			param->tracker->Initialise(frame, FloatRect(results[0].left, results[0].top, results[0].right-results[0].left, results[0].bottom-results[0].top));
			param->counter=(param->counter+1)%param->config->detection_interval;
		}
	}
	// for tracker
	else if (param->tracker->IsInitialised())
	{
		// for most of time, we just run the tracker
		param->tracker->Track(frame);
		// convert the result to the compatible format;
		CPedestrainRect rect;
		FloatRect rect2=param->tracker->GetBB();
		rect.left=rect2.XMin();
		rect.top=rect2.YMin();
		rect.right=rect2.XMin()+rect2.Width();
		rect.bottom=rect2.YMin()+rect2.Height();
		results.push_back(rect);
		param->counter=(param->counter+1)%param->config->detection_interval;
	}

	if (results.size()>0)
	{
		// we haven't activated the gesture recognition component
		if (!param->is_gesture)
		{
			// compute the optical flow
			if (param->sequence.size()>1)
			{
				Mat frame_diff;
				absdiff(cv::Mat(param->sequence.front()), cv::Mat(frame), frame_diff);
				Scalar frame_diff_sum=sum(frame_diff);
				// ok the optical flow passes, we start the gesture recognition
				if (frame_diff_sum.val[0]>=param->config->diff_threshold)
				{
					param->is_gesture=true;
				}
				cvReleaseImage(& param->sequence.front());
				param->sequence.pop();
			}
			param->sequence.push(frame);
		}
		// we start the gesture recognition now
		else
		{
			// apply the gesture recognition
			if (param->sequence.size()>=param->config->sequence_length)
			{
				int num_positive=0;
				while(param->sequence.size()>0)
				{
					IplImage *image=param->sequence.front();
					param->sequence.pop();
					if (param->gesture->recognize(image)>0)
					{
						num_positive++;
					}
					cvReleaseImage(& image);
				}
				// alternate the tracking, if gesture is recognized
				if (num_positive>param->config->gesture_threshold)
				{
					param->is_tracking=param->is_tracking ^ true;
				}
				param->is_gesture=false;
			}
			else
			{
				param->sequence.push(CropWindow(frame, results));
			}
			// cvReleaseImage(& frame);
		}

		// send the command
		if (param->counter==1 && param->is_tracking)
		{
			dlg->TrackPedestrain(results, frame);
		}

		// save the result
		param->results.clear();
		for (int i=0; i<results.size(); i++)
		{
			param->results.push_back(results[i]);
		}

		// show the detection result
		if (detected)
		{
			for(unsigned int i=0;i<results.size();i++)
				cvRectangle(frame,cvPoint(results[i].left,results[i].top),cvPoint(results[i].right,results[i].bottom),CV_RGB(255,0,0),2);
		}
		else
		{
			for(unsigned int i=0;i<results.size();i++)
				cvRectangle(frame,cvPoint(results[i].left,results[i].top),cvPoint(results[i].right,results[i].bottom),CV_RGB(0,255,0),2);
		}
	}
	dlg->ShowImage(frame, IDC_Image_View);

	// cvReleaseImage(&frame);
	param->is_processing=false;	
	return 0;
}
// this function computes the distance and orientation required for the robot according to the tracking result
void CFollowMEDlg::TrackPedestrain(std::vector<CPedestrainRect> target, IplImage *frame)
{
	// we will utilize the location of the windows to decide the movement of the robot
	int width=frame->width;
	int height=frame->height;
	// currently we are only considering one pedestrain in view
	if (target.size()>0)
	{
		// we use the size of window to predict the distance (in 50 centimeter)
		double distance=59000.0/(target[0].right-target[0].left)/(target[0].bottom-target[0].top);
		// the direction is related to the deviation of window center to the frame center
		double deviation=(target[0].left+target[0].right-width)/75;
		int direction=(int)(atan(deviation/7.5)*180/PI);
		if (distance>=7.5 && distance<=8.5)
		{
			return;
		}
		// we only consider the boxes which is large enough
		// effectively the distance is smaller than 19
		else if (distance> 8.5 && distance<=19)
		{
			//RobotMovePosition(5, direction);
			RobotMoveTime(direction, m_speed, int(abs(distance-8)*100));
		}
		else
		{
			//RobotMovePosition(-5, direction);
			RobotMoveTime(direction, -m_speed, int(abs(distance-8)*100));
		}
		// compute the moving time
		//int duration=(int) ((distance-8)*200);
		//if (distance>=8)
		//{
		//	// we need to move forward
		//	// asin returns value in [-pi/2,pi/2]
		//	RobotMoveTime(direction, 100, duration);
		//}
		//else
		//{
		//	// we need to move backward
		//	RobotMoveTime(direction, -100, duration);
		//}
		char str[80];
		sprintf(str, "%f", distance);
		m_edit_speed.SetWindowTextA(str);
		sprintf(str, "%d", direction);
		m_edit_direction.SetWindowTextA(str);
	}
}

// this function controls the movement of robot by time
// duration: the duration of movement in millisecond
void CFollowMEDlg::RobotMoveTime(int direction, int speed, int duration)
{
	// compute the speed for each wheel
	short left=(short) SQRT2*speed*10*sin(direction*PI/180+PI/4);
	short right=(short) SQRT2*speed*10*sin(direction*PI/180-PI/4);
	m_MOTSDK.SetDcMotorControlMode (0,M_VELOCITY);
	m_MOTSDK.SetDcMotorControlMode (1,M_VELOCITY);
	m_MOTSDK.SetDcMotorVelocityControlPID (0, 10, 3, 100);
	m_MOTSDK.SetDcMotorVelocityControlPID (1, 10, 3, 100);
	m_MOTSDK.DcMotorVelocityTimeCtrAll (left, right,NO_CONTROL,NO_CONTROL,NO_CONTROL,NO_CONTROL, (short)duration); 
}

// this function controls the movement of robot precisely
// distance:	the distance to move in centimeter. Note 750 roughly means the robot moves forward 0.5 meter
// direction:	the direction in degree (0~360)
// this function is deprecated, due to that it makes the robot moves ugly.
void CFollowMEDlg::RobotMovePosition(int distance, int direction)
{
	// 750 pulse == 0.5 m
	int left=(short) SQRT2*15*distance*sin(direction*PI/180+PI/4);
	int right=(short) SQRT2*15*distance*sin(direction*PI/180-PI/4);

	long cmd1,cmd2;

    cmd1 = m_encoder0 + left;
    cmd2 = m_encoder1 + right;
    
   //change cmd1, cmd2 to valid data range
    if (cmd1 < 0) cmd1 = cmd1 + cFULL_COUNT;
    if (cmd2 < 0) cmd2 = cmd2 + cFULL_COUNT;
    if (cmd1 > cFULL_COUNT) cmd1 = cmd1 - cFULL_COUNT;
    if (cmd2 > cFULL_COUNT) cmd2 = cmd2 - cFULL_COUNT;


	m_MOTSDK.SetDcMotorControlMode (0,M_POSITION);
	m_MOTSDK.SetDcMotorControlMode (1,M_POSITION);
	m_MOTSDK.SetDcMotorVelocityControlPID (0, 30, 10, 0);
	m_MOTSDK.SetDcMotorPositionControlPID (0, 600,30,600);
	m_MOTSDK.SetDcMotorPositionControlPID (1, 600,30,600);
	m_MOTSDK.DcMotorPositionTimeCtrAll (cmd1,cmd2,NO_CONTROL,NO_CONTROL,NO_CONTROL,NO_CONTROL,100);
}

/*
	function for connecting the camerea
*/
bool CFollowMEDlg::ConnectCamera(void)
{
	CString user;
	m_user.GetWindowTextA(user);
	m_VitCtrl.put_UserName((LPCTSTR) user);

	CString password;
	m_password.GetWindowTextA(password);
	m_VitCtrl.put_Password((LPCTSTR) password);

	CString ip;
	m_ip.GetWindowTextA(ip);
	m_VitCtrl.put_RemoteIPAddr((LPCTSTR) ip);

	CString port;
	m_port.GetWindowTextA(port);
	m_VitCtrl.put_HttpPort(atoi(port));

	m_VitCtrl.put_ServerModelType(2);
	m_VitCtrl.Connect();

	// set the camera location
	//m_VitCtrl.SendCameraCommand("home", 30000);
	return true;
}
