/*
 * main.c
 *
 * Author:
 * Ashrit Kankipati(akankip1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "apex_cpu.h"


int main(int argc, char const *argv[])
{   int cycles=0;
    APEX_CPU *cpu;
    int sim_fun=-1;
  if (argc <= 2)
    {
        fprintf(stderr, "APEX_Help: Incorrect number of arguments \n");
        exit(1);
    }
  
    //fprintf(stderr, "APEX CPU Pipeline Simulator v%0.1lf\n", VERSION);
    if(argc==4){ //single_step
      if((strcmp(argv[2], "fwd"))){
     fprintf(stderr, "APEX_Help: Incorrect input \n");
        exit(1);
  }
      sim_fun=0;
      cpu = APEX_cpu_init(argv[1]);
      
    if (!cpu)
    {
        fprintf(stderr, "APEX_Error: Unable to initialize CPU\n");
        exit(1);
    }
     if(!(strcmp(argv[3], "y"))){
      APEX_cpu_run_fwd(cpu,sim_fun,cycles);
      APEX_cpu_stop(cpu);
     } 
    else if(!(strcmp(argv[3], "n"))){
    APEX_cpu_run(cpu,sim_fun,cycles);
    APEX_cpu_stop(cpu);
       }
    }
    else if(argc==6){
      if((strcmp(argv[4], "fwd"))){
     fprintf(stderr, "APEX_Help: Incorrect input \n");
        exit(1);
  }
    //   if(!(strcmp(argv[2], "simulate")))   
    // sim_fun = 1; //set modes depending on the second command line argument
   if (!(strcmp(argv[2], "display"))) 
    sim_fun = 2;
  else if (!(strcmp(argv[2], "show_mem"))) 
    sim_fun = 3;
      cpu = APEX_cpu_init(argv[1]);
      
    if (!cpu)
    {
        fprintf(stderr, "APEX_Error: Unable to initialize CPU\n");
        exit(1);
    }
    //if(argc == 4){
    cycles = atoi(argv[3]);
  //  }
    if(!(strcmp(argv[5], "y"))){
      APEX_cpu_run_fwd(cpu,sim_fun,cycles);
      APEX_cpu_stop(cpu);
     } 
    else if(!(strcmp(argv[5], "n"))){
    APEX_cpu_run(cpu,sim_fun,cycles);
    APEX_cpu_stop(cpu);
       }
    
      
    }
  
  //printf("cycles input %d\n",cycles);
    

    
    return 0;
}