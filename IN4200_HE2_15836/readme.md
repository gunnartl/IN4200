Compilating and run example:

Hemmelig@Abel:~/Documents/in4200/IN4200_HE2_15836$ mpicc MPI_main.c -O3
Hemmelig@Abel:~/Documents/in4200/IN4200_HE2_15836$ mpirun --use-hwthread-cpus -np 4 ./a.out 10000 1000 100 100
parallel:  94.331616 s
serial:  292.808679 s
speedup:  3.104035
Double convolution executed successfully 




inputexplenation:                                                                             M     N   K1 K2
                                                                                              |     |    |  |
Hemmelig@Abel:~/Documents/in4200/IN4200_HE2_15836$ mpirun --use-hwthread-cpus -np 4 ./a.out 10000 1000 100 100


Beeing that we can assume K1 and K2 << than M and P i have not made any safeguards for dealing with wierd choises of procs, and viarables M,N,K1 and K2. So the program will fail with for instance huge filters on a small matrix

I ahve also made more *.c files than specified in the exam. this is because i made functions of allocating and deallocating and the convolution-operation itself. I found this to be cleaner.


i have had trouble running on the ifi-computers but it runs on my laptop with these versions: 

gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
mpirun (Open MPI) 4.0.3
mpiexec (OpenRTE) 4.0.3
