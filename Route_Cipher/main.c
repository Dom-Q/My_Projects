#include "encrypt.h"

int main(){
    printf(""
    "====================================\n"
    "                                    \n"
    "                Menu                \n"
    "                ----                \n"
    "          Type 1 to Encrypt         \n"      
    "          Type 2 to Decrypt         \n"
    "                                    \n"
    "====================================\n"
    "");
    int menu = 0;
    char garbage;
    scanf("%d", &menu);
    scanf("%c", &garbage);

    // Get file name
    int valid_title = 0;
    char title[60];
    title[0] = 'x';   
    int title_chars = 0;
    // Get file name from user
    while(valid_title == 0){
        if(menu == 1){
            printf("What is the name of the file or path you would like to encrypt?\n");
        }
        else{
            break;
        }
        fgets(title, 60, stdin); 
        int t = 0;
        // Check if filename is valid (no white space)
        while(title[title_chars] != '\n'){
            if(title[title_chars] == ' ' || title[title_chars] == '\t'){
                printf("The name of your document cannot have spaces.\n");
                valid_title = 0;
                title_chars = 0;
                break;
            }
            else{
                valid_title = 1;
                title_chars++;
                if(title_chars == 60){
                    if(title[59] != '\0'){
                        printf("Your title is too long. It can be at most 60 characters including the suffix.");
                        valid_title = 0;
                        title_chars = 0;
                        break;
                    }
                }
                continue;
            }
        }
        printf("Valid title\n");
    }
    // Dynamically allocate memory for title array
    // The extra memory location is for a null terminator
    char* filename = (char*)malloc(title_chars*sizeof(char) + 1*sizeof(char));
    // Copy title into allocated array
    int j = 0;
    for(j = 0; j < (title_chars / sizeof(char)); j++){
        filename[j] = title[j];
    }
    // Add null terminator
    filename[j] = '\0';
    if(menu == 1){
        // Encrypt
        int status = cipher(filename);
        // Print status and free memory
        printf("\nStatus - %d\n", status);
    }
    
    // Takes message previously encrypted with my encryption algorithm and decrypts
    else if(menu == 2){
        int cipher_key = -1;
        int dimension_key = 0;
        printf("What are the keys (number of shifts and width) to your file? Enter them in this format : <shifts> <width> (2 numbers separated by a space)\n");
        if(scanf("%d %d", &cipher_key, &dimension_key) != 2){
            printf("You did not enter the keys correctly.\n");
            exit(0);
        }
        printf("Succesfully entered keys\n");
        scanf("%c", &garbage);
        int status = -1;
        status = decipher(cipher_key, dimension_key);
        printf("\nStatus - %d\n", status);
    }
    free(filename);
}
