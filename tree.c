#include "tree.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct node_t {
    unsigned char symbol;
    unsigned long long frequency;
    struct node_t *next;
    struct node_t *left;
    struct node_t *right;
};

long build_freaq_table(const char *filename, unsigned long long freq[256]) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        return -1;
    }
    for (int i = 0; i < 256; i++) {
        freq[i] = 0;
    }
    long size = 0;
    int c;
    while ((c = fgetc(file)) != EOF) {
        freq[(unsigned char)c]++;
        size++;
    }
    fclose(file); 
    return size;
}

// вставляет узел new_node в список частот по возрастанию
void insert_in_order(struct node_t** head, struct node_t* new_node) {
    if (*head == NULL || (*head)->frequency > new_node->frequency) {
        // Вставить в начало
        new_node->next = *head;
        *head = new_node;
    } else {
        // Найти место для вставки
        struct node_t* curr = *head;
        while (curr->next != NULL && curr->next->frequency <= new_node->frequency) {
            curr = curr->next;
        }
        new_node->next = curr->next;
        curr->next = new_node;
    }
}

struct node_t* list_from_freq_array(unsigned long long frequencies[256]) {
    struct node_t* head = NULL;  // начало списка

    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {  // создаём узел для i

            struct node_t* node = malloc(sizeof(struct node_t));
            node->symbol = (unsigned char)i;
            node->frequency = frequencies[i];
            node->left = NULL;
            node->right = NULL;
            node->next = NULL;
            insert_in_order(&head, node); // вставляем в список в правильном порядке
        }
    }
    return head; 
}

struct node_t* make_new_node(struct node_t* left, struct node_t* right) {
    // Создаём новый узел в памяти
    struct node_t* node = malloc(sizeof(struct node_t));

    node->frequency = left->frequency + right->frequency;  // сумма частот
    node->symbol = 0;         // у родителя нет символа — ставим 0
    node->left = left;     
    node->right = right;      
    node->next = NULL;        // в списке пока не участвуе=\]


    return node;  // возвращаем указатель на новый узел
}

struct node_t* make_tree(struct node_t* curr_node) {
    //повторяем, пока в списке не останется один узел
    while (curr_node != NULL && curr_node->next != NULL) {
        struct node_t* left = curr_node;
        struct node_t* right = curr_node->next;
        curr_node = curr_node->next->next;  // отрезаем первые два узла от списка

        struct node_t* parent = make_new_node(left, right); // создаём нового родителя

        insert_in_order(&curr_node, parent); //вставляем родитеkz в правильное место
    }

    return curr_node;  //возвращаем корень дерева
}


void build_codes(struct node_t* node, char* buf, int depth, char* codes[256]){
    if (node ->left == NULL && node ->right == NULL){
        buf[depth] = '\0';
        codes[node->symbol] = strdup(buf);
        return;
    }
    if (node->left != NULL) {
        buf[depth] = '0';
        build_codes(node->left, buf, depth + 1, codes);
    }

    if (node->right != NULL) {
        buf[depth] = '1';
        build_codes(node->right, buf, depth + 1, codes);
    }
}

void make_code_table(struct node_t* root, char* codes[256]) {
    for (int i = 0; i < 256; i++) {
        codes[i] = NULL;
    }
    if (root == NULL) return;  //пустое дерево 

    //только один символ в файле
    if (root->left == NULL && root->right == NULL) {
        codes[root->symbol] = strdup("0");
        return;
    }

    char buffer[256]; 
    build_codes(root, buffer, 0, codes);
}

void write_bit(FILE* out, unsigned char* buffer, int* count, int bit) {
    if (bit) {
        *buffer |= (1 << (7 - *count)); //левый бит с самой большой степенью
    }
    (*count)++;

    //если набрался полный байт
    if (*count == 8) {
        fwrite(buffer, 1, 1, out);
        *buffer = 0;  
        *count = 0;
    } 
}

void free_tree(struct node_t* root) { 
    if (root == NULL) {
        return;
    }
    free_tree(root->left);   // все левое поддерево освобождаем
    free_tree(root->right);  // все правое поддерево освобождаем
    free(root);             
}

int decode_f(const char *inputFileName, const char *outputFileName){
    FILE* infile = fopen(inputFileName, "rb");
        if (infile == NULL) {
            fprintf(stderr, "Error opening input file %s\n",inputFileName);
            return -1; 
        }
        long size;
        fread(&size, sizeof(long), 1, infile);
        unsigned long long freq [256];
        fread(freq, sizeof(unsigned long long), 256, infile);

        struct node_t *head = list_from_freq_array(freq);
        struct node_t *root = make_tree(head);
        
        struct node_t *curr = root;
        long decoded = 0;
        unsigned char byte;

        FILE* outfile = fopen(outputFileName, "wb");
        if (outfile == NULL) {
            fprintf(stderr, "Error opening output file %s\n",outputFileName);
            return -1; 
        }

        while (decoded < size && fread(&byte, 1, 1, infile) == 1){
            for (int i = 0; i < 8 && decoded < size; i++) {
                int bit = (byte >> (7 - i)) & 1;  
                if (bit){
                    curr = curr -> right;
                }else{
                    curr = curr -> left;
                }
                if (curr && curr->left == NULL && curr->right == NULL) {
                    fputc(curr->symbol, outfile);
                    curr = root;
                    decoded++;
                }
                if (!curr) {
                    fprintf(stderr, "Error: invalid bitstream\n");
                    fclose(infile);
                    fclose(outfile);
                    return -1;
                }
            }
    }
    free_tree(root);
    fclose(infile);
    fclose(outfile);
    return 0;  
}