#pragma once

#include <stdexcept>

class NotEnoughArguments : public std::runtime_error {
public:
    NotEnoughArguments() : std::runtime_error("Not enough arguments") {
    }
};

class TooMuchArguments : public std::runtime_error {
public:
    TooMuchArguments() : std::runtime_error("Too much arguments") {
    }
};

class FileDoesntExist : public std::runtime_error {
public:
    FileDoesntExist() : std::runtime_error("File doesn't exist") {
    }
};


class ArgumentParser {
public:
    ArgumentParser(int init_argc, char** init_argv);

    void Execute();

    void PrintInstruction();

    void Compress();

    void Decompress();

private:
    int argc_;
    char** argv_;
};