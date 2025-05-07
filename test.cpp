#include <iostream>
#include "hash.h"

int main() {
    std::cout << computeSHA256("100jby6yzd0w8uadwpkh0vjdw2y1jhjuf1tkve0b" + std::to_string(4652367)) << std::endl;
}
