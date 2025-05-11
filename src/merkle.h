#ifndef MERKLE_H
#define MERKLE_H

#include <string>
#include <vector>

struct Account {
    std::string address;
    long long balance;
    std::string hash;
};

struct Node {
    std::string hash;
    Node* left = nullptr;
    Node* right = nullptr;

    bool isLeaf = false;        
    bool isLeftChild = false;   

    Node(const std::string& h);
    ~Node();
};

Node* buildMerkleTree(const std::vector<Account>& accounts, int start, int end, bool isLeft);

#endif