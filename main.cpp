#include "static_code_len/static_code_len.h"
#include "huffman/huffman.h"


int main(int argc,char** argv) {
    std::string input = "../data";
    std::string output =  "../out";
    std::string decompressed = "../decompressed_data";
    huffman::compress(&input,&output);
    huffman::decompress(&output,&decompressed);
    return 0;
}


