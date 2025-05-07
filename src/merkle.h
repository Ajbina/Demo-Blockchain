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
    Node* left;
    Node* right;
    
    Node(const std::string& h);
    ~Node();
};

Node* buildMerkleTree(const std::vector<Account>& accounts, int start, int end);

#endif