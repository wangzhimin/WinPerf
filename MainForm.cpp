//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "MainForm.h"
#include "About.h"

#include "Utility.h"


#include <fstream>
using namespace std;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "cspin"
#pragma resource "*.dfm"

TFormMain *FormMain;

//---------------------------------------------------------------------------
__fastcall TFormMain::TFormMain(TComponent* Owner)
    : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::FormCreate(TObject *Sender)
{
    m_PerfRunning = false;

    if (FormMain->Width == 0)
    {
        FormMain->Width = 1024;
    }

    if (FormMain->Height == 0)
    {
        FormMain->Height = 768;
    } //防止除0
    
    m_OptionHeightRatio = 100 * ScrollBoxOptions->Height / FormMain->Height;
    
    m_OutputWidthRatio = 100 * TabControlOutput->Width / FormMain->Width;
    m_OutputHeightRatio = 100 * TabControlOutput->Height / FormMain->Height;
    
    //默认的服务器/客户端模式选择
    SetClientMode(true);
    SetServerMode(false);

    //默认的协议选择
    SetUDPProtocol(true);    
    SetTCPProtocol(false);

    GetValueFromIni();
    
    UpdateCmdLineStr();

    BitBtnRun->Enabled = !EditIP->Text.IsEmpty(); ///默认是Client模式
    BitBtnStop->Enabled = false; //让stop按钮变灰色,不响应鼠标
 }

//---------------------------------------------------------------------------

void __fastcall TFormMain::FormResize(TObject *Sender)
{
    AjustControlPosition();    
}

//向显示控件中添加一行字符串
void __fastcall TFormMain::EditAddLines(const char* lineStr)
{
    RichEditShow->Lines->Append(AnsiString(lineStr)); //Append 会给行尾自动加 \r\n
    
    int lastIndex = RichEditShow->Lines->Count - 1;
    if (RichEditShow->Lines->Strings[lastIndex].IsEmpty())
    {
        RichEditShow->Lines->Delete(lastIndex); //删掉最后一行没有内容的字串
    }
}

//更新状态栏信息
void __fastcall TFormMain::ShowRunningInfo(char* str)
{
    StatusBarInfo->SimpleText = str;
}

bool __fastcall TFormMain::GetCmdLineStr(string& strCmdLine)   
{
    if (EditCmdLine->Text.SubString(0, 9) != "iperf.exe")
    {
        ShowRunningInfo("命令行未输入正确的执行参数!");
        return false;
    }

    strCmdLine = EditCmdLine->Text.c_str();

    return true;
}


int  __fastcall TFormMain::GetPrintInternal()
{
    return CSpinEditSeconds->Value;
}

/***************************************************************************/

//选择 Client 模式
void __fastcall TFormMain::RadioButtonClientClick(TObject *Sender)
{
    SetClientMode(true);
    SetServerMode(false); 

    SetTTL(true);
    SetToS(true);
    
    UpdateCmdLineStr();

    if (!m_PerfRunning) //需要手动停止      
    {
        BitBtnRun->Enabled = !EditIP->Text.IsEmpty();
    }
}

//选择 Server 模式
void __fastcall TFormMain::RadioButtonServerClick(TObject *Sender)
{
    SetClientMode(false);
    SetServerMode(true);

    SetTTL(false);
    SetToS(false);
    
    UpdateCmdLineStr();

    if (!m_PerfRunning)
    {
        BitBtnRun->Enabled = true;
    }
}
//---------------------------------------------------------------------------

//点击 run
void __fastcall TFormMain::BitBtnRunClick(TObject *Sender)
{
    if (RadioButtonClient->Checked && //客户端模式
        EditIP->Text.IsEmpty())       //需要server的IP地址
    {
        return;
    }
    
    BitBtnStop->Enabled = true;
    BitBtnRun->Enabled  = false;
    
    m_pRunIPerfThread = new TStartThread(false);
    m_pRunIPerfThread->OnTerminate = &TerminateCmdLine;

    m_PerfRunning = true;
}
//---------------------------------------------------------------------------

//点击 stop
void __fastcall TFormMain::BitBtnStopClick(TObject *Sender)
{
    /* 点击了stop后, 应当停止iperf进程, 并可以点击run了. */
    TerminateCmdLine(Sender);
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::BitBtnRestoreClick(TObject *Sender)
{
    RadioButtonClient->Checked = true;
    
    SetClientMode(true);
    SetServerMode(false);

    //默认的协议选择
    RadioButtonUDP->Checked = true;
    
    SetUDPProtocol(true);    
    SetTCPProtocol(false);

    //设置一些默认参数值
    CSpinEditTransmit->Value = 120;
    
    ComboBoxFormat->ItemIndex = 8;

    ComboBoxUDPBandwidth->ItemIndex = 1;
    CSpinEditUDPBandwidth->Value = 9;

    if (!m_PerfRunning && RadioButtonClient->Checked)
    {
        BitBtnRun->Enabled = !EditIP->Text.IsEmpty();
    }
    else if (RadioButtonServer->Checked)
    {
        BitBtnRun->Enabled = true;
    }
}
//---------------------------------------------------------------------------
void __fastcall TFormMain::EditIPChange(TObject *Sender)
{
    UpdateCmdLineStr();

    if (!m_PerfRunning && RadioButtonClient->Checked)
    {
        BitBtnRun->Enabled = !EditIP->Text.IsEmpty();
    }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

/* private function */

void __fastcall TFormMain::AjustControlPosition()
{
    if (FormMain->Width > 1000)
    {
        //安排Run,Stop,Restore三个按钮的位置随主窗口的变化而变化
        const int SpaceSize = 10;    
        BitBtnRestore->Left = FormMain->Width - 24 - BitBtnRestore->Width; //靠右
        BitBtnStop->Left = BitBtnRestore->Left - SpaceSize - BitBtnStop->Width;
        BitBtnRun->Left = BitBtnStop->Left - SpaceSize - BitBtnRun->Width;

        //调整 Output 宽度
        TabControlOutput->Width = FormMain->Width * m_OutputWidthRatio / 100;
        RichEditShow->Width = TabControlOutput->Width - 5;

        ButtonSaveAs->Left = TabControlOutput->Left + TabControlOutput->Width - 30 - ButtonSaveAs->Width;
        ButtonCls->Left = ButtonSaveAs->Left - 2*ButtonCls->Width;
    }

    if (FormMain->Height > 600)
    {
        //调整Option面板高度
        //if (FormMain->Height - ScrollBoxOptions->Top - StatusBarInfo->Height > 582)
        //{
        //    ScrollBoxOptions->Height = 582;
        //}
        
        int formHeight = FormMain->Height * m_OptionHeightRatio / 100;
        if (formHeight >= 582)
        {
            formHeight = 582;
        }
        ScrollBoxOptions->Height = formHeight;
    
        //调整Output高度
        TabControlOutput->Height = FormMain->Height * m_OutputHeightRatio / 100;
        RichEditShow->Height = TabControlOutput->Height - 23;
    }
}

//更新 cmdline 字符串
void __fastcall TFormMain::UpdateCmdLineStr()
{
    string strSpace = " "; //空格
    string strIperfCmdLine = "iperf.exe "; //执行程序名
    
    
    /* 选择 服务器/客户端 模式 */
    //-c and -s arguments must be given first. 
    string strMode;
    if (RadioButtonClient->Checked) //客户端
    {
        if (EditIP->Text.IsEmpty()) //无服务器地址
        {
            EditCmdLine->Font->Color = clRed;
            EditCmdLine->Text = "Please enter the host to connect to...";
            return;
        }
        
        //-c, --client    <host>   run in client mode, connecting to <host>
        strIperfCmdLine += "-c ";

        //主机地址
        strIperfCmdLine += EditIP->Text.c_str();
        strIperfCmdLine += strSpace;
    }
    else if (RadioButtonServer->Checked) //服务器
    {
        //-s, --server             run in server mode
        strIperfCmdLine += "-s ";

        if (CheckBoxLimit->Checked)
        {
            strIperfCmdLine += "-c ";
            strIperfCmdLine += EditLimit->Text.c_str();
            strIperfCmdLine += strSpace;
        }
    }

    //UDP or TCP
    if (RadioButtonUDP->Checked) //UDP
    {
        //-u, --udp                use UDP rather than TCP
        strIperfCmdLine += "-u "; //strUDP;
    }
    else if (RadioButtonTCP->Checked) //TCP
    {
        //默认,不用写
    }

    //-P, --parallel  #        number of parallel client threads to run
    if (RadioButtonClient->Checked) //客户端
    {
        //客户端要开几个线程
        strIperfCmdLine += "-P ";
        strIperfCmdLine += CSpinEditStream->Text.c_str();
        
        strIperfCmdLine += strSpace;
    }
        
    //-i, --interval  #        seconds between periodic bandwidth reports
    strIperfCmdLine += "-i ";
    strIperfCmdLine += CSpinEditSeconds->Text.c_str(); //是否应该判断一下Text是不是数字?
    strIperfCmdLine += strSpace;

    //-p, --port      #        server port to listen on/connect to
    strIperfCmdLine += "-p ";
    if (RadioButtonClient->Checked) //客户端
    {
        strIperfCmdLine += CSpinEditPort->Text.c_str();
    }
    else if (RadioButtonServer->Checked) //服务器
    {
        strIperfCmdLine += CSpinEditListenPort->Text.c_str();
    }

    strIperfCmdLine += strSpace;
    
    //-m, --print_mss          print TCP maximum segment size (MTU - TCP/IP header)
    if (RadioButtonTCP->Checked && CheckBoxMSS->Checked) //tcp, MSS
    {
        strIperfCmdLine += "-m ";        
    }
    
    //-w, --window    #[KM]    TCP window size (socket buffer size)
    if (RadioButtonUDP->Checked && CheckBoxUDPBuffSize->Checked) //udp, socket buffer size
    {
        strIperfCmdLine += "-w ";
        strIperfCmdLine += CSpinEditUDPBuffSize->Text.c_str(); //数值
        strIperfCmdLine += Utility::GetUnitStr(ComboBoxUDPBuffSize->ItemIndex); //单位的字符串
        strIperfCmdLine += strSpace;
    }
    else if (RadioButtonTCP->Checked && CheckBoxTCPWindowSize->Checked) //tcp, window size
    { 
        strIperfCmdLine += "-w ";
        strIperfCmdLine += CSpinEditTCPWinSize->Text.c_str();
        strIperfCmdLine += Utility::GetUnitStr(ComboBoxTCPWinSize->ItemIndex);
        strIperfCmdLine += strSpace;
    }

    //-B, --bind      <host>   bind to <host>, an interface or multicast address
    if (!EditBindHost->Text.IsEmpty())
    {
        strIperfCmdLine += "-B ";
        strIperfCmdLine += EditBindHost->Text.c_str();
        strIperfCmdLine += strSpace;
    }
    
    if (RadioButtonTCP->Checked)
    {
        //-M, --mss       #        set TCP maximum segment size (MTU - 40 bytes)
        if (CheckBoxMaxSegSize->Checked)
        {
            strIperfCmdLine += "-M ";
            strIperfCmdLine += CSpinEditMaxSegSize->Text.c_str();
            strIperfCmdLine += Utility::GetUnitStr(ComboBoxMaxSegSize->ItemIndex);

            strIperfCmdLine += strSpace;
        }
        
        //-N, --nodelay            set TCP no delay, disabling Nagle's Algorithm
        if (CheckBoxTCPNoDelay->Checked)
        {
            strIperfCmdLine += "-N ";
        }
    }

    //-l, --len       #[KM]    length of buffer to read or write (default 8 KB)
    //In UDP, this sets the packet size.
    if (RadioButtonUDP->Checked && CheckBoxUDPPacketSize->Checked) //udp, packet size
    {
        strIperfCmdLine += "-l ";
        strIperfCmdLine += CSpinEditUDPPacketSize->Text.c_str(); //数值
        strIperfCmdLine += Utility::GetUnitStr(ComboBoxUDPPacketSize->ItemIndex); //单位的字符串
        strIperfCmdLine += strSpace;
    }
    else if (RadioButtonTCP->Checked && CheckBoxBuffLength->Checked) //tcp, buffer len
    { 
        strIperfCmdLine += "-l ";
        strIperfCmdLine += CSpinEditBuffLength->Text.c_str();
        strIperfCmdLine += Utility::GetUnitStr(ComboBoxBuffLength->ItemIndex);
        strIperfCmdLine += strSpace;
    }
    
    //-V, --IPv6Version        Set the domain to IPv6
    if (CheckBoxIPv6->Checked)
    {
        strIperfCmdLine += "-V ";
    }

    //-f, --format    [kmKM]   format to report: Kbits, Mbits, KBytes, MBytes
    strIperfCmdLine += "-f ";
    strIperfCmdLine += Utility::GetOutputFormatUnitStr(ComboBoxFormat->ItemIndex);
    strIperfCmdLine += strSpace;

    
    //-b, --bandwidth #[KM]    for UDP, bandwidth to send at in bits/sec (default 1 Mbit/sec, implies -u)
    if (RadioButtonClient->Checked && RadioButtonUDP->Checked)
    {
        strIperfCmdLine += "-b ";
        strIperfCmdLine += CSpinEditUDPBandwidth->Text.c_str();
        strIperfCmdLine += Utility::GetUDPBandUnitStr(ComboBoxUDPBandwidth->ItemIndex);

        strIperfCmdLine += strSpace;
    }

    //-t, --time      #        time in seconds to transmit for (default 10 secs)
    //-n, --num       #[KM]    number of bytes to transmit (instead of -t)
    if (RadioButtonClient->Checked)
    {
        if (RadioButtonSecond->Checked)
        {
            strIperfCmdLine += "-t ";
        }
        else if (RadioButtonBytes->Checked)
        {
            strIperfCmdLine += "-n ";
        }
        strIperfCmdLine += CSpinEditTransmit->Text.c_str();

        strIperfCmdLine += strSpace;

        //-C, --compatibility      for use with older versions does not sent extra msgs
        if (CheckBoxCompatibility->Checked)
        {
            strIperfCmdLine += "-C ";            
        }
        else
        {
            //-d, --dualtest           Do a bidirectional test simultaneously
            if (CheckBoxDual->Checked)
            {
                strIperfCmdLine += "-d ";
            }

            //-r, --tradeoff           Do a bidirectional test individually
            if (CheckBoxTrade->Checked)
            {
                strIperfCmdLine += "-r ";
            }

            //-L, --listenport #       port to recieve bidirectional tests back on
            if (CheckBoxDual->Checked || CheckBoxTrade->Checked)
            {
                strIperfCmdLine += "-L ";
                strIperfCmdLine += CSpinEditTestPort->Text.c_str();
                strIperfCmdLine += strSpace;
            }
        }
    }
    
    //-T, --ttl       #        time-to-live, for multicast (default 1)
    if (RadioButtonClient->Checked)
    {
        strIperfCmdLine += "-T ";
        strIperfCmdLine += CSpinEditTTL->Text.c_str();
        strIperfCmdLine += strSpace;
    }    
    
    //-S ToS
    if (ComboBoxTOS->Text != "None")
    {
        strIperfCmdLine += Utility::GetToSStr(ComboBoxTOS->ItemIndex);
    }
    
    //-F, --fileinput <name>   input the data to be transmitted from a file
    //-I, --stdin              input the data to be transmitted from stdin
    //-o, --output    <filename> output the report or error message to this specified file

    
    EditCmdLine->Font->Color = clNavy; //深蓝色
    EditCmdLine->Text = strIperfCmdLine.c_str();
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::SetClientMode(bool enabled)
{
    //设置Client的各选项
    LabelServerIP->Enabled   = enabled;
    EditIP->Enabled          = enabled;

    LabelPort->Enabled       = enabled;
    CSpinEditPort->Enabled   = enabled;

    LabelParrallel->Enabled  = enabled;
    CSpinEditStream->Enabled = enabled;

    //disable transmit
    LabelTransmit->Enabled     = enabled;
    CSpinEditTransmit->Enabled = enabled;
    RadioButtonBytes->Enabled  = enabled;
    RadioButtonSecond->Enabled = enabled;

    if (!CheckBoxCompatibility->Checked)
    {
        SetTestMode(enabled);
    }
    
    //udp
    SetUDPBandWidth(enabled);

    //ttl, tos
    SetTTL(enabled);
    SetToS(enabled);
}

//server中的选项
void __fastcall TFormMain::SetServerMode(bool enabled)
{
    //设置Server选项
    LabelListenPort->Enabled     = enabled;
    CSpinEditListenPort->Enabled = enabled;

    LabelConn->Enabled           = enabled;
    CSpinEditConn->Enabled       = enabled;

    CheckBoxLimit->Enabled       = enabled;    
    EditLimit->Enabled = CheckBoxLimit->Checked ? enabled : false;
}

void __fastcall TFormMain::SetTestMode(bool enabled)
{
    //test mode
    LabelTestMode->Enabled     = enabled;
    CheckBoxDual->Enabled      = enabled;
    CheckBoxTrade->Enabled     = enabled;
    LabelTestPort->Enabled     = enabled;
    CSpinEditTestPort->Enabled = enabled;
}
    
//---------------------------------------------------------------------------
//设置 UDP 协议的参数激活状态
void __fastcall TFormMain::SetUDPProtocol(bool enabled)
{
    //UDP Bandwidth
    if (RadioButtonClient->Checked)
    {
        SetUDPBandWidth(enabled);
    }

    //UDP Buffer Size
    CheckBoxUDPBuffSize->Enabled = enabled;
    //没勾,后面的选项不应当响应
    CSpinEditUDPBuffSize->Enabled = CheckBoxUDPBuffSize->Checked ? enabled : false;
    ComboBoxUDPBuffSize->Enabled  = CSpinEditUDPBuffSize->Enabled;

    //UDP Packet Size
    CheckBoxUDPPacketSize->Enabled = enabled;
    CSpinEditUDPPacketSize->Enabled = CheckBoxUDPPacketSize->Checked ? enabled : false;
    ComboBoxUDPPacketSize->Enabled  = CSpinEditUDPPacketSize->Enabled;
}

//设置TCP协议相关参数的激活状态
void __fastcall TFormMain::SetTCPProtocol(bool enabled)
{
    CheckBoxMSS->Enabled = enabled;
    
    //TCP Buffer Length
    CheckBoxBuffLength->Enabled = enabled;
    CSpinEditBuffLength->Enabled = CheckBoxBuffLength->Checked ? enabled : false;
    ComboBoxBuffLength->Enabled  = CSpinEditBuffLength->Enabled;
            
    //TCP Windos Size
    CheckBoxTCPWindowSize->Enabled = enabled;
    CSpinEditTCPWinSize->Enabled = CheckBoxTCPWindowSize->Checked ? enabled : false;
    ComboBoxTCPWinSize->Enabled  = CSpinEditTCPWinSize->Enabled;
    
    //Max Segment Size
    CheckBoxMaxSegSize->Enabled = enabled;
    CSpinEditMaxSegSize->Enabled = CheckBoxMaxSegSize->Checked ? enabled : false;
    ComboBoxMaxSegSize->Enabled  = CSpinEditMaxSegSize->Enabled;

    //TCP No Delay
    CheckBoxTCPNoDelay->Enabled = enabled;
}

//---------------------------------------------------------------------------
void __fastcall TFormMain::SetUDPBandWidth(bool enabled)
{
    //UDP Bandwidth
    LabelUDPBandwidth->Enabled     = RadioButtonClient->Checked ? enabled : false;
    CSpinEditUDPBandwidth->Enabled = RadioButtonClient->Checked ? enabled : false;
    ComboBoxUDPBandwidth->Enabled  = RadioButtonClient->Checked ? enabled : false;
}

void __fastcall TFormMain::SetTTL(bool enabled)
{
    LabelTTL->Enabled     = enabled;
    CSpinEditTTL->Enabled = enabled;
}

void __fastcall TFormMain::SetToS(bool enabled)
{
    LabelTOS->Enabled    = enabled;
    ComboBoxTOS->Enabled = enabled;
}
//---------------------------------------------------------------------------

/* 点击来stop后, 或cmdline的线程执行结束后, 会调用这个函数. */
void __fastcall TFormMain::TerminateCmdLine(TObject *Sender)
{
    if (m_pRunIPerfThread != NULL)
    {
        //怎么停止iperf进程?
        m_pRunIPerfThread->TerminateCmd(); //此函数中停止iperf进程
        
        m_pRunIPerfThread->Suspend();
        m_pRunIPerfThread->Terminate();

        //delete m_pRunIPerfThread; //不需要delete吗？？
        m_pRunIPerfThread = NULL;
    }
    
    m_PerfRunning = false;

    if (RadioButtonClient->Checked)
    {
    	BitBtnRun->Enabled = !EditIP->Text.IsEmpty();  //可以启动iperf ?
    }
    else if (RadioButtonServer->Checked)
    {
        BitBtnRun->Enabled = true;
    }
    BitBtnStop->Enabled = false; //不能停止iperf
}

void __fastcall TFormMain::SaveIniConfig()
{
    try
    {
        char strSrvIP[25];
        strcpy(strSrvIP, EditIP->Text.c_str());
        winperfIni.SetServerIP(strSrvIP);

        char strSrvPort[10];
        strcpy(strSrvPort, CSpinEditPort->Text.c_str());
        winperfIni.SetServerPort(strSrvPort);

        char strCPort[10];
        strcpy(strCPort, CSpinEditListenPort->Text.c_str());
        winperfIni.SetClientPort(strCPort);
    }
    catch(std::exception& ex)
    {
        MessageBox(NULL, "hello", ex.what(), MB_OK);
    }

    winperfIni.WriteIniFile();
}

void __fastcall TFormMain::GetValueFromIni()
{
    CPerfIni::FileSituation res = winperfIni.ReadIni();

    if (CPerfIni::READ_OLD_INI == res)
    {
        EditIP->Text = winperfIni.GetServerIP().c_str();
        CSpinEditPort->Text = winperfIni.GetServerPort().c_str();
        CSpinEditListenPort->Text = winperfIni.GetClinePort().c_str();
    }
}

/***************************************************************************/

//选择 UDP
void __fastcall TFormMain::RadioButtonUDPClick(TObject *Sender)
{
    SetUDPProtocol(true);    
    SetTCPProtocol(false);

    SetTTL(true);
    
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

//选择 TCP
void __fastcall TFormMain::RadioButtonTCPClick(TObject *Sender)
{
    SetUDPProtocol(false);    
    SetTCPProtocol(true);

    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxUDPBuffSizeClick(TObject *Sender)
{
    CSpinEditUDPBuffSize->Enabled = CheckBoxUDPBuffSize->Checked;
    ComboBoxUDPBuffSize->Enabled = CheckBoxUDPBuffSize->Checked;

    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxUDPPacketSizeClick(TObject *Sender)
{
    CSpinEditUDPPacketSize->Enabled = CheckBoxUDPPacketSize->Checked;
    ComboBoxUDPPacketSize->Enabled = CheckBoxUDPPacketSize->Checked;

    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxBuffLengthClick(TObject *Sender)
{
    CSpinEditBuffLength->Enabled = CheckBoxBuffLength->Checked;
    ComboBoxBuffLength->Enabled  = CheckBoxBuffLength->Checked;

    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxTCPWindowSizeClick(TObject *Sender)
{
    CSpinEditTCPWinSize->Enabled = CheckBoxTCPWindowSize->Checked;
    ComboBoxTCPWinSize->Enabled  = CheckBoxTCPWindowSize->Checked;

    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxMaxSegSizeClick(TObject *Sender)
{
    CSpinEditMaxSegSize->Enabled = CheckBoxMaxSegSize->Checked;
    ComboBoxMaxSegSize->Enabled = CheckBoxMaxSegSize->Checked;

    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxTCPNoDelayClick(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxIPv6Click(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxLimitClick(TObject *Sender)
{
    EditLimit->Enabled = CheckBoxLimit->Checked;
    
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxCompatibilityClick(TObject *Sender)
{
    bool enabled = !CheckBoxCompatibility->Checked;

    //要根据ClientMode的选择与否来决定
    if (RadioButtonClient->Checked)
    {
        SetTestMode(enabled);
    }
    
    UpdateCmdLineStr();
}

void __fastcall TFormMain::CheckBoxMSSClick(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::RadioButtonBytesClick(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::RadioButtonSecondClick(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxDualClick(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CheckBoxTradeClick(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::CSpinEditPortChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditStreamChange(TObject *Sender)
{
    UpdateCmdLineStr();
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditListenPortChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::EditLimitChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditConnChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditTransmitChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxFormatChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditSecondsChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditTestPortChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditUDPBandwidthChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxUDPBandwidthChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditUDPBuffSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxUDPBuffSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditUDPPacketSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxUDPPacketSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditBuffLengthChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxBuffLengthChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditTCPWinSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxTCPWinSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditMaxSegSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxMaxSegSizeChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::CSpinEditTTLChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ComboBoxTOSChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::EditBindHostChange(TObject *Sender)
{
    UpdateCmdLineStr();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::AboutMenuClick(TObject *Sender)
{
    AboutBox->ShowModal();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

//全选
void __fastcall TFormMain::SelectAllClick(TObject *Sender)
{
    RichEditShow->SelectAll();
}
//---------------------------------------------------------------------------

//copy
void __fastcall TFormMain::CopySelectionClick(TObject *Sender)
{
    RichEditShow->CopyToClipboard();
}
//---------------------------------------------------------------------------

//clear
void __fastcall TFormMain::ClearEditClick(TObject *Sender)
{
    RichEditShow->Clear();
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::FormClose(TObject *Sender, TCloseAction &Action)
{
    TerminateCmdLine(Sender);
}
//---------------------------------------------------------------------------


void __fastcall TFormMain::ButtonClsClick(TObject *Sender)
{
    RichEditShow->Clear();    
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ButtonSaveAsClick(TObject *Sender)
{
    if (RichEditShow->Lines->Count > 0)
    {
        if (SaveTextFileDialogShow->Execute())
        {
            AnsiString saveFileName = SaveTextFileDialogShow->FileName;
            RichEditShow->Lines->SaveToFile(saveFileName);
        }
    }
}
//---------------------------------------------------------------------------

void __fastcall TFormMain::ExitMenuClick(TObject *Sender)
{
    Close();    
}
//---------------------------------------------------------------------------



void __fastcall TFormMain::SaveConfigClick(TObject *Sender)
{
    SaveIniConfig();    
}
//---------------------------------------------------------------------------

