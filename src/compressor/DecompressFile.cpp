#include <bitset>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

#include "../utils/BitsetExtension.h"
#include "../utils/IOStream.h"

struct NodeBackward {
    std::bitset<BLOCK_SIZE> symbol;
    std::size_t lson;
    std::size_t rson;
    bool terminal;
    explicit NodeBackward(std::bitset<BLOCK_SIZE> init_symbol) {
        symbol = init_symbol;
        lson = rson = -1;
        terminal = false;
    }
};

int DecompressFile(InputData& input) {
    std::unordered_map<std::bitset<BLOCK_SIZE>, std::pair<std::bitset<MAX_CODE_SIZE>, std::size_t>> code;
    std::vector<NodeBackward> trie_backward;
    std::bitset<BLOCK_SIZE> symbols_count;
    for (std::size_t i = 0; i < BLOCK_SIZE; i++) {
        symbols_count[i] = input.GetBit();
    }

    std::vector<std::bitset<BLOCK_SIZE>> symbols;
    for (uint64_t i = 0; i < symbols_count.to_ulong(); ++i) {
        std::bitset<BLOCK_SIZE> current_symbol;

        for (std::size_t j = 0; j < BLOCK_SIZE; ++j) {
            current_symbol[j] = input.GetBit();
        }

        symbols.push_back(current_symbol);
    }

    std::bitset<MAX_CODE_SIZE> current;

    std::size_t pointer = 0;

    for (std::size_t i = 0; pointer < symbols.size(); ++i) {
        std::bitset<BLOCK_SIZE> input_amount;
        std::size_t current_len_count = 0;

        for (std::size_t j = 0; j < BLOCK_SIZE; ++j) {
            input_amount[j] = input.GetBit();
        }

        for (std::size_t j = BLOCK_SIZE; j > 0; --j) {
            current_len_count *= 2;
            current_len_count += input_amount[j - 1];
        }

        for (std::size_t j = pointer; j < pointer + current_len_count; ++j) {
            code[symbols[j]] = std::make_pair(current, i + 1);
            ++current;
        }

        current <<= 1;
        pointer += current_len_count;
    }

    trie_backward.push_back(NodeBackward(NEUTRAL));

    std::function<void(std::size_t, std::bitset<BLOCK_SIZE>, std::size_t)> make_trie;

    make_trie = [&](std::size_t vertex, std::bitset<BLOCK_SIZE> symbol, std::size_t depth) {
        if (depth == 0) {
            trie_backward[vertex].terminal = true;
            return;
        }

        if (code[symbol].first[depth - 1] && trie_backward[vertex].rson == -1) {
            trie_backward[vertex].rson = trie_backward.size();
            trie_backward.push_back(NodeBackward(symbol));
        }

        if (!code[symbol].first[depth - 1] && trie_backward[vertex].lson == -1) {
            trie_backward[vertex].lson = trie_backward.size();
            trie_backward.push_back(NodeBackward(symbol));
        }

        make_trie(code[symbol].first[depth - 1] ? trie_backward[vertex].rson : trie_backward[vertex].lson, symbol,
                  depth - 1);
    };

    for (std::bitset<BLOCK_SIZE>& symbol : symbols) {
        make_trie(0, symbol, code[symbol].second);
    }

    std::size_t vertex = 0;

    std::string file_name;

    while (true) {
        bool current_bit = input.GetBit();
        if (current_bit) {
            vertex = trie_backward[vertex].rson;
        } else {
            vertex = trie_backward[vertex].lson;
        }

        if (trie_backward[vertex].terminal) {
            std::bitset<BLOCK_SIZE> decoded_symbol = trie_backward[vertex].symbol;
            if (decoded_symbol == FILENAME_END) {
                break;
            }

            int out_symbol = 0;
            for (std::size_t i = BLOCK_SIZE - 1; i > 0; i--) {
                out_symbol *= 2;
                out_symbol += decoded_symbol[i - 1];
            }

            file_name += static_cast<char>(out_symbol);
            vertex = 0;
        }
    }
    std::fstream output_file;
    output_file.open(file_name, std::ios::binary | std::fstream::out);
    OutputData output(output_file);
    vertex = 0;

    while (true) {
        bool current_bit = input.GetBit();
        if (current_bit) {
            vertex = trie_backward[vertex].rson;
        } else {
            vertex = trie_backward[vertex].lson;
        }

        if (trie_backward[vertex].terminal) {
            std::bitset<BLOCK_SIZE> decoded_symbol = trie_backward[vertex].symbol;
            if (decoded_symbol == ARCHIVE_END) {
                return true;
            }

            if (decoded_symbol == ONE_MORE_FILE) {
                return false;
            }

            for (std::size_t i = 0; i < BLOCK_SIZE - 1; ++i) {
                output.AddBit(decoded_symbol[i]);
            }

            vertex = 0;
        }
    }
    return true;
}
