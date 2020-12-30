#include "sol.h"
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

void readFile(char* filePath, string** cfArray, string** srArray, int totalRaw, int colNum) {

    ifstream fin(filePath);

    //忽略头行
    string line;
    getline(fin, line);

    unordered_map<string, int> cfMap;
    unordered_map<string, int> srMap;

    string tmp[14];
    int cfRaw = 0;
    int srRaw = 0;
    while (getline(fin, line)) {

        size_t pos = 0;
        for (int i = 0; i < colNum; i++) {
            string s;
            while (true) {
                if (pos >= line.length() || line[pos] == '\r' || line[pos] == '\n') break;
                if (line[pos] == ',') break;
                else {
                    s += line[pos];
                    pos++;
                }
            }
            pos++;
            tmp[i] = s;
        }
        string s;
        for (int i = 0; i < colNum; i++) {
            if (i != 3)
                s += tmp[i];
        }

        if (line.find("CF") != string::npos) {
            if (cfMap.find(s) != cfMap.end()) {
                int r = cfMap[s];
                cfArray[r][3] = to_string(atoi(cfArray[r][3].c_str()) + atoi(tmp[3].c_str()));

            }
            else {
                for (int i = 0; i < colNum; i++) {
                    cfArray[cfRaw][i] = tmp[i];
                }
                cfMap[s] = cfRaw;
                cfRaw++;
            }
        }
        else if (line.find("SR") != string::npos) {
            if (srMap.find(s) != srMap.end()) {
                int r = srMap[s];
                srArray[r][3] = to_string(atoi(srArray[r][3].c_str()) + atoi(tmp[3].c_str()));
            }
            else {
                for (int i = 0; i < colNum; i++) {
                    srArray[srRaw][i] = tmp[i];
                }
                srMap[s] = srRaw;
                srRaw++;
            }
        }

    }

    if (cfRaw + srRaw != totalRaw) {
        sell_cf_raws = cfRaw;
        sell_sr_raws = srRaw;
    }
    fin.close();
}


int getQueueNums(unordered_map<string, stack<int>>& buy_map, unordered_map<string, int>& buy_count_map) {
    int queue_num = 0;
    auto it = buy_map.begin();
    while (it != buy_map.end()) {
        if (!it->second.empty()) {
            queue_num++;
            it++;
        }
        else {
            buy_count_map.erase(it->first);
            it = buy_map.erase(it);
        }
    }
    return queue_num;
}

string getFitKey(unordered_map<string, int>& buy_count_map, unordered_map<string, int>& sell_count_map) {
    int min = INT32_MAX;
    string minKey;

    for (auto& entry : buy_count_map) {
        int buyCount = entry.second;
        int sellCount = sell_count_map[entry.first];
        if (sellCount - buyCount < min) {

            min = sellCount - buyCount;
            minKey = entry.first;
        }
    }
    return minKey;
}

inline int getValue(string* answer, string* question, int level) {

    int ret = 0;
    for (int i = level; i < BUYER_COL; i += 2) {
        for (int j = 4; j < SELLER_COL; j++) {
            if (question[i].empty()) {
                continue;
            }
            if (answer[j] == question[i]) {
                ret = ret | (1 << ((i - 3) >> 1));
            }
        }
    }
    return ret;
}

string getLevel(string* answer, string* question) {
    string ret;
    for (int i = 5; i < BUYER_COL; i += 2) {
        for (int j = 4; j < SELLER_COL; j++) {
            if (question[i].empty() || answer[j].empty()) {
                continue;
            }
            
            if (answer[j] == question[i]) {
                ret =  ret + to_string(i / 2 - 1) + "-";
            }
        }
    }
    if (ret.empty()) {
        return "0";
    }
    return ret.substr(0, ret.length() - 1);
}

void updateSellMap(string* answer, int sell_index, unordered_map<string, unordered_set<int>>& sell_map) {
    for (int i = 4; i < SELLER_COL; i++) {
        if (!answer[i].empty()) {
            sell_map[answer[i]].erase(sell_index);
        }
    }
}

void updateSellCountMap(string* answer, int count, unordered_map<string, int>& sell_count_map) {
    for (int i = 4; i < SELLER_COL; i++) {
        if (!answer[i].empty()) {
            int counts = sell_count_map[answer[i]];
            counts -= count;
            sell_count_map[answer[i]] = counts;
        }
    }
}

void writeResult(string output_file, vector<string>& results) {
    ofstream file(output_file);
    string col_name = "买方客户,卖方客户,品种,货物编号,仓库,分配货物数量,对应意向顺序\n";
    file << col_name;
    for (size_t i = 0; i < results.size(); i++) {
        file << results[i] << '\n';
    }
}