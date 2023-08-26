//#define SDL_MAIN_HANDLED
#define __USE_MINGW_ANSI_STDIO 1  // to avoid %ll issue. special request for windows
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string>
#include <math.h>
//#include "C:\Program Files (x86)\Microsoft SDKs\MPI\Include\mpi.h"
#include <mpi.h>
#include <iostream>
#include <assert.h>
#include <random>
#include "Mantis.h"


using namespace std;

typedef unsigned long long int UINT64;

std::mt19937 gen;
std::uniform_int_distribution<int> dis(0x0,0xf);
std::uniform_int_distribution<int> dis2(0x1,0xf);


void init_prng_mt(int offset){
    unsigned int initial_seed = 10*time(NULL) + offset;
    gen.seed(initial_seed);
    printf("[+] PRNG initialized to 0x%08X\n", initial_seed);
}

void genQuadruple(uint8_t p0[16], uint8_t p1[16], uint8_t p2[16], uint8_t p3[16], string inpatt_state){
    int i;
    
    for (i = 0; i < 16; i++){
        switch (inpatt_state[i]){
            case '-':
                p0[i] = dis(gen);
                p1[i] = p0[i];
                p2[i] = p0[i];
                p3[i] = p0[i];
                break;
            case 's':
                p0[i] = dis(gen);
                p1[i] = p0[i];
                p2[i] = p0[i] ^ dis2(gen);
                p3[i] = p2[i];
                break;
            case 'c':
                p0[i] = dis(gen);
                p1[i] = p0[i] ^ dis2(gen);
                p2[i] = p0[i];
                p3[i] = p1[i];
                break;
            case 'x':
                p0[i] = dis(gen);
                p1[i] = p0[i] ^ dis2(gen);
                p2[i] = p1[i];
                p3[i] = p0[i];
                break;
            default: // '*'
                p0[i] = dis(gen);
                p1[i] = dis(gen);
                p2[i] = dis(gen);
                p3[i] = dis(gen);
                break;
        }
    }

    
}

bool check_pattern(uint8_t c0[16], uint8_t c1[16], uint8_t c2[16], uint8_t c3[16], string patt){
    /*
    check if quadruple (c0,c1,c2,c3) satisfy patt.
    */
    bool flag = true;
    for (int i =0; i<16; i++){
        switch (patt[i]){
            case '-':
                if ((c0[i]==c1[i]) and (c0[i]==c2[i]) and (c0[i]==c3[i]) )
                    break;
                else{
                    flag = false;
                    break;
                }
            case 'c':
                if ((c0[i]==c2[i]) and (c1[i]==c3[i]))
                
                    break;
                else{
                    flag = false;
                    break;
                }
            case 'x':
                if ((c0[i]==c3[i]) and (c1[i]==c2[i]))
                    break;
                else{
                    flag = false;
                    break;
                }
            case 's':
                if ((c0[i]==c1[i]) and (c2[i]==c3[i]))
                    break;
                else{
                    flag = false;
                    break; 
                }   
            default:
                break;     
        }
    }
    return flag;
}
UINT64 * quadruple(uint8_t rd, uint8_t rd_for_rand, uint8_t tk[][16], uint8_t tk_for_rand[][16], UINT64 N3, string inpatt, string outpatt){
    /*
    Construct N3 quadruples in inpatt under a fixed tweakey.
    Return the number of right quadruples satisfying outpatt.
    */
    uint8_t p0[16],p1[16];
    uint8_t p2[16],p3[16];
    uint8_t c0[16],c1[16];
    uint8_t c2[16],c3[16];

    //bool debug = false;
    UINT64 bnum=0;
    static UINT64 count[2];
    count[0] = 0;
    count[1] = 0;
    while (bnum < N3){
        bnum++;
        genQuadruple(p0, p1, p2, p3, inpatt);
        //产生满足输入模式的四元组

        enc(rd, p0, c0, tk);
        enc(rd, p1, c1, tk);
        enc(rd, p2, c2, tk);
        enc(rd, p3, c3, tk);
        
        if (check_pattern(c0,c1,c2,c3,outpatt))
            count[0]++;
        
        
        enc(rd_for_rand, p0, c0, tk_for_rand);
        enc(rd_for_rand, p1, c1, tk_for_rand);
        enc(rd_for_rand, p2, c2, tk_for_rand);
        enc(rd_for_rand, p3, c3, tk_for_rand);
        

        if (check_pattern(c0,c1,c2,c3,outpatt))
            count[1]++;
        
        
    }
    return count;
}

UINT64 * testOnMultKey(uint8_t rd, uint8_t rd_for_rand, UINT64 N2, UINT64 N3, string inpatt, string outpatt){
    /*
    Set N2 multi masterkeys. Under each masterkey, test on N3 quadruples.
    Return sum of right quadruple numbers under each key.
    */
    uint8_t key0[16];// = {0x1, 0x5, 0x7, 0x7, 0x8, 0x9, 0xA, 0xD, 0xF, 0xC, 0xE, 0xD, 0x7, 0x8, 0xB, 0xD}; // Key 0
    uint8_t key1[16];// = {0xD, 0x9, 0xE, 0x0, 0xE, 0x3, 0x8, 0x1, 0xF, 0xE, 0x6, 0xA, 0x9, 0x4, 0xC, 0x5}; // Key 1
    uint8_t master_tweak[16];// = {0xD, 0xa, 0xA, 0x0, 0xA, 0xA, 0x0, 0xA, 0x6, 0x5, 0xC, 0x0, 0x0, 0x1, 0x1, 0x1}; // master_tweak
		    
    
    uint8_t tk[rd][16];
    uint8_t tk_for_rand[rd_for_rand][16]; //专门用于高轮数加密的轮密钥变量
    UINT64 bnum=0;
    static UINT64 count[2];
    UINT64 *count_itr;
    count[0] = 0;
    count[1] = 0;
    while (bnum < N2){
        bnum++;
        //generate a random key
        for(int i = 0; i < 16; i++) key0[i] = dis(gen);
        for(int i = 0; i < 16; i++) key1[i] = dis(gen);
        
        for(int i = 0; i < 16; i++) master_tweak[i] = dis(gen);
        
        initialize((rd-2)/2, key0, key1, master_tweak, tk);
        initialize((rd_for_rand - 2)/2, key0, key1, master_tweak, tk_for_rand); //生成专门用于高轮数加密的轮密钥变量

        count_itr = quadruple(rd, rd_for_rand, tk, tk_for_rand, N3, inpatt, outpatt);
        count[0] += *count_itr;
        count[1] += *(count_itr + 1);
    }
    return count;
}

int main(int argc, char** argv){
    
    UINT64 N2 = 1; //= 1 << 4; //test under N2 keys
    UINT64 N3 = 1;// = 1 << 10; //generate N3 quadruples under each key
    uint8_t rd = 6;
    string inpatt;
    string outpatt;
    
    switch (atoi(argv[1])) {
        case 6:
        {
            rd = 6;
            inpatt = "-c---------x----";
            outpatt = "***c*****x******";
            N2 = 1 << 8; //test under N2 keys
            N3 = 1 << 8; //generate N3 quadruples under each key
        }
            break;
        default:
            cout<<"Not supported."<<endl;
            return 0;
    }
    
    int rd_for_rand = atoi(argv[2]); //命令行中输入的第二个参数用于设定模拟随机置换的加密轮数

    /*
    For each processor, run 
    testOnMultKey(uint8_t rd, int64_t N2, int64_t N3, string inpatt, string outpatt):
    Set N2 multi masterkeys. Under each masterkey, test on N3 quadruples.
    Return sum of right quadruple numbers under each key.
    */
    MPI_Init(NULL, NULL);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    printf("\n[+] Proc %d\n", world_rank);
    init_prng_mt(world_rank);
    //Compute on each process
    UINT64 *count;
    
    clock_t clock_timer;
    clock_timer = clock();
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    //init_prng_mt(world_rank);
    double proc_time;
    
    proc_time = MPI_Wtime();
    count = testOnMultKey(rd,rd_for_rand, N2,N3,inpatt,outpatt); //增加了一个模拟随机置换的加密轮数作为参数
    proc_time = MPI_Wtime() - proc_time;
    
    MPI_Barrier(MPI_COMM_WORLD);
    //Gather all number of right quadruples down to the root process
    UINT64 *NUM = NULL;
    if (world_rank == 0){
        NUM = (UINT64 *)malloc(world_size * sizeof(UINT64) * 2);
        assert(NUM != NULL);
    }

    MPI_Gather(count, 2, MPI_UNSIGNED_LONG_LONG, NUM, 2, MPI_UNSIGNED_LONG_LONG,0,MPI_COMM_WORLD);

    //Compute sum on the root process
    if (world_rank == 0){
        UINT64 right,rand_right;
        right = 0;
        rand_right = 0;
        printf("\n Quaries on each process = 2^(%f)\n",log(N2 * N3)/log(2));
        for (int i = 0; i < world_size *2; i=i+2){
            
            right += *(NUM + i);
            rand_right += *(NUM + i + 1);
            printf(" %d proc:     Right = 2^(%f)\n", i/2, log(*(NUM + i))/log(2));
            printf(" %d proc: randRight = 2^(%f)\n", i/2, log(*(NUM + i + 1))/log(2));
        }
        printf("\n                              Total queries = 2^(%f) \n", log(N2 * N3 * world_size)/log(2));
        printf("MANTIS:     Number of total right quadruples = 2^(%f)\n", log(right)/log(2));
        printf("           Probability of right quadruples  = 2^(-%0.4f)\n", (log(N2 * N3 * world_size)-log(right))/log(2));
        printf(" RandPerm: Number of total right quadruples = 2^(%f)\n", log(rand_right)/log(2));
        printf("            Probability of right quadruples = 2^(-%0.4f)\n", (log(N2 * N3 * world_size)-log(rand_right))/log(2));
        cout << " time on clock(): " << ((double)clock() - clock_timer) / CLOCKS_PER_SEC<<endl<<endl;
    }

    
    printf("[+] Time : %lf\n",proc_time);
    printf("[+] MANTIS:         Number of right quadruples = 2^(%f)\n", log(*count)/log(2));
    printf("[+]           Probability of right quadruples = 2^(-%0.4f)\n\n", (log(N2 * N3)-log(*count))/log(2));
    printf("[+] RandPerm:      Number of right quadruples = 2^(%f)\n", log(*(count+1))/log(2));
    printf("[+]           Probability of right quadruples = 2^(-%0.4f)\n\n", (log(N2 * N3)-log(*(count+1)))/log(2));

    //Clean up
    if (world_rank == 0){
        free(NUM);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return 0;
}