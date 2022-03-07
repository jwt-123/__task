#include <algorithm>
#include "proc.h"


int main() {
    std::cout << "开始" << std::endl;
    CProcInfo cProcInfo;
    cProcInfo.show();
    cProcInfo.check();
    return 0;
}
