/*
 * apex_cpu.c
 * Contains APEX cpu pipeline implementation
 *
 * Author:
 * Ashrit Kankipati (akankip1@binghamton.edu)
 * State University of New York at Binghamton
 */
#include "apex_cpu.h"
#include "apex_macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int ENABLE_DEBUG_MESSAGES = 1;
int flag_index = 0;
/* Converts the PC(4000 series) into array index for code memory
 *
 * Note: You are not supposed to edit this function
 */
static int get_code_memory_index_from_pc(const int pc) {
  return (pc - 4000) / 4;
}

static void print_instruction(const CPU_Stage *stage) {
  switch (stage->opcode) {
  case OPCODE_ADD:
  case OPCODE_SUB:
  case OPCODE_MUL:
  case OPCODE_DIV:
  case OPCODE_AND:
  case OPCODE_OR:
  case OPCODE_EXOR:
  case OPCODE_LDR: {
    printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
           stage->rs2);
    break;
  }
  case OPCODE_CMP: {
    printf("%s,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2);
    break;
  }

  case OPCODE_MOVC: {
    printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
    break;
  }

  case OPCODE_LOAD:
  case OPCODE_ADDL:
  case OPCODE_SUBL: {
    printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
           stage->imm);
    break;
  }

  case OPCODE_STORE: {
    printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
           stage->imm);
    break;
  }

  case OPCODE_BZ:
  case OPCODE_BNZ: {
    printf("%s,#%d ", stage->opcode_str, stage->imm);
    break;
  }

  case OPCODE_HALT: {
    printf("%s", stage->opcode_str);
    break;
  }
  case OPCODE_STR: {
    printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rs3, stage->rs1,
           stage->rs2);
    break;
  }
  }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void print_stage_content(const char *name, const CPU_Stage *stage) {
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction(stage);
  printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
int print_reg_file(APEX_CPU *cpu) {
  printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n");
  for (int i = 0; i < 16; i++) {
    if (cpu->regs[i] > 0) {
      printf("R%d:[%d]  ",i, cpu->regs[i]);
    } else {
      printf("R%d:[%d]  ",i, cpu->regs[i]);
    }
  }
  return 0;
}

int register_data_memory(APEX_CPU *cpu) {
  printf("\n============== STATE OF DATA MEMORY =============\n");
  for (int i = 0; i < 100; i++) {
    if (cpu->data_memory[i] > 0) {
      printf("MEM[%d]=%d\t", i,cpu->data_memory[i]);
    }
     
  }
  printf("\n");
  return 0;
}

void display(APEX_CPU *cpu) {
  print_reg_file(cpu);
  register_data_memory(cpu);
}

void show_mem(APEX_CPU *cpu, int cycles) {
  printf("\n============== STATE OF DATA MEMORY =============\n");

  printf("MEM[%d]=%d\n", cycles,cpu->data_memory[cycles]);
}
/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_fetch(APEX_CPU *cpu) {
  APEX_Instruction *current_ins;

  if (cpu->fetch.has_insn) {
    /* This fetches new branch target instruction from next cycle */
    if (cpu->fetch_from_next_cycle == TRUE) {
      cpu->fetch_from_next_cycle = FALSE;

      /* Skip this cycle*/
      return;
    }

    /* Store current PC in fetch latch */
    cpu->fetch.pc = cpu->pc;

    /* Index into code memory using this pc and copy all instruction fields
     * into fetch latch  */
    current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
    strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
    cpu->fetch.opcode = current_ins->opcode;
    cpu->fetch.rd = current_ins->rd;
  //  printf("decode.rd fetched is R%d\n",cpu->decode.rd);
  //  printf("current_ins.rd fetched is R%d\n",cpu->decode.rd);
  //  printf("fetch.rd fetched is R%d\n",cpu->fetch.rd);
  //  cpu->flags[cpu->fetch.rd] = 1;
    cpu->fetch.rs1 = current_ins->rs1;
    cpu->fetch.rs2 = current_ins->rs2;
    cpu->fetch.rs3 = current_ins->rs3;
    cpu->fetch.imm = current_ins->imm;
   // printf("in fetch flag %d\n",cpu->stalled_flag);

    /* Update PC for next instruction */

    /* Copy data from fetch latch to decode latch*/

   // if (cpu->stalled_flag == 1) {
      cpu->pc += 4;
      cpu->decode = cpu->fetch;
   // printf("store in fetch\n");
  //  }
    if (cpu->fetch.opcode == OPCODE_HALT) {
      cpu->fetch.has_insn = FALSE;
    }
    //}
    // cpu->fetch_from_next_cycle=TRUE;
    /* Stop fetching new instructions if HALT is fetched */

    // else
    //  cpu->fetch_from_next_cycle=FALSE;
    // cpu->fetch.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Fetch", &cpu->fetch);
   }

    // /* Stop fetching new instructions if HALT is fetched */
    // if (cpu->fetch.opcode == OPCODE_HALT)
    // {
    //     cpu->fetch.has_insn = FALSE;
    // }
  }
}

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_decode(APEX_CPU *cpu) {
  if (cpu->decode.has_insn) {

    /* Read operands from register file based on the instruction type */
    switch (cpu->decode.opcode) {
    case OPCODE_ADD:
    case OPCODE_AND:
    case OPCODE_OR:
    case OPCODE_EXOR:
    case OPCODE_SUB:
    case OPCODE_MUL:
    case OPCODE_DIV:
    case OPCODE_LDR:

    {
      if (cpu->flags[cpu->decode.rs1] > 0 || cpu->flags[cpu->decode.rs2] >0){
       // printf(" add stall flag in decode%d\n",cpu->stalled_flag);
      //  printf("flags[%d]=%d flags[%d]=%d",cpu->decode.rs1,cpu->flags[cpu->decode.rs1],cpu->decode.rs2,cpu->flags[cpu->decode.rs2]);
        cpu->stalled_flag = 0;
      }
      else{
      cpu->flags[cpu->decode.rd] += 1;
      cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
      cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
     // printf("flags[%d]=%d flags[%d]=%d\n",cpu->decode.rs1,cpu->flags[cpu->decode.rs1],cpu->decode.rs2,cpu->flags[cpu->decode.rs2]);
        }
      
      
        

      break;
    }
    case OPCODE_CMP:

    { cpu->flags[cpu->decode.rd]-=1;
      if (cpu->flags[cpu->decode.rs1] > 0 || cpu->flags[cpu->decode.rs2] >0 )
        cpu->stalled_flag = 0; 
      else{
      cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
      cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
     // printf("flags[%d]=%d flags[%d]=%d",cpu->decode.rs1,cpu->flags[cpu->decode.rs1],cpu->decode.rs2,cpu->flags[cpu->decode.rs2]);
        }
      

      break;
    }
    case OPCODE_STORE:

    { 
      if (cpu->flags[cpu->decode.rs1] > 0 || cpu->flags[cpu->decode.rs2] > 0)//{
        cpu->stalled_flag = 0;
      else{
      cpu->flags[cpu->decode.rd]-=1;
     // printf("store in decode\n");
      //printf("stall flag in decode%d\n",cpu->stalled_flag);
      cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
      cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
      // printf("flags[%d]=%d flags[%d]=%d",cpu->decode.rs1,cpu->flags[cpu->decode.rs1],cpu->decode.rs2,cpu->flags[cpu->decode.rs2]);
        }//else ends
        
      break;
    }
    case OPCODE_LOAD:
      { if (cpu->flags[cpu->decode.rs1] > 0)
         cpu->stalled_flag = 0;
        else{
        cpu->flags[cpu->decode.rd]+=1;
      
      cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
          // printf("flags[%d]=%d flags[%d]=%d\n",cpu->decode.rd,cpu->flags[cpu->decode.rd],cpu->decode.rs1,cpu->flags[cpu->decode.rs1]);
          }
      
       

      break;
    }
    case OPCODE_ADDL:
    case OPCODE_SUBL:

    { if (cpu->flags[cpu->decode.rs1] > 0)
        cpu->stalled_flag = 0;
     else{ cpu->flags[cpu->decode.rd]+=1;

      cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
       }

      break;
    }
    case OPCODE_MOVC: {
      cpu->flags[cpu->decode.rd] += 1;
      /* MOVC doesn't have register operands */
      break;
    }
    case OPCODE_NOP: {
      /* MOVC doesn't have register operands */
      break;
    }
    case OPCODE_STR: {
      // {
      // if(cpu->flags[cpu->decode.rs1]==1||cpu->flags[cpu->decode.rs2]==1||cpu->flags[cpu->decode.rs3]==1)
      // {cpu->stalled_flag=0;
      if (cpu->flags[cpu->decode.rs1] > 0 ||cpu->flags[cpu->decode.rs2] > 0 || cpu->flags[cpu->decode.rs3] >0)
        cpu->stalled_flag = 0;
      else{
      cpu->flags[cpu->decode.rd]+=1;
      cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
      cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
      cpu->decode.rs3_value = cpu->regs[cpu->decode.rs3];
      //printf("RS3%d",cpu->decode.rs3_value);
        }
      
      break;
    }
    }

    /* Copy data from decode latch to execute latch*/
    if (cpu->stalled_flag == 1) {
    //  printf("add\n");
      cpu->execute = cpu->decode;
      cpu->decode.has_insn = FALSE;
    } else {
      // cpu->decode.has_insn = TRUE;
    //  printf("add1\n");
      cpu->fetch_from_next_cycle = TRUE;
      return;
    }
    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Decode/RF", &cpu->decode);
    }
  }
}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_execute(APEX_CPU *cpu) {
  if (cpu->execute.has_insn) {
    /* Execute logic based on instruction type */
    switch (cpu->execute.opcode) {
    case OPCODE_ADD: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value + cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_STR: {
      cpu->execute.memory_address =
          cpu->execute.rs1_value + cpu->execute.rs2_value;
    //  printf("str sum%d",cpu->execute.memory_address);

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_AND: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value & cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_OR: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value | cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_EXOR: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value ^ cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_LDR: {
      cpu->execute.memory_address =
          cpu->execute.rs1_value + cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_SUB: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value - cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_CMP: {
      cpu->flags[cpu->execute.rd]=-1;
    //  printf("execute flag%d\n",cpu->flags[cpu->execute.rd]);
      cpu->execute.result_buffer =
          cpu->execute.rs1_value - cpu->execute.rs2_value;
     // printf("flags[%d]=%d flags[%d]=%d",cpu->execute.rs1,cpu->flags[cpu->execute.rs1],cpu->execute.rs2,cpu->flags[cpu->execute.rs2]);

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
        cpu->execute.result_buffer=0;
       // printf("zero flag set");
      }
      else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_MUL: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value * cpu->execute.rs2_value;
      //printf("flags[%d]=%d flags[%d]=%d\n",cpu->execute.rs1,cpu->flags[cpu->execute.rs1],cpu->execute.rs2,cpu->flags[cpu->execute.rs2]);
      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_DIV: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value / cpu->execute.rs2_value;

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }

    case OPCODE_LOAD: {
      cpu->execute.memory_address = cpu->execute.rs1_value + cpu->execute.imm;
      //cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
         //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->execute.rd,cpu->flags[cpu->execute.rd],cpu->execute.rs1,cpu->flags[cpu->execute.rs1]);
      break;
    }
    case OPCODE_STORE: {
      cpu->execute.memory_address = cpu->execute.rs2_value + cpu->execute.imm;
      break;
    }
    case OPCODE_ADDL: {
      cpu->execute.result_buffer = cpu->execute.rs1_value + cpu->execute.imm;
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }

      break;
    }
    case OPCODE_SUBL: {
      cpu->execute.result_buffer = cpu->execute.rs1_value - cpu->execute.imm;
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }

    case OPCODE_BZ: {
      if (cpu->zero_flag == TRUE) {
        /* Calculate new PC, and send it to fetch unit */
        cpu->pc = cpu->execute.pc + cpu->execute.imm;
        /* Since we are using reverse callbacks for pipeline stages,
         * this will prevent the new instruction from being fetched in the
         * current cycle*/
        cpu->fetch_from_next_cycle = TRUE;

        /* Flush previous stages */
        cpu->decode.has_insn = FALSE;

        /* Make sure fetch stage is enabled to start fetching from new PC */
        cpu->fetch.has_insn = TRUE;
      }
      break;
    }

    case OPCODE_BNZ: {
      if (cpu->zero_flag == FALSE) {
        /* Calculate new PC, and send it to fetch unit */
        cpu->pc = cpu->execute.pc + cpu->execute.imm;

        /* Since we are using reverse callbacks for pipeline stages,
         * this will prevent the new instruction from being fetched in the
         * current cycle*/
        cpu->fetch_from_next_cycle = TRUE;

        /* Flush previous stages */
        cpu->decode.has_insn = FALSE;

        /* Make sure fetch stage is enabled to start fetching from new PC */
        cpu->fetch.has_insn = TRUE;
      }
      break;
    }

    case OPCODE_MOVC: {
      cpu->execute.result_buffer = cpu->execute.imm;

      // /* Set the zero flag based on the result buffer */
      // if (cpu->execute.result_buffer == 0)
      // {
      //     cpu->zero_flag = TRUE;
      // }
      // else
      // {
      //     cpu->zero_flag = FALSE;
      // }
      break;
    }
    case OPCODE_NOP: {
      break;
    }
    }
    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Execute", &cpu->execute);
    }
    /* Copy data from execute latch to memory latch*/

    cpu->memory = cpu->execute;
    cpu->execute.has_insn = FALSE;
  }
}

/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_memory(APEX_CPU *cpu) {
  if (cpu->memory.has_insn) {
    switch (cpu->memory.opcode) {
    case OPCODE_ADD: {
      /* No work for ADD */
      break;
    }
    case OPCODE_MOVC: {
      /* No work for ADD */
      break;
    }
    case OPCODE_ADDL: {
      /* No work for ADDL */
      break;
    }
    case OPCODE_SUBL: {
      /* No work for SUBL */
      break;
    }
    case OPCODE_SUB: {
      /* No work for SUB */
      break;
    }
    case OPCODE_AND: {
      /* No work for SUB */
      break;
    }
    case OPCODE_OR: {
      /* No work for SUB */
      break;
    }
    case OPCODE_EXOR:
    case OPCODE_BNZ:
    case OPCODE_BZ: {
      /* No work for SUB */
      break;
    }
    case OPCODE_STORE:
      cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs1_value;
      break;

    case OPCODE_STR:
      cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs3_value;
     // printf("memory stage%d\n",cpu->data_memory[cpu->memory.memory_address]);
     // printf("RS3%d\n",cpu->memory.rs3_value);
      break;
    case OPCODE_MUL: {
      /* No work for MUL */
      break;
    }
    case OPCODE_DIV: {
      /* No work for DIV */
      break;
    }
    case OPCODE_NOP: {
      /* No work for DIV */
      break;
    }
    case OPCODE_CMP: {
      cpu->flags[cpu->memory.rd]=-1;
      /* No work for DIV */
     // printf("flags[%d]=%d flags[%d]=%d",cpu->memory.rs1,cpu->flags[cpu->memory.rs1],cpu->memory.rs2,cpu->flags[cpu->memory.rs2]);
      break;
    }

    case OPCODE_LOAD: {
      /* Read from data memory */
      cpu->memory.result_buffer = cpu->data_memory[cpu->memory.memory_address];
    //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->memory.rd,cpu->flags[cpu->memory.rd],cpu->memory.rs1,cpu->flags[cpu->memory.rs1]);
      // printf("R%d=\n",cpu->memory.result_buffer);
      break;
    }
    case OPCODE_LDR: {
      /* Read from data memory */
      cpu->memory.result_buffer = cpu->data_memory[cpu->memory.memory_address];
      break;
    }
    }

    /* Copy data from memory latch to writeback latch*/
    cpu->writeback = cpu->memory;
    cpu->memory.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content("Memory", &cpu->memory);
    }
  }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int APEX_writeback(APEX_CPU *cpu) {
  if (cpu->writeback.has_insn) {
    /* Write result to register file based on instruction type */
    switch (cpu->writeback.opcode) {
    case OPCODE_ADD:
    case OPCODE_AND:
    case OPCODE_OR:
    case OPCODE_EXOR:
    case OPCODE_SUB:
    case OPCODE_MUL:
    case OPCODE_DIV:    
    case OPCODE_ADDL:
    case OPCODE_SUBL:
    case OPCODE_MOVC: {
      cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
      cpu->flags[cpu->writeback.rd] -=1;
    //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->writeback.rs1,cpu->flags[cpu->writeback.rs1],cpu->writeback.rs2,cpu->flags[cpu->writeback.rs2]);
      // printf("flag array element %d",cpu->flags[cpu->writeback.rd]);
      cpu->stalled_flag = 1;
      break;
    }
    case OPCODE_STORE:{
     
      break;
    }
      case OPCODE_CMP:{
        cpu->flags[cpu->writeback.rd]-=1;
       // printf("flags[%d]=%d flags[%d]=%d",cpu->writeback.rs1,cpu->flags[cpu->writeback.rs1],cpu->writeback.rs2,cpu->flags[cpu->writeback.rs2]);
cpu->stalled_flag = 1;
      break;
    }
    case OPCODE_STR: {
      

      break;
    }
    case OPCODE_NOP:
    case OPCODE_BNZ:
    case OPCODE_BZ:

    {

      break;
    }
    case OPCODE_LOAD:
    case OPCODE_LDR:

    {
      cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;

      //     printf("in load wb R%d\n",cpu->writeback.rd);
      // printf("in load wb %d",cpu->flags[cpu->writeback.rd]);
      cpu->flags[cpu->writeback.rd]-= 1;
    //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->writeback.rd,cpu->flags[cpu->writeback.rd],cpu->writeback.rs1,cpu->flags[cpu->writeback.rs1]);
      // printf("in load wb %d\n",cpu->flags[cpu->writeback.rd]);
      cpu->stalled_flag = 1;
      break;
    }
    }

    cpu->insn_completed++;
    cpu->writeback.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES) {
      //printf("R%d=%d\n", cpu->writeback.rd,cpu->writeback.result_buffer);
      print_stage_content("Writeback", &cpu->writeback);
    }

    if (cpu->writeback.opcode == OPCODE_HALT) {
      /* Stop the APEX simulator */
      return TRUE;
    }
  }

  /* Default */
  return 0;
}

/*
 * This function creates and initializes APEX cpu.
 *
 * Note: You are free to edit this function according to your implementation
 */
APEX_CPU *APEX_cpu_init(const char *filename) {
 // int i;
  APEX_CPU *cpu;

  if (!filename) {
    return NULL;
  }

  cpu = calloc(1, sizeof(APEX_CPU));

  if (!cpu) {
    return NULL;
  }

  /* Initialize PC, Registers and all pipeline stages */
  cpu->pc = 4000;
  memset(cpu->regs, 0, sizeof(int) * REG_FILE_SIZE);
  memset(cpu->data_memory, 0, sizeof(int) * DATA_MEMORY_SIZE);
  cpu->single_step = ENABLE_SINGLE_STEP;

  /* Parse input file and create code memory */
  cpu->code_memory = create_code_memory(filename, &cpu->code_memory_size);
  if (!cpu->code_memory) {
    free(cpu);
    return NULL;
  }
  for (int i = 0; i < REG_FILE_SIZE; i++) {
    cpu->flags[i] = 0;
  }

  if (ENABLE_DEBUG_MESSAGES) {
    fprintf(stderr,
            "APEX_CPU: Initialized APEX CPU, loaded %d instructions\n",
            cpu->code_memory_size);
    fprintf(stderr, "APEX_CPU: PC initialized to %d\n", cpu->pc);
    fprintf(stderr, "APEX_CPU: Printing Code Memory\n");
    printf("%-9s %-9s %-9s %-9s %-9s\n", "opcode_str", "rd", "rs1", "rs2",
           "imm");

    for (int i = 0; i < cpu->code_memory_size; ++i)
    {
        printf("%-9s %-9d %-9d %-9d %-9d\n", cpu->code_memory[i].opcode_str,
               cpu->code_memory[i].rd, cpu->code_memory[i].rs1,
               cpu->code_memory[i].rs2, cpu->code_memory[i].imm);
    }
  }

  /* To start fetch stage */
  cpu->fetch.has_insn = TRUE;
  return cpu;
}

/*
 * APEX CPU simulation loop
 *
 * Note: You are free to edit this function according to your implementation
 */
void APEX_cpu_run(APEX_CPU *cpu, int sim_fun, int cycles) {
  cpu->stalled_flag = 1;
  char user_prompt_val[20];
  char array[200];
  if (sim_fun == 0) { // single step
    while (TRUE) {
      if (ENABLE_DEBUG_MESSAGES) {
        printf("-------------------------------------------------\n");
        printf("Clock Cycle #: %d\n", cpu->clock);
        printf("-------------------------------------------------\n");
      }

      if (APEX_writeback(cpu)) {
        /* Halt in writeback stage */
        printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n",
               cpu->clock, cpu->insn_completed);
        break;
      }

      APEX_memory(cpu);

      APEX_execute(cpu);

      APEX_decode(cpu);
      APEX_fetch(cpu);
      // print_reg_file(cpu);
      printf("zero flag= %d\n",cpu->zero_flag);
      

      printf("Press any key to advance CPU Clock or <exit> to quit:\n");
      fgets(user_prompt_val, 20, stdin);
      strcpy(array, user_prompt_val);
     // printf("string entered is %s",array);
     

      if(!(strcmp(array,"exit\n"))) {
        
        printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n",
               cpu->clock, cpu->insn_completed);        
        break;
      }

      cpu->clock++;
    }
    display(cpu);
  } else {

    if (sim_fun == 1) { // simulate
      // printf("cycles entered %d\n",cycles);
      ENABLE_DEBUG_MESSAGES = 0;
      while (cpu->clock < cycles) {
        if (ENABLE_DEBUG_MESSAGES) {
          printf("-------------------------------------------------\n");
        printf("Clock Cycle #: %d\n", cpu->clock);
        printf("-------------------------------------------------\n");
        }
        if (APEX_writeback(cpu)) {
          /* Halt in writeback stage */
          // printf("APEX_CPU: Simulation Complete, cycles = %d instructions =
          // %d\n", cpu->clock, cpu->insn_completed);
          break;
        }

        APEX_memory(cpu);
        APEX_execute(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);

        cpu->clock++;

      } // while ends
      display(cpu);
    } // if ends

    else if (sim_fun == 2) {
      // display
      while (cpu->clock < cycles) {

        if (ENABLE_DEBUG_MESSAGES) {
          printf("-------------------------------------------------\n");
        printf("Clock Cycle #: %d\n", cpu->clock);
        printf("-------------------------------------------------\n");
        }
         
       //  printf("R0 val is %d\n",cpu->regs[0]);
        if (APEX_writeback(cpu)) {
          /* Halt in writeback stage */
          printf(
              "APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n",
              cpu->clock, cpu->insn_completed);
          break;
        }

        APEX_memory(cpu);
        APEX_execute(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);
       // printf(" zero flag= %d\n",cpu->zero_flag);
     
        cpu->clock++;
      } // while ends
          display(cpu);  
        // printf("clock ran for %d\n",cpu->clock);
    } // if ends
    else if (sim_fun == 3) {
       ENABLE_DEBUG_MESSAGES=0;
      while (TRUE) {
        if (APEX_writeback(cpu)) {
          /* Halt in writeback stage */
          // printf("APEX_CPU: Simulation Complete, cycles = %d instructions =
          // %d\n", cpu->clock, cpu->insn_completed);
          break;
        }

        APEX_memory(cpu);
        APEX_execute(cpu);
        APEX_decode(cpu);
        APEX_fetch(cpu);

        cpu->clock++;
      }

      show_mem(cpu, cycles);
    }
  } // else ends
} // function ends

/*
 * This function deallocates APEX CPU.
 *
 * Note: You are free to edit this function according to your implementation
 */
void APEX_cpu_stop(APEX_CPU *cpu) {
  free(cpu->code_memory);
  free(cpu);
}
//
//
//Data forwarding:
//
//
static void print_instruction_fwd(const CPU_Stage *stage) {
  switch (stage->opcode) {
  case OPCODE_ADD:
  case OPCODE_SUB:
  case OPCODE_MUL:
  case OPCODE_DIV:
  case OPCODE_AND:
  case OPCODE_OR:
  case OPCODE_EXOR:
  case OPCODE_LDR: {
    printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rd, stage->rs1,
           stage->rs2);
    break;
  }
  case OPCODE_CMP: {
    printf("%s,R%d,R%d ", stage->opcode_str, stage->rs1, stage->rs2);
    break;
  }

  case OPCODE_MOVC: {
    printf("%s,R%d,#%d ", stage->opcode_str, stage->rd, stage->imm);
    break;
  }

  case OPCODE_LOAD:
  case OPCODE_ADDL:
  case OPCODE_SUBL: {
    printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rd, stage->rs1,
           stage->imm);
    break;
  }

  case OPCODE_STORE: {
    printf("%s,R%d,R%d,#%d ", stage->opcode_str, stage->rs1, stage->rs2,
           stage->imm);
    break;
  }

  case OPCODE_BZ:
  case OPCODE_BNZ: {
    printf("%s,#%d ", stage->opcode_str, stage->imm);
    break;
  }

  case OPCODE_HALT: {
    printf("%s", stage->opcode_str);
    break;
  }
  case OPCODE_STR: {
    printf("%s,R%d,R%d,R%d ", stage->opcode_str, stage->rs3, stage->rs1,
           stage->rs2);
    break;
  }
  }
}

/* Debug function which prints the CPU stage content
 *
 * Note: You can edit this function to print in more detail
 */
static void print_stage_content_fwd(const char *name, const CPU_Stage *stage) {
  printf("%-15s: pc(%d) ", name, stage->pc);
  print_instruction_fwd(stage);
  printf("\n");
}

/* Debug function which prints the register file
 *
 * Note: You are not supposed to edit this function
 */
int print_reg_file_fwd(APEX_CPU *cpu) {
  printf("=============== STATE OF ARCHITECTURAL REGISTER FILE ==========\n");
  for (int i = 0; i < 16; i++) {
    if (cpu->regs[i] > 0) {
      printf("R%d:[%d]  ",i, cpu->regs[i]);
    } 
    else {
      printf("R%d:[%d]  ",i, cpu->regs[i]);
    }
  }
  return 0;
}

int register_data_memory_fwd(APEX_CPU *cpu) {
  printf("\n============== STATE OF DATA MEMORY =============\n");
  for (int i = 0; i < 100; i++) {
    if (cpu->data_memory[i] > 0) {
      printf("MEM[%d]=%d\t", i,cpu->data_memory[i]);
    }
     
  }
  printf("\n");
  return 0;
}

void display_fwd(APEX_CPU *cpu) {
  print_reg_file_fwd(cpu);
  register_data_memory_fwd(cpu);
}

void show_mem_fwd(APEX_CPU *cpu, int cycles) {
  printf("\n============== STATE OF DATA MEMORY =============\n");
  printf("MEM[%d]=%d\n", cycles,cpu->data_memory[cycles]);
}
/*
 * Fetch Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_fetch_fwd(APEX_CPU *cpu) {
  APEX_Instruction *current_ins;

  if (cpu->fetch.has_insn) {
    /* This fetches new branch target instruction from next cycle */
    if (cpu->fetch_from_next_cycle == TRUE) {
      cpu->fetch_from_next_cycle = FALSE;

      /* Skip this cycle*/
      return;
    }

    /* Store current PC in fetch latch */
    cpu->fetch.pc = cpu->pc;

    /* Index into code memory using this pc and copy all instruction fields
     * into fetch latch  */
    current_ins = &cpu->code_memory[get_code_memory_index_from_pc(cpu->pc)];
    strcpy(cpu->fetch.opcode_str, current_ins->opcode_str);
    cpu->fetch.opcode = current_ins->opcode;
    cpu->fetch.rd = current_ins->rd;
  //  printf("decode.rd fetched is R%d\n",cpu->decode.rd);
  //  printf("current_ins.rd fetched is R%d\n",cpu->decode.rd);
  //  printf("fetch.rd fetched is R%d\n",cpu->fetch.rd);
  //  cpu->flags[cpu->fetch.rd] = 1;
    cpu->fetch.rs1 = current_ins->rs1;
    cpu->fetch.rs2 = current_ins->rs2;
    cpu->fetch.rs3 = current_ins->rs3;
    cpu->fetch.imm = current_ins->imm;
   // printf("in fetch flag %d\n",cpu->stalled_flag);

    /* Update PC for next instruction */

    /* Copy data from fetch latch to decode latch*/

   // if (cpu->stalled_flag == 1) {
      cpu->pc += 4;
      cpu->decode = cpu->fetch;
   // printf("store in fetch\n");
  //  }
    if (cpu->fetch.opcode == OPCODE_HALT) {
      cpu->fetch.has_insn = FALSE;
    }
    //}
    // cpu->fetch_from_next_cycle=TRUE;
    /* Stop fetching new instructions if HALT is fetched */

    // else
    //  cpu->fetch_from_next_cycle=FALSE;
    // cpu->fetch.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content_fwd("Fetch", &cpu->fetch);
   }

    // /* Stop fetching new instructions if HALT is fetched */
    // if (cpu->fetch.opcode == OPCODE_HALT)
    // {
    //     cpu->fetch.has_insn = FALSE;
    // }
  }
}

/*
 * Decode Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_decode_fwd(APEX_CPU *cpu) {
  if (cpu->decode.has_insn) {
   // printf("fwd[%d]=%d\n",cpu->execute.rd,cpu->fwd[cpu->execute.rd]);
     //   
    /* Read operands from register file based on the instruction type */
    switch (cpu->decode.opcode) {
    case OPCODE_ADD:
    case OPCODE_AND:
    case OPCODE_OR:
    case OPCODE_EXOR:
    case OPCODE_SUB:
    case OPCODE_MUL:
    case OPCODE_DIV:
    case OPCODE_LDR:

    {
      if (cpu->flags[cpu->decode.rs1] > 0 || cpu->flags[cpu->decode.rs2] >0){
       // printf(" add stall flag in decode%d\n",cpu->stalled_flag);
      //  printf("flags[%d]=%d flags[%d]=%d",cpu->decode.rs1,cpu->flags[cpu->decode.rs1],cpu->decode.rs2,cpu->flags[cpu->decode.rs2]);
        cpu->stalled_flag = 0;
      }
      else{
      cpu->flags[cpu->decode.rd] += 1;
        if(cpu->fwd[cpu->decode.rs1]>-10000){
      cpu->decode.rs1_value = cpu->fwd[cpu->decode.rs1];
        //  cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
          }
        else{
           cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
        }
         if(cpu->fwd[cpu->decode.rs2]>-1000){
      cpu->decode.rs2_value = cpu->fwd[cpu->decode.rs2];
        //  cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
          }
        else{
         
          cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
        }
     // printf("flags[%d]=%d flags[%d]=%d\n",cpu->decode.rs1,cpu->flags[cpu->decode.rs1],cpu->decode.rs2,cpu->flags[cpu->decode.rs2]);
        
       
        }
      
        

      break;
    }
    case OPCODE_CMP:

    { cpu->flags[cpu->decode.rd]-=1;
      if (cpu->flags[cpu->decode.rs1] > 0 || cpu->flags[cpu->decode.rs2] >0 )
        cpu->stalled_flag = 0; 
      else{
        if(cpu->fwd[cpu->decode.rs1]>-10000){
      cpu->decode.rs1_value = cpu->fwd[cpu->decode.rs1];
          }
        else{
          cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
        }
      if(cpu->fwd[cpu->decode.rs2]>-10000){
      cpu->decode.rs2_value = cpu->fwd[cpu->decode.rs2];
        }
       else{
          
          cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
        } 
        }

      break;
    }
    case OPCODE_STORE:

    { 
      if (cpu->flags[cpu->decode.rs1] > 0 || cpu->flags[cpu->decode.rs2] > 0)//{
        cpu->stalled_flag = 0;
      else{
      cpu->flags[cpu->decode.rd]-=1;
     // printf("store in decode\n");
      //printf("stall flag in decode%d\n",cpu->stalled_flag);
        if(cpu->fwd[cpu->decode.rs1]>-10000){
      cpu->decode.rs1_value = cpu->fwd[cpu->decode.rs1];
          }
        else{
           cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
        }
      if(cpu->fwd[cpu->decode.rs2]>-10000){
      cpu->decode.rs2_value = cpu->fwd[cpu->decode.rs2];
        }
       else{
         
          cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
        } 
        }//else ends
        
      break;
    }
    case OPCODE_LOAD:
      { if (cpu->flags[cpu->decode.rs1] > 0)
         cpu->stalled_flag = 0;
        else{
        cpu->flags[cpu->decode.rd]+=1;
      if(cpu->fwd[cpu->decode.rs1]>-10000){
      cpu->decode.rs1_value = cpu->fwd[cpu->decode.rs1];
          }
      // if(cpu->fwd[cpu->decode.rs2]>-10000){
      // cpu->decode.rs2_value = cpu->fwd[cpu->decode.rs2];
      //   }
       else{
          cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
        //  cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
        } 
          }
       

      break;
    }
    case OPCODE_ADDL:
    case OPCODE_SUBL:

    { if (cpu->flags[cpu->decode.rs1] > 0)
        cpu->stalled_flag = 0;
     else{ 
      // if(cpu->fwd[cpu->decode.rd]>-10000){
       cpu->flags[cpu->decode.rd]+=1;
       if(cpu->fwd[cpu->decode.rs1]>-10000){
      cpu->decode.rs1_value = cpu->fwd[cpu->decode.rs1];
         }
     //  }
       else{
         cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
       }
      // printf("fwd[%d]=%d\n",cpu->execute.rd,cpu->fwd[cpu->decode.rd]);
       }

      break;
    }
    case OPCODE_MOVC: {
      cpu->flags[cpu->decode.rd] += 1;
      
      /* MOVC doesn't have register operands */
      break;
    }
    case OPCODE_NOP: {
      /* MOVC doesn't have register operands */
      break;
    }
    case OPCODE_STR: {
      // {
      // if(cpu->flags[cpu->decode.rs1]==1||cpu->flags[cpu->decode.rs2]==1||cpu->flags[cpu->decode.rs3]==1)
      // {cpu->stalled_flag=0;
      if (cpu->flags[cpu->decode.rs1] > 0 ||cpu->flags[cpu->decode.rs2] > 0 || cpu->flags[cpu->decode.rs3] >0)
        cpu->stalled_flag = 0;
      else{
      cpu->flags[cpu->decode.rd]+=1;
        if(cpu->fwd[cpu->decode.rs1]>-10000){
      cpu->decode.rs1_value = cpu->fwd[cpu->decode.rs1];
          }
        else{
          cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
        }
      if(cpu->fwd[cpu->decode.rs2]>-10000){
      cpu->decode.rs2_value = cpu->fwd[cpu->decode.rs2];
        }
        else{
          cpu->decode.rs2_value = cpu->regs[cpu->decode.rs2];
        }
        if(cpu->fwd[cpu->decode.rs3]>-10000){
      cpu->decode.rs3_value = cpu->fwd[cpu->decode.rs3];
        }
       else{       
          
          cpu->decode.rs3_value = cpu->fwd[cpu->decode.rs3];
        } 
        }
      break;
    }
    }

    /* Copy data from decode latch to execute latch*/
    if (cpu->stalled_flag == 1) {
    //  printf("add\n");
      cpu->execute = cpu->decode;
      cpu->decode.has_insn = FALSE;
    } else {
      // cpu->decode.has_insn = TRUE;
    //  printf("add1\n");
      cpu->fetch_from_next_cycle = TRUE;
      return;
    }
    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content_fwd("Decode/RF", &cpu->decode);
    }
  }
}

/*
 * Execute Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_execute_fwd(APEX_CPU *cpu) {
  if (cpu->execute.has_insn) {
    /* Execute logic based on instruction type */
    switch (cpu->execute.opcode) {
    case OPCODE_ADD: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value + cpu->execute.rs2_value;
    if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_STR: {
      cpu->execute.memory_address =
          cpu->execute.rs1_value + cpu->execute.rs2_value;
    //  printf("str sum%d",cpu->execute.memory_address);

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_AND: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value & cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_OR: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value | cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_EXOR: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value ^ cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_LDR: {
      cpu->execute.memory_address =
          cpu->execute.rs1_value + cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->decode.rd]=cpu->execute.memory_address;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */

      break;
    }
    case OPCODE_SUB: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value - cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_CMP: {
      cpu->flags[cpu->execute.rd]-=1;
    //  printf("execute flag%d\n",cpu->flags[cpu->execute.rd]);
      cpu->execute.result_buffer =
          cpu->execute.rs1_value - cpu->execute.rs2_value;
     // printf("flags[%d]=%d flags[%d]=%d",cpu->execute.rs1,cpu->flags[cpu->execute.rs1],cpu->execute.rs2,cpu->flags[cpu->execute.rs2]);

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
        cpu->execute.result_buffer=0;
       // printf("zero flag set");
      }
      else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_MUL: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value * cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }
      //printf("flags[%d]=%d flags[%d]=%d\n",cpu->execute.rs1,cpu->flags[cpu->execute.rs1],cpu->execute.rs2,cpu->flags[cpu->execute.rs2]);
      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }
    case OPCODE_DIV: {
      cpu->execute.result_buffer =
          cpu->execute.rs1_value / cpu->execute.rs2_value;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }

      /* Set the zero flag based on the result buffer */
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }

    case OPCODE_LOAD: {
      cpu->execute.memory_address = cpu->execute.rs1_value + cpu->execute.imm;
      
      //cpu->decode.rs1_value = cpu->regs[cpu->decode.rs1];
         //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->execute.rd,cpu->flags[cpu->execute.rd],cpu->execute.rs1,cpu->flags[cpu->execute.rs1]);
      break;
    }
    case OPCODE_STORE: {
      cpu->execute.memory_address = cpu->execute.rs2_value + cpu->execute.imm;
      
      break;
    }
    case OPCODE_ADDL: {
      cpu->execute.result_buffer = cpu->execute.rs1_value + cpu->execute.imm;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }

      break;
    }
    case OPCODE_SUBL: {
      cpu->execute.result_buffer = cpu->execute.rs1_value - cpu->execute.imm;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }
      if (cpu->execute.result_buffer == 0) {
        cpu->zero_flag = TRUE;
      } else {
        cpu->zero_flag = FALSE;
      }
      break;
    }

    case OPCODE_BZ: {
      if (cpu->zero_flag == TRUE) {
        /* Calculate new PC, and send it to fetch unit */
        cpu->pc = cpu->execute.pc + cpu->execute.imm;
        /* Since we are using reverse callbacks for pipeline stages,
         * this will prevent the new instruction from being fetched in the
         * current cycle*/
        cpu->fetch_from_next_cycle = TRUE;

        /* Flush previous stages */
        cpu->decode.has_insn = FALSE;

        /* Make sure fetch stage is enabled to start fetching from new PC */
        cpu->fetch.has_insn = TRUE;
      }
      break;
    }

    case OPCODE_BNZ: {
      if (cpu->zero_flag == FALSE) {
        /* Calculate new PC, and send it to fetch unit */
        cpu->pc = cpu->execute.pc + cpu->execute.imm;

        /* Since we are using reverse callbacks for pipeline stages,
         * this will prevent the new instruction from being fetched in the
         * current cycle*/
        cpu->fetch_from_next_cycle = TRUE;

        /* Flush previous stages */
        cpu->decode.has_insn = FALSE;

        /* Make sure fetch stage is enabled to start fetching from new PC */
        cpu->fetch.has_insn = TRUE;
      }
      break;
    }

    case OPCODE_MOVC: {
      cpu->execute.result_buffer = cpu->execute.imm;
      if(cpu->flags[cpu->execute.rd]>0){
      cpu->flags[cpu->execute.rd] -= 1;
      cpu->fwd[cpu->execute.rd]=cpu->execute.result_buffer;
        cpu->stalled_flag=1;
        }
      

      // /* Set the zero flag based on the result buffer */
      // if (cpu->execute.result_buffer == 0)
      // {
      //     cpu->zero_flag = TRUE;
      // }
      // else
      // {
      //     cpu->zero_flag = FALSE;
      // }
      break;
    }
    case OPCODE_NOP: {
      break;
    }
    }
    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content_fwd("Execute", &cpu->execute);
    }
    /* Copy data from execute latch to memory latch*/

    cpu->memory = cpu->execute;
    cpu->execute.has_insn = FALSE;
  }
}

/*
 * Memory Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static void APEX_memory_fwd(APEX_CPU *cpu) {
  if (cpu->memory.has_insn) {
    switch (cpu->memory.opcode) {
    case OPCODE_ADD: {
      /* No work for ADD */
      break;
    }
    case OPCODE_MOVC: {
      /* No work for ADD */
      break;
    }
    case OPCODE_ADDL: {
      /* No work for ADDL */
      break;
    }
    case OPCODE_SUBL: {
      /* No work for SUBL */
      break;
    }
    case OPCODE_SUB: {
      /* No work for SUB */
      break;
    }
    case OPCODE_AND: {
      /* No work for SUB */
      break;
    }
    case OPCODE_OR: {
      /* No work for SUB */
      break;
    }
    case OPCODE_EXOR:
    case OPCODE_BNZ:
    case OPCODE_BZ: {
      /* No work for SUB */
      break;
    }
    case OPCODE_STORE:
      cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs1_value;
      break;

    case OPCODE_STR:
      cpu->data_memory[cpu->memory.memory_address] = cpu->memory.rs3_value;
     // printf("memory stage%d\n",cpu->data_memory[cpu->memory.memory_address]);
     // printf("RS3%d\n",cpu->memory.rs3_value);
      break;
    case OPCODE_MUL: {
      /* No work for MUL */
      break;
    }
    case OPCODE_DIV: {
      /* No work for DIV */
      break;
    }
    case OPCODE_NOP: {
      /* No work for DIV */
      break;
    }
    case OPCODE_CMP: {
      cpu->flags[cpu->memory.rd]=-1;
      /* No work for DIV */
     // printf("flags[%d]=%d flags[%d]=%d",cpu->memory.rs1,cpu->flags[cpu->memory.rs1],cpu->memory.rs2,cpu->flags[cpu->memory.rs2]);
      break;
    }

    case OPCODE_LOAD: {
      /* Read from data memory */
      cpu->memory.result_buffer = cpu->data_memory[cpu->memory.memory_address];
      if(cpu->flags[cpu->memory.rd]>0){
      cpu->flags[cpu->memory.rd] -= 1;
      cpu->fwd[cpu->memory.rd]=cpu->memory.result_buffer;
        cpu->stalled_flag=1;
        }
    //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->memory.rd,cpu->flags[cpu->memory.rd],cpu->memory.rs1,cpu->flags[cpu->memory.rs1]);
      // printf("R%d=\n",cpu->memory.result_buffer);
      break;
    }
    case OPCODE_LDR: {
      /* Read from data memory */
      cpu->memory.result_buffer = cpu->data_memory[cpu->memory.memory_address];
      if(cpu->flags[cpu->memory.rd]>0){
      cpu->flags[cpu->memory.rd] -= 1;
      cpu->fwd[cpu->memory.rd]=cpu->memory.result_buffer;
        cpu->stalled_flag=1;
        }
      break;
    }
    }

    /* Copy data from memory latch to writeback latch*/
    cpu->writeback = cpu->memory;
    cpu->memory.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES) {
      print_stage_content_fwd("Memory", &cpu->memory);
    }
  }
}

/*
 * Writeback Stage of APEX Pipeline
 *
 * Note: You are free to edit this function according to your implementation
 */
static int APEX_writeback_fwd(APEX_CPU *cpu) {
  if (cpu->writeback.has_insn) {
    /* Write result to register file based on instruction type */
    switch (cpu->writeback.opcode) {
    case OPCODE_ADD:
    case OPCODE_AND:
    case OPCODE_OR:
    case OPCODE_EXOR:
    case OPCODE_SUB:
    case OPCODE_MUL:
    case OPCODE_DIV:    
    case OPCODE_ADDL:
    case OPCODE_SUBL:
    case OPCODE_MOVC: {
      cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;
     // cpu->flags[cpu->writeback.rd] -=1;
    //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->writeback.rs1,cpu->flags[cpu->writeback.rs1],cpu->writeback.rs2,cpu->flags[cpu->writeback.rs2]);
      // printf("flag array element %d",cpu->flags[cpu->writeback.rd]);
    //  cpu->stalled_flag = 1;
      break;
    }
    case OPCODE_STORE:{
     
      break;
    }
      case OPCODE_CMP:{
       // cpu->flags[cpu->writeback.rd]-=1;
       // printf("flags[%d]=%d flags[%d]=%d",cpu->writeback.rs1,cpu->flags[cpu->writeback.rs1],cpu->writeback.rs2,cpu->flags[cpu->writeback.rs2]);
//cpu->stalled_flag = 1;
      break;
    }
    case OPCODE_STR: {
      

      break;
    }
    case OPCODE_NOP:
    case OPCODE_BNZ:
    case OPCODE_BZ:

    {

      break;
    }
    case OPCODE_LOAD:
    case OPCODE_LDR:

    {
      cpu->regs[cpu->writeback.rd] = cpu->writeback.result_buffer;

      //     printf("in load wb R%d\n",cpu->writeback.rd);
      // printf("in load wb %d",cpu->flags[cpu->writeback.rd]);
    //  cpu->flags[cpu->writeback.rd]-= 1;
    //  printf("flags[%d]=%d flags[%d]=%d\n",cpu->writeback.rd,cpu->flags[cpu->writeback.rd],cpu->writeback.rs1,cpu->flags[cpu->writeback.rs1]);
      // printf("in load wb %d\n",cpu->flags[cpu->writeback.rd]);
     // cpu->stalled_flag = 1;
      break;
    }
    }

    cpu->insn_completed++;
    cpu->writeback.has_insn = FALSE;

    if (ENABLE_DEBUG_MESSAGES) {
      //printf("R%d=%d\n", cpu->writeback.rd,cpu->writeback.result_buffer);
      print_stage_content_fwd("Writeback", &cpu->writeback);
    }

    if (cpu->writeback.opcode == OPCODE_HALT) {
      /* Stop the APEX simulator */
      return TRUE;
    }
  }

  /* Default */
  return 0;
}
void APEX_cpu_run_fwd(APEX_CPU *cpu, int sim_fun, int cycles) {
  cpu->stalled_flag = 1;
  char user_prompt_val[20];
  char array[200];
  if (sim_fun == 0) { // single step
    while (TRUE) {
      if (ENABLE_DEBUG_MESSAGES) {
        printf("-------------------------------------------------\n");
        printf("Clock Cycle #: %d\n", cpu->clock);
        printf("-------------------------------------------------\n");
      }

      if (APEX_writeback_fwd(cpu)) {
        /* Halt in writeback stage */
        printf("APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n",
               cpu->clock, cpu->insn_completed);
        break;
      }

      APEX_memory_fwd(cpu);

      APEX_execute_fwd(cpu);

      APEX_decode_fwd(cpu);
      APEX_fetch_fwd(cpu);
      // print_reg_file_fwd(cpu);
      printf("zero flag= %d\n",cpu->zero_flag);
      

      printf("Press any key to advance CPU Clock or <exit> to quit:\n");
      fgets(user_prompt_val, 20, stdin);
      strcpy(array, user_prompt_val);
     // printf("string entered is %s",array);
     

      if(!(strcmp(array,"exit\n"))) {
        
        printf("APEX_CPU: Simulation Stopped, cycles = %d instructions = %d\n",
               cpu->clock, cpu->insn_completed);        
        break;
      }

      cpu->clock++;
    }
    display_fwd(cpu);
  } else {

    if (sim_fun == 1) { // simulate
      // printf("cycles entered %d\n",cycles);
      ENABLE_DEBUG_MESSAGES = 0;
      while (cpu->clock < cycles) {
        if (ENABLE_DEBUG_MESSAGES) {
          printf("-------------------------------------------------\n");
        printf("Clock Cycle #: %d\n", cpu->clock);
        printf("-------------------------------------------------\n");
        }
        if (APEX_writeback_fwd(cpu)) {
          /* Halt in writeback stage */
          // printf("APEX_CPU: Simulation Complete, cycles = %d instructions =
          // %d\n", cpu->clock, cpu->insn_completed);
          break;
        }

        APEX_memory_fwd(cpu);
        APEX_execute_fwd(cpu);
        APEX_decode_fwd(cpu);
        APEX_fetch_fwd(cpu);

        cpu->clock++;

      } // while ends
      display_fwd(cpu);
    } // if ends

    else if (sim_fun == 2) {
      // display_fwd
      while (cpu->clock < cycles) {

        if (ENABLE_DEBUG_MESSAGES) {
          printf("-------------------------------------------------\n");
        printf("Clock Cycle #: %d\n", cpu->clock);
        printf("-------------------------------------------------\n");
        }
         
       //  printf("R0 val is %d\n",cpu->regs[0]);
        if (APEX_writeback_fwd(cpu)) {
          /* Halt in writeback stage */
          printf(
              "APEX_CPU: Simulation Complete, cycles = %d instructions = %d\n",
              cpu->clock, cpu->insn_completed);
          break;
        }

        APEX_memory_fwd(cpu);
        APEX_execute_fwd(cpu);
        APEX_decode_fwd(cpu);
        APEX_fetch_fwd(cpu);
       // printf(" zero flag= %d\n",cpu->zero_flag);
     
        cpu->clock++;
      } // while ends
          display_fwd(cpu);  
        // printf("clock ran for %d\n",cpu->clock);
    } // if ends
    else if (sim_fun == 3) {
       ENABLE_DEBUG_MESSAGES=0;
      while (TRUE) {
        if (APEX_writeback_fwd(cpu)) {
          /* Halt in writeback stage */
          // printf("APEX_CPU: Simulation Complete, cycles = %d instructions =
          // %d\n", cpu->clock, cpu->insn_completed);
          break;
        }

        APEX_memory_fwd(cpu);
        APEX_execute_fwd(cpu);
        APEX_decode_fwd(cpu);
        APEX_fetch_fwd(cpu);

        cpu->clock++;
      }

      show_mem_fwd(cpu, cycles);
    }
  } // else ends
} // function ends