#include <algorithm>
#include <bitset>
#include <cstdint>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "../utils/BitsetExtension.h"
#include "../utils/PriorityQueue.cpp"
#include "../utils/IOStream.h"

struct NodeForward {
    std::bitset<BLOCK_SIZE> symbol;
    std::size_t lson;
    std::size_t rson;
    int parent;
    explicit NodeForward(const std::bitset<BLOCK_SIZE> init_symbol) {
        symbol = init_symbol;
        lson = rson = -1;
        parent = -1;
    }
};

void CompressFile(InputData input, OutputData& output, const char* file_name, bool last_file) {
    std::unordered_map<std::bitset<BLOCK_SIZE>, int32_t> frequency;
    PriorityQueue<std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t>> heap(std::make_tuple(0, NEUTRAL, 0));
    std::vector<NodeForward> trie_forward;

    while (!input.HasReachedEOF()) {
        std::bitset<BLOCK_SIZE> symbol;
        for (std::size_t i = 0; i < BLOCK_SIZE - 1; i++) {
            symbol[i] = input.GetBit();
        }

        if (input.HasReachedEOF()) {
            break;
        }

        ++frequency[symbol];
    }

    for (auto it = file_name; *it; ++it) {
        std::bitset<BLOCK_SIZE> symbol;
        char current = *it;
        for (std::size_t i = 0; i < BLOCK_SIZE - 1; i++) {
            symbol[i] = current % 2;
            current /= 2;
        }

        ++frequency[symbol];
    }

    ++frequency[FILENAME_END];
    ++frequency[ONE_MORE_FILE];
    ++frequency[ARCHIVE_END];

    std::size_t index = 0;
    int16_t symbols_count = 0;
    std::vector<int> vertex_height;

    for (auto& element : frequency) {
        heap.Push(std::make_tuple(element.second, element.first, index++));
        trie_forward.push_back(NodeForward(element.first));
        vertex_height.push_back(0);
        ++symbols_count;
    }

    int max_symbol_code_size = 1;

    while (heap.Size() > 1) {
        std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t> first_min = heap.Top();
        heap.Pop();
        std::tuple<int32_t, std::bitset<BLOCK_SIZE>, int32_t> second_min = heap.Top();
        heap.Pop();

        std::bitset<BLOCK_SIZE> result_symbol;

        if (std::get<1>(first_min) < std::get<1>(second_min)) {
            result_symbol = std::get<1>(first_min);
        } else {
            result_symbol = std::get<1>(second_min);
        }

        NodeForward new_node(result_symbol);
        new_node.lson = std::get<2>(first_min);
        new_node.rson = std::get<2>(second_min);
        trie_forward.push_back(new_node);

        vertex_height.push_back(1 + std::max(vertex_height[new_node.lson], vertex_height[new_node.rson]));
        max_symbol_code_size = std::max(max_symbol_code_size, vertex_height.back());

        trie_forward[new_node.lson].parent = static_cast<int>(trie_forward.size() - 1);
        trie_forward[new_node.rson].parent = static_cast<int>(trie_forward.size() - 1);

        int32_t sum_frequency = std::get<0>(first_min) + std::get<0>(second_min);
        heap.Push(std::make_tuple(sum_frequency, result_symbol, trie_forward.size() - 1));
    }

    std::bitset<BLOCK_SIZE> symbols_count_output(symbols_count);
    output.AddBlock(symbols_count_output);

    std::vector<std::vector<std::bitset<BLOCK_SIZE>>> encoded(max_symbol_code_size + 1);

    for (int16_t i = 0; i < symbols_count; i++) {
        int32_t len = 0;
        int index = i;
        while (trie_forward[index].parent != -1) {
            ++len;
            index = trie_forward[index].parent;
        }

        encoded[len].push_back(trie_forward[i].symbol);
    }

    std::unordered_map<std::bitset<BLOCK_SIZE>, std::pair<std::bitset<MAX_CODE_SIZE>, std::size_t>> canonical_form;
    std::bitset<MAX_CODE_SIZE> current_code;

    for (int i = 1; i <= max_symbol_code_size; ++i) {
        std::sort(encoded[i].begin(), encoded[i].end(),
                  [](std::bitset<BLOCK_SIZE>& a, std::bitset<BLOCK_SIZE>& b) { return a < b; });

        for (std::size_t j = 0; j < encoded[i].size(); ++j) {
            canonical_form[encoded[i][j]] = std::make_pair(current_code, i);
            ++current_code;

            output.AddBlock(encoded[i][j]);
        }

        current_code <<= 1;
    }

    for (int i = 0; i < max_symbol_code_size; ++i) {
        std::bitset<BLOCK_SIZE> current_value;
        std::size_t current_size = encoded[i + 1].size();

        for (std::size_t j = 0; j < BLOCK_SIZE; j++) {
            current_value[j] = current_size % 2;
            current_size /= 2;
        }

        output.AddBlock(current_value);
    }

    for (auto it = file_name; *it; ++it) {
        std::bitset<BLOCK_SIZE> symbol;
        char current = *it;

        for (std::size_t i = 0; i < BLOCK_SIZE - 1; i++) {
            symbol[i] = current % 2;
            current /= 2;
        }

        for (std::size_t j = canonical_form[symbol].second; j > 0; --j) {
            output.AddBit(canonical_form[symbol].first[j - 1]);
        }
    }

    for (std::size_t j = canonical_form[FILENAME_END].second; j > 0; j--) {
        output.AddBit(canonical_form[FILENAME_END].first[j - 1]);
    }

    input.Rewind();

    while (!input.HasReachedEOF()) {
        std::bitset<BLOCK_SIZE> symbol;
        for (std::size_t i = 0; i < BLOCK_SIZE - 1; i++) {
            symbol[i] = input.GetBit();
        }
        if (input.HasReachedEOF()) {
            break;
        }
        for (std::size_t i = canonical_form[symbol].second; i > 0; --i) {
            output.AddBit(canonical_form[symbol].first[i - 1]);
        }
    }

    if (last_file) {
        for (std::size_t j = canonical_form[ARCHIVE_END].second; j > 0; j--) {
            output.AddBit(canonical_form[ARCHIVE_END].first[j - 1]);
        }

    } else {
        for (std::size_t j = canonical_form[ONE_MORE_FILE].second; j > 0; j--) {
            output.AddBit(canonical_form[ONE_MORE_FILE].first[j - 1]);
        }
    }

    trie_forward.clear();
}
