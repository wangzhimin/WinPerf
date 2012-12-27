
#include <string>
#include <sstream>

using namespace std;


template <typename T>
std::string ValueToStr(T value)
{
    ostringstream ost;
    ost << value;
    return ost.str();
}

enum StatusThread
{
    Status_Start = 0,
    Status_ParaError,
    Status_Abort,
    Status_End
};
