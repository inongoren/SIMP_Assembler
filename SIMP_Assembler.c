#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#define MAX_LABEL 50
#define MAX_LINE_LENGTH 500
#define MAX_LINES 512
#define NAME_LENGTH 5
#define MAX_OPCODE_LEN 3
#define NUM_OF_COMMANDS 20
#define NUM_OF_REGS 16
#define MEMIN_LINE_LENGTH 8

//defining the structures needed for the Assembler (for the labels, commands and registers)
typedef struct labels {
    char name[MAX_LABEL];
    int location;
} Label;

typedef struct Command {
    char name[NAME_LENGTH];
    char opcode[MAX_OPCODE_LEN];
}command;

typedef struct REG {
    char name[NAME_LENGTH];
    char reg_num[MAX_OPCODE_LEN];
}reg;

void writing_func(char* line, FILE* fop, int label_num, Label* labels_arr, int line_fop_counter);
void dec_to_hex(char* token, char hex_arr[]);
void fill_the_line(char* data, char word_line[]);
int hex2Dec(char* hexdecnumber[]);
void uppercase(char* c);


int main()
{
    char input_file[] = "fib.asm";                                         //the input file name
    // Pass one procedure - search for labels and save their location
    int location_counter = 0, label_num = 0;                               // variables
    int max_word_location = 0;
    int word_address;
    Label labels_arr[MAX_LINES] = { "0", 0 };                              //initializing labels array
    char word[] = ".word";
    char hex_sign[] = "0x";
    // Open a file in read mode
    FILE* fptr;
    fptr = fopen(input_file, "r");

    //run on the file line by line
    while (!feof(fptr)) {
        int inst_line = 0;
        char* line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));         //allocating memory for line
        if (line != NULL) {
            fgets(line, sizeof(char) * MAX_LINE_LENGTH, fptr);               //gets a line from text
            if (strchr(line, ':') != NULL) {
                if (strchr(line, '$') != NULL)
                    inst_line = 1;
                char* token = strtok(line, ":");                            //check if there is a label in this line, searching for ":"
                if (strchr(token, '#') == NULL) {                           //check that : doesnt appear in a comment
                    strcpy(labels_arr[label_num].name, token);              //saving the label in the labels array
                    labels_arr[label_num].location = location_counter;
                    label_num++;
                    if (inst_line == 1)                                     //jumping location of lines if line is not empty
                        location_counter++;
                    free(line);
                    continue;
                }
            }
            else {                                                              //there is no label in this line
                if (strchr(line, '#') != NULL)                                  //switching "#" by '\0'
                    *strchr(line, '#') = '\0';
                if (strchr(line, '$') != NULL)                                // check if line is an instruction
                    location_counter++;                                       // if so - location ++
                if (strstr(line, word) != NULL) {                             // check if line is an .word line and if so save the address it sends to
                    char* address = strtok(line, " .word\t\n");              
                    if (strncmp(address, hex_sign, 2) == 0)
                        word_address = hex2Dec(address + 2);
                    else {
                        word_address = atoi(address);
                    }
                    if (max_word_location < word_address)
                        max_word_location = word_address;
                }
            }
            free(line);
        }
        else
            printf("Memory not allocated. \n");
    }

    //Between Passes - creating a text of zeros in the length as needed by the number of lines in the assembly files or by the command of .word
    FILE* fop;
    fop = fopen("memin.txt", "w");
    int max_len_memin;
    if (location_counter > max_word_location)
        max_len_memin = location_counter;
    else
        max_len_memin = max_word_location;
    for (int i = 0; i < max_len_memin; i++)
        fputs("00000000\n", fop);


    //pass two procedure - write the output file by going line by line on the input file and replacing the labels by their location


    fseek(fop, 0, SEEK_SET);
    fptr = fopen(input_file, "r");
    int line_fop_counter = 0;
    while (!feof(fptr)) {
        char* line = (char*)malloc(MAX_LINE_LENGTH * sizeof(char));         //allocating memory for line
        fgets(line, sizeof(char) * MAX_LINE_LENGTH, fptr);                  //gets a line from text
        if (line != NULL) {
            if (strchr(line, '#') != NULL)                                  //switching "#" by '\0'
                *strchr(line, '#') = '\0';
            if (strchr(line, '$') != NULL || strstr(line, word) != NULL) {  //if its a command line, start the writing process of the line
                writing_func(line, fop, label_num, labels_arr,line_fop_counter);
                line_fop_counter++;
            }
        }
        else
            printf("Memory not allocated. \n");
    }

    fclose(fptr);
    fclose(fop);
    return 0;
}


void writing_func(char* line, FILE* fop, int label_num, Label* labels_arr, int line_fop_counter)
{
    // writing funtion - translate from simp to machine language and making the writing procedure to the memin file
    char hex_sign[] = "0x";
    int decimalnum;
    FILE* fop2 = fop;
    char hex_arr[4] = { '\0' };
    char word[] = ".word";
    char word_line[MEMIN_LINE_LENGTH+1] = { '\0' };

    //initializing the table of commands
    command commands_arr[NUM_OF_COMMANDS] = {
            {"add", "00"},
            {"sub", "01"},
            {"and", "02"},
            {"or", "03"},
            {"sll", "04"},
            {"sra", "05"},
            {"srl", "06"},
            {"beq", "07"},
            {"bne", "08"},
            {"blt", "09"},
            {"bgt", "0A"},
            {"ble", "0B"},
            {"bge", "0C"},
            {"jal", "0D"},
            {"lw", "0E"},
            {"sw", "0F"},
            {"fut", "10"},
            {"fut", "11"},
            {"fut", "12"},
            {"halt", "13"},
    };


    //initializing the table of registers
    reg regs_arr[NUM_OF_REGS] = {
            {"zero","0"},
            {"imm","1"},
            {"v0","2"},
            {"a0","3"},
            {"a1","4"},
            {"t0","5"},
            {"t1","6"},
            {"t2","7"},
            {"t3","8"},
            {"s0","9"},
            {"s1","A"},
            {"s2","B"},
            {"gp","C"},
            {"sp","D"},
            {"fp","E"},
            {"ra", "F"},
    };

    if (strstr(line, word) != NULL) {
        // Params:
        int word_address;
        // Get:
        // Address
        // Data
        char* token = strtok(line, " .word\t\n");
        char* address = token;
        token = strtok(NULL, " .word\t\n");
        char* data = token;
        // Checks if ADDRESS is Hex, else Dec.
        if (strncmp(address, hex_sign, 2) == 0) {
            uppercase(address + 2);
            word_address = hex2Dec(address + 2);
        }
        else {
            word_address = atoi(address);
        }
        // Save the instruction.
        fseek(fop, 10 * word_address, SEEK_SET);
        // Checks if DATA is Hex, else Dec.
        if (strncmp(data, hex_sign, 2) == 0) {
            uppercase(data + 2);
            fputs(data + 2, fop);
        }
        else {
            fill_the_line(data, word_line);
            uppercase(data);
            fputs(word_line, fop);
        }
        fputs("\n", fop);
        fseek(fop, 10 * line_fop_counter, SEEK_SET);
    }

    else {
        char* token = strtok(line, " $,\t\n");
        if (strchr(token, ':') != NULL) {                               //check if : appear in a the line
            token = strtok(NULL, " $,\t\n");
        }
        while (token != NULL) {
            int feild = 1;
            if (feild == 1) {
                for (int i = 0; i <
                    NUM_OF_COMMANDS; i++) {                            //loop for searching and writing the opcode for commands
                    if (strcmp(token, commands_arr[i].name) == 0) {
                        fputs(commands_arr[i].opcode, fop);
                        feild++;
                        token = strtok(NULL, " $,\t\n");
                        break;
                    }
                }                        
            }
            if (feild == 2) {
                for (int i = 0;
                    i < NUM_OF_REGS; i++) {                            //loop for searching and writing the rd number
                    if (strcmp(token, regs_arr[i].name) == 0) {
                        fputs(regs_arr[i].reg_num, fop);
                        feild++;
                        token = strtok(NULL, " $,\t\n");
                        break;
                    }
                }
            }
            if (feild == 3) {
                for (int i = 0;
                    i < NUM_OF_REGS; i++) {                           //loop for searching and writing the rs number
                    if (strcmp(token, regs_arr[i].name) == 0) {
                        fputs(regs_arr[i].reg_num, fop);
                        feild++;
                        token = strtok(NULL, " $,\t\n");
                        break;
                    }
                }
            }
            if (feild == 4) {
                for (int i = 0;
                    i < NUM_OF_REGS; i++) {                          //loop for searching and writing the rt number
                    if (strcmp(token, regs_arr[i].name) == 0) {
                        fputs(regs_arr[i].reg_num, fop);
                        feild++;
                        token = strtok(NULL, " $,\t\n");
                        break;
                    }
                }
            }
            if (feild == 5) {

                // Case 1: immediate is Label:

                if (isalpha(token[0]) != 0) {
                    for (int i = 0; i <
                        label_num; i++) {                                     //loop for searchig and writing the location of the label
                        if (strcmp(token, labels_arr[i].name) == 0) {
                            dec_to_hex(labels_arr[i].location, hex_arr);
                            fputs(hex_arr, fop);
                            fputs("\n", fop);
                            token = strtok(NULL, " $,\t\n");
                            break;
                        }
                    }
                    break;
                }

                // Case 2 : immediate is Hex number:                            //we need to check if the hexa we get in the input is three digits

                if (strncmp(token, hex_sign, 2) == 0) {                         //checking if its in a hexadecimal formation (with 0x)                            
                    uppercase(token + 2);
                    fputs(token + 2, fop);                                      
                    fputs("\n", fop);
                    token = strtok(NULL, " $,\t\n");
                    break;
                }

                //case 3 : immediate is a decimal number

                else {                                                          //the feild is a decimal number so converting it to hexa and writing it
                    decimalnum = atoi(token);
                    dec_to_hex(decimalnum, hex_arr);
                    fputs(hex_arr, fop);
                    fputs("\n", fop);
                    token = strtok(NULL, " $,\t\n");
                    break;
                }
            }
        }
    }
}


void dec_to_hex(int decimalnum, char hex_arr[])                              
{
    //funtion to covert decimal to hexadeimal 3 bits
    int quotient, remainder;
    int i;
    int j = 0;
    int k = 0;
    char hexadecimalnum[100] = { '\0' };
    if (decimalnum < 0)
        decimalnum += 4096;

    quotient = decimalnum;

    while (quotient != 0)
    {
        remainder = quotient % 16;
        if (remainder < 10)
            hexadecimalnum[j] = 48 + remainder;
        else
            hexadecimalnum[j] = 55 + remainder;
        quotient = quotient / 16;
        j++;
    }
    j--;
    // reverse the order of the string
    for (j; j >= 0; j--) {
        hex_arr[k] = hexadecimalnum[j];
        k++;
    }
    if (k == 0) {                                                   //adding zeros to make the size of the string 3
        hex_arr[0] = '0';
        hex_arr[1] = '0';
        hex_arr[2] = '0';
    }
    if (k == 1) {
        hex_arr[2] = hex_arr[0];
        hex_arr[0] = '0';
        hex_arr[1] = '0';
    }
    if (k == 2) {
        hex_arr[2] = hex_arr[1];
        hex_arr[1] = hex_arr[0];
        hex_arr[0] = '0';
    }
}


void fill_the_line(char* data,char word_line[])                     //fill a line with 8 bits and convert dec to hex (.word line command)
{
    int length = 0;
    char* checker;
    unsigned int quotient;
    int remainder;
    int i;
    int j = 0;
    int k = 0;
    int m = 0;
    char hexadecimalnum[100] = { '\0' };
    char tempo[100] = { '\0' };
    int decimalnum = atoi(data);
    unsigned int decnum = 0;
    if (decimalnum < 0) {
        decnum = decimalnum;
        quotient = decnum;
    }
    else
        quotient = decimalnum;

    while (quotient != 0)
    {
        remainder = quotient % 16;
        if (remainder < 10)
            hexadecimalnum[j] = 48 + remainder;
        else
            hexadecimalnum[j] = 55 + remainder;
        quotient = quotient / 16;
        j++;
    }
    j--;
    // reverse the order of the string
    for (j; j >= 0; j--) {
        tempo[k] = hexadecimalnum[j];
        k++;
    }
    checker = tempo;
    while (*checker != '\0')
    {
        length++;
        checker++;
    }
    for (int i = 0; i < MEMIN_LINE_LENGTH-length; i++)
    {
        word_line[i] = '0';
    }
    for (int i = MEMIN_LINE_LENGTH-length; i < MEMIN_LINE_LENGTH; i++)
    {
        word_line[i] = tempo[m];
        m++;
        data++;
    }
}

int hex2Dec(char hexdecnumber[]) {
    
    //funtion to convert hexadedimal number to decimal
    int decimalnumber, i;
    // used to store the power index
    int cnt;
    // used to store the digit
    int digit;

    cnt = 0;
    decimalnumber = 0;

    // iterating the loop using length of hexadecnumber
    for (i = (strlen(hexdecnumber) - 1); i >= 0; i--) {

        // using switch case finding the equivalent decimal
        // digit for each hexa digit
        switch (hexdecnumber[i]) {
        case 'A':
            digit = 10;
            break;
        case 'B':
            digit = 11;
            break;
        case 'C':
            digit = 12;
            break;
        case 'D':
            digit = 13;
            break;
        case 'E':
            digit = 14;
            break;
        case 'F':
            digit = 15;
            break;
        default:
            digit = hexdecnumber[i] - 0x30;
        }

        // obtaining the decimal number
        decimalnumber = decimalnumber + (digit)*pow((double)16, (double)cnt);
        cnt++;
    }
    return decimalnumber;
}



void uppercase(char* c) {
    while (*c != '\0')
    {
        *c = toupper(*c);
        c++;
    }
}