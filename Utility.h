//---------------------------------------------------------------------------

#ifndef WINPERF_UTILITY_H
#define WINPERF_UTILITY_H
//---------------------------------------------------------------------------

#include <string>
using namespace std;


class Utility
{
public:
    static string GetOutputFormatUnitStr(int index);
    static string GetUDPBandUnitStr(int index);
    static string GetUnitStr(int index); //ѡ��ĵ�λ���ַ���
    static string GetToSStr(int index);
};


#endif

