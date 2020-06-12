#include "encrypt.h"

int count_chars(FILE** file_ptr, int* chars, int* spaces, int* tabs, int* newlines){    
    char temp;
    int total_chars =0;
    // Parse file
    while(fscanf(*file_ptr, "%c", &temp) != EOF){
        // If space char add 1 to space counter
        if(temp == ' '){
            (*spaces)++;
            total_chars++;
            continue;
        }
        // If newline char add 1 to newline counter
        else if(temp == '\n'){
            (*newlines)++;
            total_chars++;
            continue;
        }
        // If tab char add 1 to tab counter
        else if(temp == '\t'){
            (*tabs)++;
            total_chars++;
            continue;
        }
        // Otherwise not whitespace, add 1 to regular char counter
        else{
            (*chars)++;
            total_chars++;
            continue;
        }
    }
    if(total_chars > 15000){
        return 0;
    }
    else{
        return 1;
    }
}

int get_row(int size){
    int i = 2;
    int num_factors = 0;
    // Count number of factors of the size of the matrix
    for(i = 2; i < size; i++){
        if(size % i == 0){
            num_factors++;
        }
        else{
            continue;
        }
    }
    printf("factors = %d\n", num_factors);
    // Put all factors (greater than 1) in a list
    int* factors = (int*)malloc(sizeof(int)*num_factors);
    i = 2;
    int j = 0;
    for(i = 2; i < size; i++){
        if(size % i == 0){
            factors[j] = i;
            j++;
        }
        else{
            continue;
        }
    }
    int row = 0;
    // Randomly select a one of those factors from the list
    // Makes sure number of rows is greater than 3
    while(row < 4)
    {
        srand(time(NULL));
        int random = (rand() % num_factors);
        printf("index = %d\n", random);
        row = factors[random];
        // Check if col will also be greater than 3
        if((size / row) < 4){
            row = -1;
        }
        printf("row = %d\n", row);
    }
    free(factors);
    return row;
}

int diagonal_route(char** matrix, int size, int width, int height){
    srand(time(NULL));
    int num_shifts = rand() % (width - 2);
    int largest;
    int remainder;
    int num_in_diag;
    int i = 0;
    int j = 0;
    int new_index = 0;
    char* new_matrix = (char*)malloc(size*(sizeof(char)));
    // First half of diagonals
    // Loop through each diagonal index in each column
    for(i = 1; i < width; i++){
        largest = i * width;
        if(i >= height){
            num_in_diag = height;
        }
        else{
            num_in_diag = i + 1;
        }
        for(j = i; j <= largest; j = j + (width - 1)){
            new_index = (num_shifts % (i + 1))*(width - 1) + j;
            // Check overflow
            if(new_index > largest){
                remainder = new_index - largest;
                new_index = largest - (width - 1)*(num_in_diag -((remainder / (width - 1)) % num_in_diag));
                new_matrix[new_index] = (*matrix)[j];
            }
            else{
                new_matrix[new_index] = (*matrix)[j];
            }
        }        
    }
    // Second half of diagonals
    int row = 1;
    int first;
    largest = 0;
    new_index = 0;
    num_in_diag = 0;
    remainder = 0;
    j = 0;
    for(row = 1; row < height; row++){
        first = ((row + 1)*(width) - 1);
        // alg to find number of elements in diagonal for variable dimensioned matrix
        if((width + row) <= height){
            num_in_diag = width;
        }
        else{
            num_in_diag = height - row;
        }
        largest = first + ((num_in_diag-1)*(width - 1));
        for(j = first; j <= largest; j = j + (width - 1)){
            new_index = (num_shifts % num_in_diag)*(width - 1) + j;
            if(new_index > largest){
                remainder = new_index - largest;
                new_index = largest - (width - 1)*(num_in_diag - ((remainder / ((width - 1))) % num_in_diag));
                new_matrix[new_index] = (*matrix)[j];
            }
            else{
                new_matrix[new_index] = (*matrix)[j];
            }
        }
    }
    new_matrix[0] = (*matrix)[0];
    // Print matrices
    int l = 0;
    int m = 0;
    for(l = 0; l < height; l++){
        for(m = 0; m < width; m++){
            printf("%c ", (*matrix)[(width*l + m)]);
        }
        printf("\n");
    }
    printf("\nEncrpyted Matrix\n");
    l = 0;
    m = 0;
    for(l = 0; l < height; l++){
        for(m = 0; m < width; m++){
            printf("%c ", new_matrix[(width*l + m)]);
        }
        printf("\n");
    }

    // Replace original matrix with encrypted matrix and free memory
    int z = 0;
    for(z = 0; z < size; z++){
        (*matrix)[z] = new_matrix[z];
    }
    free(new_matrix);
    return num_shifts;
}

int inward_spiral_route(char** matrix, int size, int width, int height){
    int num_shifts;

    char* new_matrix = (char*)malloc(sizeof(char)*size);
    int col = width - 1;
    int row = height - 1;
    int i = 0;
    int j;
    int new_index;
    int row_end;
    int remainder;
    int largest_in_row;
    // Find which row to stop after
    if(height % 2 == 1){
        row_end = (height + 1) / 2;
    }
    else{
        row_end = height / 2;
    }
    for(i = 0; i < row_end; i++){
        largest_in_row = (((i+1) * width) - 1) - ((width - 1) - col);
        for(j = 0; j <= col; j++){
            new_index = (width*i + j) + num_shifts;
            if(new_index > largest_in_row){
                // Also need to check if larger than last in column?
                remainder = new_index - largest_in_row;
                new_index = remainder*width + largest_in_row;

            }
            else{
                new_matrix[new_index] = (*matrix)[width*i + j];
            }
        }
    }
}