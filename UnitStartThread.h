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

    void __fastcall TerminateCmd(); //�ж�iperf��cmd����

private:
    HANDLE m_hCmdThread;
    HANDLE m_hCmdProcess;

    void _execute();
};
//---------------------------------------------------------------------------
#endif
