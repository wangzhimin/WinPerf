//---------------------------------------------------------------------------

#ifndef UnitStartThreadH
#define UnitStartThreadH
//---------------------------------------------------------------------------
#include <Classes.hpp>
//---------------------------------------------------------------------------
class TStartThread : public TThread
{
protected:
    void __fastcall Execute();
public:
    __fastcall TStartThread(bool CreateSuspended);

    void __fastcall TerminateCmd(); //中断iperf的cmd进程

private:
    HANDLE m_hCmdThread;
    HANDLE m_hCmdProcess;

    void _execute();
};
//---------------------------------------------------------------------------
#endif
