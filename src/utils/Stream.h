#pragma once

#include "BitsetExtension.h"

#include <bitset>
#include <istream>
#include <ostream>

class InputData {
public:
    explicit InputData(std::istream& input_stream);

    unsigned char GetChar();

    bool GetBit();

    std::bitset<BLOCK_SIZE> GetBlock();

    void ClearBuffer();

    bool HasReachedEOF() const;

    void Rewind();

private:
    std::bitset<BLOCK_SIZE> buffer_;
    std::istream& input_;
    std::size_t pointer_;
    std::size_t buffer_size_;
    bool end_of_stream_;
};

class OutputData {
public:
    explicit OutputData(std::ostream& output_stream);

    ~OutputData();

    void PrintChar();

    void AddBit(bool bit);

    void AddBlock(std::bitset<BLOCK_SIZE> a);

    void ClearBuffer();

private:
    std::bitset<BLOCK_SIZE - 1> buffer_;
    std::ostream& output_;
    std::size_t pointer_;
};