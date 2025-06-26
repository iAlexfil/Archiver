#include "argparser/ArgParser.cpp"

int main(int argc, char** argv) {
    ArgumentParser argparse(argc, argv);
    argparse.Execute();

    return 0;
}
