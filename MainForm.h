//---------------------------------------------------------------------------

#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>

#include "UnitStartThread.h"
#include <Menus.hpp>
#include <Buttons.hpp>
#include "cspin.h"
#include <ExtCtrls.hpp>
#include <ActnList.hpp>
#include <ExtActns.hpp>
#include <StdActns.hpp>

#include "Common.h"
#include <Dialogs.hpp>
#include <ExtDlgs.hpp>

#include <fstream>
#include <string>
using namespace std;

#include "CPerfIni.h"

//---------------------------------------------------------------------------
class TFormMain : public TForm
{
__published:	// IDE-managed Components

    TMainMenu *MainMenuHelp;
    TMenuItem *HelpMemu;
    TMenuItem *AboutMenu;

    /* Iperf 模式选择 */
    TGroupBox *GroupBoxIperf;
    
    TLabel * LabelCmdLine;
    TEdit *  EditCmdLine; //cmdline字符串

    TLabel * LabelMode;
    
    //Client
    TGroupBox *GroupBoxClient;
    
    TRadioButton *RadioButtonClient;
    
    TLabel * LabelServerIP;
    TEdit *  EditIP;

    TLabel * LabelPort;
    TCSpinEdit *CSpinEditPort;

    TLabel * LabelParrallel;
    TCSpinEdit *CSpinEditStream;

    //Server
    TGroupBox *GroupBoxServer;
    
    TRadioButton *RadioButtonServer;
    
    TLabel *LabelListenPort;
    TCSpinEdit *CSpinEditListenPort;    
    
    TCheckBox *CheckBoxLimit;
    TEdit *EditLimit;

    TLabel *LabelConn;
    TCSpinEdit *CSpinEditConn;


    /* 三个 iperf 主控制按钮 */
    TBitBtn *BitBtnRun;    
    TBitBtn *BitBtnStop;
    TBitBtn *BitBtnRestore;
        

    TScrollBox *ScrollBoxOptions;
    
    /* 应用程序选项 */
    TPanel *PanelOptions;
    
    TCheckBox *CheckBoxCompatibility;
    
    TLabel *LabelTransmit;
    TCSpinEdit *CSpinEditTransmit;
    
    TRadioButton *RadioButtonSecond;
    TRadioButton *RadioButtonBytes;
    
    TLabel *LabelOutPutFormat;
    TComboBox *ComboBoxFormat;
    
    TLabel *LabelInterval;
    TCSpinEdit *CSpinEditSeconds;
    TLabel *LabelSeconds;
    
    TLabel *LabelTestMode;
    TCheckBox *CheckBoxDual;
    TCheckBox *CheckBoxTrade;
    
    TLabel *LabelTestPort;
    TCSpinEdit *CSpinEditTestPort;
    
    TCheckBox *CheckBoxMSS;        

    /* 协议选项参数 */
    TPanel *PanelProtocol;
    
    TLabel *LabelChoose;

    /** UDP **/
    TRadioButton *RadioButtonUDP; //UDP 单选框

    TGroupBox *GroupBoxUDP;
    //UDP Bandwidth
    TLabel *LabelUDPBandwidth;
    TCSpinEdit *CSpinEditUDPBandwidth;
    TComboBox *ComboBoxUDPBandwidth;

    //UDP Buffer Size
    TCheckBox *CheckBoxUDPBuffSize;
    TCSpinEdit *CSpinEditUDPBuffSize;
    TComboBox *ComboBoxUDPBuffSize;

    //UDP Packet Size
    TCheckBox *CheckBoxUDPPacketSize;
    TCSpinEdit *CSpinEditUDPPacketSize;
    TComboBox *ComboBoxUDPPacketSize;

    /** TCP **/
    TRadioButton *RadioButtonTCP; //TCP 单选框

    TGroupBox *GroupBoxTCP;
    //TCP Buffer Length
    TCheckBox *CheckBoxBuffLength;
    TCSpinEdit *CSpinEditBuffLength;
    TComboBox *ComboBoxBuffLength;
        
    //TCP Windos Size
    TCheckBox *CheckBoxTCPWindowSize;
    TCSpinEdit *CSpinEditTCPWinSize;
    TComboBox *ComboBoxTCPWinSize;
    
    //Max Segment Size
    TCheckBox *CheckBoxMaxSegSize;
    TCSpinEdit *CSpinEditMaxSegSize;
    TComboBox *ComboBoxMaxSegSize;

    //TCP No Delay
    TCheckBox *CheckBoxTCPNoDelay;

    /* IP 选项参数 */
    TPanel *PanelIPOptions; //容器面板
    
    TLabel *LabelTTL;
    TCSpinEdit *CSpinEditTTL;
    
    TLabel *LabelTOS;
    TComboBox *ComboBoxTOS;
    
    TLabel *LabelBindHost;
    TEdit *EditBindHost;

    TCheckBox *CheckBoxIPv6; //IPv6
    
    
    /* 文本输出 */
    TTabControl *TabControlOutput;        
    TRichEdit *RichEditShow;
    
    TPopupMenu *PopupMenuEdit;

    //状态栏
    TStatusBar *StatusBarInfo;
    
    TMenuItem *CopySelection;
    TMenuItem *ClearEdit;
    TMenuItem *SelectAll;
    TButton *ButtonCls;
    TButton *ButtonSaveAs;
    TSaveTextFileDialog *SaveTextFileDialogShow;
    TMenuItem *FileMenu;
    TMenuItem *ExitMenu;
    TMenuItem *ConfigMenu;
    TMenuItem *SaveConfig;
    
    
    /* 接口函数. */    
    void __fastcall FormCreate(TObject *Sender);
    void __fastcall FormResize(TObject *Sender);

    void __fastcall EditAddLines(const char* lineStr);
    void __fastcall ShowRunningInfo(char* str);
    
    bool __fastcall GetCmdLineStr(string& strCmdLine);

    int  __fastcall GetPrintInternal();
    
    //三个主按钮的响应函数
    void __fastcall BitBtnRunClick(TObject *Sender);
    void __fastcall BitBtnStopClick(TObject *Sender);
    void __fastcall BitBtnRestoreClick(TObject *Sender);

    void __fastcall RadioButtonClientClick(TObject *Sender);
    void __fastcall RadioButtonServerClick(TObject *Sender);

    void __fastcall CheckBoxCompatibilityClick(TObject *Sender);
    
    void __fastcall RadioButtonBytesClick(TObject *Sender);
    void __fastcall RadioButtonSecondClick(TObject *Sender);

    
    void __fastcall EditIPChange(TObject *Sender);
    
    void __fastcall RadioButtonUDPClick(TObject *Sender);
    void __fastcall RadioButtonTCPClick(TObject *Sender);
    
    void __fastcall CheckBoxUDPBuffSizeClick(TObject *Sender);
    void __fastcall CheckBoxUDPPacketSizeClick(TObject *Sender);
    void __fastcall CheckBoxBuffLengthClick(TObject *Sender);
    void __fastcall CheckBoxTCPWindowSizeClick(TObject *Sender);
    void __fastcall CheckBoxMaxSegSizeClick(TObject *Sender);
    void __fastcall CheckBoxTCPNoDelayClick(TObject *Sender);
    void __fastcall CheckBoxIPv6Click(TObject *Sender);
    void __fastcall CheckBoxLimitClick(TObject *Sender);
    
    void __fastcall CheckBoxMSSClick(TObject *Sender);
    void __fastcall CheckBoxDualClick(TObject *Sender);
    void __fastcall CheckBoxTradeClick(TObject *Sender);
    void __fastcall CSpinEditPortChange(TObject *Sender);
    void __fastcall CSpinEditStreamChange(TObject *Sender);
    void __fastcall CSpinEditListenPortChange(TObject *Sender);
    void __fastcall EditLimitChange(TObject *Sender);
    void __fastcall CSpinEditConnChange(TObject *Sender);
    void __fastcall CSpinEditTransmitChange(TObject *Sender);
    void __fastcall ComboBoxFormatChange(TObject *Sender);
    void __fastcall CSpinEditSecondsChange(TObject *Sender);
    void __fastcall CSpinEditTestPortChange(TObject *Sender);
    void __fastcall CSpinEditUDPBandwidthChange(TObject *Sender);
    void __fastcall ComboBoxUDPBandwidthChange(TObject *Sender);
    void __fastcall CSpinEditUDPBuffSizeChange(TObject *Sender);
    void __fastcall ComboBoxUDPBuffSizeChange(TObject *Sender);
    void __fastcall CSpinEditUDPPacketSizeChange(TObject *Sender);
    void __fastcall ComboBoxUDPPacketSizeChange(TObject *Sender);
    void __fastcall CSpinEditBuffLengthChange(TObject *Sender);
    void __fastcall ComboBoxBuffLengthChange(TObject *Sender);
    void __fastcall CSpinEditTCPWinSizeChange(TObject *Sender);
    void __fastcall ComboBoxTCPWinSizeChange(TObject *Sender);
    void __fastcall CSpinEditMaxSegSizeChange(TObject *Sender);
    void __fastcall ComboBoxMaxSegSizeChange(TObject *Sender);
    void __fastcall CSpinEditTTLChange(TObject *Sender);
    void __fastcall ComboBoxTOSChange(TObject *Sender);
    void __fastcall EditBindHostChange(TObject *Sender);
    void __fastcall AboutMenuClick(TObject *Sender);
    void __fastcall ClearEditClick(TObject *Sender);
    void __fastcall CopySelectionClick(TObject *Sender);
    void __fastcall SelectAllClick(TObject *Sender);

    void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
    
    void __fastcall ButtonClsClick(TObject *Sender);
    void __fastcall ButtonSaveAsClick(TObject *Sender);
    void __fastcall ExitMenuClick(TObject *Sender);
    void __fastcall SaveConfigClick(TObject *Sender);

    
private:	// User declarations
    TStartThread* m_pRunIPerfThread;

    bool m_PerfRunning; //是否正在运行

    int m_OptionHeightRatio; //Option面板的高度比例
    
    int m_OutputWidthRatio;
    int m_OutputHeightRatio;

    CPerfIni winperfIni;
   
    void __fastcall AjustControlPosition();

    void __fastcall UpdateCmdLineStr(); //更新cmdline字符串
    
    void __fastcall SetClientMode(bool enabled);
    void __fastcall SetServerMode(bool enabled);

    void __fastcall SetTestMode(bool enabled);
    
    void __fastcall SetUDPProtocol(bool enabled);
    void __fastcall SetTCPProtocol(bool enabled);

    void __fastcall SetUDPBandWidth(bool enabled);
    void __fastcall SetTTL(bool enabled);
    void __fastcall SetToS(bool enabled);

    void __fastcall TerminateCmdLine(TObject *Sender);

    void __fastcall SaveIniConfig();
    void __fastcall GetValueFromIni();

public:		// User declarations
    __fastcall TFormMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TFormMain *FormMain;
//---------------------------------------------------------------------------
#endif
