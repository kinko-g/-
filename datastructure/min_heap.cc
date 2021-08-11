#include <iostream>

void swap(int &a,int &b) {
    int temp = a;
    a = b;
    b = std::move(temp);
}

void heapify(int *tree,int tree_size,int parent) {
    if(parent >= tree_size) return;
    int c1 = parent * 2 + 1;
    int c2 = parent * 2 + 2;
    int min = parent;

    if(c1 < tree_size && tree[c1] < tree[min]) min = c1;
    if(c2 < tree_size && tree[c2] < tree[min]) min = c2;

    if(min != parent) {
        swap(tree[parent],tree[min]);
        heapify(tree,tree_size,min);
    }
}

void build_heap(int *arr,int size) {
    int parent = (size - 1) / 2;
    for(int i = parent - 1;i >= 0;i --) {
        heapify(arr,size,i);
    }
}


void insert(int *tree,int size,int data) {
    int ist_idx = size - 1;
    for(;tree[(ist_idx - 1) / 2] > data && ist_idx > 0;ist_idx = (ist_idx - 1) / 2) {
        std::cout << "ist_idx: " << ist_idx << "\t data: " << tree[ist_idx] << "\n";
        tree[ist_idx] = tree[(ist_idx - 1) / 2];
    }
    std::cout << "ist_idx: " << ist_idx << "\t data: " << tree[ist_idx] << "\n";

    tree[ist_idx] = data;
}

int main() {
    int arr[6] = {23,26,10,46,24};
    build_heap(arr,5);
    insert(arr,6,9);
    for(auto&n:arr)std::cout << n << "\t"; 
    return 0;
}