#ifndef PIC18SIM_H
#define PIC18SIM_H

#include <stdint.h>

typedef struct _21bitInteger  _21bitInteger;
struct _21bitInteger {
  int value:21;
};

#define status      fileRegisters[0xfd8]
#define wreg        fileRegisters[0xfe8]
#define bsr         fileRegisters[0xfe0]
// pcl = 0xff9, pclath = 0xffa, pclatu = 0xffb
#define pc          ((_21bitInteger *)&fileRegisters[0xff9])->value

#define STATUS_C    (1)
#define STATUS_DC   (1 << 1)
#define STATUS_Z    (1 << 2)
#define STATUS_OV   (1 << 3)
#define STATUS_N    (1 << 4)

extern uint8_t fileRegisters[];
extern uint8_t codeMemory[];

//InstructionExecutor is a pointer to function taking in no arg
typedef void(*InstructionExecutor)(void);

int add(int v1, int v2);

void executeInstruction();
void addwf();
void addwfc();
void incf();
void decf();
void clrf();
void bsf();
void bcf();
void bn();
void bc();
void bnz();
void bnov();
void movwf();
void andwf();
void rlcf();

#endif // PIC18SIM_H
