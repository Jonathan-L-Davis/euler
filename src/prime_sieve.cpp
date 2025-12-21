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
int num_bits(uint64_t n){
    int upper_1 = 0;
    
    for(int i = 0; i < 64; i++){
        if((n>>i)&1) upper_1 = i;
    }
    
    return upper_1;
}

int num_bytes(uint64_t n){
    return num_bits(n)/8+1;
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
    
    uint64_t start = saved?primes[saved-1]:2;
    
    if(start==2){
        start=3;
        primes={2};// makes it easier to skip evens later.
    }else start += 2;// skip over a few.
    
    std::cout << "Have already computed " << saved << " PRIME numbers!!!\n";
    std::cout << "Starting at: " << start << std::hex << " : 0x" << start << std::dec << "\n";
    
    assert(start&1 && "Uh oh, why are we starting on an even number?");
    for( uint64_t i = start; i < 0xFFFF'FFFF; i+=2){
        bool is_prime = true;
        
        // should always be greater than the real sqrt & is a relatively accurate approx for how cheap it is.
        // Conceptually, given i, we take the next power of 2 greater, then square root it with bit shifts.
        // Then we cut off half the bits ( I do this with a single bit shift of half the expected bits).
        // +1 because numbits returns one less than you'd expect (it's hightest bit pos).
        // +1 again in case we have an odd number of bits. (otherwise the div by 2 would put us under the square root. Basically a no op for even number of bits)
        // instead of shifting over all the way and half way back I just do a single left shift of half length.
        // can definitely do better for similar complexity, but this is okay for now.
        uint64_t fake_sqrt = 1<<( (num_bits(i)+2)/2);
        
        for(auto P : primes){
            
            if(P>fake_sqrt) break;
            
            if(i%P==0)
                is_prime = false;
            
            if(!is_prime)break;// early out because I don't want to waste time.
        }
        
        if( is_prime ) primes.push_back(i);
        
        constexpr uint64_t group_size = 4096*4;
        if( saved+group_size <= primes.size()){
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
