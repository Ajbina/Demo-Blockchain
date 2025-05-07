#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <random>
#include <algorithm>
#include <climits>
#include "merkle.h"
#include "hash.h"

struct BlockHeader {
    std::string prevHash;
    std::string merkleRoot;
    long timestamp;
    std::string target;
    long nonce;
};

struct Block {
    BlockHeader header;
    std::vector<Account> accounts;
    std::string hash;
};

std::string computeBlockHash(const BlockHeader& header) {
    std::string data = header.prevHash + header.merkleRoot + 
                      std::to_string(header.timestamp) + 
                      header.target + std::to_string(header.nonce);
    return computeSHA256(data);
}

void mineBlock(Block& block, const std::string& target) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long> dist(0, LONG_MAX);
    
    block.header.timestamp = time(nullptr);
    block.header.target = target;
    
    while (true) {
        block.header.nonce = dist(gen);
        block.hash = computeBlockHash(block.header);
        
        if (block.hash <= target) {
            break;
        }
    }
}

void printBlock(const Block& block, bool printAccounts, std::ostream& out = std::cout) {
    out << "BEGIN BLOCK" << std::endl;
    out << "BEGIN HEADER" << std::endl;
    out << block.header.prevHash << std::endl;
    out << block.header.merkleRoot << std::endl;
    out << block.header.timestamp << std::endl;
    out << block.header.target << std::endl;
    out << block.header.nonce << std::endl;
    out << "END HEADER" << std::endl;
    
    if (printAccounts) {
        for (const auto& account : block.accounts) {
            out << account.address << " " << account.balance << std::endl;
        }
    }
    
    out << "END BLOCK" << std::endl << std::endl;
}

Block createGenesisBlock(const std::vector<Account>& accounts, Node* merkleRoot) {
    Block genesis;
    genesis.header.prevHash = "0";
    genesis.header.merkleRoot = merkleRoot ? merkleRoot->hash : "";
    genesis.accounts = accounts;
    
    std::string target = "7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
    mineBlock(genesis, target);
    
    return genesis;
}

Block createNewBlock(const std::vector<Account>& accounts, Node* merkleRoot, const Block& prevBlock) {
    Block newBlock;
    newBlock.header.prevHash = prevBlock.hash;
    newBlock.header.merkleRoot = merkleRoot ? merkleRoot->hash : "";
    newBlock.accounts = accounts;
    
    mineBlock(newBlock, prevBlock.header.target);
    
    return newBlock;
}
std::vector<Account> readAccountsFromFile(const std::string& filename) {
    std::vector<Account> accounts;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return accounts;
    }
    
    std::string address;
    long long balance;
    
    while (file >> address >> balance) {
        Account account;
        account.address = address;
        account.balance = balance;
        account.hash = computeSHA256(account.address + std::to_string(account.balance));
        accounts.push_back(account);
    }
    
    file.close();
    return accounts;
}

// int main() {
//     std::vector<std::string> filenames;
//     std::string filename;
    
//     std::cout << "Enter input file names (enter 'done' when finished):" << std::endl;
//     while (true) {
//         std::cin >> filename;
//         if (filename == "done") break;
//         filenames.push_back(filename);
//     }
    
//     if (filenames.empty()) {
//         std::cerr << "No input files provided" << std::endl;
//         return 1;
//     }
    
//     std::vector<Block> blockchain;
    
//     for (size_t i = 0; i < filenames.size(); i++) {
//         std::vector<Account> accounts = readAccountsFromFile(filenames[i]);
//         Node* root = buildMerkleTree(accounts, 0, accounts.size() - 1);
        
//         Block block;
//         if (blockchain.empty()) {
//             block = createGenesisBlock(accounts, root);
//         } else {
//             block = createNewBlock(accounts, root, blockchain.back());
//         }
        
//         blockchain.push_back(block);
//         delete root;
        
//         // Print the block to console
//         printBlock(block, true);
        
//         // Write to output file
//         std::string outFilename = filenames[i].substr(0, filenames[i].find_last_of('.')) + ".block.out";
//         std::ofstream outFile(../bin/outFilename);
//         if (!outFile.is_open()) {
//             std::cerr << "Error creating output file: " << outFilename << std::endl;
//             continue;
//         }
        
//         // Print all blocks in reverse order (newest first)
//         for (auto it = blockchain.rbegin(); it != blockchain.rend(); ++it) {
//             printBlock(*it, true, outFile);
//         }
        
//         outFile.close();
//     }
    
//     return 0;
// }