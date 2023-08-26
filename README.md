# LightweightMixtureDifferentialCipher-Mantis
Codes for paper "Security Evaluation of Lightweight Block Ciphers against Mixture Differential Cryptanalysis"

This repository contains supplementary materials of the paper .

The structure of the folder is as follows.

- [MANTIS](#MANTIS)

All the experiments are executed on a laptop with 8 logic processors.

[Gurobi]: https://www.gurobi.com/

[MANTIS] This repository contains two folders: [MILP](https://github.com/shenshensunshine/LightweightMixtureDifferentialCipher-Mantis/tree/main/MILP) and [Verification](https://github.com/shenshensunshine/LightweightMixtureDifferentialCipher-Mantis/tree/main/Verification). [MILP](https://github.com/shenshensunshine/LightweightMixtureDifferentialCipher-Mantis/tree/main/MILP) folder contains source codes to search for the mixture differential distinguishers covering 4-  6- and 8- round of AES. The codes are launched on Linux OS. To run our codes preinstallation is [Gurobi](https://www.gurobi.com/) for solving constraint programming problems. To get the 15 mixture trails for 5-round AES, run

+ `g++ Mantis1.cpp basic.cpp -I/$GUROBI_HOME/include -L/$GUROBI_HOME/lib -lgurobi_c++ -lgurobi100 -lm -o Mantis1.out`
+ `nohup ./Mantis1.out`

[Verification](https://github.com/shenshensunshine/LightweightMixtureDifferentialCipher-Mantis/tree/main/Verification) folder contains the verification codes to verify the quadruple distinguishers we found. To run the programm, MPI package need to be installed and change the library path and include path in Makefile according to your installation path.  Run `mingw32-make` (in Windows) to compile the C++ codes. 

To test 6-round in algorithm with 14-round as parameter in random permutaion by running on 8 processors, run

```sh
mpiexec -n 8 CRAFT_Quadruple_MPI_RNG 6 16
```

You get a summary of number of quaries and number of right quadruples on each process, total queries, number of total right quadruples and the probability. We also reserved codes (commented in current version) for comparison with full round MANTIS as random permutation. 

Results on 6-round with 14-round as parameter in random permutaion：

```
[+] Proc 0
[+] PRNG initialized to 0xF113BEB6

 Quaries on each process = 2^(16.000000)
 0 proc:     Right = 2^(8.400879)
 0 proc: randRight = 2^(0.000000)
 1 proc:     Right = 2^(8.375039)
 1 proc: randRight = 2^(0.000000)
 2 proc:     Right = 2^(8.164907)
 2 proc: randRight = 2^(0.000000)
 3 proc:     Right = 2^(8.071462)
 3 proc: randRight = 2^(0.000000)
 4 proc:     Right = 2^(8.179909)
 4 proc: randRight = 2^(1.000000)
 5 proc:     Right = 2^(8.087463)
 5 proc: randRight = 2^(0.000000)
 6 proc:     Right = 2^(8.303781)
 6 proc: randRight = 2^(0.000000)
 7 proc:     Right = 2^(8.022368)
 7 proc: randRight = 2^(1.000000)

                              Total queries = 2^(19.000000)
MANTIS:     Number of total right quadruples = 2^(11.207014)
           Probability of right quadruples  = 2^(-7.7930)
 RandPerm: Number of total right quadruples = 2^(3.321928)
            Probability of right quadruples = 2^(-15.6781)
 time on clock(): 0.25


[+] Proc 2
[+] PRNG initialized to 0xF113BEB8
[+] Time : 0.239844
[+] MANTIS:         Number of right quadruples = 2^(8.164907)
[+]           Probability of right quadruples = 2^(-7.8351)

[+] RandPerm:      Number of right quadruples = 2^(0.000000)
[+]           Probability of right quadruples = 2^(-16.0000)


[+] Proc 3
[+] PRNG initialized to 0xF113BEB9
[+] Time : 0.238484
[+] MANTIS:         Number of right quadruples = 2^(8.071462)
[+]           Probability of right quadruples = 2^(-7.9285)

[+] RandPerm:      Number of right quadruples = 2^(0.000000)
[+]           Probability of right quadruples = 2^(-16.0000)


[+] Proc 6
[+] PRNG initialized to 0xF113BEBC
[+] Time : 0.238071
[+] MANTIS:         Number of right quadruples = 2^(8.303781)
[+]           Probability of right quadruples = 2^(-7.6962)

[+] RandPerm:      Number of right quadruples = 2^(0.000000)
[+]           Probability of right quadruples = 2^(-16.0000)


[+] Proc 7
[+] PRNG initialized to 0xF113BEBD
[+] Time : 0.237936
[+] MANTIS:         Number of right quadruples = 2^(8.022368)
[+]           Probability of right quadruples = 2^(-7.9776)

[+] RandPerm:      Number of right quadruples = 2^(1.000000)
[+]           Probability of right quadruples = 2^(-15.0000)

[+] Time : 0.238200
[+] MANTIS:         Number of right quadruples = 2^(8.400879)
[+]           Probability of right quadruples = 2^(-7.5991)

[+] RandPerm:      Number of right quadruples = 2^(0.000000)
[+]           Probability of right quadruples = 2^(-16.0000)


[+] Proc 5
[+] PRNG initialized to 0xF113BEBB
[+] Time : 0.240682
[+] MANTIS:         Number of right quadruples = 2^(8.087463)
[+]           Probability of right quadruples = 2^(-7.9125)

[+] RandPerm:      Number of right quadruples = 2^(0.000000)
[+]           Probability of right quadruples = 2^(-16.0000)


[+] Proc 1
[+] PRNG initialized to 0xF113BEB7
[+] Time : 0.241979
[+] MANTIS:         Number of right quadruples = 2^(8.375039)
[+]           Probability of right quadruples = 2^(-7.6250)

[+] RandPerm:      Number of right quadruples = 2^(0.000000)
[+]           Probability of right quadruples = 2^(-16.0000)


[+] Proc 4
[+] PRNG initialized to 0xF113BEBA
[+] Time : 0.240845
[+] MANTIS:         Number of right quadruples = 2^(8.179909)
[+]           Probability of right quadruples = 2^(-7.8201)

[+] RandPerm:      Number of right quadruples = 2^(1.000000)
[+]           Probability of right quadruples = 2^(-15.0000)

```

