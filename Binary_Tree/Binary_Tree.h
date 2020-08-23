#include "stdlib.h"
#include "math.h"
#include <iostream>
#include <vector>
#include <queue>

#include "Algorithms.h"

using std::vector;
using std::queue;
using std::cout;
using std::max;

struct Node{
    private:
    int value;
    public:
    // Member Variables
    Node* left;
    Node* right;
    // Node Constructors
    Node(int data);
    // Methods
    int get_value();
    void set_value(int data);
    // Get/Set Height of any Node
    int get_height();
};

struct Binary_Tree{
    private:

    //------------------//
    // Member Variables //
    //----------------- //

    Node* root;
    int max_height;

    // Internal Methods
    Node* build_tree(const vector<int> &data, int start, int end);

    public:

    // Contructors
    Binary_Tree();
    Binary_Tree(const vector<int> &data);

    //---------//
    // Methods //
    //---------//

    // Get tree root node
    Node* get_root();

    // Traversal
    // Returns tree values in level order
    vector<int> BFS(Node* current);
    // Returns tree values in order
    vector<int> inorder(Node* current);

    // Insertion and Deletion
    // Returns balanced tree with inserted node
    Binary_Tree* balanced_insert(int new_val);
    // Returns balanced tree with deleted node
    Binary_Tree* balanced_delete(Node* &current, int new_val);

    // Print tree
    void print(Node* top, int current_height);

    // Returns node with minimum value in tree
    Node* find_min(Node* current);
};

