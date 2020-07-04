#include "encrypt.h"

// Iterative Function for Diagonal Route //
// ------------------------------------- //

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
    printf("\nOriginal Matrix\n");
    int p = 0;
    int q = 0;
    for(p = 0; p < height; p++){
        for(q = 0; q < width; q++){
            printf("%c ", (*matrix)[(width*p) + q]);
        }
        printf("\n");
    }
    printf("\n");

    printf("\nEncrpyted Matrix\n");
    int l = 0;
    int m = 0;
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

// Recursive Function for Inward Spiral //
// ------------------------------------ //

int find_inward_spiral_index(int shifts, int width, int height, int index){
    if(shifts == 0){
        return index;
    }
    int quadrant = find_quadrant(index, width, height);
    int row = floor(index / width);
    int col = index % width;
    int row_bound = find_row_bound(quadrant, index, width, height);
    int col_bound = find_col_bound(quadrant, index, width, height);

    // printf("Current row = %d\tCurrent col = %d\n", row, col);
    // printf("Quadrant %d\n", quadrant);

    if(shifts > 0){
        switch(quadrant){
            // Quadrant 1
            case 1:
            // If col med and cannot go right
            if((is_col_median(width, col)) && (col_bound + 1 >= width / 2)){
                // Always go down if not row-med
                if(!(is_row_median(height, row))){
                    shifts--;
                    index = index + width;
                    return find_inward_spiral_index(shifts, width, height, index);
                }
                // If dead center and and cant go right and pre row less than med, we should be able to go down
                else if(row_bound < (height / 2)){
                    shifts--;
                    index = index + width;
                    return find_inward_spiral_index(shifts, width, height, index);
                }
                // Otherwise, if center and cant go down or right it should be restart
                else{
                    index - 0;
                    shifts--;
                    return find_inward_spiral_index(shifts, width, height, index);
                }
            }
            // Check if an upward shift is available
            else if(row > row_bound){
                index = index - width;
                shifts--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // Check if med-row (should have already tried to go up)
            else if(is_row_median(height, row)){
                // If no col med
                if(!(col_med_exists(width))){
                    // Can we go right?
                    // If not, it must be restart
                    if((col_bound + 1) >= (width / 2)){
                        index = 0;
                        shifts--;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                    // Otherwise go right
                    else{
                        index++;
                        shifts--;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                }
                // If there is a col med
                else{
                    // If not in the col median go right
                    if(!(is_col_median)){
                        shifts--;
                        index++;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                    // If dead center and pre-col bound < med we should be able to shift right
                    else if((col_bound + 1) < (width / 2)){
                        shifts--;
                        index++;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                    // Otherwise it should be restart since it 
                    else{
                        shifts--;
                        index = 0;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                }
            }
            // If not col median AND pre-col < width / 2 go right
            else if(!(is_col_median(width, col)) && ((col_bound + 1) < (width / 2.0))){
                index++;
                shifts--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // if med but pre col + 1 !> width / 2.0
            else if((is_col_median(width, col)) && ((col_bound + 1) < (width / 2))){
                // Shift right
                shifts--;
                index++;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // If can't go up or right and not median (final case?)
            else if(col_bound + 1 >= width / 2.0){
                shifts--;
                return find_inward_spiral_index(shifts, width, height, 0);
            }
            // Should cover all cases, but look out for more

            // Quadrant 2
            case 2:
            // First always go right if possible
            if(col_bound > col){
                index++;
                shifts--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // If no row median
            else if(!(row_med_exists(height))){
                // Always go down (should cover all cases)
                index = index + width;
                shifts--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // If there is a median
            else{
                // If not the row median
                if(!(is_row_median(height, row))){
                    // Go down
                    index = index + width;
                    shifts--;
                    return find_inward_spiral_index(shifts, width, height, index);
                }
                // If row med
                else{
                    // Check if can go down
                    if(row_bound < (height / 2)){
                        index = index + width;
                        shifts--;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                    else if(row_bound == (width / 2)){
                        // If at post bound, restart
                        if(col_bound == col){
                            shifts--;
                            index = 0;
                            return find_inward_spiral_index(shifts, width, height, index);
                        }
                        // Otherwise go right
                        else{
                            shifts--;
                            index++;
                            return find_inward_spiral_index(shifts, width, height, index);
                        }
                    }
                    // Should be all cases
                }
            }

            // Quadrant 3
            case 3:
            if(is_col_median(width, col)){
                // Col med and can go left
                if(col_bound < (width / 2)){
                    shifts--;
                    index--;
                    return find_inward_spiral_index(shifts, width, height, index);
                }
                // Can't go left
                else{
                    // If can go down go down
                    if(row_bound > row){
                        index = index + width;
                        shifts--;
                        return find_inward_spiral_index(shifts, width, height, index);
                    }
                    // Otherwise restart
                    else{
                        shifts--;
                        return find_inward_spiral_index(shifts, width, height, 0);
                    }
                }
            }
            // IF not col med
            else if(col > col_bound){
                // Always go left first
                index--;
                shifts--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // Can we go up
            else if((row_bound - 1) < (height / 2)){
                index = 0;
                shifts--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            else{
                shifts--;
                index = index - width;
                return find_inward_spiral_index(shifts, width, height, index);
            }

            // Quadrant 4
            case 4:
            // If can go down
            if(row < row_bound){
                shifts--;
                index = index + width;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            // Otherwise always go left
            else{
                shifts--;
                index--;
                return find_inward_spiral_index(shifts, width, height, index);
            }
            default:
            return -1;
        }
    }
    else{
        return -1;
    }
}

// Function for moving the chars to their new locations for spiral route //
// --------------------------------------------------------------------- //

int inward_spiral_route(char** matrix, int size, int width, int height){
    int num_shifts;
    srand(time(NULL));
    num_shifts = rand() % (width + height);
    char* new_matrix = (char*)malloc(sizeof(char)*size);
    // Parse original matrix and fill new matrix with shifted chars
    int i = 0;
    for(i = 0; i < size; i++){
        int new_index;
        new_index = find_inward_spiral_index(num_shifts, width, height, i);
        if(new_index == -1){
            return 0;
        }
        new_matrix[new_index] = (*matrix)[i];
        
    }

    // Print original matrix for testing
    // Print matrix
    int p = 0;
    int q = 0;
    for(p = 0; p < height; p++){
        for(q = 0; q < width; q++){
            printf("%c ", (*matrix)[(width*p) + q]);
        }
        printf("\n");
    }
    printf("\n");

    i = 0;
    // Replace old matrix with new matrix
    for(i = 0; i < size; i++){
        (*matrix)[i] = new_matrix[i];
    }

    // Print matrix for testing
    printf("\nEncrypted Matrix\n");
    i = 0;
    int j = 0;
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            printf("%c ", (*matrix)[width*i + j]);
        }
        printf("\n");
    }

    // Free memory and return key (number of shifts)
    free(new_matrix);
    return num_shifts;
}


//------------//
// Decryption //
//------------//

int find_reverse_spiral_index(int shifts, int width, int height, int index){
    // Base case
    if(shifts == 0){
        return index;
    }
    int row = floor(index / width);
    int col = index % width;
    int quadrant;
    int col_bound;
    int row_bound;
    quadrant = find_quadrant(index, width, height);
    row_bound = find_row_bound(quadrant, index, width, height);
    col_bound = find_col_bound(quadrant, index, width, height);

    // Diverge based on quadrant
    switch(quadrant){
        case 1:
        // Quadrant 1
        if(index == 0){
            index = find_spiral_end(width, height);
            shifts--;
            return find_reverse_spiral_index(shifts, width, height, index);
        }

        case 2:

        case 3:

        case 4:

        default:

    }
}

void reverse_inward_spiral(char** matrix, int shifts, int width, int height){
    char* new_matrix = (char*)malloc(width*height*(sizeof(char)));
    int i = 0;
    // Parse full matrix and find new index of each index
    for(i = 0; i < width*height; i++){

    }
}