#include "encrypt.h"

int main(){
    int valid_title = 0;
    char title[30];   
    int title_chars = 0;
    // Get file name from user
    while(valid_title == 0){
        printf("What is the name of the file or path you would like to encrypt?\n");
        fgets(title, 60, stdin); 
        int t = 0;
        // Check if filename is valid (no white space)
        while(title[title_chars] != '\n'){
            if(title[title_chars] == ' ' || title[title_chars] == '\t'){
                printf("The name of your document cannot have spaces.\n");
                valid_title = 0;
                title_chars++;
                break;
            }
            else{
                valid_title = 1;
                title_chars++;
                continue;
            }
        }
    }
    printf("%d", title_chars);
    printf("Valid title\n");
    printf("%s", title);
    // dynamically allocate memory for title array
    // The 3 extra memory locations are for 2 quotation marks and a null terminator
    char* filename = (char*)malloc(title_chars*sizeof(char) + 1*sizeof(char));
    // Copy title into allocated array
    int j = 0;
    for(j = 0; j < (title_chars / sizeof(char)); j++){
        filename[j] = title[j];
        printf("%c\n", filename[j]);
    }
    // Add null terminator
    filename[j] = '\0';

    // Encrypt
    int status = cipher(filename, title_chars);

    // Print status and free memory
    printf("%d", status);
    free(filename);
}
