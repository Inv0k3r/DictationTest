/////////////////////////////////////////////////////////////////////
//    文件名： 0151122244.cpp
//    工程:  0151122244
//    作者：刘嘉磊，修改者：刘嘉磊
//    描述: 主界面实现文件。
//    版本: 1.0 2017/8/14。
//    修改: 创建 2017/8/14。
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
// CMainInterface 对话框
CString g_StringTableTemp;//全局变量 用于将string table的资源转为cstring

IMPLEMENT_DYNAMIC(CMainInterface, CDialogEx)
	CMainInterface::CMainInterface(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMainInterface::IDD, pParent)
	, m_StrExamnumber(_T(""))
	, m_StrIDNumber(_T(""))
	, m_StrTimeleft(_T(""))
{
	//初始化各变量
	
	iTimeSecond = 0;
	iTimeLeft = 0;
	iTotalMinutes = 0;
	iTotalQuestions = 0;
	dMaxSpeed = 0;
	dPercentageAccuracy = 0.0;
	dPercentageSpeed = 0.0;
	dScore = 0.0;
	dAccuracy = 0;
	iWordNumber = 0;
	iInputTimeSecond = 0;
	bIsInputing = FALSE;
	bIsPDFCreat = FALSE;
	pPDF = PDF_new();  
	char buf[2001];
	GetCurrentDirectory(2000,buf);
	m_StrCurrentDirectory = buf;
	//读取配置文件,文件地址默认为 C:\Users\weihongxi\Desktop\typlist.ini
	//若文件不存在则提示错误并弹出对话框手动选择配置文件
	//第一次读取配置文件
	char temp[20];
	strIniFilename = m_StrCurrentDirectory;
	strIniFilename += "\\typlist.ini";
	iTotalMinutes = GetPrivateProfileInt("ExamInfo","total_minutes",-1,strIniFilename);
	iTotalQuestions = GetPrivateProfileInt("ExamInfo","total_questions",-1,strIniFilename);
	GetPrivateProfileString("ExamInfo","audio_format",NULL,temp,-1,strIniFilename);
	strAudioFormat = temp;
	GetPrivateProfileString("ExamInfo","max_speed",NULL,temp,-1,strIniFilename);
	dMaxSpeed = atof(temp);
	GetPrivateProfileString("ExamInfo","percentage_accuracy",NULL,temp,-1,strIniFilename);
	dPercentageAccuracy = atof(temp);
	GetPrivateProfileString("ExamInfo","percentage_speed",NULL,temp,-1,strIniFilename);
	dPercentageSpeed = atof(temp);
	//读取配置文件完成
	//检查读取情况
	while(iTotalMinutes <= 0 && iTotalQuestions <= 0)//时间错误或者题目数量错误->配置文件错误->重新选择
	{
		g_StringTableTemp.LoadStringA(IDS_INI_OPEN_ERROR);
		if(MessageBox(g_StringTableTemp,"",MB_ICONASTERISK|MB_OKCANCEL) == IDCANCEL){
			OnBnClickedExit();//点了取消直接退出
		}
		CString filter;
		filter.LoadString(IDS_FILE_TYPE);   //文件过滤的类型  
		CFileDialog openFileDialog(TRUE,"C:\\Users","typlist.ini",OFN_HIDEREADONLY|OFN_READONLY, filter);
		openFileDialog.GetOFN().lpstrInitialDir = "C:\\Users\\weihongxi\\Desktop\\typlist.ini";
		INT_PTR result = openFileDialog.DoModal();  
		CString filePath = "C:\\Users\\weihongxi\\Desktop\\typlist.ini";  
		if(result == IDOK)
		{  
			filePath = openFileDialog.GetPathName();
			strIniFilename = filePath;
			iTotalMinutes = GetPrivateProfileInt("ExamInfo","total_minutes",0,strIniFilename);
			iTotalQuestions = GetPrivateProfileInt("ExamInfo","total_questions",0,strIniFilename);
			GetPrivateProfileString("ExamInfo","audio_format",NULL,temp,6,strIniFilename);
			dMaxSpeed = GetPrivateProfileInt("ExamInfo","max_speed",0,strIniFilename);
			dPercentageAccuracy = GetPrivateProfileInt("ExamInfo","percentage_accuracy",0,strIniFilename);
			dPercentageSpeed = GetPrivateProfileInt("ExamInfo","percentage_speed",0,strIniFilename);
			//读取配置文件完成
		}else{

		}
	}
	//成功读取到配置文件
	//根据配置文件的内容对变量赋值操作
	iTimeLeft = iTotalMinutes;
}
CMainInterface::~CMainInterface()
{
}
void CMainInterface::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NUMBER_EDIT, m_StrExamnumber);
	DDX_Text(pDX, IDC_ID_NUM_EDIT, m_StrIDNumber);
	DDX_Text(pDX, IDC_TIME, m_StrTimeleft);
	DDX_Control(pDX, IDC_EXAM_NUM, m_QuestionsNum);
	DDX_Control(pDX, IDC_TIME, m_Edit);
}
BEGIN_MESSAGE_MAP(CMainInterface, CDialogEx)
	ON_BN_CLICKED(IDC_START_EXAM, &CMainInterface::OnBnClickedStartExam)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_SUBMIT, &CMainInterface::OnBnClickedSubmit)
//	ON_WM_LBUTTONDOWN()
	ON_EN_SETFOCUS(IDC_MAIN_EDIT, &CMainInterface::OnSetfocusMainEdit)
//	ON_EN_CHANGE(IDC_TIME, &CMainInterface::OnEnChangeTime)
	ON_BN_CLICKED(IDC_PRINT_RESULT, &CMainInterface::OnBnClickedPrintResult)
	ON_BN_CLICKED(IDC_EXIT, &CMainInterface::OnBnClickedExit)
	ON_WM_CTLCOLOR()
	ON_WM_ENABLE()
END_MESSAGE_MAP()
void CMainInterface::OnBnClickedStartExam()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CEdit *Edit_ExamNumber = (CEdit *)GetDlgItem(IDC_NUMBER_EDIT);
	CEdit *Edit_IDNumber = (CEdit *)GetDlgItem(IDC_ID_NUM_EDIT);
	CComboBox *ComboBox_QuestionChoice = (CComboBox *)GetDlgItem(IDC_EXAM_NUM);
	CButton *StartTest = (CButton *)GetDlgItem(IDC_START_EXAM);
	CEdit *MainEdit = (CEdit *)GetDlgItem(IDC_MAIN_EDIT);
	CButton *Submit = (CButton *)GetDlgItem(IDC_SUBMIT);
	CButton *Exit = (CButton *)GetDlgItem(IDC_EXIT);
	CString temp1,temp2;
	int iCurrentChoice = -1;
	Edit_ExamNumber->GetWindowText(temp1);
	Edit_IDNumber->GetWindowText(temp2);
	iCurrentChoice = ComboBox_QuestionChoice->GetCurSel();
	if(temp1 == "")
	{
		g_StringTableTemp.LoadString(IDS_CANDIDIATE_EMPTY_ERROR);
		MessageBox(g_StringTableTemp);
	}else if(temp2 == ""){
		g_StringTableTemp.LoadString(IDS_ID_NUMBER_EMPTY_ERROR);
		MessageBox(g_StringTableTemp);
	}else if(iCurrentChoice == -1){
		g_StringTableTemp.LoadString(IDS_TEST_CHOICE_ERROR);
		MessageBox(g_StringTableTemp);
	}
	else if(!isIDNumberValidate(temp2)){
		g_StringTableTemp.LoadString(IDS_IDCARD_ERROR);
		MessageBox(g_StringTableTemp);
	}
	else{
		//登录成功
		Edit_ExamNumber->EnableWindow(FALSE);
		Edit_IDNumber->EnableWindow(FALSE);
		ComboBox_QuestionChoice->EnableWindow(FALSE);
		StartTest->EnableWindow(FALSE);
		MainEdit->EnableWindow(TRUE);
		Submit->EnableWindow(TRUE);
		Exit->EnableWindow(FALSE);
		g_StringTableTemp.LoadString(IDS_TEST_START_TIPS);
		MainEdit->SetWindowText(g_StringTableTemp);
		//开始倒计时
		//设置计时器
		SetTimer(ID_TIMER,1000,NULL);
		//初始化提示消息
		m_Font.DeleteObject();
		g_StringTableTemp.LoadString(IDS_FONT_TYPE);
		m_Font.CreatePointFont(150,g_StringTableTemp);
		CEdit *pTimetips = (CEdit *)GetDlgItem(IDC_TIME);
		pTimetips->SetFont(&m_Font);
		CString temp;
		temp.LoadString(IDS_TIME_LEFT_TIPS);
		CString time;
		if(iTimeLeft == iTotalMinutes){
			time.Format("%d",iTimeLeft);
			temp += time;
			g_StringTableTemp.LoadString(IDS_MINUTES);
			temp += g_StringTableTemp;
			pTimetips->SetWindowTextA(temp);
			iTimeSecond = 0;
		}
		//开始倒计时后立即播放音频文件，默认音频文件长度与考试时间相同
		//播放文件的编号来自ComboBox
		//音频文件的储存路径是相对路径, 即与应用程序在同一路径下
		CString strAudioPath = m_StrCurrentDirectory + "\\Audio\\";
		CString num;
		CString command;
		num.Format("%d",iCurrentChoice + 1);
		strAudioPath += num;
		strAudioPath += ".";
		strAudioPath += strAudioFormat;
		//加载音频文件并播放
		LoadAudioFile(m_hWnd,strAudioPath);
		PlayAudio();
	}
}
void CMainInterface::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//计时，每过一秒，输入时间+1s
	iInputTimeSecond ++;
	//初始化时间提示
	CEdit *pTimetips = (CEdit *)GetDlgItem(IDC_TIME);
	CString temp;
	temp.LoadString(IDS_TIME_LEFT_TIPS);
	CString time;

	if(1 == nIDEvent)
	{
		if(iTimeLeft == 0 && iTimeSecond == 0){
			g_StringTableTemp.LoadString(IDS_TIMEOUT);
			MessageBox(g_StringTableTemp);
			KillTimer(1);
			//时间到
			return;
		}
		if(iTimeSecond == 0){

			iTimeLeft--;
			iTimeSecond = 59;
			time.Format("%d",iTimeLeft);
			temp += time;
			g_StringTableTemp.LoadString(IDS_MINUTES);
			temp += g_StringTableTemp;
			time.Format("%d",iTimeSecond);
			temp += time;
			g_StringTableTemp.LoadString(IDS_SECOND);
			temp += g_StringTableTemp;
			pTimetips->SetWindowTextA(temp);

		}else{
			iTimeSecond--;
			time.Format("%d",iTimeLeft);
			temp += time;
			g_StringTableTemp.LoadString(IDS_MINUTES);
			temp += g_StringTableTemp;
			time.Format("%d",iTimeSecond);
			temp += time;
			g_StringTableTemp.LoadString(IDS_SECOND);
			temp += g_StringTableTemp;
			pTimetips->SetWindowTextA(temp);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}
BOOL CMainInterface::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CComboBox *QuestionsNum = (CComboBox *)GetDlgItem(IDC_EXAM_NUM);
	CString num = "", str;
	for(int i = 1; i <= iTotalQuestions ; i++) 
	{
		if(i < 10)
		{
			str = "0";
		}
		num.Format("%d",i);
		str += num;
		QuestionsNum->AddString(str);
	}
	CButton *Print = (CButton *)GetDlgItem(IDC_PRINT_RESULT);
	CButton *Submit = (CButton *)GetDlgItem(IDC_SUBMIT);
	CEdit *MainEdit = (CEdit *)GetDlgItem(IDC_MAIN_EDIT);
	CEdit *Time = (CEdit *)GetDlgItem(IDC_TIME);
	Print->EnableWindow(FALSE);
	Submit->EnableWindow(FALSE);
	MainEdit->EnableWindow(FALSE);
	Time->EnableWindow(FALSE);
	m_RedBrush.CreateSolidBrush(GetSysColor(COLOR_3DFACE));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
void CMainInterface::OnBnClickedSubmit()
{
	// TODO: 在此添加控件通知处理程序代码
	//点击提交之后弹窗确认
	KillTimer(1);
	PauseAudio();
	g_StringTableTemp.LoadString(IDS_SUBMIT);
	CString strTipTitle;
	strTipTitle.LoadString(IDS_TIP_TITLE);
	int iChoice = MessageBox(g_StringTableTemp,"",MB_ICONASTERISK|MB_OKCANCEL);
	if(iChoice == 1){//确定提交
		StopAudio();
		CEdit *UserInputAnswer = (CEdit *)GetDlgItem(IDC_MAIN_EDIT);
		CButton *SubmitButton = (CButton *)GetDlgItem(IDC_SUBMIT);
		CButton *PrintResult = (CButton *)GetDlgItem(IDC_PRINT_RESULT);
		UserInputAnswer->EnableWindow(FALSE);
		SubmitButton->EnableWindow(FALSE);
		PrintResult->EnableWindow(TRUE);
		Submit();
	}else{
		SetTimer(1,1000,NULL);
		ResumeAudio();
	}
}
void CMainInterface::OnSetfocusMainEdit()
{
	// TODO: 在此添加控件通知处理程序代码
	if(bIsInputing)//如果已经点击过输入框，则返回
		return;
	else//否则清空输入框
	{
		CEdit *MainEdit = (CEdit *)GetDlgItem(IDC_MAIN_EDIT);
		MainEdit->SetWindowTextA("");
		bIsInputing = TRUE;
	}
}
void CMainInterface::CaculationScore(CString strStandardAnswer, CString strUserAnswer)
{
	//首先清理用户输入文本内容中的标点符号，答案文本中应当没有标点符号
	//for(int i = 0; i < strStandardAnswer.GetLength(); i ++){
	//	if(strStandardAnswer[i] >=65 && strStandardAnswer[i] <= 90){

	//	}else if(strStandardAnswer[i] >= 97 && strStandardAnswer[i] <= 122){

	//	}else if(strStandardAnswer[i]);
	//}
	//CString strPunctuation[] = {"`","~","!","@","#","$","%","^","&","*","(",")","-","_","+","=",
	//	"{","}","[","]",":",";","\"","\'","<",">",",",".","?","/","\\","|","，","。"};
	int len1 = strStandardAnswer.GetLength();
	int len2 = strUserAnswer.GetLength();
	if(len2 == 0){
		dAccuracy = 0;
		dSpeed = 0;
		dScore = 0;
		return;
	}
	/*防止这种情况*/
	if(len1 == 0){
		g_StringTableTemp.LoadString(IDS_ANSWER_FILE_ERROR);
		MessageBox(g_StringTableTemp);
		OnBnClickedExit();
	}
	/*-------------------*/
	//顺带给录入文字个数赋值
	iWordNumber = len2;
	//计算字符串长度
	int **dif = new int *[len1 + 1];
	for(int i = 0; i < len1 + 1; i ++){
		dif[i] = new int[len2 + 1];
	}
	for(int i = 0; i < len1 + 1; i ++){
		dif[i][0] = i;
	}
	for(int i = 0; i< len2 + 1; i ++){
		dif[0][i] = i;
	}
	int temp;
	for(int i = 1; i <= len1; i++){
		for(int j = 1; j <= len2; j ++){
			if(strStandardAnswer[i - 1] == strUserAnswer[j - 1]){
				temp = 0;
			}else{
				temp = 1;
			}
			int min = dif[i - 1][j - 1] + temp;
			if(dif[i][j - 1] + 1 < min)
				min = dif[i][j - 1] + 1;
			else if(dif[i - 1][j] + 1 < min)
				min = dif[i - 1][j] + 1;
			dif[i][j] = min;
		}
	}
	dAccuracy = 1.0 - (float)dif[len1][len2]/(len1 > len2 ? len1 : len2);
	//dif数组使用完应释放内存
	for(int i = 0; i < len1 + 1; i ++){
		delete [len2 + 1]dif[i];
	}
	delete [len1 + 1]dif;
	dif = NULL;

	dSpeed = iWordNumber / (double)(iInputTimeSecond / 60.0);//按分钟计输入速度
	if(dSpeed > dMaxSpeed)
		dSpeed = 100.0;
	dScore = dPercentageAccuracy * dAccuracy * 100.0 + dPercentageSpeed * dSpeed;
}
void CMainInterface::Submit()
{
	//提交函数，先读取答案文件，再读取用户文件，然后调用CaculationScore()函数计算总分
	CComboBox *ComboBox_QuestionChoice = (CComboBox *)GetDlgItem(IDC_EXAM_NUM);
	CEdit *UserInputAnswer = (CEdit *)GetDlgItem(IDC_MAIN_EDIT);
	int iChoiceAudio = ComboBox_QuestionChoice->GetCurSel();
	CString strAnswerPath = m_StrCurrentDirectory + "\\Answer\\Answer";
	CString temp;
	temp.Format("%d",iChoiceAudio + 1);
	strAnswerPath += temp;
	strAnswerPath += ".txt";
	CString strStandardAnswer, strUserAnswer;
	/*---------------------------从文件读取答案--------------------*/
	CFile fileRead(strAnswerPath, CFile::modeRead);
	int fileLen = (int)fileRead.GetLength();
	CStringA strA;
	char *pBuf = strA.GetBuffer(fileLen + 1);
	fileRead.Read(pBuf,fileLen);
	pBuf[fileLen] = '\0';
	strA.ReleaseBuffer();
	fileRead.Close();
	//读来的答案文件是加密状态 需要先解密
	string strDecryption;
	strDecryption = strA.GetBuffer(0);
	strDecryption = DecryptionAES(strDecryption);
	strStandardAnswer = strDecryption.c_str();
	//解密完成并复制给变量strStandardAnswer
	/*------------------------------------------------------------*/
	UserInputAnswer->GetWindowTextA(strUserAnswer);//获取用户输入的文本

	CaculationScore(strStandardAnswer,strUserAnswer);//计算总分
}
void CMainInterface::PrintResult()
{

	//获取准考证号/身份证号/
	CString strTestNumber = "";
	CString strIDNumber = "";
	//用于辅助输出的变量字符串
	CString tempOutput = "";
	CEdit *pTestNumberEdit = (CEdit *)GetDlgItem(IDC_NUMBER_EDIT);
	CEdit *pIDNumberEdit = (CEdit *)GetDlgItem(IDC_ID_NUM_EDIT);
	//输出路径
	CString strOutputPath = m_StrCurrentDirectory;
	strOutputPath += "\\Result\\";
	pTestNumberEdit->GetWindowText(strTestNumber);
	pIDNumberEdit->GetWindowText(strIDNumber);

	tempOutput = strTestNumber;
	tempOutput += ".pdf";
	CString fileName = tempOutput;//pdf文件的名字

	strOutputPath += tempOutput;
	if(bIsPDFCreat){
		ShellExecute(NULL,"open",strOutputPath,NULL,NULL,SW_SHOWMAXIMIZED);
		return;
	}
	PDF_set_parameter(pPDF,"license","X600605-009100-4A9970-073B13");	//破解PDFlib
	PDF_begin_document(pPDF,strOutputPath,0,NULL);
	PDF_set_parameter(pPDF,"hypertextencoding","host");
	PDF_set_info(pPDF,"Creator","ljl");
	PDF_set_info(pPDF,"Author","ljl");
	PDF_begin_page_ext(pPDF,a4_width,a4_height,"");
	iFont = PDF_load_font(pPDF,"STSongStd-Light-Acro",0,"GB-EUC-H","");
	PDF_setfont(pPDF,iFont,12);
	PDF_set_text_pos(pPDF,50,700);

	g_StringTableTemp.LoadString(IDS_CANDIDIATE);
	tempOutput = g_StringTableTemp;
	tempOutput += strTestNumber;
	PDF_show(pPDF,tempOutput);
	g_StringTableTemp.LoadString(IDS_ID_NUMBER);
	tempOutput = g_StringTableTemp;
	tempOutput += strIDNumber;
	PDF_continue_text(pPDF,tempOutput);
	g_StringTableTemp.LoadString(IDS_ACCURACY);
	tempOutput = g_StringTableTemp;
	CString strChangeTemp;
	strChangeTemp.Format("%.2f",dAccuracy * 100);
	tempOutput += strChangeTemp;
	g_StringTableTemp.LoadString(IDS_SPEED);
	tempOutput += g_StringTableTemp;
	strChangeTemp.Format("%.2f",dSpeed);
	tempOutput += strChangeTemp;
	g_StringTableTemp.LoadString(IDS_SCORE);
	tempOutput += g_StringTableTemp;
	strChangeTemp.Format("%.2f",dScore);
	tempOutput += strChangeTemp;
	g_StringTableTemp.LoadString(IDS_SCORE_1);
	tempOutput += g_StringTableTemp;
	PDF_continue_text(pPDF,tempOutput);
	g_StringTableTemp.LoadString(IDS_INPUT);
	tempOutput = g_StringTableTemp;
	PDF_continue_text(pPDF,tempOutput);
	PDF_setfont(pPDF,iFont,10);

	CString strUserAnswer;
	CEdit *UserInputAnswer = (CEdit *)GetDlgItem(IDC_MAIN_EDIT);
	UserInputAnswer->GetWindowTextA(strUserAnswer);//获取用户输入的文本
	CString lineOutput = "";
	for(int i = 0; i < strUserAnswer.GetLength(); i++){
		if(!(i % 80)){
			PDF_continue_text(pPDF,lineOutput);
			lineOutput = strUserAnswer[i];
		}else{
			lineOutput += strUserAnswer[i];
		}
	}
	PDF_continue_text(pPDF,lineOutput);
	PDF_end_page_ext(pPDF,"");
	PDF_end_document(pPDF,"");
	PDF_delete(pPDF);
	ShellExecute(NULL,"open",strOutputPath,NULL,NULL,SW_SHOWMAXIMIZED);
}
void CMainInterface::OnBnClickedPrintResult()
{
	// TODO: 在此添加控件通知处理程序代码
	PrintResult();
	CButton *pExitButton = (CButton *)GetDlgItem(IDC_EXIT);
	pExitButton->EnableWindow(TRUE);
}
void CMainInterface::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	::PostQuitMessage(0);
}
HBRUSH CMainInterface::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何特性
	CEdit *p =(CEdit *) GetDlgItem(IDC_TIME);
	p->EnableWindow(TRUE);
	if(pWnd->GetDlgCtrlID()==IDC_TIME)//注意此处的（pWnd->），否则没效果
	{
		pDC->SetTextColor(RGB(255,0,0));

		hbr = (HBRUSH)m_RedBrush.GetSafeHandle();
	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
void CMainInterface::OnEnable(BOOL bEnable)
{
	CDialogEx::OnEnable(bEnable);
	// TODO: 在此处添加消息处理程序代码
}
string CMainInterface::DecryptionAES(const string& strSrc)
{
	string strData = base64_decode(strSrc);
	size_t length = strData.length();
	//密文
	char *szDataIn = new char[length + 1];
	memcpy(szDataIn, strData.c_str(), length+1);
	//明文
	char *szDataOut = new char[length + 1];
	memcpy(szDataOut, strData.c_str(), length+1);

	//进行AES的CBC模式解密
	AES aes;
	aes.MakeKey(g_key, g_iv, 16, 16);
	aes.Decrypt(szDataIn, szDataOut, length, AES::CBC);

	//去PKCS7Padding填充
	if (0x00 < szDataOut[length - 1] <= 0x16)
	{
		int tmp = szDataOut[length - 1];
		for (int i = length - 1; i >= length - tmp; i--)
		{
			if (szDataOut[i] != tmp)
			{
				memset(szDataOut, 0, length);
				g_StringTableTemp.LoadString(IDS_DECRYPTION_ERROR);
				MessageBox(g_StringTableTemp);
				break;
			}
			else
				szDataOut[i] = 0;
		}
	}
	string strDest(szDataOut);
	delete[] szDataIn;
	delete[] szDataOut;
	return strDest;
}
BOOL CMainInterface::isIDNumberValidate(CString strIDNumber)
{
	int weight[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2};
	char validate[]={ '1','0','X','9','8','7','6','5','4','3','2'};  
	int sum=0;
	int mode=0;
	if (strIDNumber.GetLength()==18)
	{
		for(int i=0;i<strIDNumber.GetLength()-1;i++){
			sum=sum+(strIDNumber[i]-'0')*weight[i];
		}
		mode=sum%11;
		if (validate[mode]==strIDNumber[17])
		{
			return true;
		}
	}
	return false;
}
void CMainInterface::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
}
void CMainInterface::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	return;
}
void CMainInterface::LoadAudioFile(HWND hWnd, CString strFilepath)
{
	m_hWnd = hWnd;  
	mciSendCommand(DeviceId, MCI_CLOSE, 0, 0); //在加载文件前先清空上一次播放的设备  
	mciOpenParms.lpstrElementName = strFilepath; //将音乐文件路径传给设备  
	DWORD dwReturn;  
	if (dwReturn = mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT, (DWORD)(LPVOID)&mciOpenParms))  
	{  
		//如果打开文件失败,将出错信息储存在buffer,并显示出错警告  
		char buffer[256];  
		mciGetErrorString(dwReturn, buffer, 256); //获取错误码对应的错误信息  
		::MessageBox(hWnd, buffer, "出错警告！", MB_ICONHAND | MB_ICONERROR | MB_ICONSTOP); //弹出错误信息提示对话框  
	}  
	DeviceId = mciOpenParms.wDeviceID;  
}
void CMainInterface::PlayAudio()
{
	MCI_PLAY_PARMS mciplayparms;  
	mciplayparms.dwCallback = (DWORD)m_hWnd;  
	mciplayparms.dwFrom = 0; //每次播放都是从0开始播放  
	mciSendCommand(DeviceId, MCI_PLAY, MCI_FROM | MCI_NOTIFY, (DWORD)(LPVOID)&mciplayparms);  
}
void CMainInterface::PauseAudio()
{
	mciSendCommand(DeviceId, MCI_PAUSE, 0, 0);  
}
void CMainInterface::ResumeAudio()
{
	mciSendCommand(DeviceId, MCI_RESUME, 0, 0); 
}
void CMainInterface::StopAudio()
{
	mciSendCommand(DeviceId, MCI_STOP, 0, 0);  
	mciSendCommand(DeviceId, MCI_CLOSE, 0, 0);  
}