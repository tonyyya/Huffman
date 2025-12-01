#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "tree.h"

int main(int argc, char *argv[]) {
    char *inputFileName;
    char *outputFileName;
    if (argc < 4) {
        printf("Not enough arguments. Please enter input and output file names and mode.\n");
        return 0;
    }else if (argc > 4){
        printf("To many arguments\n");
        return 0;
    }
    
    inputFileName = argv[1]; 
    outputFileName = argv[2]; 

    int decode = 0;
    int encode = 0;

    if (strcmp(argv[3], "encode") == 0){
        encode = 1;
    }else if (strcmp(argv[3], "decode")==0){
        decode = 1;
    }else{
        printf("Error: mode not specified");
        return 0;
    }

    if (encode){
        unsigned long long freq [256];
        long size = build_freaq_table(inputFileName, freq);
        if (size == -1) {
            return 0;
        }
        struct node_t *head = list_from_freq_array(freq);
        struct node_t *root = make_tree(head);
        char *codes[256];
        make_code_table(root, codes);

        int c;
        unsigned char bit_buf = 0;
        int bit_count = 0;

        FILE* infile = fopen(inputFileName, "rb");
        if (infile == NULL) {
            fprintf(stderr, "Error opening input file %s\n",inputFileName);
            return 0; 
        }
        FILE* outfile = fopen(outputFileName, "wb");
        if (outfile == NULL) {
            fprintf(stderr, "Error opening output file %s\n",outputFileName);
            fclose(infile);
            return 0; 
        }

        fwrite(&size, sizeof(long), 1, outfile);
        fwrite(freq, sizeof(unsigned long long), 256, outfile);

        while ((c = fgetc(infile)) != EOF) {
            char* code = codes[(unsigned char)c];
            for (int i = 0; code[i] != '\0'; i++) {
                int bit = (code[i] == '1');
                write_bit(outfile, &bit_buf, &bit_count, bit);
            }
        } 
        if (bit_count > 0) {
            fwrite(&bit_buf, 1, 1, outfile);
        }  
        long compressed_size = ftell(outfile);
        printf("original size: %ld. compressed size: %ld. compression ratio: %.3f\n",
        size, compressed_size, (double)compressed_size / size);
        free_tree(root);
        fclose(infile);
        fclose(outfile);
        for (int i = 0; i < 256; i++) {
            free(codes[i]);
        }
    }

    if (decode){
        int k = decode_f(inputFileName, outputFileName);
        if (k==0){
            printf("decoded succesfully");
        }
    }
    return 0;
}