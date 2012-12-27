//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "UnitStartThread.h"

#include "MainForm.h"

#include <fstream>
#include <string>
#include <stdexcept>
using namespace std;

#pragma package(smart_init)
//---------------------------------------------------------------------------

//   Important: Methods and properties of objects in VCL can only be
//   used in a method called using Synchronize, for example:
//
//      Synchronize(&UpdateCaption);
//
//   where UpdateCaption could look like:
//
//      void __fastcall TStartThread::UpdateCaption()
//      {
//        Form1->Caption = "Updated in a thread";
//      }
//---------------------------------------------------------------------------

__fastcall TStartThread::TStartThread(bool CreateSuspended)
    : TThread(CreateSuspended)
{
    //FreeOnTerminate = true; //线程执行完后自己退出
}
//---------------------------------------------------------------------------
void __fastcall TStartThread::Execute()
{
    try
    {
        _execute();
    }
    catch(exception& ex)
    {
        string strInfo = "异常信息:";
        strInfo += ex.what();

        MessageBox(Application->Handle, strInfo.c_str(), "警告", MB_OK);
    }

}
//---------------------------------------------------------------------------
//强制中断cmdline的执行
void __fastcall TStartThread::TerminateCmd()
{
    CloseHandle(m_hCmdThread); 
        
    //强制中止cmdline进程
    if(m_hCmdProcess != NULL)
    {
        int code = 0;
        TerminateProcess(m_hCmdProcess, code);

        CloseHandle(m_hCmdProcess); 
        m_hCmdProcess = NULL;
    }
}


void TStartThread::_execute()
{
    string strCmdLine;
    if (!FormMain->GetCmdLineStr(strCmdLine))
    {
        return;
    }
    
    SECURITY_ATTRIBUTES tSecurityAttr = {sizeof(tSecurityAttr), NULL, TRUE};
    SECURITY_ATTRIBUTES* psa = NULL; 
    DWORD dwShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE; 
    OSVERSIONINFO osVersion = {0}; 
    osVersion.dwOSVersionInfoSize = sizeof(osVersion);

    //根据版本设置共享模式和安全属性
    if(GetVersionEx(&osVersion))
    { 
        if(osVersion.dwPlatformId == VER_PLATFORM_WIN32_NT)
        { 
            psa = &tSecurityAttr; 
            dwShareMode |= FILE_SHARE_DELETE; 
        } 
    } 

    HANDLE   hRead, hWrite;
    if (!CreatePipe(&hRead, &hWrite, &tSecurityAttr, 0))
    {   
        FormMain->ShowRunningInfo("线程被中断!");
        return;
    }
    
  
    //---- Place thread code here ----
    STARTUPINFO StartUpInfo = {sizeof(StartUpInfo),}; 
    StartUpInfo.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; //使用标准柄和显示窗口 
    StartUpInfo.hStdOutput  = hWrite; //standard output handle for the process
    StartUpInfo.wShowWindow = SW_HIDE;//隐藏控制台窗口 
    
    //创建进程,执行程序
    PROCESS_INFORMATION tProcessInfo = {0}; 

    BOOL resCP = CreateProcess(NULL, //LPCWSTR lpszImageName, //指向可执行的模块的指针 
                      const_cast<char*>(strCmdLine.c_str()), //LPCWSTR lpszCmdLine, //指向可执行命令行字符串的指针 
                      NULL, //LPSECURITY_ATTRIBUTES lpsaProcess, 决定是否返回的句柄可以被子进程继承
                      NULL, //LPSECURITY_ATTRIBUTES lpsaThread  
                      TRUE, //BOOL fInheritHandles  //指示新进程是否从调用进程处继承了句柄
                      NULL, //DWORD fdwCreate, //创建标志 指定附加的、用来控制优先类和进程的创建的标志
                      NULL, //LPVOID lpvEnvironment
                      NULL, //LPWSTR lpszCurDir //指定子进程的工作路径
                      &StartUpInfo, //LPSTARTUPINFOW lpsiStartInfo //决定新进程的主窗体如何显示的STARTUPINFO结构体
                      &tProcessInfo); //LPPROCESS_INFORMATION lppiProcInfo

    if (!resCP) //进程创建失败
    {
        FormMain->EditAddLines("警告！请确认 Winperf 所在目录是否有 iperf.exe？");

        return;
    }

    
    m_hCmdThread  = tProcessInfo.hThread; 
    m_hCmdProcess = tProcessInfo.hProcess;
        
    //WaitForSingleObject(pi.hProcess ,INFINITE);  //等待进程执行完毕 
        
    const int buff_size = 1024;
        
    int reportInternal = 1000 * FormMain->GetPrintInternal();
        
    while(!Terminated)
    {        
        char buffer[buff_size];
            
        unsigned long bytesRead = 0;

        memset(buffer, 0, buff_size);
        bool res = ReadFile(hRead, &buffer, buff_size, &bytesRead, NULL);
        
        if (res && bytesRead > 0)
        {
            FormMain->EditAddLines(buffer);
                
            Sleep(reportInternal);
        }
        else
        {
            CloseHandle(hWrite);
            CloseHandle(hRead);

            FormMain->ShowRunningInfo("iperf 执行完毕.");
                
            break;
        }
    }
}


