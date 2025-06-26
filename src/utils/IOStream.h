#pragma once

#include "Stream.h"
#include "BitsetExtension.h"

#include <iostream>
#include <fstream>

InputData::InputData(std::istream& input_stream)
    : input_(input_stream), pointer_(0), buffer_size_(0), end_of_stream_(false) {
}

unsigned char InputData::GetChar() {
    unsigned char symbol = input_.get();
    if (input_.tellg() == -1) {
        end_of_stream_ = true;
        return static_cast<unsigned char>(0);
    }
    return symbol;
}

bool InputData::GetBit() {
    if (pointer_ < buffer_size_) {
        return buffer_[pointer_++ % BLOCK_SIZE];
    } else {
        unsigned char new_symbol = GetChar();

        for (std::size_t i = BLOCK_SIZE - 1; i > 0; i--) {
            buffer_[(buffer_size_ + i - 1) % BLOCK_SIZE] = new_symbol % 2;
            new_symbol /= 2;
        }

        buffer_size_ += BLOCK_SIZE - 1;
        return buffer_[pointer_++ % BLOCK_SIZE];
    }
}

std::bitset<BLOCK_SIZE> InputData::GetBlock() {
    std::bitset<BLOCK_SIZE> result;
    for (std::size_t i = 0; i < BLOCK_SIZE; i++) {
        result[i] = GetBit();
    }

    return result;
}

void InputData::ClearBuffer() {
    buffer_.reset();
    buffer_size_ = 0;
    end_of_stream_ = false;
    pointer_ = 0;
}

bool InputData::HasReachedEOF() const {
    return end_of_stream_;
}

void InputData::Rewind() {
    input_.clear();
    input_.seekg(0);
    ClearBuffer();
}

OutputData::OutputData(std::ostream& output_stream) : output_(output_stream), pointer_(0) {
}

OutputData::~OutputData() {
    if (pointer_ != 0) {
        PrintChar();
    }
}

void OutputData::PrintChar() {
    unsigned char symbol{0};
    for (std::size_t i = 0; i < BLOCK_SIZE - 1; ++i) {
        symbol *= 2;
        symbol += buffer_[i];
    }
    output_ << symbol;
}

void OutputData::AddBit(bool bit) {
    buffer_[pointer_++] = bit;
    if (pointer_ == BLOCK_SIZE - 1) {
        pointer_ = 0;
        PrintChar();
        ClearBuffer();
    }
}

void OutputData::AddBlock(std::bitset<BLOCK_SIZE> a) {
    for (std::size_t i = 0; i < BLOCK_SIZE; i++) {
        AddBit(a[i]);
    }
}

void OutputData::ClearBuffer() {
    buffer_.reset();
    pointer_ = 0;
}
