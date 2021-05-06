Compilating and run example:

hemmelig@Abel:~/Documents/in4200/IN4200_HE2_15836$ mpicc test.c -O3
Hemmelig@Abel:~/Documents/in4200/IN4200_HE2_15836$ mpirun --use-hwthread-cpus -np 4 ./a.out 10000 10000 10 10
parallel:  12.331511 s
seriall:  29.849816 s
speedup:  2.420613


inputexplenation:                                                                             M     N   K1 K2
                                                                                              |     |    |  |
Hemmelig@Abel:~/Documents/in4200/IN4200_HE2_15836$ mpirun --use-hwthread-cpus -np 4 ./a.out 10000 10000 10 10