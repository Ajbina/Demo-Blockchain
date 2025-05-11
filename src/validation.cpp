#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <sstream>
#include <algorithm>
#include "merkle.h"
#include "hash.h"

struct BlockHeader {
    std::string prevHash, merkleRoot, target;
    long timestamp, nonce;
};

struct Block {
    BlockHeader header;
    std::vector<Account> accounts;
    std::string hash;
};

struct ProofStep {
    std::string siblingHash;
    bool isLeft; // true if original node was on the left
};

struct BalanceProof {
    bool found = false;
    long long balance = 0;
    std::string merkleRoot;
    std::vector<ProofStep> merkleProof;
    //std::vector<std::string> chainProof;
    std::string leafHash;
};

std::string computeBlockHash(const BlockHeader& h) {
    return computeSHA256(h.prevHash + h.merkleRoot + std::to_string(h.timestamp) + h.target + std::to_string(h.nonce));
}

Block deserializeBlock(std::ifstream& file) {
    Block block;
    std::string line;
    while (std::getline(file, line) && line != "BEGIN BLOCK");
    if (!std::getline(file, line) || line != "BEGIN HEADER") return block;

    std::getline(file, block.header.prevHash);
    std::getline(file, block.header.merkleRoot);
    file >> block.header.timestamp; file.ignore();
    std::getline(file, block.header.target);
    file >> block.header.nonce; file.ignore();
    std::getline(file, line);

    while (std::getline(file, line) && line != "END BLOCK") {
        std::istringstream iss(line);
        Account acc;
        if (iss >> acc.address >> acc.balance) {
            acc.hash = computeSHA256(acc.address + std::to_string(acc.balance));
            block.accounts.push_back(acc);
        }
    }

    block.hash = computeBlockHash(block.header);
    return block;
}

bool validateBlock(const Block& b, bool isGenesis = false) {
    Node* root = buildMerkleTree(b.accounts, 0, b.accounts.size() - 1, false);
    bool valid = (root->hash == b.header.merkleRoot);
    delete root;

    valid &= computeBlockHash(b.header) == b.hash && b.hash <= b.header.target;
    if (!isGenesis) valid &= b.header.timestamp <= time(nullptr);

    return valid;
}

bool validateBlockchain(const std::vector<Block>& chain) {
    if (chain.empty() || chain.back().header.prevHash != "0" || !validateBlock(chain.back(), true)) return false;
    for (int i = chain.size() - 2; i >= 0; --i) {
        if (chain[i].header.prevHash != chain[i + 1].hash || !validateBlock(chain[i])) return false;
    }
    return true;
}

bool findLeafAndBuildProof(Node* node, const std::string& targetHash, std::vector<ProofStep>& proof) {
    if (!node) return false;
    if (node->isLeaf && node->hash == targetHash) return true;

    if (node->left && findLeafAndBuildProof(node->left, targetHash, proof)) {
        if (node->right) {
            proof.push_back({ node->right->hash, true }); // sibling on right, current is left
        }
        return true;
    }
    if (node->right && findLeafAndBuildProof(node->right, targetHash, proof)) {
        proof.push_back({ node->left->hash, false }); // sibling on left, current is right
        return true;
    }
    return false;
}

bool verifyMerkleProof(const std::vector<ProofStep>& proof, const std::string& expectedRoot, const std::string& leafHash) {
    std::string hash = leafHash;
    for (const auto& step : proof) {
        hash = step.isLeft ? computeSHA256(hash + step.siblingHash)
                           : computeSHA256(step.siblingHash + hash);
    }
    std::cout << "DEBUG: Computed root = " << hash << std::endl;
    std::cout << "DEBUG: Expected root = " << expectedRoot << std::endl;
    return hash == expectedRoot;
}

BalanceProof getBalance(const std::string& addr, const std::vector<Block>& chain) {
    for (auto it = chain.rbegin(); it != chain.rend(); ++it) {
        const auto& blk = *it;
        std::vector<std::string> leafHashes;
        for (const auto& acc : blk.accounts) {
            leafHashes.push_back(computeSHA256(acc.address + std::to_string(acc.balance)));
        }

        auto accIt = std::find_if(blk.accounts.begin(), blk.accounts.end(), [&](const Account& a) {
            return a.address == addr;
        });

        if (accIt != blk.accounts.end()) {
            int idx = std::distance(blk.accounts.begin(), accIt);
            BalanceProof proof;
            proof.found = true;
            proof.balance = accIt->balance;
            proof.leafHash = computeSHA256(addr + std::to_string(proof.balance));
        
            Node* root = buildMerkleTree(blk.accounts, 0, blk.accounts.size() - 1, false);
            findLeafAndBuildProof(root, proof.leafHash, proof.merkleProof);
            proof.merkleRoot = root->hash;
            delete root;
        
            //proof.chainProof.push_back(blk.hash);
            //for (auto fwd = std::next(it).base(); fwd != chain.end(); ++fwd) {
            //    proof.chainProof.push_back(fwd->hash);
            //}
        
            return proof;
        }
    }
    return {};
}

int main() {
    std::string fileName;
    std::cout << "Enter blockchain file to load and validate: ";
    std::cin >> fileName;
    std::ifstream file(fileName);
    if (!file) {
        std::cerr << "ERROR: Cannot open file\n";
        return 1;
    }

    std::vector<Block> chain;
    while (true) {
        Block b = deserializeBlock(file);
        if (b.accounts.empty()) {
            if (file.eof()) break;
            continue;
        }
        chain.push_back(b);
        while (file.peek() == '\n') file.get();
    }

    std::cout << "Blockchain validation status: "
              << (validateBlockchain(chain) ? "VALID" : "INVALID") << "\n\n";

    std::string address;
    while (true) {
        std::cout << "Enter address to get balance and proof (or 'exit'): ";
        std::cin >> address;
        if (address == "exit") break;

        BalanceProof proof = getBalance(address, chain);
        if (proof.found) {
            std::cout << "\nBalance: " << proof.balance << "\n";
            std::cout << "Merkle Proof Path:\n";
            for (const auto& step : proof.merkleProof) {
                std::cout << "  " << step.siblingHash << " (" << (step.isLeft ? "left" : "right") << ")";
            }

            //std::cout << "\nChain Proof (block hashes):\n";
            //for (const auto& h : proof.chainProof) std::cout << "  " << h << "\n";

            //std::cout << "\nMerkle proof is " << (verifyMerkleProof(proof.merkleProof, proof.merkleRoot, proof.leafHash) ? "VALID" : "INVALID") << "\n";
        } else {
            std::cout << "Address not found in blockchain.\n";
        }
        std::cout << "\n";
    }

    return 0;
}
