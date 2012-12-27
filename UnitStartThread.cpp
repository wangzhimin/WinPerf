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
    //FreeOnTerminate = true; //�߳�ִ������Լ��˳�
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
        string strInfo = "�쳣��Ϣ:";
        strInfo += ex.what();

        MessageBox(Application->Handle, strInfo.c_str(), "����", MB_OK);
    }

}
//---------------------------------------------------------------------------
//ǿ���ж�cmdline��ִ��
void __fastcall TStartThread::TerminateCmd()
{
    CloseHandle(m_hCmdThread); 
        
    //ǿ����ֹcmdline����
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

    //���ݰ汾���ù���ģʽ�Ͱ�ȫ����
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
        FormMain->ShowRunningInfo("�̱߳��ж�!");
        return;
    }
    
  
    //---- Place thread code here ----
    STARTUPINFO StartUpInfo = {sizeof(StartUpInfo),}; 
    StartUpInfo.dwFlags     = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES; //ʹ�ñ�׼������ʾ���� 
    StartUpInfo.hStdOutput  = hWrite; //standard output handle for the process
    StartUpInfo.wShowWindow = SW_HIDE;//���ؿ���̨���� 
    
    //��������,ִ�г���
    PROCESS_INFORMATION tProcessInfo = {0}; 

    BOOL resCP = CreateProcess(NULL, //LPCWSTR lpszImageName, //ָ���ִ�е�ģ���ָ�� 
                      const_cast<char*>(strCmdLine.c_str()), //LPCWSTR lpszCmdLine, //ָ���ִ���������ַ�����ָ�� 
                      NULL, //LPSECURITY_ATTRIBUTES lpsaProcess, �����Ƿ񷵻صľ�����Ա��ӽ��̼̳�
                      NULL, //LPSECURITY_ATTRIBUTES lpsaThread  
                      TRUE, //BOOL fInheritHandles  //ָʾ�½����Ƿ�ӵ��ý��̴��̳��˾��
                      NULL, //DWORD fdwCreate, //������־ ָ�����ӵġ���������������ͽ��̵Ĵ����ı�־
                      NULL, //LPVOID lpvEnvironment
                      NULL, //LPWSTR lpszCurDir //ָ���ӽ��̵Ĺ���·��
                      &StartUpInfo, //LPSTARTUPINFOW lpsiStartInfo //�����½��̵������������ʾ��STARTUPINFO�ṹ��
                      &tProcessInfo); //LPPROCESS_INFORMATION lppiProcInfo

    if (!resCP) //���̴���ʧ��
    {
        FormMain->EditAddLines("���棡��ȷ�� Winperf ����Ŀ¼�Ƿ��� iperf.exe��");

        return;
    }

    
    m_hCmdThread  = tProcessInfo.hThread; 
    m_hCmdProcess = tProcessInfo.hProcess;
        
    //WaitForSingleObject(pi.hProcess ,INFINITE);  //�ȴ�����ִ����� 
        
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

            FormMain->ShowRunningInfo("iperf ִ�����.");
                
            break;
        }
    }
}


