#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <assert.h>
#include "util/util.h"

std::vector<uint64_t> primes;

/**
 *  primes_n.bin contains primes of n byte length. n can be any positive integer. They are stored in an array of bytes in big endian format. Otherwise there is no formatting.
**/
void load_primes(){
    if(!std::filesystem::exists("primes")) return;
    
    uint64_t num_primes = 0;
    // pre allocate primes global with correct size for known primes.
    for(uint64_t i = 1; std::filesystem::exists("primes/primes_"+std::to_string(i)+".bin"); i++ ){
        assert( i > 0 && i <= 8 && "primes/primes_n.bin file has unexpected size.\n(not between 1 & 8 inclusive)" );
        num_primes += std::filesystem::file_size("primes/primes_"+std::to_string(i)+".bin")/i;
    }
    
    primes.reserve(num_primes);
    
    //
    for(uint64_t i = 1; std::filesystem::exists("primes/primes_"+std::to_string(i)+".bin"); i++ ){
        assert( i > 0 && i <= 8 && "primes/primes_n.bin file has unexpected size.\n(not between 1 & 8 inclusive)" );
        
        auto file = load_file("primes/primes_"+std::to_string(i)+".bin");
        
        for(int j = 0; j < file.size(); j+= i){
            uint64_t n = 0;
            
            for(int k = 0; k < i; k++ ){
                n <<= 8;
                n |= file[j+k];
            }
            
            primes.push_back(n);
        }
    }
    
    for( auto p : primes ) assert(p!=0);
    for( auto p : primes ) assert(p!=1);
    
}

// I know this is not even close to optimal.
int num_bytes(uint64_t n){
    int upper_1 = 0;
    
    for(int i = 0; i < 64; i++){
        if((n>>i)&1) upper_1 = i;
    }
    
    return upper_1/8+1;
}

// want to avoid rewriting/saving lower primes that we know are complete.
// making the assumption that if the primes_n+1.bin exists primes_n.bin is complete.
void save_primes(){
    if(!std::filesystem::exists("primes"))
        std::filesystem::create_directory("primes");
    
    int num_saved_primes = 0;
    
    uint64_t idx = 0;
    for(uint64_t i = 1; std::filesystem::exists("primes/primes_"+std::to_string(i)+".bin"); i++ ){
        assert( i > 0 && i <= 8 && "primes/primes_n.bin file has unexpected size.\n(not between 1 & 8 inclusive)" );
        num_saved_primes += std::filesystem::file_size("primes/primes_"+std::to_string(i)+".bin")/i;
    }
    
    // not the most efficient to save them one at a time, but it works for now. Really doesn't matter when they get so large any how.
    for(int i = num_saved_primes; i < primes.size(); i++){
        std::vector<uint8_t> data;
        uint64_t P = primes[i];
        int NB = num_bytes(primes[i]);
        assert(NB>0&&NB<=8);
        for( int j = 0; j < NB; j++ )
            data.push_back( ( P >> (8*(NB-(j+1))) ) & 0xFF);
        append_file("primes/primes_"+std::to_string(NB)+".bin",data);
    }
    
}

int main(int argc, char** argv){
    
    load_primes();
    
    uint64_t saved = primes.size();
    
    uint64_t start = saved?primes[saved-1]+1:2;
    
    std::cout << "Have already computed " << saved << " PRIME numbers!!!\n";
    std::cout << "Starting at: " << start << std::hex << " : 0x" << start << std::dec << "\n";
    
    for( uint64_t i = start; i < 0xFFFF'FFFF; i++){
        bool is_prime = true;
        for(auto P : primes)
            if(i%P==0)
                is_prime = false;
        if( is_prime ) primes.push_back(i);
        
        if( saved+1024 <= primes.size()){
            save_primes();
            saved = primes.size();
            
            //*This should be commented out when the numbers get large enough to be worth dropping the #num found per save constant
            std::cout << "We now have " << saved << " primes!\n";
            std::cout << "Our biggest prime is now " << primes[primes.size()-1] << std::hex << " (0x" << primes[primes.size()-1] << std::dec << ")!\n";
            //*/
        }
    }
    
    save_primes();
}
