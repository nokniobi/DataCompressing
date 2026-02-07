#include <iostream>
#include "huffman.h"
#include<map>
#include <list>
#include <vector>
#include <fstream>
#include <assert.h>
#include<bitset>
#include <cstring>
#include "../binary/dynamic_bitset.h"
#include "../static_code_len/dictionary.h"

# define claster 4096


// list of tree leaves for fast access after feel codes
std::map<char,huffman::node*> threaded_tree;
huffman::node* decode_char(huffman::node*,dynamic_bitset::dynamic_bitset*);
std::vector<char> *read_data(std::string *);
void feel_codes(huffman::node*);
huffman::node* create_tree(std::list<huffman::node*>*);

unsigned long current_code ;
short depth ;


/**
 * Compress function takes filenames for input and output
 * Create map character-> count in text
 * Create huffman tree
 * Feel codes for character and add node to list for fast access in the future
 * Create bitset of charcodes
 * Write bytes in the file
 *
 * @param:file_in input file
 * @param:file_out compressed file
 *
 * */
void huffman::compress(std::string *file_in, std::string *file_out) {
    std::vector<char> *data = read_data(file_in);
    std::map<char, int> pf;
    std::list<node *> nodes;

    for (char c : *data)
        pf[c]++;

    for (auto pair : pf)
        nodes.push_back(new node(pair.first, pair.second));

    node* root = create_tree(&nodes);

    feel_codes(root);

    dynamic_bitset::dynamic_bitset bitset;
    for (char c : *data) {
        bitset.push(threaded_tree[c]->code,threaded_tree[c]->code_len);
    }

    for(std::pair<char,huffman::node*> n : threaded_tree)
        std::cout<< n.first << " - "<<std::bitset<16>(n.second->code)<< " code len = "<< static_cast<unsigned short>(n.second->code_len)<<std::endl;


    bitset.print_bytes();
    std::cout<<std::endl;
    std::ofstream out(*file_out, std::ios_base::binary);

    size_t file_size = data->size();
    size_t map_size =  pf.size();
    out.write(reinterpret_cast<const char*>(&file_size), sizeof(file_size));
    out.write(reinterpret_cast<const char*>(&map_size), sizeof(map_size));
    for(std::pair<char,int> pair: pf){
        out.write(reinterpret_cast<const char *>(&pair.first), sizeof(pair.first));
        out.write(reinterpret_cast<const char *>(&pair.second), sizeof(pair.second));

    }
    out.write(reinterpret_cast<const char *>(bitset.bytes()), bitset.size()*8);
    out.close();

    delete data;
}

void huffman::decompress(std::string *file_in, std::string *file_out) {

    std::map<char,int> pf;
    std::vector<char>* data =  read_data(file_in);
    unsigned int byte_position = 0;
    std::list<node*> nodes;
    size_t map_size;
    size_t bytes_count;

    memcpy(&bytes_count,&(*data)[0], sizeof(bytes_count));
    memcpy(&map_size,&(*data)[sizeof(bytes_count)], sizeof(map_size));

    byte_position += sizeof(map_size) + sizeof(bytes_count);
    for(int i = 0; i < map_size; ++i){
        unsigned int freq;
        char c = (*data)[5*i + byte_position];
        memcpy(&freq,&(*data)[5*i+1 + byte_position],sizeof(freq));
        pf[c]=freq;
    }

    byte_position += map_size*5;
    dynamic_bitset::dynamic_bitset* bitset = new dynamic_bitset::dynamic_bitset(&(*data)[byte_position] , data->size() - byte_position);

    for (auto pair : pf)
        nodes.push_back(new node(pair.first, pair.second));

    node* root = create_tree(&nodes);
    current_code = 0l;
    depth=0;
    feel_codes(root);
    bitset->print_bytes();

    std::ofstream out(*file_out,std::ios_base::binary);
    for (int i =0; i < bytes_count; ++i) {
        out.write(&decode_char(root,bitset)->character, sizeof(char));
    }
    delete data;
}



huffman::node* create_tree(std::list<huffman::node*>* nodes){
    using namespace huffman;
    node* current_root = nullptr;
    auto cmp = [](node *left, node *rigth) { return left->weight < rigth->weight; };

    while(nodes->size()!=1){
        nodes->sort(cmp);
        node*left=nodes->front();
        nodes->pop_front();
        node* right=nodes->front();
        nodes->pop_front();
        nodes->push_back(new node(left,right));
    }
    return nodes->front();
}



/**
 * Recursive tree following
 * Takes root, than add for every tree leaf charcode and charcode length
 * If node is tree leaf add this node to the threaded tree for fast access in the future
 *
 * @param: current_code - global variable for current charcode
 * @param: depth - global variable for charcode length or tree depth
 * @param: node - current processing node
 * @return: void
 */
void feel_codes(huffman::node* _node){
    using namespace huffman;
    using namespace std;

    if (_node->left != nullptr){
        depth++;
        current_code<<=1;
        feel_codes(_node->left);
    }
    if (_node->right != nullptr){
        depth++;
        current_code <<=1;
        current_code |=1;
        feel_codes(_node->right);
    }
    if(_node->left == nullptr && _node->right == nullptr){
        _node->code_len=depth;
        _node->code=current_code;
//        cout<<"Char - "<<_node->character<< " set code "<<bitset<8>(current_code)<<" code len = "<<depth<<endl;
        threaded_tree[_node->character]=_node;
    }
    current_code >>=1;
    depth--;
}


huffman::node* decode_char(huffman::node* nod,dynamic_bitset::dynamic_bitset* bitset){
    if(nod->left == nullptr && nod->right == nullptr)
        return nod;

    byte b = bitset->pop(1);
    if(nod->left!= nullptr && b == 0) {
        nod = decode_char(nod->left, bitset);
        if(nod->left == nullptr && nod->right == nullptr)
            return nod;
    }
    if(nod->right!= nullptr && b == 1) {
        nod = decode_char(nod->right, bitset);
        if(nod->left == nullptr && nod->right == nullptr)
            return nod;
    }
}


/**
 * Read data from file
 *
 * @param:filename
 * @return:vector<char> - text
 * */
std::vector<char> *read_data(std::string *filename) {
    std::ifstream file(*filename, std::ios_base::binary | std::ios_base::ate);
    long length = file.tellg();
    file.seekg(std::ios_base::beg);
    assert(file.is_open());
    char *buff = new char[length]; // init pointer
    auto *data = new std::vector<char>();
    for (int i = 0; !file.eof(); i++) {
        file.read(&buff[i * claster], claster);
    }
    data->assign(buff, buff + length);

    delete buff;
    return data;
}