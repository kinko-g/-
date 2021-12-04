#include <iostream>
#include <cmath>
int get_left_nodes_num(int tree_size) {
    int left_height = log2(tree_size + 1);
    // std::cout << left_height << "\t";
    int left_leaf_num = tree_size - (std::pow(2,left_height) - 1);
    // std::cout << left_leaf_num << "\t";

    left_leaf_num = 
        left_leaf_num > std::pow(2,left_height) / 2 ? 
        std::pow(2,left_height) / 2 : left_leaf_num;

    // std::cout << left_leaf_num << "\t";
    
    int left_nodes_num = (std::pow(2,left_height) / 2) - 1 + left_leaf_num;

    return left_nodes_num;
}

// solve(0,len - 1,0);
void solve(int *tree,int *cstree,int left,int right,int troot) {
    int n = right - left + 1;
    if(n == 0) return;
    int left_nodes = get_left_nodes_num(n);
    // std::cout << left_nodes << "\n";
    cstree[troot] = tree[left + left_nodes];

    int left_child = troot * 2 + 1;
    int right_child = left_child + 1;

    solve(tree,cstree,left,left + left_nodes - 1,left_child);
    solve(tree,cstree,left + left_nodes + 1,right,right_child);

}

int main() {
    int tree[10] = {0,1,2,3,4,5,6,7,8,9};
    int tree_size = sizeof(tree) / sizeof(int);
    
    int cstree[10] = {0};
    solve(tree,cstree,0,tree_size - 1,0);

    for(auto &n:cstree)std::cout << n << "\t";
    // std::cout << get_left_nodes_num(tree_size);
    return 0;
}