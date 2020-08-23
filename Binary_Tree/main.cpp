#include "Binary_Tree.h"

int main(){
    vector<int> test_values = {9, 4, 6, 15, 7, 25, 23, 19, 8, 5, 8, 8, 15, 23};
    int i = 0;
    for(i = 0; i < test_values.size(); i++){
        cout << test_values[i] << " ";
    }
    cout << '\n';

    MergeSort(test_values);
    i = 0;
    for(i = 0; i < test_values.size(); i++){
        cout << test_values[i] << " ";
    }
    cout << "\n";

    remove_duplicates(test_values);

    i = 0;
    for(i = 0; i < test_values.size(); i++){
        cout << test_values[i] << " ";
    }
    cout << "\nOriginal Values Above\n";

    Binary_Tree* test_tree = new Binary_Tree(test_values);

    cout << "Tree Constructed: Root = " << test_tree->get_root() << '\n';

    Node* root_add = test_tree->get_root();
    test_tree = test_tree->balanced_delete(root_add,23);

    cout << "Value Deleted\n";

    test_values = test_tree->inorder(test_tree->get_root());

    cout << "Data sorted in order\n";

    i = 0;
    for(i = 0; i < test_values.size(); i++){
        cout << test_values[i] << " ";
    }
    cout << "\nValues with Deleted Node Inorder Above\n";

    /*test_values = test_tree->BFS(test_tree->get_root());

    i = 0;
    for(i = 0; i < test_values.size(); i++){
        cout << test_values[i] << " ";
    }
    cout << "\nValues with Deleted Node Level Order Above\n";
*/

    
    return 1;

}