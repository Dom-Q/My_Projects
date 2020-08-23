#include "Binary_Tree.h"

Node::Node(int data){
    left = NULL;
    right = NULL;
    value = data;
}

// Returns value of a given node
int Node::get_value(){
    return value;
}

void Node::set_value(int data){
    value = data;
}

// Get height of any node
int Node::get_height(){
    // Base case
    if(this == NULL){
        // Level -1
        return -1;
    }
    // Recursively get height of sub-trees
    int left_height = this->left->get_height();
    int right_height = this->right->get_height();
    return max(left_height, right_height) + 1;
}