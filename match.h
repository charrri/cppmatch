#pragma once
#include <iostream>
#include <stdint.h>
#include <string>
#include <memory>
#include <map>
#include <list>
#include <vector>
#include <sys/time.h>
//#include <atomic>

/*Macro*/
#define BUY 0
#define SELL 1
#define MAX_DIR 2
#define	PRDUCT_NUM 20 // 合约数量
#define ORDR_NUM 1000000

/*struction*/
typedef struct OrderS
{
    int     ordrCd;          //订单编号
	int     trdngAcntCd;     //交易账户代码
	int     bondCd;          //债券序列号
	int     price;          //价格
	int     vol;            //量
    int     dir;            //方向
    /* data */
}OrderT,*pOrderT;

typedef std::shared_ptr<OrderT> spOrderT;
typedef std::list<spOrderT> PrcLeadrT;
typedef std::list<spOrderT>* pPrcLeadrT;
typedef std::map<int,pPrcLeadrT> PrcLinkT;

typedef struct TradeS{
    int     trdCd;              //买方交易账户代码
	int     buyTrdngAcntCd;     //买方交易账户代码
	int     selltrdngAcntCd;    //卖方交易账户代码
	int     bondCd;             //债券序列号
    int     price;              //价格
	int     vol;                //量
    int     dir;                //方向
}TradeT;

typedef enum {
    PRINT_DEFAULT,
    PRINT_PRCLEADR,
    PRINT_PRCLINK
}OrdrPrintType;

void initOrdrBk(int ordrCnt);
void addOrdr(spOrderT o);
void showOrdr(spOrderT inOrdr, OrdrPrintType type = PRINT_DEFAULT);
void GetSysTimestamp(int64_t* timestamp);
template<typename T>
void log(T s);
void freeAll();
void setLog(bool f);


