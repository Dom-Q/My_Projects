#include "encrypt.h"

int decipher(char* filename, int shifts, int width){
    // Open file
    FILE* file_ptr = fopen(filename, "r");
    if(!(file_ptr)){
        perror("fopen");
        return 0;
    }

    printf("File Opened Succesfully\n");

    // Count characters in file for array sizing
    char temp;
    int cipher = -1;
    int num_chars = 0;
    int num_spaces = 0;
    // Get which cipher was done on the file
    if(fscanf(file_ptr, "%d\n", &temp)){
        cipher = temp;
    }
    
    // Count regular charachters
    int x = 0;
    char select = 240;
    while(fscanf(file_ptr, "%c", &temp) != EOF){
        if(temp == ' ' || temp == '\n'){
            continue;
        }
        else if(temp == select){
            break;
        }
        else{
            num_chars++;
        }
    }
    int height = num_chars / width;

    // Count spaces
    while(fscanf(file_ptr, "%d ", &temp) != EOF){
        num_spaces++;
    }
    // Close file
    fclose(file_ptr);

    // Allocate memory
    int* space_indices = (int*)malloc(num_spaces*(sizeof(int)));
    char* matrix = (char*)malloc(num_chars*(sizeof(char)));


    // Re-Open and read in file contents
    FILE* new_file = fopen(filename, "r");
    fscanf(file_ptr, "%d\n", &temp);
    int index = 0;
    while(fscanf(file_ptr, "%c", &temp) != EOF){
        if(temp == ' ' || temp == '\n'){
            continue;
        }
        else{
            matrix[index] = temp;
            index++;
        }
    }

    // Some Testing
    int i = 0;
    int j = 0;
    for(i = 0; i < height; i++){
        for(j = 0; j < width; j++){
            printf("%c ", matrix[width * i + j]);
        }
        printf("\n");
    }
    printf("Cipher = %d\n", cipher);

    // Change to cipher
    switch(1){
        case 0:
        // Diagonal Route 
        break;
        case 1:
        printf("Case 1 entered\n");
        reverse_inward_spiral(&matrix, shifts, width, height);
        printf("Decryption Ran\n");
        break;
        // Inward Spiral from (0,0)
        default:
        break;

    }
    // Add spaces and remove fill ins
    

    // Free Memory
    free(space_indices);
    free(matrix);
    return 1;
}