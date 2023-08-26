#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "Mantis.h"
using namespace std;


void print_state(uint8_t state[16])
{
    for (int i = 0; i < 16; i++)
        printf("%01x", state[i]);
    printf("\n");
}


void initialize(int r, uint8_t key0[16], uint8_t key1[16],uint8_t master_tweak[16],uint8_t tk[][16]){
  //Mantis_r
  
  uint8_t key0_[16];
  for (uint8_t i = 0; i < 16; i++) {
        key0[i] = key0[i] & 0xf;    
        key1[i] = key1[i] & 0xf;   
        master_tweak[i] = master_tweak[i] & 0xf;
    }
    
  key0_[0]=((key0[15]&0x1)<<3)^(key0[0]>>1);//先将循环右移操作最末位置的数调到合适的位置
  for(uint8_t i=1;i<16;i++)
  {
    key0_[i]=((key0[i-1]&0x1)<<3)^(key0[i]>>1);
  }
  key0_[15]=key0_[15]^(key0[0]>>3);//至此完成求k0'的操作

  uint8_t T[r+1][16]; //T[i] 中存放的是 mastertweak 做 i 次 h 置换后的状态
  for(uint8_t j=0;j<16;j++)
  {
      T[0][j] = master_tweak[j];
  }
  for(uint8_t i=1;i <= r;i++)
  {
    for(uint8_t j=0;j<16;j++)
    {
        T[i][j] = T[i-1][h[j]];
    }
  }

//一共用到 (2*r+2) 个 tk (包含首尾的白化密钥)

  //前 r+1 个
  for(uint8_t i=0;i <= r; i++)
  {
    for(uint8_t j=0;j<16;j++)
    {
      tk[i][j]=T[i][j]^key1[j];
    }
  }
  for(uint8_t j=0;j<16;j++)
    {
      tk[0][j]=tk[0][j]^key0[j];
    }
  //后 r+1 个
  for(uint8_t i = r+1; i < 2*r + 2; i++)
  {
    for(uint8_t j=0;j<16;j++)
    {
      tk[i][j]=T[2*r+1-i][j]^key1[j] ^alpha[j]; //注意T
    }
  }
   for(uint8_t j=0;j<16;j++)
    {
      tk[2*r + 1][j]=tk[2*r + 1][j]^key0_[j];
    }

    
}
void enc(uint8_t R, uint8_t plaintext[16], uint8_t ciphertext[16], uint8_t tk[][16]) {
    
    
    for (uint8_t i = 0; i < 16; i++) {
        ciphertext[i] = plaintext[i] & 0xf;
    }
    
    for (uint8_t i = 0; i < 16; i++){
        ciphertext[i] = ciphertext[i]^tk[0][i]; //异或白化密钥
    } 

    uint8_t R1=(R-2)/2;//分成前半段和后半段来求，过程不同        
    for (uint8_t r = 0; r < R; r++) {   
        
        uint8_t Temp1[16];
        if (r < R1)
        {
          //SubCells
            for (uint8_t i = 0; i < 16; i++)
                ciphertext[i] = S[ciphertext[i]];
          //AddConstant
            for (uint8_t i = 0; i < 16; i++)
                ciphertext[i] = ciphertext[i]^RC[r][i];
          //ADDTweakey
            for (uint8_t i = 0; i < 16; i++)            
            ciphertext[i] ^= tk[r+1][i]; //注意异或哪个 tk
        
          //Permutation
            
            for (uint8_t i = 0; i < 16; i++){
                //Temp1[P[i]] = ciphertext[i];
                Temp1[i] = ciphertext[P[i]];
            }
                
          //MixColumn
            for (uint8_t i = 0; i < 4; i++) {
                ciphertext[i] = Temp1[i+4]^Temp1[i + 8] ^ Temp1[i + 12];
                ciphertext[i+4] = Temp1[i]^Temp1[i + 8] ^ Temp1[i + 12];
                ciphertext[i+8] = Temp1[i]^Temp1[i + 4] ^ Temp1[i + 12];
                ciphertext[i+12] = Temp1[i]^Temp1[i + 4] ^ Temp1[i + 8];
            }      
        }
        if(r == R1){
          //SubCells
            for (uint8_t i = 0; i < 16; i++)
                Temp1[i] = S[ciphertext[i]];
          //MixColumn
            for (uint8_t i = 0; i < 4; i++) {
                ciphertext[i] = Temp1[i+4]^Temp1[i + 8] ^ Temp1[i + 12];
                ciphertext[i+4] = Temp1[i]^Temp1[i + 8] ^ Temp1[i + 12];
                ciphertext[i+8] = Temp1[i]^Temp1[i + 4] ^ Temp1[i + 12];
                ciphertext[i+12] = Temp1[i]^Temp1[i + 4] ^ Temp1[i + 8];
            }    
          //SubCells
            for (uint8_t i = 0; i < 16; i++)
                ciphertext[i] = S[ciphertext[i]];
        }
        if(r >= R1 + 2)
        {
            //MixColumn
            for (uint8_t i = 0; i < 4; i++) {
                Temp1[i] = ciphertext[i+4]^ciphertext[i + 8] ^ ciphertext[i + 12];
                Temp1[i+4] = ciphertext[i]^ciphertext[i + 8] ^ ciphertext[i + 12];
                Temp1[i+8] = ciphertext[i]^ciphertext[i + 4] ^ ciphertext[i + 12];
                Temp1[i+12] = ciphertext[i]^ciphertext[i + 4] ^ ciphertext[i + 8];
            } 
            //Permutation
            for (uint8_t i = 0; i < 16; i++)
                //ciphertext[P_[i]] = Temp1[i];
                ciphertext[i] = Temp1[P_[i]];

            
            //ADDTweakey
            for (uint8_t i = 0; i < 16; i++)          
                ciphertext[i] ^= (tk[r-1][i]); //注意 tk 的调用
        
            //AddConstant
            for (uint8_t i = 0; i < 16; i++)
                ciphertext[i] = ciphertext[i]^RC[R-r-1][i];
            //SubCells
            for (uint8_t i = 0; i < 16; i++)
                ciphertext[i] = S[ciphertext[i]];
            
                
            
        }
    }
    for (uint8_t i = 0; i < 16; i++)
            ciphertext[i] = ciphertext[i]^tk[R-1][i]; //注意 tk 的调用

    
}

