#ifndef TREELIST_H
#define TREELIST_H
#include <stdio.h>

struct node_t;

long build_freaq_table(const char *filename, unsigned long long freq[256]);

// вставляет в список частот правильно
void insert_in_order(struct node_t** head, struct node_t* new_node);

// создаёт упорядоченный связный по возрастанию частот список из массива частот
struct node_t* list_from_freq_array(unsigned long long frequencies[256]);

// создаёт новый узел из двух потомков
struct node_t* make_new_node(struct node_t* left, struct node_t* right);

// строит дерево из упорядоченного списка и возвращает корень, принимает указатель на голову
struct node_t* make_tree(struct node_t* curr_node);

// заполняет таблицу кодов
void make_code_table(struct node_t* root, char* codes[256]);

//рекурсивно генерирует коды
void build_codes(struct node_t* node, char* buf, int depth, char* codes[256]);

void write_bit(FILE* out, unsigned char* buffer, int* count, int bit);

void free_tree(struct node_t* root);

int decode_f(const char *inputFileName, const char *outputFileName);

#endif