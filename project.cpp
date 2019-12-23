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
        int data;
        bool nru_bit;
        int Index;
        int Tag;
    public:
        block():data(0), nru_bit(1){};
        block(int a):data(a), nru_bit(1){};
        int get_int_data(){
            return data;
        }
        void set_data(int a){
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

    // output file
    ofstream output(argv[3]);
    output << "Address bits: " << Address_bits << endl;
    output << "Block size: " << Block_size << endl;
    output << "Cache sets: " << Cache_sets << endl;
    output << "Associativity: " << Associativity << endl;
    output << endl;
    output << "Offset bit count: " << Offset_bits << endl;
    output << "Indexing bit count: " << Index_bits << endl;
    output << "Indexing bits: ";
    for(int i=0; i<Index_bits; i++)
        output << Offset_bits+i << " ";
    output << endl;
    output << endl;

    output << ".benchmark testcase1" << endl;

    // initialize cache
    block b[Cache_sets][Associativity];
    if(Associativity == 1){
        for(int i=0; i<Cache_sets; i++){
            b[i][0].set_data(-1);
        }
    }
    else{
        char a1[1];
        a1[0] = 0;
        for(int i=0; i<Cache_sets; i++){
            for(int j=0; j<Associativity; j++){
                b[i][j].set_data(-1);
            }
        }
    }
    

    while(1){
        bool ans;
        // read one line of data once a line
        char c[Address_bits];
        input1 >> c[0];
        if(c[0]=='.') // if the input is .end, break
            break;
        for(int i=1; i<Address_bits; i++)
            input1 >> c[i];

        // output c
        for(int i=0; i<Address_bits; i++)
            output << c[i];
        output << " ";

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
        
        if(Associativity == 1){ // direct mapped
            // insert to cache
            if(b[index][0].get_int_data() == -1){ // cache miss & nothing in this block
                b[index][0].set_data(binary_to_int(c, Address_bits));
                b[index][0].set_nru(0);
                b[index][0].set_index(index);
                b[index][0].set_tag(tag);
                table.push_back(0);
                ans = 0;
            } else { // there are data in the block
                if(b[index][0].get_tag() != tag){ // cache miss
                    b[index][0].set_data(binary_to_int(c, Address_bits));
                    b[index][0].set_nru(0);
                    b[index][0].set_index(index);
                    b[index][0].set_tag(tag);
                    table.push_back(0);
                    ans = 0;
                }
                else { // cache hit
                    table.push_back(1);
                    ans = 1;
                }
            }
        }
        else {
            /*for(int i=0; i<Cache_sets; i++){
                cout << b[i][0].get_int_data() << " " << b[i][1].get_int_data() << endl;
            }
            cout << endl;*/

            int flag=1;
            for(int i=0; i<Associativity; i++){
                if(b[index][i].get_int_data() != -1){
                    if(b[index][i].get_tag() == tag){ // hit
                        b[index][i].set_nru(0);
                        table.push_back(1);
                        ans = 1;
                        flag=0;
                        break;
                    }
                }
            }
            if(flag){ // miss
                int j;
                for(j=0; j<Associativity; j++){
                    if(b[index][j].get_nru() == 1)
                        break;
                }
                if(j == Associativity){
                    for(int k=0; k<Associativity; k++)
                        b[index][k].set_nru(1);

                    for(j=0; j<Associativity; j++){
                        if(b[index][j].get_nru() == 1)
                            break;
                    }
                }
                b[index][j].set_data(binary_to_int(c, Address_bits));
                b[index][j].set_nru(0);
                b[index][j].set_index(index);
                b[index][j].set_tag(tag);
                table.push_back(0);
                ans = 0;
            }
        }

        if(ans)
            output << "hit" << endl;
        else
            output << "miss" << endl;
    }

    output << ".end" << endl;
    output << endl;

    int miss_count = 0;
    for(int i=0; i<table.size(); i++){
        if(table[i] == 0)
            miss_count++;
    }
    output << "Total cache miss count: " << miss_count << endl;
}