#include "match.h"

std::map<int,PrcLinkT*> gOrdrBk[MAX_DIR];
int64_t gOrdrCnt = 0;
int64_t gTradeCnt = 0;
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
                std::cout << "bkO: ";
                showOrdr(*it);
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
                    tradesV.push_back(trd);
					std::cout << "trade:" << trd.vol << std::endl;
					return;
				} else {
					trd.vol += bkO.vol;
					o.vol -= bkO.vol;
					bkO.vol = 0;
					ordrList.erase(it++);
                    gTradeCnt++;
					//trade() // to do
                    tradesV.push_back(trd);
                    std::cout << "trade:" << trd.vol << std::endl;
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