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

int find_quadrant(int index, int width, int height){
    double width_split = width / 2.0;
    double height_split = height / 2.0;
    int row = floor(index / width);
    int col = index % width;
    if((col < width_split) && (row < height_split)){
        return 1;
    }
    else if((col >= width_split) && (row < height_split)){
        return 2;
    }
    else if((col < width_split) && (row >= height_split)){
        return 3;
    }
    else if((col >= width_split) && (row >= height_split)){
        return 4;
    }
    else{
        return 0;
    }
}

int find_row_bound(int quadrant, int index, int width, int height){
    // Algorithm for finding row bound of a given index
    int row_bound = -1;
    int row = floor(index / width);
    int col = index % width;
    if(quadrant == 1){
        // Pre-row bound
        if(col >= row){
            row_bound = row;
        }
        else{
            row_bound = col + 1;
        }
    }
    if(quadrant == 2){
        // Pre-row bound
        if(((width - col) - 1) >= row){
            row_bound = row;
        }
        else{
            row_bound = (width - col) - 1;
        }
    }
    if(quadrant == 3){
        // Post-row bound
        if(((height - 1) - row) <= col){
            row_bound = row;
        }
        else{
            row_bound = (height - 1) - col;
        }
    }
    if(quadrant == 4){
        // Post-row bound
        if(((height - 1) - row) > ((width - 1) - col)){
            row_bound = (height - 1) - ((width - 1) - col);
        }
        else{
            row_bound = row;
        }
    }
    return row_bound;
}

int find_col_bound(int quadrant, int index, int width, int height){
    // Find column bound for each index
    int row = floor(index / width);
    int col = index % width;
    int col_bound = -1;
    if(quadrant == 1){
        if(col >= row){
            col_bound = row - 1;
        }
        else{
            col_bound = col;
        }
    }
    if(quadrant == 2){
        if(((width - 1) - col) > row){
            col_bound = (width - 1) - row;
        }
        else{
            col_bound = col;
        }
    }
    if(quadrant == 3){
        if(col <= ((height-1) - row)){
            col_bound = col;
        }
        else{
            col_bound = (height - 1) - row;
        }
    }
    if(quadrant == 4){
        if(((width - 1) - col) <= ((height - 1) - row)){
            col_bound = col;
        }
        else{
            col_bound = (width - 1) - ((height - 1) - row);
        }
    }
    return col_bound;
}


int is_row_median(int height, int row){
    double med = height / 2.0;
    if(med != height / 2){
        if(row == floor(med)){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }   
    return 0;
}

int is_col_median(int width, int col){
    double med = width / 2.0;
    if(med != width / 2){
        if(col == floor(med)){
            return 1;
        }
        else{
            return 0;
        }
    }
    else{
        return 0;
    }   
    return 0;
}

int col_med_exists(int width){
    int bisector = width / 2.0;
    if(bisector != (width / 2)){
        return 1;
    }
    else{
        return 0;
    }
}

int row_med_exists(int height){
    int bisector = height / 2.0;
    if(bisector != (height / 2)){
        return 1;
    }
    else{
        return 0;
    }
}

int find_spiral_end(int width, int height){ 
    // If both medians exist
    if(col_med_exists(width) && row_med_exists(height)){
        // Square (Odd dimensions)
        if(width == height){
            return (width * (height / 2)) + (width / 2);
        }
        // Horizontal
        else if (width > height){
            int difference = width - height;
            return (width * (height / 2)) + ((width / 2 ) + (difference / 2));
        }
        // Vertical
        else{
            int difference = height - width;
            return (width * ((difference / 2) + (height / 2))) + (width / 2);
        }
    }
    // If only a col med
    else if(col_med_exists(width)){
        if(width > height){
            int row = height / 2;
            int col = (height / 2) - 1;
            return width*row + col;
        }
        // height > width
        else{
            int col = width / 2;
            int row = height - (width / 2) - 1;
            return width*row + col;
        }
    }
    else if(row_med_exists(height)){
        if(width > height){
            int row = height / 2;
            int col = width - (height / 2) - 1;
        }
        // height > width
        else{
            int col = (width / 2) - 1;
            int row = width / 2; 
        }
    }
    // No med
    else{
        if(width > height){

        }
        else if(width < height){

        }
        // If square (even dimensions)
        else{
            int row = height / 2;
            int col = (width / 2) - 1;
            return width * row + col;
        }
    }
}