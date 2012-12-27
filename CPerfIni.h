
#ifndef CPERF_INI_H
#define CPERF_INI_H

#include <string>
#include <fstream>

using namespace std;


class CPerfIni
{
public:
    enum FileSituation
    {
        CRATE_NEW_INI = 0,
        READ_OLD_INI,
        ERROR_INI
    };

    CPerfIni()
        :m_enable(true)
    {
    }

    FileSituation ReadIni();
    void ReadIniFile();
    void WriteIniFile();
    
    string GetServerIP();
    string GetServerPort();
    string GetClinePort();

    void SetServerIP(string strIP);
    void SetServerPort(string strSPort);
    void SetClientPort(string strCPort);

private:
    void CreateIniFile();

    string m_strFileName;

    bool m_enable;

    /*  µº ÷µ. */
    string m_strServerIP;
    string m_strServerPort;
    string m_strClientPort;

    //const string strKeyServerPort = "ServerPort";
    //const string strKeyClientPort = "ClientPort";
};


#endif

