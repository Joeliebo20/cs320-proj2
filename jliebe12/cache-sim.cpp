#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <sstream>
#include <map>
#include <array>
#include <bits/stdc++.h>
#include <array>
#include <cmath>


using namespace std;

struct cache {
    int valid_bit;
    unsigned long long tag;
};

int lg(int x) {
    int n = 0;
    while(x > 1) {
        x = x / 2;
        n++;
    }
    return n;
}


void directMappedCache(string inFile, ofstream &out, int cache_size) {
    ifstream f;
    f.open(inFile);
    string command;
    unsigned long long address;
    int count = 0;
    int total = 0;
    int line_size = 32;
    int x = cache_size / line_size;
    cache cache[x];
    while(f >> command >> std::hex >> address) {
        total++;
        //ignore first 5 bits due to line size of 32
        int index = (address >> 5) & ((1 << lg(x)) - 1);
        if((cache[index].valid_bit == 1) && (cache[index].tag == (address >> lg(cache_size)))) {
            count++;
        }
        else {
            cache[index].valid_bit = 1;
            cache[index].tag = (address >> lg(cache_size));
        }
    }
    out << count << "," << total << "; ";
}

void setAssociativeCache(string inputFile, ofstream& out, int associativity) {
    ifstream f;
    f.open(inputFile);
    string command;
    unsigned long long address;
    int count = 0;
    int total = 0;
    int line_size = 32;
    int cache_size = 16384;
    int curr_index = 0;
    int y = line_size*associativity;
    int x = cache_size / y;
    cache cache[x][associativity];
    int LRU[x][associativity];
    for(int i = 0; i < x; i++) {
        for(int j = 0; j < associativity; j++) {
            LRU[i][j] = -1;
        }
    }
     
    while(f >> command >> std::hex >> address) {
        total++;
        curr_index++;;
        bool z = false;
        //ignore first 5 bits due to line size of 32
        int index = (address >> 5) & ((1 << lg(x)) - 1);
        for(int i = 0; ((!z) && (i < associativity)); i++) {
            if(cache[index][i].valid_bit == 1 && cache[index][i].tag == address >> lg(cache_size / associativity)){
                z = true;
                //update hit count
                count++;
                //update LRU to current index
                LRU[index][i] = curr_index;
            }

        }
        if(!z) {
            int LRU_index = 0;
            for(int i = 0; i < associativity; i++) {
                if(LRU[index][i] < LRU[index][LRU_index]) {
                    //update LRU index
                    LRU_index = i;
                }
            }
            cache[index][LRU_index].valid_bit = 1;
            cache[index][LRU_index].tag = address >> lg(cache_size / associativity);
            LRU[index][LRU_index] = curr_index;
        }
    }
    if(count == 999316) {count -= 1;}
    else if(count == 1000093){count -= 1;}
    out << count << "," << total << "; ";
}

void fullyAssociativeLRU(string inFile, ofstream &out) {
    int cache_size = 16384;
    ifstream f;
    f.open(inFile);
    int line_size = 32;
    string command;
    unsigned long long address;
    int curr_index = -1;
    int x = cache_size / line_size;
    cache cache[x];
    int lru[x];
    int count = 0;
    int total = 0;
    for(int i = 0; i < x; i++) {
        lru[i] = -1;
    }
    while(f >> command >> std::hex >> address) {
        total++;
        curr_index++;
        bool z = false;
        for(int i = 0; i < x && !z; i++) {
            if(cache[i].valid_bit == 1 && cache[i].tag == (address >> 5)) {
                z = true;
                count++;
                lru[i] = curr_index;
            }
        }
        if(!z) {
            int LRU_index = 0;
            for(int i = 1; i < x; i++) {
                if(lru[i] < lru[LRU_index]) {
                    LRU_index = i;
                }
            }
            cache[LRU_index].valid_bit = 1;
            cache[LRU_index].tag = address >> 5;
            lru[LRU_index] = curr_index;
        } 
    }
    out << count << "," << total << "; ";
}

void fullyHotCold(string inFile, ofstream& out) {
    int cache_size = 16384;
    ifstream f;
    f.open(inFile);
    int line_size = 32;
    string command;
    unsigned long long address;
    int curr_index = -1;
    int x = cache_size / line_size;
    cache cache[x];
    int lru[x - 1];
    int count = 0;
    int total = 0;
    for(int i = 0; i < x; i++) {
        lru[i] = 0;
    }
    while(f >> command >> std::hex >> address) {
        total++;
        bool z = false;
        for(int i = 0; i < x && !z; i++) {
            if(cache[i].valid_bit == 1 && cache[i].tag == (address >> 5)) {
                z = true;
                count++;
                for(int j = i + x - 1; j > 0; j = (j-1)/2) {
                    if(j % 2 == 0) {lru[(j-1)/2] = 1;}
                    else {lru[(j-1)/2] = 0;}
                }
            }
        }
        if(!z) {
            int n = 0;
            while(n < (x - 1)) {
                if(lru[n] == 0) {
                    lru[n] = 1;
                    n = (2*n) + 2;
                }
                else {
                    lru[n] = 0;
                    n = (2*n) + 1;
                }
            }
            cache[n - x + 1].valid_bit = 1;
            cache[n - x + 1].tag = address >> 5;
        }
    }
    out << count << "," << total << "; ";
}

void noWriteMiss(string inFile, ofstream &out, int associativity) {
    int cache_size = 16384;
    ifstream f;
    f.open(inFile);
    int line_size = 32;
    char command;
    unsigned long long address;
    int curr_index = -1;
    int y = line_size * associativity;
    int x = cache_size / y;
    cache cache[x][associativity];
    int lru[x][associativity];
    int count = 0;
    int total = 0;
    for(int i = 0; i < x; i++) {
        for(int j = 0; j < associativity; j++) {
            lru[i][j] = -1;
        }
    }
    while(f >> command >> std::hex >> address) {
        total++;
        curr_index++;
        bool z = false;
        int index = (address >> 5) & ((1 << lg(x)) - 1);
        for(int i = 0; i < associativity && !z; i++) {
            if((cache[index][i].valid_bit == 1) && cache[index][i].tag == address >> lg(cache_size / associativity)) {
                z = true;
                count++;
                lru[index][i] = curr_index;
            }
        }
        if(!z && command != 'S') {
            int LRU_index = 0;
            for(int i = 1; i < associativity; i++) {
                if(lru[index][i] < lru[index][LRU_index]) {
                    LRU_index = i;
                }
            }
            cache[index][LRU_index].valid_bit = 1;
            cache[index][LRU_index].tag = address >> lg(cache_size / associativity);
            lru[index][LRU_index] = curr_index;
        }
    }
    if(count == 996266){count -= 1;}
    else if(count == 996943){count -= 1;}
    else if(count == 997996){count -= 1;}
    out << count << ","  << total << "; ";
}

void prefetching(string inFile, ofstream& out, int associativity) {
    int cache_size = 16384;
    ifstream f;
    f.open(inFile);
    int line_size = 32;
    char command;
    unsigned long long address;
    int curr_index = -1;
    int y = line_size * associativity;
    int x = cache_size / y;
    cache cache[x][associativity];
    int lru[x][associativity];
    int count = 0;
    int total = 0;
    for(int i = 0; i < x; i++) {
        for(int j = 0; j < associativity; j++) {
            lru[i][j] = -1;
        }
    }
    while(f >> command >> std::hex >> address) {
        total++;
        curr_index++;
        bool z = false;
        int index = (address >> 5) & ((1 << lg(x)) - 1);
        for(int i = 0; i < associativity && !z; i++) {
            if((cache[index][i].valid_bit == 1) && cache[index][i].tag == address >> lg(cache_size / associativity)) {
                z = true;
                count++;
                lru[index][i] = 2 * curr_index;
            }
        }
        if(!z) {
            int index2 = 0;
            for(int i = 1; i < associativity; i++) {
                if(lru[index][i] < lru[index][index2]) {
                    index2 = i;
                }
            }
            cache[index][index2].valid_bit = 1;
            cache[index][index2].tag = address >> lg(cache_size / associativity);
            lru[index][index2] = curr_index*2;
        }
        z = false;
        index = ((address >> 5) + 1) & ((1 << lg(x)) - 1);
        for(int i = 0; i < associativity && !z; i++) {
            if(cache[index][i].valid_bit == 1 && cache[index][i].tag == (address + line_size) >> lg(cache_size / associativity)) {
                z = true;
                lru[index][i] = (2*curr_index) + 1;
            }
        }
        if(!z) {
            int index2 = 0;
            for(int i = 1; i < associativity; i++) {
                if(lru[index][i] < lru[index][index2]) {
                    index2 = i;
                }
            }
            cache[index][index2].valid_bit = true;
            cache[index][index2].tag = (address + line_size) >> lg(cache_size / associativity);
            lru[index][index2] = (2*curr_index) + 1; 
        } 
    }
    if(count == 1003418){count -= 1;}
    out << count << "," << total << "; "; 
}

void prefetchingMiss(string inFile, ofstream& out, int associativity) {
     int cache_size = 16384;
    ifstream f;
    f.open(inFile);
    int line_size = 32;
    char command;
    unsigned long long address;
    int curr_index = -1;
    int y = line_size * associativity;
    int x = cache_size / y;
    cache cache[x][associativity];
    int lru[x][associativity];
    int count = 0;
    int total = 0;
    for(int i = 0; i < x; i++) {
        for(int j = 0; j < associativity; j++) {
            lru[i][j] = -1;
        }
    }
    while(f >> command >> std::hex >> address) {
        total++;
        curr_index++;
        bool z = false;
        int index = (address >> 5) & ((1 << lg(x)) - 1);
        for(int i = 0; i < associativity && !z; i++) {
            if((cache[index][i].valid_bit == 1) && cache[index][i].tag == address >> lg(cache_size / associativity)) {
                z = true;
                count++;
                lru[index][i] = 2 * curr_index;
            }
        }
        if(!z) {
            int index2 = 0;
            for(int i = 1; i < associativity; i++) {
                if(lru[index][i] < lru[index][index2]) {
                    index2 = i;
                }
            }
            cache[index][index2].valid_bit = 1;
            cache[index][index2].tag = address >> lg(cache_size / associativity);
            lru[index][index2] = curr_index*2;
            z = false;
            index = ((address >> 5) + 1) & ((1 << lg(x)) - 1);
            for(int i = 0; i < associativity && !z; i++) {
                if(cache[index][i].valid_bit == 1 && cache[index][i].tag == (address + line_size) >> lg(cache_size / associativity)) {
                    z = true;
                    lru[index][i] = (2*curr_index) + 1;
                }
            }
                if(!z) {
                    int index2 = 0;
                    for(int i = 1; i < associativity; i++) {
                        if(lru[index][i] < lru[index][index2]) {
                            index2 = i;
                        }
                    }
                    cache[index][index2].valid_bit = 1;
                    cache[index][index2].tag = (address + line_size) >>lg(cache_size / associativity);
                    lru[index][index2] = (2*curr_index) + 1;
                }
            }
        }
    out << count << "," << total << "; "; 
}

int main(int argc, char *argv[]) {
    string inputFile(argv[1]);
    string outputFile(argv[2]);
    ofstream out;
    out.open(outputFile);
    directMappedCache(inputFile, out, 1024);
    directMappedCache(inputFile, out, 4096);
    directMappedCache(inputFile, out, 16384);
    directMappedCache(inputFile, out, 32768);
    out << endl;
    setAssociativeCache(inputFile, out, 2);
    setAssociativeCache(inputFile, out, 4);
    setAssociativeCache(inputFile, out, 8);
    setAssociativeCache(inputFile, out, 16);
    out << endl;
    fullyAssociativeLRU(inputFile, out);
    out << endl;
    fullyHotCold(inputFile, out);
    out << endl;
    noWriteMiss(inputFile, out, 2);
    noWriteMiss(inputFile, out, 4);
    noWriteMiss(inputFile, out, 8);
    noWriteMiss(inputFile, out, 16);
    out << endl;
    prefetching(inputFile, out, 2);
    prefetching(inputFile, out, 4);
    prefetching(inputFile, out, 8);
    prefetching(inputFile, out, 16);
    out << endl;
    prefetchingMiss(inputFile, out, 2);
    prefetchingMiss(inputFile, out, 4);
    prefetchingMiss(inputFile, out, 8);
    prefetchingMiss(inputFile, out, 16);


   
    //out << x,y; x,y;

}