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

// Searches for and returns the node with the given value
Node* Binary_Tree::search(int data){
    // Temporary address holder
    Node* current = root;
    // Traverse until node with identical value found
    while(data != current->get_value()){
        if(current == NULL){
            cout << "There is no node with this value.\n";
            return NULL;
        }
        else if(data < current->get_value()){
            current = current->left;
        }
        else{
            current = current->right;
        }
    }
    return current;
}

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
        next.push(current->left);
    }
    // Push right child onto queue
    if(current->right != NULL){
        next.push(current->right);
    }

    // Recursively call on next node in queue if not empty
    if(next.empty() != true){
        BFS(next.front());
    }
    return level_order;
}

// Inorder Traversal
vector<int> Binary_Tree::inorder(Node* current, vector<int> &inorder_values){
    // Base case
    if(current == NULL){
        return inorder_values;
    }
    // Left
    inorder(current->left, inorder_values);
    // Push current value
    inorder_values.push_back(current->get_value());
    // Right
    inorder(current->right, inorder_values);

    return inorder_values;
}

// Preoder Traversal
vector<int> Binary_Tree::preorder(Node* current, vector<int> &preorder_values){
    // Base Case
    if(current == NULL){
        return preorder_values;
    }
    // Push current
    preorder_values.push_back(current->get_value());
    // Left
    preorder(current->left, preorder_values);
    // Right
    preorder(current->right, preorder_values);

    return preorder_values;
}

// Postorder Traversal
vector<int> Binary_Tree::postorder(Node* current, vector<int> &postorder_values){
    // Base Case
    if(current == NULL){
        return postorder_values;
    }
    // Left
    postorder(current->left, postorder_values);
    // Right
    postorder(current->right, postorder_values);
    // Current
    postorder_values.push_back(current->get_value());

    return postorder_values;
}

// Self-Balancing insertion
Binary_Tree* Binary_Tree::balanced_insert(int new_val){
    vector<int> inorder_values = {};
    inorder_values = inorder(root, inorder_values);
    int i = 0;
    // Insert new value in order
    for(i = 0; i < inorder_values.size(); i++){
        if(new_val < inorder_values[i]){
            inorder_values.insert(inorder_values.begin() + i, new_val);
            break;
        }
        else if(new_val > inorder_values[i]){
            if(i == (inorder_values.size() - 1)){
                inorder_values.insert(inorder_values.begin() + i + 1, new_val);
                break;
            }
            else{
                continue;
            }
        }
        else if(new_val == inorder_values[i]){
            cout << "Value Already in Tree";
            return this;
        }
    }
    // Return balanced tree
    Binary_Tree* adjusted_tree = new Binary_Tree(inorder_values);
    return adjusted_tree;
}

// Self-Balancing Deletion
Binary_Tree* Binary_Tree::balanced_delete(Node* &current, int removed_value){
    if(current == NULL){
        cout << "This number is not in the tree.\n";
        vector<int> new_tree = {};
        new_tree = inorder(root, new_tree);
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
    if(current->get_height() == max_height){
        vector<int> new_tree = {};
        new_tree = inorder(root, new_tree);
        Binary_Tree* adjusted_tree = new Binary_Tree(new_tree);
        return adjusted_tree;
    }
    return NULL;
}

void Binary_Tree::print(Node* current, int current_height){

}