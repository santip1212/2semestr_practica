#pragma once
#include <iostream>
#include <string>
#include <map>
#include <queue>

// Структура узла дерева Хаффмана
struct Node {
    char ch;
    int freq;
    Node *left, *right;

    Node(char c, int f);
};

struct Compare { // сравнение узлов
    bool operator()(Node* l, Node* r);
};

// Рекурсивная функция для генерации кодов Хаффмана
void generateCodes(Node* root, std::string str, std::map<char, std::string>& huffmanCodes);

// Функция для построения дерева Хаффмана и вывода кодов
void buildHuffmanTree(std::string text);