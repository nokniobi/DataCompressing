#include<string>

namespace huffman {


    struct node{
        char character;
        unsigned long code;
        unsigned char code_len;
        int weight;
        node *left = nullptr, *right = nullptr;


        node(){}

        node(char c, int w){
            this->character = c;
            this->weight = w;
        }

        node(node* l, node* r){
            this->weight = l->weight + r->weight;
            this->left = l;
            this->right = r;
        }
    };

    void compress(std::string*, std::string*);

    void decompress(std::string*, std::string*);
}
