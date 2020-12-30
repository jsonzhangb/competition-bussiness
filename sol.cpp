#include <vector>
#include "sol.h"
#include "util.h"

using namespace std;

void initSellMap(unordered_map<string, unordered_set<int>>& sell_map, string** sell_array, int sell_raws, unordered_map<string, int>& sell_count_map) {
    Timer timer;
    printf("init begin.\n");

    for (int i = 0; i < sell_raws; i++) {
        for (int j = 4; j < SELLER_COL; j++) {
            string val = sell_array[i][j];

            if (val.empty()) {
                continue;
            }

            if (sell_map.find(val) != sell_map.end()) {
                unordered_set<int>& set = sell_map[val];
                set.insert(i);
            }
            else {
                unordered_set<int> sets;
                sets.insert(i);
                sell_map[val] = sets;
            }

            if (sell_count_map.find(val) != sell_count_map.end()) {
                int count = sell_count_map[val];
                count += atoi(sell_array[i][3].c_str());
                sell_count_map[val] = count;
            }
            else {
                int count = atoi(sell_array[i][3].c_str());
                sell_count_map[val] = count;
            }
        }
    }
    printf("end of init. elapsed:%.3f s\n", timer.time());
}

void updateBuyMap(string** buy_array, deque<int>& remains, deque<int>& unmatch, int level, unordered_map<string, int>& buy_count_map, unordered_map<string, stack<int>>& tmp_map) {
    Timer timer;
    printf("update start.\n");
    unordered_map<string, vector<int>> buy_map;

    for (auto it = remains.begin(); it != remains.end(); it++) {
        int index = *it;
        string val = buy_array[index][level];

        if (val.empty()) {
            unmatch.push_back(index);
        }
        else {
            if (buy_map.find(val) != buy_map.end()) {
                auto& lists = buy_map[val];
                lists.push_back(index);
            }
            else {
                vector<int> lists;
                lists.push_back(index);
                buy_map[val] = lists;
            }

            if (buy_count_map.find(val) != buy_count_map.end()) {
                int count = buy_count_map[val];
                count += atoi(buy_array[index][2].c_str());
                buy_count_map[val] = count;
            }
            else {
                int count = atoi(buy_array[index][2].c_str());
                buy_count_map[val] = count;
            }
        }
    }
    remains.clear();


    for (auto& entry : buy_map) {
        vector<int>& lists = entry.second;

        sort(lists.begin(), lists.end(), [buy_array](int t1, int t2) {
            return atoi(buy_array[t1][2].c_str()) > atoi(buy_array[t2][2].c_str());
            });


        if (level == 5) {
            sort(lists.begin(), lists.end(), [buy_array](int t1, int t2) {
                return atoi(buy_array[t1][1].c_str()) > atoi(buy_array[t2][1].c_str());
                });
        }

        stack<int> stack;
        for (int i = lists.size() - 1; i >= 0; i--) {
            stack.push(lists[i]);
        }
        tmp_map[entry.first] = stack;
    }

    printf("end of update. elapsed:%.3f s\n", timer.time());
}

inline int getSolveIndex(string* question, unordered_set<int>& answers, int level, string** sell_array, unordered_set<string>& house) {
    if (answers.size() == 0) {
        return -1;
    }
    else {
        int ITER = 0;
        int iter = 0;
        int index = -1;
        int min = -1;
        int one, two, three, four, five;

        if (question[3] == "CF") {
            ITER = 80000;
            one = 33;
            two = 27;
            three = 20;
            four = 13;
            five = 7;
        }
        else {
            ITER = 10000;
            one = 40;
            two = 30;
            three = 20;
            four = 10;
            five = 0;
        }
        int max_score = 0;
        switch (level) {
        case 5: max_score = one + two + three + four + five; break;
        case 7: max_score = two + three + four + five; break;
        case 9: max_score = three + four + five; break;
        case 11:max_score = four + five; break;
        case 13:max_score = five; break;
        }


        int house_size = house.size();

        for (auto i : answers) {
            iter++;
           
            string* answer = sell_array[i];
            int m = 0;
            int val = 0;
            for (int i = level; i < BUYER_COL; i += 2) {
                for (int j = 4; j < SELLER_COL; j++) {
                    if (question[i].empty()) {
                        continue;
                    }
                    if (answer[j] == question[i]) {
                        val = val | (1 << ((i - 3) >> 1));
                    }
                }
            }

            int punish = 0;
            if (house_size == 0 || house.find(answer[4]) != house.end()) {
                punish = 0;
            }
            else {
                punish = -10 * (house_size + 1);
            }
            
            for (int i = 1; i <= 5; i++) {
                val >>= 1;
                if (val & 1) {
                    switch (i) {
                    case 1: m += one; break;
                    case 2: m += two; break;
                    case 3: m += three; break;
                    case 4: m += four; break;
                    case 5: m += five; break;
                    }
                }
            }
            m += punish;
            if (m == max_score) {
                return i;
            }
            if (m > min) {
                index = i;
                min = m;
            }
            
            if (iter > ITER) break;
        }
        return index;
    }
}

int getLevelResults(string** buy_array, string** sell_array, unordered_map<string, stack<int>>& buy_map,
    unordered_map<string, unordered_set<int>>& sell_map, vector<string>& results, deque<int> &remains, bool* use,
    int level, unordered_map<string, int>& buy_count_map, unordered_map<string, int>& sell_count_map) {

    int solves = 0;
    unordered_set<string> house;
    while (true) {
        int queueNums = getQueueNums(buy_map, buy_count_map);
        if (queueNums == 0) {
            break;
        }
        string key = getFitKey(buy_count_map, sell_count_map);

        string* question = buy_array[buy_map[key].top()];
        unordered_set<int>& sell_set = sell_map[key];
        int count = atoi(question[2].c_str());
        int buy_count = buy_count_map[key] - count;

        while (count > 0) {
            int solve_index = getSolveIndex(question, sell_set, level, sell_array, house);
            if (solve_index < 0) break;

            string* answer = sell_array[solve_index];
            house.insert(answer[4]);
            int sell_count = atoi(answer[3].c_str());
            string result = question[0] + "," + answer[0] + "," + answer[1] + "," + answer[2] + "," + answer[4] + ",";
            string level_sort = getLevel(answer, question);

            if (count >= sell_count) {
                result += to_string(sell_count) + "," + level_sort;
                use[solve_index] = true;
                sell_set.erase(solve_index);
                updateSellMap(answer, solve_index, sell_map);

                updateSellCountMap(answer, sell_count, sell_count_map);
                count -= sell_count;
            }
            else {
                updateSellCountMap(answer, count, sell_count_map);
                result += to_string(count) + "," + level_sort;
                answer[3] = to_string(sell_count - count);
                count = 0;
            }
            results.push_back(result);
        }

        house.clear();
        buy_count += count;
        buy_count_map[key] = buy_count;
        if (count > 0) {
            question[2] = to_string(count);
            while (!buy_map[key].empty()) {
                int top = buy_map[key].top();
                buy_map[key].pop();
                remains.push_back(top);
            }
        }
        else {
            solves++;
            buy_map[key].pop();

            if (solves % 1000 == 0) {
                printf("solves:%d\n", solves);
            }
        }

    }
    return solves;
}


void countVertify(deque<int> &unmatch, bool* use, string** sell_array, string** buy_array, int sell_num) {
    int buy_count = 0;
    int sell_count = 0;
    for (int inx : unmatch) {
        buy_count += atoi(buy_array[inx][2].c_str());
    }
    for (int i = 0; i < sell_num; i++) {
        if (!use[i]) {
            sell_count += atoi(sell_array[i][3].c_str());
        }
    }
    printf("buyCount:%d, sellCount:%d\n", buy_count, sell_count);
}

void getUnmatchResult(string** buy_array, string** sell_array, deque<int>& remains, vector<string>& results,
    bool* use, int sell_num) {

    deque<int> list;
    for (int i = 0; i < sell_num; i++) {
        if (!use[i]) {
            list.push_back(i);
        }
    }

    sort(list.begin(), list.end(), [sell_array](int t1, int t2) {
        return atoi(sell_array[t1][4].c_str()) > atoi(sell_array[t2][4].c_str());
        });
    for (size_t i = 0; i < remains.size(); i++) {
        string* question = buy_array[remains[i]];
        int count = atoi(question[2].c_str());
        auto it = list.begin();
        while (it != list.end()) {
            string* answer = sell_array[*it];
            if (answer[1] == question[3]) {
                int tmp = atoi(answer[3].c_str());
                if (count > tmp) {
                    string result = question[0] + "," + answer[0] + "," + answer[1] + "," + answer[2] + "," + answer[4] +
                        "," + to_string(tmp) + "," + to_string(0);
                    results.push_back(result);
                    count -= tmp;
                    list.erase(it++);
                    continue;
                }
                if (count == tmp) {
                    string result = question[0] + "," + answer[0] + "," + answer[1] + "," + answer[2] + "," + answer[4] +
                        "," + to_string(tmp) + "," + to_string(0);
                    results.push_back(result);
                    count -= tmp;
                    list.erase(it++);
                    break;
                }
                if (count < tmp) {
                    string result = question[0] + "," + answer[0] + "," + answer[1] + "," + answer[2] + "," + answer[4] +
                        "," + to_string(count) + "," + to_string(0);
                    results.push_back(result);
                    answer[3] = to_string(tmp - count);
                    count = 0;
                    break;
                }
            }
        }

        if (count > 0) {
            printf("unmatch solve error\n");
        }
    }
    if (list.size() > 0) {
        printf("unmatch solve error\n");
    }
}

void process(string** buy_array, string** sell_array, bool *use, vector<string>& results, int sell_raws) {
    int solves = 0;
    unordered_map<string, unordered_set<int>> sell_map;
    deque<int> remains;
    for (int i = 0; i < BUY_RAWS; i++) {
        remains.push_back(i);
    }

    unordered_map<string, int> buy_count_map;
    unordered_map<string, int> sell_count_map;

    initSellMap(sell_map, sell_array, sell_raws, sell_count_map);

    deque<int> unmatch;
    for (int level = 5; level < BUYER_COL; level += 2) {
        unordered_map<string, stack<int>> buy_map;
        printf("level is %d--------------------------\n", (level - 3) / 2);
        updateBuyMap(buy_array, remains, unmatch, level, buy_count_map, buy_map);
        Timer timer;
        printf("process start.\n");
        solves += getLevelResults(buy_array, sell_array, buy_map, sell_map, results, remains, use, level, buy_count_map, sell_count_map);
        printf("solves:%d, elapsed:%.3f s\n", solves, timer.timeAndReset());
    }
    for (int x : remains) {
        unmatch.push_back(x);
    }
    countVertify(unmatch, use, sell_array, buy_array, sell_raws);

    Timer timer;
    printf("unmatch start\n");
    getUnmatchResult(buy_array, sell_array, unmatch, results, use, sell_raws);
    printf("end of unmatch. elapsed:%.3f s\n", timer.time());
}


int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("args length < 3\n");
        exit(-1);
    }

    Timer timer;
    printf("begin\n");
    Solution sol;
    sol.init();
    readFile(argv[1], sol.org_buy_cf, sol.org_buy_sr, BUY_RAWS * 2, BUYER_COL);
    readFile(argv[2], sol.org_sell_cf, sol.org_sell_sr, SELL_ALL_RAWS, SELLER_COL);
    printf("end of read file. elapsed:%.3f s\n", timer.timeAndReset());

    sol.cf_use = new bool[sell_cf_raws];
    sol.sr_use = new bool[sell_sr_raws];
    memset(sol.cf_use, 0, sizeof(bool) * sell_cf_raws);
    memset(sol.sr_use, 0, sizeof(bool) * sell_sr_raws);
    vector<string> results;
    process(sol.org_buy_cf, sol.org_sell_cf, sol.cf_use, results, sell_cf_raws);
    process(sol.org_buy_sr, sol.org_sell_sr, sol.sr_use, results, sell_sr_raws);

    writeResult(argv[3], results);
    printf("end. total time:%.3f s\n", timer.t_time());
}