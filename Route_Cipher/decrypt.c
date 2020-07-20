#include "encrypt.h"

int decipher(int shifts, int width){
    // Open file
    FILE* file_ptr = fopen("Encrypted.txt", "r");
    if(!(file_ptr)){
        perror("fopen");
        printf("Make sure you previously encrypted a file using this program.");
        return 0;
    }

    printf("\nFile successfully opened.\nThis does not mean the keys are correct. If the keys are not entered properly the file will not be decrypted properly!\n");

    // Count characters in file for array sizing
    char temp;
    int temp_int;
    int cipher = -1;
    int num_chars = 0;
    int num_spaces = 0;
    int num_newlines = 0;
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
    while(fscanf(file_ptr, "%d ", &temp_int)){
        num_spaces++;
    }
    fscanf(file_ptr,"%c", &temp);
    fscanf(file_ptr,"%c", &temp);
    while(fscanf(file_ptr, "%d ", &temp_int) != EOF){
        num_newlines++;
    }
    // Close file
    fclose(file_ptr);

    // Allocate memory
    int* space_indices = (int*)malloc(num_spaces*(sizeof(int)));
    int* newline_indices = (int*)malloc(num_newlines*(sizeof(int)));
    char* matrix = (char*)malloc(num_chars*(sizeof(char)));


    // Re-Open and read in file contents
    FILE* new_file = fopen("Encrypted.txt", "r");
    fscanf(file_ptr, "%d\n", &temp);
    int index = 0;
    while(fscanf(file_ptr, "%c", &temp) != EOF){
        if(temp == ' ' || temp == '\n'){
            continue;
        }
        else if(temp == select){
            break;
        }
        else{
            matrix[index] = temp;
            index++;
        }
    }
    fscanf(new_file, "%c", &temp);
    // Get spaces
    int i = 0;
    while(fscanf(file_ptr, "%d ", &temp_int)){
        space_indices[i] = temp_int;
        i++;
    }
    fscanf(file_ptr,"%c", &temp);
    fscanf(file_ptr,"%c", &temp);
    int newline_counter = 0;
    while(fscanf(file_ptr, "%d ", &temp_int) != EOF){
        newline_indices[newline_counter] = temp_int;
        newline_counter++;
    }
    // Perform Decryption
    switch(cipher){
        case 0:
        // Diagonal Route 
        reverse_diagonal_route(&matrix, shifts, width, height);
        printf("Decryption Ran\n");
        break;
        case 1:
        reverse_inward_spiral(&matrix, shifts, width, height);
        printf("Decryption Ran\n");
        break;
        // Inward Spiral from (0,0)
        default:
        break;

    }
    // Add spaces, remove fill ins, write to file
    char* final_message = (char*)malloc((num_chars + num_spaces)*sizeof(char));
    i = 0;
    int space_matrix_index = 0;
    int full_index_counter = 0;
    int newline_matrix_index = 0;
    char fill_in = 237;
    for(i = 0; i < num_chars; i++){
        // Check space
        while(space_indices[space_matrix_index] == full_index_counter){
            final_message[full_index_counter] = ' ';
            space_matrix_index++;
            full_index_counter++;
        }
        // Check newlines
        while(newline_indices[newline_matrix_index] == full_index_counter){
            final_message[full_index_counter] = '\n';
            newline_matrix_index++;
            full_index_counter++;
        }
        // Check fill ins
        if(matrix[i] == fill_in){
            continue;
        }
        // Add Char
        final_message[full_index_counter] = matrix[i];
        full_index_counter++;
    }
    // Write to file
    FILE* final_file = fopen("Decrypted.txt", "w");
    i = 0;
    space_matrix_index = 0;
    for(i = 0; i < full_index_counter; i++){
        fprintf(final_file,"%c", final_message[i]);
    }
    printf("-------------------------------------------------------------\n"
           " Your decrypted message is in Decrypted.txt.\n" 
           " If it is not correct, then you did not enter the right keys.\n"
           "-------------------------------------------------------------\n");

    // Free Memory
    fclose(new_file);
    free(space_indices);
    free(matrix);
    free(final_message);
    return 1;
}