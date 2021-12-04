#include <iostream>
#include <memory>
#include <array>
#include <unordered_map>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <map>
#include <climits>
std::unordered_map<char,std::vector<std::pair<char,int>>> g {
    {'a',{{'b',5},{'c',1}}},
    {'b',{{'a',5},{'c',2},{'d',1}}},
    {'c',{{'a',1},{'b',2},{'d',4},{'e',8}}},
    {'d',{{'b',1},{'c',4},{'e',3},{'f',6}}},
    {'e',{{'c',8},{'d',3}}},
    {'f',{{'d',6}}}
};

struct cmp {
    bool operator()(std::pair<char,int> &l,std::pair<char,int> &r) {
        return l.second > r.second;
    }
};

int main() {
    std::priority_queue<std::pair<char,int>,
        std::vector<std::pair<char,int>>,
        cmp> q;
    std::set<char> visited {};
    std::map<char,int> distance {{'a',0}};
    for(auto &&p:g) {
        if(distance.count(p.first) <= 0) distance[p.first] = INT_MAX;
    }
    q.emplace('a',0);
    while(!q.empty()) {
        auto p = q.top();
        q.pop();
        if(visited.count(p.first)) continue;
        visited.insert(p.first);
        for(auto &&vw:g[p.first]) {
            if(visited.count(vw.first) <= 0) {
                if(p.second + vw.second < distance[vw.first]) {
                    q.emplace(vw.first,vw.second + p.second);
                    distance[vw.first] = p.second + vw.second;
                }
            }
        }
    }
    for(auto &&p:distance) std::cout << p.first << ":" << p.second << "\n";
    return 0;
}