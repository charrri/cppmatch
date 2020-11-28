#include "match.h"
#include <stdlib.h>

extern int64_t gOrdrCnt;
extern int64_t gTradeCnt;

int main() {
    //g++ mth_eng.cpp match.cpp -o test.exe -std=c++11

    initOrdrBk(ORDR_NUM);
    setLog(false);
    int64_t testCnt = 0;
    std::cout << "test ordrCnt:";
    std::cin >> testCnt;
    int64_t tm_start;
    int64_t tm_end;

    while(testCnt) {
        GetSysTimestamp(&tm_start);
        for (size_t i = 0; i < testCnt; i++)
        {
            OrderT newOrdr = {
                0,                              //订单编号
                100001 + rand() % 100,           //交易账户代码
                1 + rand() % PRDUCT_NUM,      //债券序列号
                //1,                              
                90 + rand() % 10,               //价格
                1000000 + 1000000 * (rand() % 5),   //量
                rand() % 2                      //方向
            };
            spOrderT o = std::make_shared<OrderS>(newOrdr);
            addOrdr(o);
        }
        GetSysTimestamp(&tm_end);
        std::cout << "ordr:" << gOrdrCnt << " tradeCnt:" << gTradeCnt << std::endl; 
        std::cout << "time:" << tm_end - tm_start << "\t" << (tm_end - tm_start)/gOrdrCnt << "ms/order" << std::endl;  
        freeAll();
        initOrdrBk(ORDR_NUM);
        std::cout << "test ordrCnt:";
        std::cin >> testCnt;
    }

    return 0;
}
