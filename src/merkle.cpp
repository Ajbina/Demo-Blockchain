#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "merkle.h"
#include "hash.h"

// Node constructor implementation
Node::Node(const std::string& h) : hash(h), left(nullptr), right(nullptr) {}

// Node destructor implementation
Node::~Node() {
    delete left;
    delete right;
}

// Build Merkle Tree implementation
Node* buildMerkleTree(const std::vector<Account>& accounts, int start, int end) {
    if (start > end) return nullptr;
    
    // Base case: single account
    if (start == end) {
        return new Node(accounts[start].hash);
    }
    
    // Recursive case: build left and right subtrees
    int mid = (start + end) / 2;
    Node* left = buildMerkleTree(accounts, start, mid);
    Node* right = buildMerkleTree(accounts, mid + 1, end);
    
    // Create new node with combined hash
    Node* node = new Node("");
    node->left = left;
    node->right = right;
    
    // Compute hash based on children
    if (right) {
        node->hash = computeSHA256(left->hash + right->hash);
    } else {
        // If odd number of nodes, propagate the left hash up
        node->hash = left->hash;
    }
    
    return node;
}

// Main function implementation
// int main() {
//     std::string filename;
//     std::cout << "Enter input file name: ";
//     std::cin >> filename;
    
//     std::ifstream file(filename);
//     if (!file.is_open()) {
//         std::cerr << "Error opening file" << std::endl;
//         return 1;
//     }
    
//     // Read accounts from file
//     std::vector<Account> accounts;
//     std::string address;
//     long long balance;
    
//     while (file >> address >> balance) {
//         Account account;
//         account.address = address;
//         account.balance = balance;
//         account.hash = computeSHA256(account.address + std::to_string(account.balance));
//         accounts.push_back(account);
//     }
    
//     // Build Merkle tree
//     Node* root = nullptr;
//     if (!accounts.empty()) {
//         root = buildMerkleTree(accounts, 0, accounts.size() - 1);
//     }
    
//     // Output root hash
//     if (root) {
//         std::cout << "Merkle Root Hash: " << root->hash << std::endl;
//     } else {
//         std::cout << "No data to process" << std::endl;
//     }
    
//     // Clean up
//     delete root;
//     file.close();
    
//     return 0;
// }