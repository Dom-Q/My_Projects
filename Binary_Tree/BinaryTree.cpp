#include "Binary_Tree.h"


// Constructors

// No-arg 
Binary_Tree::Binary_Tree(){
    root = NULL;
    max_height = root->get_height();
}

// Paramaterized 
Binary_Tree::Binary_Tree(const vector<int> &data){
    root = build_tree(data, 0, (data.size())-1);
    max_height = root->get_height();
}

// Returns root node
Node* Binary_Tree::get_root(){
    return root;
}

// Internal Methods

// Build Balanced Binary Search Tree
Node* Binary_Tree::build_tree(const vector<int> &data, int start_index, int end_index){
    // Check bounds
    if(start_index > end_index){
        return NULL;
    }
    else{
        // Divide and create subtree
        int center = (start_index + end_index) / 2;
        Node* new_root = new Node(data[center]);
        new_root->left = build_tree(data, start_index, center-1);
        new_root->right = build_tree(data, center + 1, end_index);
        return new_root;
    }
}


// Accessible Methods

// Returns minimum node in tree
Node* Binary_Tree::find_min(Node* current){
    Node* temp = current;
    while(temp->left != NULL){
        temp = temp->left;
    }
    return temp;
}

// Breadth-First-Search (Level Order) Traversal
vector<int> Binary_Tree::BFS(Node* current){
    // Static containers to keep track of next levels
    static vector<int> level_order = {};
    static queue<Node*> next = {};
    // Push value of root first 
    level_order.push_back(current->get_value());
    // Pop first element off queue if there is one
    if(next.empty() != true){
        next.pop();
    }
    // Push left child onto queue
    if(current->left != NULL){
        cout << "Next left = Address : " << current->left << " Value : " << current->left->get_value() << '\n';
        next.push(current->left);
    }
    // Push right child onto queue
    if(current->right != NULL){
        cout << "Next right = Address : " << current->right << " Value : " << current->right->get_value() << '\n';
        next.push(current->right);
    }

    // Recursively call on next node in queue if not empty
    if(next.empty() != true){
        BFS(next.front());
    }
    return level_order;
}

// Inorder Traversal
vector<int> Binary_Tree::inorder(Node* current){
    // Vector to hold values in order
    static vector<int> inorder_values = {};
    // Base case
    if(current == NULL){
        return inorder_values;
    }
    // Left
    inorder(current->left);
    // Push current value
    inorder_values.push_back(current->get_value());
    // Right
    inorder(current->right);
    
    return inorder_values;
}

void Binary_Tree::print(Node* current, int current_height){

}

// Self-Balancing insertion
Binary_Tree* Binary_Tree::balanced_insert(int new_val){
    vector<int> inorder_values = inorder(root);
    int i = 0;
    // Insert new value in order
    for(i = 0; i < inorder_values.size(); i++){
        if(new_val > inorder_values[i]){
            inorder_values.insert(inorder_values.begin(), new_val);
            break;
        }
        else{
            continue;
        }
    }
    
    // Return balanced tree
    Binary_Tree* adjusted_tree = new Binary_Tree(inorder_values);
    return adjusted_tree;
}

Binary_Tree* Binary_Tree::balanced_delete(Node* &current, int removed_value){
    if(current == NULL){
        cout << "This number is not in the tree.\n";
        vector<int> new_tree = inorder(root);
        Binary_Tree* adjusted_tree = new Binary_Tree(new_tree);
        return adjusted_tree;
    }
    // Right if greater
    if(removed_value > (current->get_value())){
        balanced_delete(current->right, removed_value);
    }
    // Left if less
    else if(removed_value < (current->get_value())){
        balanced_delete(current->left, removed_value);
    }
    // Delete if equal
    else{
        // Node with 0 children
        if(current->right == NULL && current->left == NULL){
            delete current;
            current = NULL;
            return NULL;
        }
        // Node with no right child
        else if(current->right == NULL){
            Node* temp = current;
            current = current->left;
            delete temp;
            return NULL;
        }
        // Node with no left child
        else if(current->left == NULL){
            Node* temp = current;
            current = current->right;
            delete temp;
            return NULL;
        }
        // Node with both children
        else{
            Node* temp = find_min(current->right);
            current->set_value(temp->get_value());
            delete temp;
            return NULL;
        }
    }
    // Balance tree and return
    vector<int> new_tree = inorder(root);

    int j = 0;
    for(j = 0; j < new_tree.size(); j++){
        cout << new_tree[j] << " ";
    }
    cout << '\n';

    cout << "Left BFS\n";
    Binary_Tree* adjusted_tree = new Binary_Tree(new_tree);
    return adjusted_tree;

}