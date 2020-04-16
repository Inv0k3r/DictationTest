/////////////////////////////////////////////////////////////////////
//    文件名： MainInterface.h
//    工程:  0151122244
//    作者：刘嘉磊，修改者：刘嘉磊
//    描述: 应用程序主页面头文件。
//    版本: 1.0 2017/8/14。
//    修改: 创建 2017/8/14。
/////////////////////////////////////////////////////////////////////

#pragma once
//加密密钥
const char g_key[17] = "asdfwetyhjuytrfd";
const char g_iv[17] = "gfdertfghjkuyrtg";
// CMainInterface 对话框

class CMainInterface : public CDialogEx
{
	DECLARE_DYNAMIC(CMainInterface)

public:
	CMainInterface(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMainInterface();
	// 对话框数据
	enum { IDD = IDD_MAIN_PAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_StrExamnumber;
	CString m_StrIDNumber;
	CString m_StrTimeleft;
	CString m_StrCurrentDirectory;
	int iTimeLeft;//剩余时间
	int iTimeSecond;//秒数
	bool bIsInputing;//是否正在输入
	bool bIsPDFCreat;
	//配置文件所需变量
	CString strIniFilename;//配置文件路径
	int iTotalMinutes;//总时间
	int iTotalQuestions;//音频数
	CString strAudioFormat;//音频格式
	double dMaxSpeed;//最大录入速度
	double dPercentageAccuracy;//准确率所占比重
	double dPercentageSpeed;//录入速度所占比重
	//PDF指针
	PDF *pPDF;
	int iFont;//用于PDF的字体
	CFont m_Font;//用于倒计时的字体
	CBrush m_RedBrush;//红色的倒计时
	//分数计算
	double dScore;//总分数
	double dAccuracy;//准确率
	double dSpeed;//录入速度
	int iWordNumber;//录入文字个数
	int iInputTimeSecond;//录入时间->用秒表示
	//系统变量/函数
	afx_msg void OnBnClickedStartExam();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CComboBox m_QuestionsNum;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedSubmit();
	afx_msg void OnSetfocusMainEdit();
	void CaculationScore(CString strStandardAnswer, CString strUserAnswer);
	void Submit();
	void PrintResult();
	afx_msg void OnBnClickedPrintResult();
	afx_msg void OnBnClickedExit();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CEdit m_Edit;
	afx_msg void OnEnable(BOOL bEnable);
	string DecryptionAES(const string& strSrc);
	virtual void OnOK();
	virtual void OnCancel();
	//音频操作函数和变量
	HWND m_hWnd;  
	DWORD DeviceId;  
	MCI_OPEN_PARMS mciOpenParms;  
	void LoadAudioFile(HWND hWnd, CString strFilepath);
	void PlayAudio();
	void PauseAudio();
	void ResumeAudio();
	void StopAudio();
	//身份证验证函数
	BOOL isIDNumberValidate(CString strIDNumber);

};
