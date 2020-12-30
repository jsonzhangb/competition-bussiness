#pragma once

#include <cstdio>
#include <cstdlib>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <set>
#include <stack>
#include <deque>
#include <list>
#include "timer.h"
#include "utf8.h"

#define BUYER_COL 14
#define SELLER_COL 10
#define SCORE_LEVEL 6
#define BUY_RAWS 50000
#define SELL_ALL_RAWS 429679
#define SELL_CF_RAWS 399837
#define SELL_SR_RAWS 29842


int sell_cf_raws = SELL_CF_RAWS;
int sell_sr_raws = SELL_SR_RAWS;


class Solution {
public:
    std::string** org_sell_cf;
    std::string** org_sell_sr;

    std::string** org_buy_cf;
    std::string** org_buy_sr;

    bool* cf_use;
    bool* sr_use;

    void init() {
        org_sell_cf = new std::string * [SELL_CF_RAWS];
        for (int i = 0; i < SELL_CF_RAWS; i++) {
            org_sell_cf[i] = new std::string[SELLER_COL];
        }
        org_sell_sr = new std::string * [SELL_SR_RAWS];
        for (int i = 0; i < SELL_SR_RAWS; i++) {
            org_sell_sr[i] = new std::string[SELLER_COL];
        }

        org_buy_cf = new std::string * [BUY_RAWS];
        for (int i = 0; i < BUY_RAWS; i++) {
            org_buy_cf[i] = new std::string[BUYER_COL];
        }
        org_buy_sr = new std::string * [BUY_RAWS];
        for (int i = 0; i < BUY_RAWS; i++) {
            org_buy_sr[i] = new std::string[BUYER_COL];
        }
    }

};