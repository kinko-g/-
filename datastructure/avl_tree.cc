// #include <functional>
// #include <map>
// #include <tuple>
// #include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cmath>
struct Node {
    Node(int data_):
        data{data_},
        left{nullptr},
        right{nullptr},
        height{1} {

    }
    int data;
    size_t height;
    Node *left;
    Node *right;
};

int get_height(Node *root) {
    if(root == nullptr) return 0;
    return root->height;
}

// RR
Node* rotate_left(Node *root) {
    Node *temp = root->right;
    root->right = temp->left;
    temp->left = root;
    root->height = std::max(get_height(root->left),get_height(root->right)) + 1;
    temp->height = std::max(get_height(temp->left),get_height(temp->right)) + 1;

    return temp;
}
// LL
Node* rotate_right(Node *root) {
    Node *temp = root->left;
    root->left = temp->right;
    temp->right = root;
    root->height = std::max(get_height(root->left),get_height(root->right)) + 1;
    temp->height = std::max(get_height(temp->left),get_height(temp->right)) + 1;

    return temp;
}

Node* rotate_LR(Node *root) {
    root->left = rotate_left(root->left);
    return rotate_right(root);
}

Node* rotate_RL(Node *root) {
    root->right = rotate_right(root->right);
    return rotate_left(root);
}
Node* insert(Node *root,int data) {
    if(root == nullptr) {
        root = new Node(data);
    }
    if(data < root->data) {

        root->left = insert(root->left,data);
        root->height = std::max(get_height(root->left),get_height(root->right)) + 1;
        
        if(get_height(root->left) - get_height(root->right) > 1) {
            if(data < root->left->data) {
                root = rotate_right(root);
            }
            else {
                root = rotate_LR(root);
            }
        }
    }
    else if(data > root->data) {
        root->right = insert(root->right,data);
        root->height = std::max(get_height(root->left),get_height(root->right)) + 1;

        if(get_height(root->left) - get_height(root->right) > 1) {
            if(data > root->right->data) {
                root = rotate_left(root);
            }
            else {
                root = rotate_RL(root);
            }
        }
    }
    return root;
}

void print_tree(Node *root) {
    if(!root) return;
    print_tree(root->left);
    std::cout << root->data << "\t";
    print_tree(root->right);
}

int main() {
    std::vector<int> vec = {2,1,123,12,8,9,7,5,4,3,1111};
    Node *root = new Node(0);
    for(auto n:vec) {
        insert(root,n);
    }

    print_tree(root);

    return 0;
}