#include <iostream>
#include <math.h>
#include <string>
#include <map>
#include <fstream>
#include <assert.h>
#include <cstring>
#include "dictionary.h"
#include "../binary/bitset.h"


#define claster 4096

namespace static_code{
    void compress(std::string, std::string);
    void decompress(std::string, std::string);
}