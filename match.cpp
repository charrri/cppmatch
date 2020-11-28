#include "match.h"

std::map<int,PrcLinkT*> gOrdrBk[MAX_DIR];
int64_t gOrdrCnt = 0;
int64_t gTradeCnt = 0;
bool gIfLog = true;
//atomic_int64_t

void match(spOrderT inOrdr);

void initOrdrBk(int ordrCnt) {
    for (size_t i = 0; i < MAX_DIR; i++)
    {
        for (size_t p = 0; p < PRDUCT_NUM + 1; p++)
        {
            gOrdrBk[i][p] = new PrcLinkT();
        }
    }
    log("initOrdrBk done");
}

template<typename T>
void log(T s) {
    if (!gIfLog) return;
    std::cout << s << std::endl;
}

void addOrdrToOrdrBk(spOrderT inOrdr) {
    auto& o = *inOrdr;
    auto& prcLink = *(gOrdrBk[o.dir][o.bondCd]);
    auto it = prcLink.find(o.price);
    if (it == prcLink.end()) {
        prcLink[o.price] = new PrcLeadrT();
        prcLink[o.price]->push_back(inOrdr);
    }
    else {
        it->second->push_back(inOrdr);
    }
}

void addOrdr(spOrderT o) {
    showOrdr(o);
    match(o);
    if ((*o).vol > 0) {
        addOrdrToOrdrBk(o);
    }
    else {
        //to do txnlog
    }
    
    gOrdrCnt++;
}

bool CheckPriceCanDeal(int inOrdrDir, int inOrdrPrc, int bkOrdrPrc) {
	if (inOrdrDir == BUY) {
		return inOrdrPrc >= bkOrdrPrc;
	} else if (inOrdrDir == SELL) {
		return inOrdrPrc <= bkOrdrPrc;
	} else {
		log("Error direction!");
		return false;
	}
	return false;
}
bool CheckAccountCanDeal(int inOrdrAcnt,int bkOrdrAcnt) {
    return !(inOrdrAcnt == bkOrdrAcnt);
}

template<typename T>
void matchInPrcLink(T begin, T end, spOrderT inOrdr, std::vector<TradeS>& tradesV) {
    auto& o = *inOrdr;
    while(begin != end) {
        if (CheckPriceCanDeal(o.dir, o.price, begin->first)) {
			auto& ordrList = *(begin->second);
			for (auto it = ordrList.begin(); it != ordrList.end();) {
                auto& bkO = *(it->get());
                if (gIfLog) {
                    std::cout << "bkO: ";
                    showOrdr(*it);
                }                
                if (!CheckAccountCanDeal(o.trdngAcntCd,bkO.trdngAcntCd)) {
                	log("same trading account,skip");
                    it++;
                	continue;
                }
                TradeS trd = {};
                // o -> bko
                if (bkO.vol >= o.vol) { // 订单完全成交                    
					trd.vol += o.vol;
					bkO.vol -= o.vol;
					o.vol = 0;
					if (bkO.vol == 0) {
						ordrList.erase(it);
					}
					gTradeCnt++;
					//trade() // to do
                    //tradesV.push_back(trd);
                    if (gIfLog)	std::cout << "trade:" << trd.vol << std::endl;
					return;
				} else {
					trd.vol += bkO.vol;
					o.vol -= bkO.vol;
					bkO.vol = 0;
					ordrList.erase(it++);
                    gTradeCnt++;
					//trade() // to do
                    //tradesV.push_back(trd);
                    if (gIfLog)	std::cout << "trade:" << trd.vol << std::endl;
				}
            }
        }
        begin++;
    }
}

void match(spOrderT inOrdr) {
    std::vector<TradeS> trds;
    auto& o = *inOrdr;
    int bkDir = o.dir == BUY ? SELL : BUY;
    auto& prcLink = *(gOrdrBk[bkDir][o.bondCd]); 

    //map 按key 升序
    //map.rbegin() 与 map.begin() 返回的迭代器类型不同
    if (bkDir == BUY) {
        auto it = prcLink.rbegin();
        if (it == prcLink.rend()) return;
        else matchInPrcLink(it, prcLink.rend(), inOrdr, trds);
    }
    else {
        auto it = prcLink.begin();
        if (it == prcLink.end()) return;
        else matchInPrcLink(it, prcLink.end(), inOrdr, trds);
    }

    return;
}

void showOrdr(spOrderT inOrdr, OrdrPrintType t) {
    if (!gIfLog) return;
    char buf[128] = {0};
    auto &o = *inOrdr;
    if (t == PRINT_DEFAULT) {
        std::snprintf(buf, 128, "trdngAcntCd:%d\tbondCd:%d\tprice:%d\tvol:%d\tdir:%d",
                        o.trdngAcntCd, o.bondCd, o.price, o.vol, o.dir);
    }
    if (t == PRINT_PRCLEADR) {
        std::snprintf(buf, 128, "[bondCd:%d\tprice:%d\tdir:%d]",
                        o.bondCd, o.price, o.dir);
    }
    if (t == PRINT_PRCLINK) {
        std::snprintf(buf, 128, "{trdngAcntCd:%d\tvol:%d}",
                        o.trdngAcntCd, o.vol);
    }
    
    log(buf);
}

void GetSysTimestamp(int64_t* timestamp) {
    struct timeval t_val;
    gettimeofday(&t_val, nullptr);
    
    int64_t us = (int64_t)t_val.tv_sec * 1000000 + (int64_t)t_val.tv_usec;
    *timestamp = us;
}

void freeAll() {
    for (size_t i = 0; i < MAX_DIR; i++)
    {
        for (size_t p = 0; p < PRDUCT_NUM + 1; p++)
        {
            auto m = gOrdrBk[i][p];
            for (auto it = m->begin(); it != m->end(); it++)
            {
                it->second->clear();
                delete it->second;
            }
            m->clear();
            delete m;
        }
    }
    gOrdrCnt = 0;
    gTradeCnt = 0;
    log("free all");
}

void setLog(bool f) {
    gIfLog = f;
}