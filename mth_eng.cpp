#include "match.h"
#include <stdlib.h>

extern int64_t gOrdrCnt;
extern int64_t gTradeCnt;

int main() {
    //g++ mth_eng.cpp match.cpp -o test.exe -std=c++11

    initOrdrBk(ORDR_NUM);

    for (size_t i = 0; i < 1000; i++)
    {
        OrderT newOrdr = {
            0,                              //订单编号
            100001 + rand() % 10,           //交易账户代码
            1 + rand() % PRDUCT_NUM,      //债券序列号
            //1,                              
            90 + rand() % 10,               //价格
            1000000 + 1000000 * (rand() % 5),   //量
			rand() % 2                      //方向
        };
        spOrderT o = std::make_shared<OrderS>(newOrdr);
        addOrdr(o);
    }
    std::cout << "ordr:" << gOrdrCnt << " tradeCnt:" << gTradeCnt << std::endl;
    return 0;
}
