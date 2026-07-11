#include "huffman.h"

Node::Node(char c, int f) {
    ch = c;
    freq = f;
    left = right = nullptr;
}

bool Compare::operator()(Node* l, Node* r) { // оператор сравнения
    return l->freq > r->freq;
}

// Реализация функции генерации кодов
void generateCodes(Node* root, std::string str, std::map<char, std::string>& huffmanCodes) { // генерация кодов
    if (!root)
        return;

    if (!root->left && !root->right) {
        huffmanCodes[root->ch] = str;
    }

    generateCodes(root->left, str + "0", huffmanCodes);
    generateCodes(root->right, str + "1", huffmanCodes);
}

void buildHuffmanTree(std::string text) { // построение дерева хаффмана
    if (text.empty()) return;
    std::map<char, int> freq;
    for (char ch : text) {
        freq[ch]++;
    }

    std::priority_queue<Node*, std::vector<Node*>, Compare> pq;

    for (auto pair : freq) { // ставим символ в очередь 
        pq.push(new Node(pair.first, pair.second));
    }

    while (pq.size() > 1) { // выполняется, пока в очереди не останется корень 
        Node* left = pq.top(); pq.pop();
        Node* right = pq.top(); pq.pop();
        Node* parent = new Node('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    Node* root = pq.top(); // корень дерева

    std::map<char, std::string> huffmanCodes; // генерация ходов в обход дерева
    generateCodes(root, "", huffmanCodes);

    std::cout << "Символы и их коды Хаффмана:\n";
    for (auto pair : huffmanCodes) {
        std::cout << pair.first << " : " << pair.second << "\n";
    }

    std::cout << "\nИсходная строка: " << text << "\n";
    std::string encodedString = "";
    for (char ch : text) {
        encodedString += huffmanCodes[ch];
    }
    std::cout << "Закодированная строка: " << encodedString << "\n";
}