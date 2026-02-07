#include "dynamic_bitset.h"
#include <bitset>
#include <iostream>
#include <cstring>


dynamic_bitset::dynamic_bitset::dynamic_bitset() {
    this->bits = new std::vector<long>();
}

dynamic_bitset::dynamic_bitset::dynamic_bitset(char *data,size_t size) {
    this->bits = new std::vector<long>(size/8);
    memcpy(&(*this->bits)[0],data,size);
    current_pop_byte_index=0;
    current_pop_bit_index=0;
    current_push_byte_index=0;
    current_push_bit_index=0;
}

void dynamic_bitset::dynamic_bitset::push(long value, byte code_len) {
    if(current_push_byte_index >= bits->size())
        bits->push_back(0l);

    byte available_bits = 64 - current_push_bit_index;

    if(code_len > available_bits){ // bit overflow   000 0_01 01| 0
        (*bits)[current_push_byte_index] |= ( value >> (code_len - available_bits));
        current_push_byte_index++;
        bits->push_back(0);
        (*bits)[current_push_byte_index] |= ( value <<( 64 - (code_len - available_bits)));
        current_push_bit_index = code_len - available_bits;
    }

    else{
        (*bits)[current_push_byte_index] |= ( value << (available_bits - code_len));
        current_push_bit_index= code_len + 64 - available_bits;
        if(current_push_bit_index == 64 ){
            current_push_byte_index++;
            current_push_bit_index = 0;
        }
    }
}

dynamic_bitset::byte dynamic_bitset::dynamic_bitset::pop(byte code_len) {
    byte b = 0;
    b = 0b01 & ((*bits)[current_pop_byte_index] >> (64 - current_pop_bit_index - code_len));
    current_pop_bit_index++;
    if (current_pop_bit_index == 64)
    {
        current_pop_bit_index=0;
        current_pop_byte_index++;
    }
    return b;
}


dynamic_bitset::dynamic_bitset::~dynamic_bitset() {
    delete bits;
}

void dynamic_bitset::dynamic_bitset::print_current_byte_state() {
    std::cout<< std::bitset<64>(bits->at(current_push_byte_index))<<std::endl;
}

void dynamic_bitset::dynamic_bitset::print_bytes() {
    using namespace std;
    for(auto b : *bits)
        cout<< bitset<64>(b)<<"-";
}

long* dynamic_bitset::dynamic_bitset::bytes() {
    return this->bits->data();
}

size_t dynamic_bitset::dynamic_bitset::size() {
    return this->bits->size();
}
