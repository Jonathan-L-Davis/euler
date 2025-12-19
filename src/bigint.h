#pragma once

#include <vector>

struct bigint{
    
    std::vector<uint64_t> chunks;
    
    
};


bigint operator + (bigint a, bigint b){
    bigint c;
    
    bool overflow = false;
    for( int i = 0; i < a.chunks.size() && i < b.chunks.size(); i++ ){
        
    }
    
    return c;
}
