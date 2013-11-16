// FollowMEDlg.h : header file
//

#pragma once
#include "vitaminctrl1.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "drrobotsdkcontrolctrl1.h"
#include "d:\robot\follow_me\followme\pedestrian_icra\pedestrian.h"


typedef struct PedestrainThreadParam
{
	DetectionScanner *scanner;
	std::vector<CPedestrainRect> results;
	bool is_processing;
	void *window;
}PEDESTRAINTHREADPARAM;

UINT PedestrainThreadFunction(LPVOID pParam);

// CFollowMEDlg dialog
class CFollowMEDlg : public CDialog
{
// Construction
public:
	CFollowMEDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_FOLLOWME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// the camera variable
	CVitaminctrl1 m_VitCtrl;
	afx_msg void OnBnClickedButtonConnect();
	afx_msg void OnBnClickedOk();
	// the variable for the connect button
	CButton m_connect;
private:
	// the flag to check whether the camera is connected or not
	bool is_connected;
	// the user name variable
	CEdit m_user;
public:
	// the password
	CEdit m_password;
	// the port
	CEdit m_port;
	// the ip address
	CIPAddressCtrl m_ip;
	// // the robot
	CDrrobotsdkcontrolctrl1 m_MOTSDK;
	afx_msg void OnBnClickedButtonForward();
	afx_msg void OnBnClickedButtonLeft();
	afx_msg void OnBnClickedButtonRight();
	afx_msg void OnBnClickedButtonBackward();
	afx_msg void OnBnClickedButtonStop();
	short m_ir0;
	short m_ir1;
	short m_ir2;
	short m_ir3;
	short m_ir4;
	short m_ir5;
	short m_ir6;
	short m_sonar0;
	short m_sonar1;
	short m_sonar2;
	DECLARE_EVENTSINK_MAP()
	void StandardSensorEventDrrobotsdkcontrolctrl1();
	void MotorSensorEventDrrobotsdkcontrolctrl1();
	void CustomSensorEventDrrobotsdkcontrolctrl1();
	// motion direction
	int m_motion_direction;
	short m_encoder0;
	short m_encoder1;
	// this stuff controls the speed
	int m_speed;
	// this variable stores the current frame for the camera
	// IplImage* frame;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	// // this function implements the robot moving function
	void RobotMove(int direction, int speed);
	// this function controls the movement of robot precisely
	void RobotMoveTime(int direction, int speed, int duration);
public:
	afx_msg void OnBnClickedButtonMove();
	// this controls the speed
	CSliderCtrl m_slider_speed;
	// direction
	CSliderCtrl m_slider_direction;
	int m_direction;
	// show the speed
	CEdit m_edit_speed;
	// show the direction
	CEdit m_edit_direction;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnStnClickedImageView();
	void OnClickVitaminctrl1(long lX, long lY);
	// the parameter for multi-threading
	PEDESTRAINTHREADPARAM *pedestrain_thread_param;
	// this function computes the distance and orientation required for the robot
	void TrackPedestrain(std::vector<CPedestrainRect> target, IplImage *frame);
	// this function converts this VARIANT to IplImage
	IplImage * Variant2IpplImage(VARIANT vData , VARIANT vInfo);
	// this variable stores the scascade scanner
	DetectionScanner *scanner;
	// this function shows the IplImage in the picture control
	void ShowImage(IplImage* img, UINT ID);
};
