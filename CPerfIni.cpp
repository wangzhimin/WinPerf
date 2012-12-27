
#include "CPerfIni.h"

#include <windows.h>


/**
 * 提供给外部的接口.若ini文件不存在,即创建默认文件;若存在,则读取其中的配置.
 */
CPerfIni::FileSituation CPerfIni::ReadIni()
{
    char dirArray[255];

    DWORD res = GetCurrentDirectory(255, dirArray);
    if (res == 0)
    {
        m_enable = false;
        return ERROR_INI;
    }

    m_strFileName = string(dirArray) + "\\WinPerf.ini";

    WIN32_FIND_DATA tData;

    HANDLE hFind = FindFirstFile(m_strFileName.c_str(), &tData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
        CreateIniFile();

        return CRATE_NEW_INI;
    }

    ReadIniFile();

    FindClose(hFind);

    return READ_OLD_INI;
}

void CPerfIni::ReadIniFile()
{
    if (!m_enable)
    {
        return;
    }

    char strBuff[255];

    GetPrivateProfileString("winperf", "ServerIP", "192.168.1.1", strBuff, 255, m_strFileName.c_str());
    m_strServerIP.assign(strBuff);

    GetPrivateProfileString("winperf", "ServerPort", "5001", strBuff, 255, m_strFileName.c_str());
    m_strServerPort.assign(strBuff);

    GetPrivateProfileString("winperf", "ClientPort", "5001", strBuff, 255, m_strFileName.c_str());
    m_strClientPort.assign(strBuff);
}

/**
 * 配置写入ini文件.
 */
void CPerfIni::WriteIniFile()
{
    if (!m_enable)
    {
        return;
    }

    WritePrivateProfileString("winperf", "ServerIP", m_strServerIP.c_str(), m_strFileName.c_str());
    WritePrivateProfileString("winperf", "ServerPort", m_strServerPort.c_str(), m_strFileName.c_str());
    WritePrivateProfileString("winperf", "ClientPort", m_strClientPort.c_str(), m_strFileName.c_str());
}

string CPerfIni::GetServerIP()
{
    return m_strServerIP;
}

string CPerfIni::GetServerPort()
{
    return m_strServerPort;
}

string CPerfIni::GetClinePort()
{
    return m_strClientPort;
}

void CPerfIni::SetServerIP(string strIP)
{
    m_strServerIP = strIP;
}

void CPerfIni::SetServerPort(string strSPort)
{
    m_strServerPort = strSPort;
}

void CPerfIni::SetClientPort(string strCPort)
{
    m_strClientPort = strCPort;
}

void CPerfIni::CreateIniFile()
{
    fstream fIniFile;
    
    fIniFile.open("WinPerf.ini", ios_base::out);

    if (!fIniFile)
    {
        m_enable = false;
        return;
    }

    m_strServerIP   = "192.168.1.1";
    m_strServerPort = "5001";
    m_strClientPort = "5001";
    
    fIniFile.close();

    WriteIniFile();
}




