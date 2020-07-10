#ifndef PIC18SIM_H
#define PIC18SIM_H

#include <stdint.h>

#define status      fileRegisters[0xfd8]
#define wreg        fileRegisters[0xfe8]
#define bsr         fileRegisters[0xfe0]
#define pcl         fileRegisters[0xff9]

#define STATUS_C    (1)
#define STATUS_DC   (1 << 1)
#define STATUS_Z    (1 << 2)
#define STATUS_OV   (1 << 3)
#define STATUS_N    (1 << 4)


//InstructionExecutor is a pointer to function taking in
//(uint8_t *codePtr) returning pointer to uint8_t
//InstructionExecutor is an alias of uint8_t *(*InstructionExecutor)(uint8_t *codePtr)
typedef uint8_t *(*InstructionExecutor)(uint8_t *codePtr);

int add(int v1, int v2);
uint8_t *executeInstruction(uint8_t *codePtr);
uint8_t *addwf(uint8_t *codePtr);
uint8_t *incf(uint8_t *codePtr);
uint8_t *clrf(uint8_t *codePtr);
uint8_t *bsf(uint8_t *codePtr);
uint8_t *bcf(uint8_t *codePtr);
uint8_t *bn(uint8_t *codePtr);
uint8_t *bnz(uint8_t *codePtr);
uint8_t *bnov(uint8_t *codePtr);
uint8_t *movwf(uint8_t *codePtr);
uint8_t *andwf(uint8_t *codePtr);
uint8_t *rlcf(uint8_t *codePtr);

#endif // PIC18SIM_H
