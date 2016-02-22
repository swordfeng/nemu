extern "C" {
#include "common.h"
}

#include <map>
#include <vector>
#include <algorithm>
#include <cstdio>
using namespace std;

map<const char *, long long> prof_counts;

extern "C"
void profile_record(const char *func_name) {
    prof_counts[func_name]++;
}

extern "C"
void profile_print_result() {
    long long totalcounts = 0;
    vector<pair<const char *, long long>> res;
    for (auto rec: prof_counts) {
        totalcounts += rec.second;
        res.push_back(rec);
    }
    if (totalcounts == 0) return;
    sort(res.begin(), res.end(), 
            [](const pair<const char *, long long> &a, const pair <const char *, long long> &b) {
            return a.second > b.second;
            });
    if (res.size() > 20) res.resize(20); // limits to 20 outputs
    for (auto rec: res) {
        printf("%s: %.2f\n", rec.first, (double) rec.second / totalcounts);
    }
}

