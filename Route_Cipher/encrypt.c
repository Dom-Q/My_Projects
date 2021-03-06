#include "encrypt.h"

int cipher(char* filename){
    // Open file 
    FILE* file_ptr;
    file_ptr = fopen(filename, "r");
    // Check if file opened successfully
    if (!file_ptr){
        perror("fopen");
        return 0;
    }

    // Count number of encryptable characters
    int num_chars = 0;
    int num_spaces = 0;
    int num_tabs = 0;
    int num_newlines = 0;
    char temp;
    int counter_status;

    // Function to count number of characters (including whitespace chars) in file
    counter_status = count_chars(&file_ptr, &num_chars, &num_spaces, &num_tabs, &num_newlines);
    if(counter_status == 0){
        printf("Your file exceeds 15000 characters.\n");
        return 0;
    }

    // Add fill-in chars to get a perfect square
    // This ensures that there will be a factor of the array size that is greater than 3,
    // regardless of the number of characters in the file
    while(sqrt(num_chars) != floor(sqrt(num_chars))){
        num_chars++;
    }
    // Make sure file isn't too small
    if(num_chars < 20){
        printf("Your file must contain more than 20 characters.\n");
        return 0;
    }

    //Dynamically create arrays to hold indices of white spaces in file
    int* space_indices = (int*)malloc(num_spaces*(sizeof(int)));
    int* tab_indices = (int*)malloc(num_tabs*(sizeof(int)));
    int* newline_indices = (int*)malloc(num_newlines*(sizeof(int)));

    // Allocate memory for and build matrix
    char* table = malloc((sizeof(char))*num_chars);
    int size = sizeof(table) / sizeof(char);
    int rows;
    int cols;
    rows = get_row(num_chars);
    cols = num_chars / rows;

    // Close and re-open and re-read file and fill the matrix ommitting whitespace
    fclose(file_ptr);
    file_ptr = fopen(filename, "r");
    if(file_ptr != NULL){
        int i = 0;
        int file_index = 0;
        int sp = 0;
        int t = 0;
        int nl = 0;
        // Load non-whitespace chars into 1D array and store index of any white space chars
        while(fscanf(file_ptr, "%c", &temp) != EOF){
            if(temp == ' '){
                space_indices[sp] = file_index;
                sp++;
            }
            else if(temp == '\t'){
                tab_indices[t] = file_index;
                t++;
            }
            else if(temp == '\n'){
                newline_indices[nl] = file_index;
                nl++;
            }
            else{
                table[i] = temp;
                i++;
            }
            file_index++;
        }
        while(i < num_chars){
            table[i] = 237;
            i++;
        }

        // Perform Cipher
        int num_shifts = -1;
        int num_ciphers = 2;
        srand(time(NULL));
        int cipher = rand() % num_ciphers;
        switch(cipher){
            case 0:
            // Diagonal route
            printf("Diagonal Route Encryption\n");
            num_shifts = diagonal_route(&table, num_chars, cols, rows);
            break;
            case 1:
            // Inward spiral from (0,0)
            printf("Spiral Route Encryption\n");
            num_shifts = inward_spiral_route(&table, num_chars, cols, rows);
            if(num_shifts == -1){
                return 0;
            }
            default:
            break; 
        }
        printf("- KEYS -\n");
        printf("Rows = %d\tCols = %d\n", rows, cols);
        printf("Shifts = %d\n", num_shifts);
        //Write encrypted matrix to file in matrix format
        FILE* new_file = fopen("Encrypted.txt", "w");
        fprintf(new_file, "%d\n", cipher);
        i = 0;
        int j = 0;
        for(i = 0; i < rows; i++){
            for(j = 0; j < cols; j++){
                fprintf(new_file, "%c ", table[i*cols + j]);
            }
            fprintf(new_file, "\n");
        }
        fprintf(new_file, "\n");
        // Char to mark end of matrix
        fprintf(new_file, "%c\n", 240);
        i = 0;
        j = 0;
        // Space indices
        for(i = 0; i < num_spaces; i++){
            fprintf(new_file, "%d ", space_indices[i]);
        }
        fprintf(new_file, "\n");
        fprintf(new_file, "%c\n", 240);
        // Newline indices
        i = 0;
        for(i = 0; i < num_newlines; i++){
            fprintf(new_file, "%d ", newline_indices[i]);
        }
        printf("-------------------------------------------------------------------------------------------\n"
               "Your file has been encrypted.\n"
               "The encrypted text has been written to Encrypted.txt\n"
               "To decrypt, simply choose 2 from the menu.\n"
               "Only the most recent text written to Encrypted.txt may be decrypted.\n"
               "Your keys are printed above. You will need to remember them in order to decrypt the file.\n"
               "-------------------------------------------------------------------------------------------\n");
    }
    else{
        printf("There was an error fetching your file.\n");
        return 0;
    }
    fclose(file_ptr);
    free(space_indices);
    free(tab_indices);
    free(newline_indices);
    free(table);
    return 1;
}