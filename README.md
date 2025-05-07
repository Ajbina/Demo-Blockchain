# Blockchain Project

This project implements a simplified blockchain system in C++ with Merkle tree verification and lightweight client proofs, structured across three parts: HW3, HW4, and HW5.

---

## Project Components

### HW3: Merkle Tree Construction (`merkle.cpp`)
- Constructs a Merkle tree from account data.
- Computes and returns the Merkle root.
- Used as a foundation for block integrity validation.

### HW4: Block and Blockchain Structure (`block.cpp`)
- Implements a `Block` class with headers and transactions.
- Includes serialization and deserialization of blocks from `.block.out` files.
- Hashes the block and validates internal structure.

### HW5: Validation and Proof System (`validation.cpp`)
- Validates a full blockchain by checking:
  - Merkle root correctness
  - Block hash target compliance
  - Previous hash link consistency
- Allows querying of balances by address.
- Verifies Merkle membership for lightweight clients.

---

## Compilation

g++ -std=c++11 validation.cpp merkle.cpp block.cpp hash.cpp -o validation -lssl -lcrypto
