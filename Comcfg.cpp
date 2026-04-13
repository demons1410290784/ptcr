/*
**	FILENAME			ComCfg.cpp
**
**	PURPOSE				此类可在进行万用表、温控表设置时，
**						对串行通讯设置、事件响应、发送缓冲
**						区大小等参数进行改变
*/

#include "stdafx.h"
#include "ptcr.h"
#include "ComCfg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

int F45; // modified on 1/18/2000

/////////////////////////////////////////////////////////////////////////////
// CComCfg 对话框

//
//	构造函数：初始化并打开“线路设置”对话框
//
CComCfg::CComCfg(CPtcrDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CComCfg::IDD, pParent)
{
	m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CComCfg)
	m_BaudRate = -1;
	m_CommRingDetected = FALSE;
	m_CommBreakDetected = FALSE;
	m_CommRxchar = FALSE;
	m_CommCTSDetected = FALSE;
	m_CommDSRDetected = FALSE;
	m_CommERRDetected = FALSE;
	m_CommRLSDDetected = FALSE;
	m_CommTXEmpty = FALSE;
	m_CommRxcharFlag = FALSE;
	m_DataBits = -1;
	m_Parity = -1;
	m_SerialPort = -1;
	m_StopBits = -1;
	//}}AFX_DATA_INIT

	if(F45==0){//modeified on 1/18/2000	
	DWORD dwCommEvents = pDoc->m_sPort.GetCommEvents();
	m_SerialPort=pDoc->m_sPort.m_nPortNr;
	m_strSendBuffer.Format("%d", pDoc->m_sPort.GetWriteBufferSize());
	m_BaudRate=pDoc->m_sPort.m_baudno;
	m_StopBits=pDoc->m_sPort.m_stopno;
	m_DataBits=pDoc->m_sPort.m_datano;
	m_Parity=pDoc->m_sPort.m_parityno;
	m_CommBreakDetected = (dwCommEvents & EV_BREAK) > 0 ? TRUE : FALSE;
	m_CommCTSDetected   = (dwCommEvents & EV_CTS) > 0 ? TRUE : FALSE;
	m_CommDSRDetected   = (dwCommEvents & EV_DSR) > 0 ? TRUE : FALSE;
	m_CommERRDetected   = (dwCommEvents & EV_ERR) > 0 ? TRUE : FALSE;
	m_CommRingDetected  = (dwCommEvents & EV_RING) > 0 ? TRUE : FALSE;
	m_CommRLSDDetected  = (dwCommEvents & EV_RLSD) > 0 ? TRUE : FALSE;
	m_CommRxchar        = (dwCommEvents & EV_RXCHAR) > 0 ? TRUE : FALSE;
	m_CommRxcharFlag    = (dwCommEvents & EV_RXFLAG) > 0 ? TRUE : FALSE;
	m_CommTXEmpty       = (dwCommEvents & EV_TXEMPTY) > 0 ? TRUE : FALSE;
	}
else {	
	DWORD dwCommEvents = pDoc->m_sPortF45.GetCommEvents();
 if (m_pDoc->m_sPortF45.m_SPAvailable){
	m_SerialPort=pDoc->m_sPortF45.m_nPortNr;
	m_strSendBuffer.Format("%d", pDoc->m_sPortF45.GetWriteBufferSize());
	m_BaudRate=pDoc->m_sPortF45.m_baudno;
    m_StopBits=pDoc->m_sPortF45.m_stopno;
	m_DataBits=pDoc->m_sPortF45.m_datano;
	m_Parity=pDoc->m_sPortF45.m_parityno;
	}
 else	{
	m_SerialPort=pDoc->m_sPortF45.m_nPortNr;
	m_strSendBuffer.Format("%d", pDoc->m_sPortF45.GetWriteBufferSize());
	m_BaudRate=pDoc->m_sPortF45.m_baudno;
	m_DataBits=pDoc->m_sPortF45.m_datano;
	m_Parity=pDoc->m_sPortF45.m_parityno;

	
	if(pDoc->MultiMeter==4)m_StopBits=2;
    else m_StopBits=pDoc->m_sPortF45.m_stopno;

 }
	m_CommBreakDetected = (dwCommEvents & EV_BREAK) > 0 ? TRUE : FALSE;
	m_CommCTSDetected   = (dwCommEvents & EV_CTS) > 0 ? TRUE : FALSE;
	m_CommDSRDetected   = (dwCommEvents & EV_DSR) > 0 ? TRUE : FALSE;
	m_CommERRDetected   = (dwCommEvents & EV_ERR) > 0 ? TRUE : FALSE;
	m_CommRingDetected  = (dwCommEvents & EV_RING) > 0 ? TRUE : FALSE;
	m_CommRLSDDetected  = (dwCommEvents & EV_RLSD) > 0 ? TRUE : FALSE;
	m_CommRxchar        = (dwCommEvents & EV_RXCHAR) > 0 ? TRUE : FALSE;
	m_CommRxcharFlag    = (dwCommEvents & EV_RXFLAG) > 0 ? TRUE : FALSE;
	m_CommTXEmpty       = (dwCommEvents & EV_TXEMPTY) > 0 ? TRUE : FALSE;
	}
}

//
//	对话框数据交换
//
void CComCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComCfg)
	DDX_CBIndex(pDX, IDC_BAUDRATECOMBO, m_BaudRate);
	DDX_Check(pDX, IDC_CHECK5, m_CommRingDetected);
	DDX_Check(pDX, IDC_CHECK1, m_CommBreakDetected);
	DDX_Check(pDX, IDC_CHECK7, m_CommRxchar);
	DDX_Check(pDX, IDC_CHECK2, m_CommCTSDetected);
	DDX_Check(pDX, IDC_CHECK3, m_CommDSRDetected);
	DDX_Check(pDX, IDC_CHECK4, m_CommERRDetected);
	DDX_Check(pDX, IDC_CHECK6, m_CommRLSDDetected);
	DDX_Check(pDX, IDC_CHECK9, m_CommTXEmpty);
	DDX_Check(pDX, IDC_CHECK8, m_CommRxcharFlag);
	DDX_CBIndex(pDX, IDC_DATABITSCOMBO, m_DataBits);
	DDX_CBIndex(pDX, IDC_PARITYCOMBO, m_Parity);
	DDX_CBIndex(pDX, IDC_SERIALPORTCOMBO, m_SerialPort);
	DDX_CBIndex(pDX, IDC_STOPBITSCOMBO, m_StopBits);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComCfg, CDialog)
	//{{AFX_MSG_MAP(CComCfg)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_CBN_SELCHANGE(IDC_MultiMeter, OnSelchangeMultiMeter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComCfg消息处理程序

//
//	“确认”键操作：检测程序运行状态；检测使用的温控装为万用表或是温控表
//	并检测通讯状态；读取并保存窗口内设置的参数与设置。
//
void CComCfg::OnOK() 
{
	// TODO: Add extra validation here
    GetDlgItem(IDC_MultiMeter)->EnableWindow(FALSE);
    GetDlgItem(IDC_DigitConctollor)->EnableWindow(FALSE);	//将“万用表”和“温控表”选项无效化

	if(m_pDoc->m_Running)
	{
        MessageBox("控制中,不能改变串口参数!","提示",MB_OK);
		return;
	}	//若当前测试程序正在运行，弹出消息提示对话框并退出


	// --- 【新增开始】在 CDialog::OnOK 销毁窗口前，先读取温控表的选项 ---
    int nCtrlIdx = -1;
    CComboBox* pCtrlCombo = (CComboBox*)GetDlgItem(IDC_DigitConctollor);
    if (pCtrlCombo != NULL) {
        nCtrlIdx = pCtrlCombo->GetCurSel();
    }
    // --- 【新增结束】 ---

	CDialog::OnOK();

	// --- 【新增开始】如果是宇电表(假设它是第4个选项,索引为3),强行覆盖刚才读取的串口参数 ---
    if (nCtrlIdx == 3) {
        m_Parity = 0;   // 0=无校验(N)
        m_DataBits = 0; // 0=8位
        m_StopBits = 0; // 0=1位停止位
        m_BaudRate = 1; // 1=9600波特率
        m_pDoc->m_ControllerType = 3; // 告诉全局：现在是宇电表
    }
    // --- 【新增结束】 ---

	if(F45==0){	//使用温控表	//modeified on 1/18/2000	  
		m_pDoc->m_sPort.m_nPortNr=m_SerialPort;
		m_pDoc->m_sPort.m_nWriteBufferSize=atoi(m_strSendBuffer);
	    m_pDoc->m_sPort.m_baudno=m_BaudRate;
  		m_pDoc->m_sPort.m_stopno=m_StopBits;
		m_pDoc->m_sPort.m_datano=m_DataBits;
		m_pDoc->m_sPort.m_parityno=m_Parity;
		m_pDoc->m_sPort.m_dwCommEvents=0;		//保存对话框内设置的事件响应参数
	if (m_CommBreakDetected)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_BREAK;
	if (m_CommCTSDetected)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_CTS;
	if (m_CommDSRDetected)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_DSR;
	if (m_CommERRDetected)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_ERR;
	if (m_CommRingDetected)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_RING;
	if (m_CommRLSDDetected)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_RLSD;
	if (m_CommRxchar)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_RXCHAR;
	if (m_CommRxcharFlag)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_RXFLAG;
	if (m_CommTXEmpty)
			m_pDoc->m_sPort.m_dwCommEvents |= EV_TXEMPTY;
    //根据设置的事件响应修改端口的串口事件参数
	if(m_pDoc->m_sPort.InitPort(this))	//端口通讯成功时
	{
		m_pDoc->m_sPort.StartMonitoring();
		MessageBox("温控表通讯成功!","提示",MB_OK);	
	}
	else{
		MessageBox("温控表通讯不成功!","提示",MB_OK);	
		return;
	}
	}
	if(F45==1){	//使用万能表	//modefied on 1/18/2000
		m_pDoc->m_sPortF45.m_nPortNr=m_SerialPort;
		m_pDoc->m_sPortF45.m_nWriteBufferSize=atoi(m_strSendBuffer);
		m_pDoc->m_sPortF45.m_baudno=m_BaudRate;
  		m_pDoc->m_sPortF45.m_stopno=m_StopBits;
		m_pDoc->m_sPortF45.m_datano=m_DataBits;
		m_pDoc->m_sPortF45.m_parityno=m_Parity;
		m_pDoc->m_sPortF45.m_dwCommEvents=0;	//保存对话框内设置的事件响应参数
	if (m_CommBreakDetected)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_BREAK;
	if (m_CommCTSDetected)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_CTS;
	if (m_CommDSRDetected)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_DSR;
	if (m_CommERRDetected)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_ERR;
	if (m_CommRingDetected)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_RING;
	if (m_CommRLSDDetected)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_RLSD;
	if (m_CommRxchar)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_RXCHAR;
	if (m_CommRxcharFlag)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_RXFLAG;
	if (m_CommTXEmpty)
			m_pDoc->m_sPortF45.m_dwCommEvents |= EV_TXEMPTY;
//根据设置的事件响应修改端口的串口事件参数

	if(m_pDoc->m_sPortF45.InitPort(this))	//端口通讯成功时
	{
		m_pDoc->m_sPortF45.StartMonitoring();
		MessageBox("万用表通讯成功!","提示",MB_OK);	}
	else	{
		MessageBox("万用表通讯不成功!","提示",MB_OK);
		return;
	}
} 

	CDialog::OnOK();

	return;

}

//
//	序列化函数
//
void CComCfg::Serialize(CArchive& ar) 
{
	if (ar.IsStoring())
	{	// storing code
	}
	else
	{	// loading code
	}
}


//
//	打开文件
//
void CComCfg::OnFileOpen() 
{
	// TODO: Add your command handler code here
	
}
/////////////////////////////////////////////////////////////////////////////
// CCmdCfg 对话框

//
// 构造函数
//
CCmdCfg::CCmdCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdCfg::IDD, pParent)
{

	//{{AFX_DATA_INIT(CCmdCfg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//
//	对话框数据交换
//
void CCmdCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdCfg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCmdCfg, CDialog)
	//{{AFX_MSG_MAP(CCmdCfg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdCfg 消息处理程序

//
//	更改“万用表”列表框内容时，根据使用仪器为温控表或是万能表控制对应选项是否可用
void CComCfg::OnSelchangeMultiMeter() 
{
	// TODO: Add your control notification handler code here
	if(F45==0){//modeified on 1/18/2000	
     GetDlgItem(IDC_MultiMeter)->EnableWindow(FALSE);
     GetDlgItem(IDC_DigitConctollor)->EnableWindow(TRUE);
	}
	if(F45==1){//modeified on 1/18/2000	
     GetDlgItem(IDC_MultiMeter)->EnableWindow(TRUE);
     GetDlgItem(IDC_DigitConctollor)->EnableWindow(FALSE);
	}

}
// 在文件最末尾添加 OnInitDialog 的实现：
BOOL CComCfg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_SERIALPORTCOMBO);
    if (pCombo != NULL) {
        pCombo->ResetContent(); // 清空原本固定的 COM1-COM4

        // 自动从系统注册表搜索真正存在的串口 (支持 USB 转串口)
        HKEY hKey;
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char szValueName[256], szPortName[256];
            DWORD dwIndex = 0, dwValueSize = 256, dwDataSize = 256, dwType;
            while (RegEnumValue(hKey, dwIndex++, szValueName, &dwValueSize, NULL, &dwType, (LPBYTE)szPortName, &dwDataSize) == ERROR_SUCCESS) {
                pCombo->AddString(szPortName);
                dwValueSize = 256; dwDataSize = 256;
            }
            RegCloseKey(hKey);
        }
        if (pCombo->GetCount() > 0) pCombo->SetCurSel(0);
    }
    return TRUE;
}