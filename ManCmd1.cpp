/*
**	FILENAME			ManCmd1.cpp
**
**	PURPOSE				此类可用于将“命令”文本框内的命令字符串
**						按相应的通讯协议修改后传输入万用表或温控
**						表，若仪表有结果返回则将结果字符串显示在
**						相应文本框内
*/

#include "stdafx.h"
#include "ptcr.h"
#include "ManCmd1.h"
#include "ptcrdoc.h"
#include "string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern int F45;
/////////////////////////////////////////////////////////////////////////////
// CManCmd对话框

//
//	构造函数：初始化并打开对话框
//
CManCmd::CManCmd(CPtcrDoc *pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CManCmd::IDD, pParent)
{
    m_pDoc=pDoc;
	//{{AFX_DATA_INIT(CManCmd)
	m_Cmd = _T("");
	m_Result = _T("");
	//}}AFX_DATA_INIT
}

//
//	对话框数据交换
//
void CManCmd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CManCmd)
	DDX_Text(pDX, IDC_CMD_EDIT, m_Cmd);
	DDX_Text(pDX, IDC_RESULT_EDIT, m_Result);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CManCmd, CDialog)
	//{{AFX_MSG_MAP(CManCmd)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CManCmd消息处理程序

//
//	点击“发送”键发送命令，显示返回的结果
//
void CManCmd::OnOK() 
{
    int Sr253BCC;
    if(F45==0) Sr253BCC=m_pDoc->m_sPort.m_rxdata.Sr253BCC;
    else Sr253BCC=m_pDoc->m_sPortF45.m_rxdata.Sr253BCC;	

    // ================= 【加上这一行！】强制接管 =================
    Sr253BCC = 10; // 不管系统原来是什么协议，在这里强行使用宇电的十六进制协议！
    // ==========================================================

    UpdateData(TRUE);
    if(m_Cmd.IsEmpty()) return;

    // ================= 宇电 MODBUS 十六进制收发专线 =================
    if (Sr253BCC == 10) {
        ::SendMessage(AfxGetMainWnd()->m_hWnd, WM_STATUS_MESSAGE, 0, AFX_IDS_SENDCMD);
        
        // 1. 过滤掉用户输入中的空格
        CString strInput = m_Cmd;
        
        int byteLen = strInput.GetLength() / 2;
        if (byteLen == 0) {
            MessageBox("请输入有效的十六进制命令！", "提示", MB_OK);
            return;
        }

        // 2. 将字符串转换为二进制字节流
        unsigned char* sendBuf = new unsigned char[byteLen];
        for (int i = 0; i < byteLen; i++) {
            CString byteStr = strInput.Mid(i * 2, 2);
            int val;
            sscanf(byteStr, "%x", &val); // 将十六进制文本转为真实数值
            sendBuf[i] = (unsigned char)val;
        }

        // 3. 清空接收缓冲区并发送纯二进制数据
        m_pDoc->m_sPort.m_rxdata.count = 0;
        m_pDoc->m_sPort.m_rxdata.theend = FALSE;
        memset(m_pDoc->m_sPort.m_rxdata.results, 0, sizeof(m_pDoc->m_sPort.m_rxdata.results));
        
        m_pDoc->m_sPort.WriteToPortBinary((char*)sendBuf, byteLen);

        // 4. 等待仪表返回数据
        CTime ct1 = CTime::GetCurrentTime();
        MSG msg;
        CString strResponse = "";
        while (1) {
            ::PostMessage(AfxGetMainWnd()->m_hWnd, WM_COMM_RXLINE, 1, 1);
            GetMessage(&msg, NULL, 0, 0);
            
            // 如果接收到了数据
            if (m_pDoc->m_sPort.m_rxdata.count > 0) {
                Sleep(150); // 微小延时，确保这一包的字节全部进站
                int rxLen = m_pDoc->m_sPort.m_rxdata.count;
                
                // 将接收到的二进制字节流，转换回十六进制文本显示给用户
                for (int k = 0; k < rxLen; k++) {
                    CString hexFmt;
                    hexFmt.Format("%02X ", (unsigned char)m_pDoc->m_sPort.m_rxdata.results[k]);
                    strResponse += hexFmt;
                }
                break;
            } else {
                // 2秒超时判定
                if ((CTime::GetCurrentTime() - ct1).GetTotalSeconds() >= 2) {
                    strResponse = "接收超时，请检查接线或命令格式！";
                    break;
                }
            }
        }

        delete[] sendBuf;
        m_Result = strResponse; // 把结果显示在下面的文本框中
        UpdateData(FALSE);
        ::SendMessage(AfxGetMainWnd()->m_hWnd, WM_STATUS_MESSAGE, 0, AFX_IDS_FINISH);
        return; // 宇电处理完毕，直接返回
    }
    // ========================================================================

    // === 下面是原有的其它仪表的 ASCII 处理逻辑，保持不变 ===
    char cmd[50];
    char results[128];
    results[0]=0;

    if(Sr253BCC==6){//万用表
        strcpy(cmd,m_Cmd.GetBuffer(50));
        if(m_pDoc->MultiMeter==1)  strcat(cmd,"\r");
        if(m_pDoc->MultiMeter==4)  strcat(cmd,"\n");
        if(m_pDoc->MultiMeter==5)  strcat(cmd,"\r\n");	
    }
    else if(Sr253BCC==2){//温控表，协议SR253
        strcpy(cmd,m_Cmd.GetBuffer(50));		
    }
    else if(Sr253BCC==3){
        strcpy(cmd,"@");
        strcat(cmd,m_Cmd.GetBuffer(50));
        strcat(cmd,":\r");
    }
    else if(Sr253BCC==4){//温控表，协议SR93
        strcpy(cmd,m_Cmd.GetBuffer(50));
    }

    ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUS_MESSAGE,0,AFX_IDS_SENDCMD);
    if(!m_pDoc->ReadPvsv(cmd,results,4,3,Sr253BCC))	
    {
        MessageBox("操作不成功!","提示",MB_OK);	
    }
    if(strlen(results))	
        results[strlen(results)-1]=0;
    m_Result=results;	
    UpdateData(FALSE);
    ::SendMessage(AfxGetMainWnd()->m_hWnd,WM_STATUS_MESSAGE,0,AFX_IDS_FINISH);
}
