#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

int Address_bits;

int binary_to_int(char* a, int n){
    int temp=1, sum=0;
    for(int i=n-1; i>=0; i--){
        if(a[i] == '1')
            sum += temp;
        temp *= 2;
    }
    return sum;
}

class block{
    private:
        char* data;
        bool nru_bit;
        int Index;
        int Tag;
    public:
        block():data(0), nru_bit(1){};
        block(char* a):data(a), nru_bit(1){};
        char* get_array_data(){
            return data;
        }
        int get_int_data(){
            return binary_to_int(data, Address_bits);
        }
        void set_data(char* a){
            data = a;
        }
        bool get_nru(){
            return nru_bit;
        }
        void set_nru(bool nru){
            nru_bit = nru;
        }
        int get_index(){
            return Index;
        }
        void set_index(int index){
            Index = index;
        }
        int get_tag(){
            return Tag;
        }
        void set_tag(int tag){
            Tag = tag;
        }
};

int main(int argc, char* argv[]){
    string a;
    int Block_size, Cache_sets, Associativity;
    int Offset_bits, Index_bits, tag_bits, table_index=0;
    
    // read cache#.org
    ifstream input(argv[1]);
    input >> a;
    input >> Address_bits;
    //cout << Address_bits << endl;
    input >> a;
    input >> Block_size;
    //cout << Block_size << endl;
    input >> a;
    input >> Cache_sets;
    //cout << Cache_sets << endl;
    input >> a;
    input >> Associativity;
    //cout << Associativity << endl;

    Offset_bits = int(log2(Block_size));
    Index_bits = int(log2(Cache_sets));
    tag_bits = Address_bits - Offset_bits - Index_bits;

    // read reference#.lst
    ifstream input1(argv[2]);
    //cout << argv[2] << endl;
    input1 >> a;
    input1 >> a;

    vector<bool> table;

    // initialize cache (for direct mapped)
    vector<block> cache;
    cache.resize(Cache_sets);
    char a1[1];
    a1[0]=0;
    for(int i=0; i<cache.size(); i++){
        cache[i].set_data(a1);
    }

    while(1){
        // read one line of data once a line
        char c[Address_bits];
        input1 >> c[0];
        if(c[0]=='.') // if the input is .end, break
            break;
        for(int i=1; i<Address_bits; i++)
            input1 >> c[i];

        /*for(int i=0; i<Address_bits; i++)
            cout << c[i];
        cout << endl;*/

        if(Associativity == 1){
            // calculate index
            char index1[Index_bits];
            for(int i=0; i<Index_bits; i++)
                index1[i] = c[tag_bits+i];

            int index = binary_to_int(index1, Index_bits);

            // calculate tag
            char tag1[tag_bits];
            for(int i=0; i<tag_bits; i++)
                tag1[i] = c[i];
        
            int tag = binary_to_int(tag1, tag_bits);
        
            // insert to cache
            if(cache[index].get_int_data() == 0){ // cache miss & nothing in this block
                cache[index].set_data(c);
                cache[index].set_nru(0);
                cache[index].set_index(index);
                cache[index].set_tag(tag);
                table.push_back(0);
            } else { // there are data in the block
                if(cache[index].get_tag() != tag){ // cache miss
                    cache[index].set_data(c);
                    cache[index].set_nru(0);
                    cache[index].set_index(index);
                    cache[index].set_tag(tag);
                    table.push_back(0);
                }
                else { // cache hit
                    table.push_back(1);
                }
            }
        }
        /*else {

        }*/
    }

    for(int i=0; i<table.size(); i++){
        if(table[i] == 0)
            cout << "miss" << endl;
        else
            cout << "hit" << endl;
    }
}