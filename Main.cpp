#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <cmath>
#include <sstream>
#include <random>

ulong M_TABLE_SIZE = std::pow(2, 22);
size_t HASH_MATRIX_ROWS = 22;

struct TableValue {
    std::vector<ulong> values;
    size_t h_rows;
    TableValue** second_table;
    size_t second_table_size;
    ulong* h;
    TableValue() {
        h_rows = 0;
        second_table_size = 0;
        h = nullptr;
        second_table = nullptr;
        values = std::vector<ulong>();
    }
};

ulong BitCount (ulong value) {
    ulong count = 0;
    // Loop until all bits are zero
    while (value > 0) {
        // Check lowest bit
        if ((value & 1) == 1) {
            count++;
        }
        // Remove lowest bit
        value >>= 1;
    }
    return count;
}

bool PerfectHash(TableValue** h, size_t size) {
    for(size_t i = 0; i < size; ++i) {
        // Check for collisions
        if(h[i] != nullptr && h[i]->values.size() > 1) {
            return false;
        }
    }
    return true;
}

void RandomizeArray(ulong* h, size_t size) {
    std::random_device rd;
    std::mt19937_64 e(rd());
    for(size_t i = 0; i < size; ++i) {
        // Random ulong value
        h[i] = e();
    }
}

void Hash(TableValue** table, ulong* h, size_t size, ulong x) {
    unsigned long result = 0;
    // Matrix multiplication
    for(size_t i = 0; i < size; ++i) {
        ulong row = h[i];
        ulong res = x bitand row;
        ulong bits = BitCount(res);
        if(bits % 2 == 1) {
            result = result bitor (1 << i);
        }
    }
    if(table[result] == nullptr) {
        table[result] = new TableValue();
    }
    table[result]->values.push_back(x);
}

void PrintStats(TableValue** table, size_t size) {
    int num_collisions = 0;
    int num_h = 0;
    for(size_t i = 0; i < size; ++i) {
        if(table[i] == nullptr) {
            continue;
        }
        if(table[i]->values.size() > 1) {
            ++num_collisions;
            if(table[i]->values.size() >= 3) {
                ++num_h;
            }
        }
    }
    std::cout << "Collisions: " << num_collisions << std::endl;
    std::cout << "Num H_i: "    << num_h << std::endl;
}

void PrintMatrix(ulong* h, size_t size) {
    std::cout << "H:" << std::endl;
    for(size_t i = 0; i < size; ++i) {
        std::cout << std::hex << h[i] << std::endl;
    }
}

void PrintMatrixHi(TableValue** table, size_t size) {
    for(size_t i = 0; i < size; ++i) {
        if(table[i] != nullptr && table[i]->values.size() > 2) {
            std::cout << std::dec << i << " " << table[i]->h_rows << std::endl;
            std::cout << std::hex;
            for(size_t j = 0; j < table[i]->h_rows; ++j) {
                std::cout << table[i]->h[j] << std::endl;
            }
            // std::cout << std::endl;
        }
    }
}

int main() {
    // Table
    TableValue** hash_table = new TableValue*[M_TABLE_SIZE];
    for(size_t i = 0; i < M_TABLE_SIZE; ++i) {
        hash_table[i] = nullptr;
    }

    // Hash matrix
    ulong* h = new ulong[HASH_MATRIX_ROWS];
    for(size_t i = 0; i < HASH_MATRIX_ROWS; ++i) {
        h[i] = 0;
    }
    RandomizeArray(h, HASH_MATRIX_ROWS);

    // Read 2²² entries of length 64 written in hex
    // and construct a double hash table
    ulong x = 0;
    std::stringstream ss;
    for(std::string line; std::getline(std::cin, line);) {
        ss << std::hex << line;
        ss >> x;
        Hash(hash_table, h, HASH_MATRIX_ROWS, x);
        ss.clear();
        x = 0;
    }

    std::cerr << "Initial hash done" << std::endl;
    for(size_t i = 0; i < M_TABLE_SIZE; ++i) {
        if(hash_table[i] == nullptr) {
            continue;
        }
        size_t collisions = hash_table[i]->values.size();

        // Three or more items to the same value
        if(collisions > 2) {
            // Use the fewest bits possible for H_i
            size_t h_rows = std::ceil(std::log2(collisions));

            // Create second table size depending on num of collisions
            size_t second_table_size = std::pow(2, h_rows);
            TableValue** second_table = new TableValue*[second_table_size];
            std::fill(second_table, second_table+second_table_size, nullptr);

            // H_i matrix
            ulong* h = nullptr;
            do {
                // Remove old (bad) table if exists
                delete[] h;
                for(size_t j = 0; j < second_table_size; ++j) {
                    delete second_table[j];
                    second_table[j] = nullptr;
                }
                // Create new H_i matrix
                h = new ulong[h_rows];
                RandomizeArray(h, h_rows);

                // Hash the values
                for(size_t j = 0; j < collisions; ++j) {
                    Hash(second_table, h, h_rows, hash_table[i]->values[j]);
                }
            } while(!PerfectHash(second_table, second_table_size));

            // Set variables
            hash_table[i]->second_table_size = second_table_size;
            hash_table[i]->second_table = second_table;
            hash_table[i]->h = h;
            hash_table[i]->h_rows = h_rows;
        }
    }
    std::cerr << "Perfect hash done" << std::endl;

    PrintStats(hash_table, M_TABLE_SIZE);
    PrintMatrix(h, HASH_MATRIX_ROWS);
    PrintMatrixHi(hash_table, M_TABLE_SIZE);
}
