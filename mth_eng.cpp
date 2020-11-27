#include "match.h"
#include <stdlib.h>

extern int64_t gOrdrCnt;
extern int64_t gTradeCnt;

int main() {
    /*
    std::shared_ptr<int> ptra;
    if (true) {
        int a = 10;
        ptra = std::make_shared<int>(a);
    }

    std::shared_ptr<int> ptra2(ptra); //copy
    std::cout << *ptra << std::endl;
    std::cout << ptra.use_count() << std::endl;

    int a1 = 1;
    int b = 20;
    int *pb = &a1;
    //std::shared_ptr<int> ptrb = pb;  //error
    std::shared_ptr<int> ptrb = std::make_shared<int>(b);
    ptra2 = ptrb; //assign
    pb = ptrb.get(); //获取原始指针

    std::cout << ptra.use_count() << std::endl;
    std::cout << ptrb.use_count() << std::endl;
    
    std::map<int,int> m = {{0,0},{1,1},{2,2}};
    auto it = m.rbegin();
    std::cout << it->second << std::endl;

    std::list<int> l = {11,12,13};
    for (auto& i : l) {
        std::cout << i << std::endl;
    }
    */

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
