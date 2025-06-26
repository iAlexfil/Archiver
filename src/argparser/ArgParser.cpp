#include "ArgParser.h"
#include "../compressor/CompressFiles.cpp"
#include "../compressor/DecompressFile.cpp"

#include <iostream>
#include <string>
#include <fstream>

ArgumentParser::ArgumentParser(int init_argc, char** init_argv) : argc_(init_argc), argv_(init_argv) {
}

void ArgumentParser::Execute() {
    if (argc_ == 1) {
        throw NotEnoughArguments{};
    }

    if (argc_ == 2) {
        if (std::string(argv_[1]) == "-h") {
            PrintInstruction();
        } else {
            throw NotEnoughArguments{};
        }
    }

    if (std::string(argv_[1]) == "-d" && argc_ > 3) {
        throw TooMuchArguments{};
    }

    if (std::string(argv_[1]) == "-c" && argc_ == 3) {
        throw NotEnoughArguments{};
    }

    if (std::string(argv_[1]) == "-d") {
        Decompress();
    }

    if (std::string(argv_[1]) == "-c") {
        Compress();
    }
}

void ArgumentParser::PrintInstruction() {
    std::cout << "Archiver\n"
                 "\n"
                 "usage: archiver [-h] [-c] [-d]\n"
                 "\n"
                 "Options\n"
                 "	-h					Print usage\n"
                 "	-c <archive_name> <file1> <file2> ...	Archivate files into <archive_name>\n"
                 "	-d <archive_name>			Decode <archive_name> back to the files\n"
                 "\n";
}


void ArgumentParser::Compress() {
    std::string archive_name = argv_[2];

    std::fstream output;
    output.open(archive_name, std::ios::binary | std::fstream::out);

    OutputData output_stream(output);

    for (int i = 3; i < argc_; ++i) {
        std::string current_file_name = argv_[i];
        for (std::size_t j = current_file_name.size(); j > 0; --j) {
            if (current_file_name[j - 1] == '/') {
                current_file_name = current_file_name.substr(j, current_file_name.size() - j);
                break;
            }
        }

        std::fstream file_input(argv_[i], std::ios::binary | std::fstream::in);
        if (!file_input.is_open()) {
            throw FileDoesntExist{};
        }

        InputData in(file_input);
        CompressFile(in, output_stream, current_file_name.c_str(), i == argc_ - 1);
    }
}

void ArgumentParser::Decompress() {
    std::string archive_name = argv_[2];

    std::fstream input(archive_name, std::ios::binary | std::fstream::in);

    if (!input.is_open()) {
        throw FileDoesntExist{};
    }

    InputData input_stream(input);

    bool continue_decompress = true;

    while (continue_decompress) {
        continue_decompress = !DecompressFile(input_stream);
    }
}