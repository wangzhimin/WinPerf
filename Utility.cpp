

#include "Utility.h"

//---------------------------------------------------------------------------

string Utility::GetOutputFormatUnitStr(int index)
{
    /* Adaptive Bits, Adaptive Bytes
       Bits, Bytes
       GBits, GBytes
       KBits, KBytes
       MBits, MBytes
    */
    switch(index)
    {
    case 0:
        return "a";
        
    case 1:
        return "A";
        
    case 2:
        return "b";
        
    case 3:
        return "B";
        
    case 4:
        return "g";
        
    case 5:
        return "G";
        
    case 6:
        return "k";
        
    case 7:
        return "K";
        
    case 8:
        return "m";
        
    case 9:
        return "M";
        
    default:
        return "";
    }
}

     
string Utility::GetUDPBandUnitStr(int index)
{
    switch(index)
    {
    case 0:
        return "K";

    case 1:
        return "M";

    default:
        return "";
    }
}


string Utility::GetUnitStr(int index)
{
    switch(index)
    {
    case 0:
        return "k";

    case 1:
        return "K";

    case 2:
        return "m";

    case 3:
        return "M";

    default:
        return "";
    }
}

string Utility::GetToSStr(int index)
{
    switch(index)
    {
    case 0:
        return "";

    case 1:
        return "-S 0x02";

    case 2:
        return "-S 0x10";

    case 3:
        return "-S 0x04";

    default:
        return "";
    }
}


