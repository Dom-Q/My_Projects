#include "Binary_Tree.h"

// Remove duplicates in a sorted vector
void remove_duplicates(vector<int> &data){
    int i = 0;
    int j = 0;
    for(i = 0; i < data.size()-1; i++){
        j = i + 1;
        while(data[j] == data[i]){
            data.erase(data.begin() + j);
        }
    }
}

// Merge 2 sorted arrays
void Merge_Arrays(vector<int> &complete, vector<int> &left, vector<int> &right){
    // Indices for 3 vectors
    int complete_index = 0;
    int left_index = 0;
    int right_index = 0;
    // Compare sub-arrays starting from beginning and place merge into complete
    while((left_index < left.size()) && (right_index < right.size())){
        if(left[left_index] < right[right_index]){
            complete[complete_index] = left[left_index];
            complete_index++;
            left_index++;
        }
        else{
            complete[complete_index] = right[right_index];
            right_index++;
            complete_index++;
        }
    }
    // Once one sub-array has been merged
    if(left_index == left.size()){
        while(right_index < right.size()){
            complete[complete_index] = right[right_index];
            complete_index++;
            right_index++;
        }
    }
    else{
        while(left_index < left.size()){
            complete[complete_index] = left[left_index];
            complete_index++;
            left_index++;
        }
    }
    return;
}

// Mergesort
void MergeSort(vector<int> &data){
    // Base
    if(data.size() < 2){
        return;
    }
    // Divide
    size_t center = (data.size()) / 2;
    vector<int> left;
    vector<int> right;
    int i = 0;
    // Fill 2 sub-arrays
    for(i = 0; i < center; i++){
        left.push_back(data[i]);
    }
    i = 0;
    for(i = center; i < data.size(); i++){
        right.push_back(data[i]);
    }

    // Recursively call mergesort on sub-arrays
    MergeSort(left);
    MergeSort(right);

    // Merge sorted sub-arrays
    Merge_Arrays(data, left, right);
    
    return;
}