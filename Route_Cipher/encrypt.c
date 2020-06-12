#include "encrypt.h"

int cipher(char* filename, int array_size){
    // Open file 
    printf("%s\n", filename);
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
    printf("%d\n", num_chars);

    // Add fill-in chars to get a perfect square
    // This ensures that there will be a factor of the array size that is greater than 3,
    // regardless of the number of characters in the file
    while(sqrt(num_chars) != floor(sqrt(num_chars))){
        num_chars++;
    }
    printf("Adjusted number of characters in file = %d\n", num_chars);

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

    printf("Rows = %d\tCols = %d\n", rows, cols);

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
                continue;
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
            printf("%d\n", i);
        }
        printf("%d\n", i);

        int testers = 0;
        for(testers = 0; testers < sp; testers ++){
            printf("%d ", space_indices[testers]);
        }
        printf("\n");

        // Print matrix
        int p = 0;
        for(p = 0; p < num_chars; p++){
            printf("%c", table[p]);
        }
        printf("\n");

        // Perform Cipher
        int num_shifts = -1;
        int num_ciphers = 1;
        srand(time(NULL));
        int cipher = rand() % num_ciphers;
        switch(num_ciphers){
            case 0:
            // Diagonal route
            num_shifts = diagonal_route(&table, num_chars, cols, rows);
            printf("Shifts = %d\n", num_shifts);
            break;
            default:
            
        }

    }
    else{
        printf("There was an error fetching your file.\n");
        return 0;
    }
    fclose(file_ptr);
    free(space_indices);
    free(tab_indices);
    free(newline_indices);
    return 1;
}