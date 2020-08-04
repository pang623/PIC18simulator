#include "PIC18sim.h"
#include  <stdint.h>

InstructionExecutor pic18ExecutionTable[256] = {
 [0x04] = decf, decf, decf, decf,
 [0x20] = addwfc, addwfc, addwfc, addwfc,
 [0x24] = addwf, addwf, addwf, addwf,
 [0x28] = incf, incf, incf, incf,
 [0x6A] = clrf, clrf,
 [0x6E] = movwf, movwf,
 [0x90] = bcf, bcf, bcf, bcf, bcf, bcf, bcf, bcf,
          bcf, bcf, bcf, bcf, bcf, bcf, bcf, bcf,
 [0x80] = bsf, bsf, bsf, bsf, bsf, bsf, bsf, bsf,
          bsf, bsf, bsf, bsf, bsf, bsf, bsf, bsf,
 [0xE1] = bnz,
 [0xE2] = bc,
 [0xE5] = bnov,
 [0xE6] = bn,
 [0x14] = andwf, andwf, andwf, andwf,
 [0x34] = rlcf, rlcf, rlcf, rlcf,
};

// Range of file register is 0x000 to 0xFFF
uint8_t fileRegisters[0x1000];

// Max range of code memory: 0x000000 - 0x1FFFFF
// Range of code memory for this simulator: 0x0000 - 0xffff
uint8_t codeMemory[0x10000];

/* Signed 8 bit

  0111 1111   127
  0111 1110   126
      .
      .
      .
  0000 0001   1
  0000 0000   0
  -----------------
  1111 1111   -1
  1111 1110   -2
      .
      .
      .
  1000 0001   -127
  1000 0000   -128

 */

int add(int v1, int v2) {
  int result = v1 + v2;
  int lowNibble_v1 = v1 & 0x0F;
  int lowNibble_v2 = v2 & 0x0F;
  int nibbleAddLow = lowNibble_v1 + lowNibble_v2;

  if((int8_t)result == 0x00)
    status |= STATUS_Z;      //Set Z flag to 1
  else
    status &= ~STATUS_Z;

  if(result & 0x80)
    status |= STATUS_N;      //Set N flag to 1
  else
    status &= ~STATUS_N;

  if(result & 0x100)
    status |= STATUS_C;      //Set C flag to 1
  else
    status &= ~STATUS_C;

  if(nibbleAddLow > 0xF)
    status |= STATUS_DC;     //Set DC flag to 1
  else
    status &= ~STATUS_DC;

  //if both operands is positive, result must +ve, else set OV
  //if both operands is negative, result must -ve, else set OV

  if(v1 & 0x80 && v2 & 0x80){
    if(!(result & 0x80))     //if result is +ve, Set OV flag
      status |= STATUS_OV;
    else
      status &= ~STATUS_OV;
  }else if(!(v1 & 0x80) && !(v2 & 0x80)) {
    if(result & 0x80)
      status |= STATUS_OV;   //if result is -ve, Set OV flag
    else
      status &= ~STATUS_OV;
  }else
      status &= ~STATUS_OV;  //Clear OV flag if one operand is +ve and the other is -ve,
                             //overflow will not occur when adding operands of different sign
  return result;
}

/*
           MEMORY
            data       address
             xx  (low)  0x100
codePtr ---> 45         0x101
             25         0x102
             xx  (high) 0x103
*/

void executeInstruction() {
  uint8_t *codePtr = &codeMemory[pc];
  InstructionExecutor executor = pic18ExecutionTable[codePtr[1]];
  executor();
}

int adjustAddrForBankedOrAccess(int a, int addr) {
  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xf00;
  }
  return addr;
}

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: addwf f, d, a
Opcode: 0010 01da ffff ffff
*/
void addwf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] = add(fileRegisters[addr], wreg);
  }else {
    //d is a 0 ==> wreg
    wreg = add(fileRegisters[addr], wreg);
  }
  pc += 2;
}

/*
Mnemonic: addwfc f, d, a
Opcode: 0010 00da ffff ffff
*/
void addwfc() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);
  int tempNum = add(status & 0x01, wreg);
  int DC_flag = status & 0x02;

  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] = add(tempNum, fileRegisters[addr]);
  }else {
    //d is a 0 ==> wreg
    wreg = add(tempNum, fileRegisters[addr]);
  }
  
  status |= DC_flag;         //if the first execution of add function
                             //set DC flag and the second execution does
                             //not, after the whole calculation is done,
                             //the DC flag will still be set although it
                             //is cleared by the second execution
  pc += 2;
}

/*
Mnemonic: incf   f, d, a
Opcode: 0010 10da ffff ffff
*/
void incf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] = add(fileRegisters[addr], 1);
  }else {
    //d is a 0 ==> wreg
    wreg = add(fileRegisters[addr], 1);
  }
  pc += 2;
}

/*
Mnemonic: decf   f, d, a
Opcode: 0000 01da ffff ffff
*/
void decf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] = add(fileRegisters[addr], 0xFF);
  }else {
    //d is a 0 ==> wreg
    wreg = add(fileRegisters[addr], 0xFF);
  }
  pc += 2;
}

/*
Mnemonic: clrf   f, a
Opcode: 0110 101a ffff ffff
*/
void clrf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  fileRegisters[addr] = 0x00;
  status |= STATUS_Z;

  pc += 2;
}

/*
Bit           (b):    range from 0 to 7
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: bsf f, b, a
Opcode: 1000 bbba ffff ffff
*/
void bsf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;
  int b = (codePtr[1] & 0x0E) >> 1;

  int addr = adjustAddrForBankedOrAccess(a, address);

  switch(b) {
    case 0: fileRegisters[addr] |= 0x01;
            break;
    case 1: fileRegisters[addr] |= 0x02;
            break;
    case 2: fileRegisters[addr] |= 0x04;
            break;
    case 3: fileRegisters[addr] |= 0x08;
            break;
    case 4: fileRegisters[addr] |= 0x10;
            break;
    case 5: fileRegisters[addr] |= 0x20;
            break;
    case 6: fileRegisters[addr] |= 0x40;
            break;
    case 7: fileRegisters[addr] |= 0x80;
            break;
  }
  pc += 2;
}

/*
Mnemonic: bcf f, b, a
Opcode: 1001 bbba ffff ffff
*/
void bcf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;
  int b = (codePtr[1] & 0x0E) >> 1;

  int addr = adjustAddrForBankedOrAccess(a, address);

  switch(b) {
    case 0:  fileRegisters[addr] &= 0xFE;
             break;
    case 1:  fileRegisters[addr] &= 0xFD;
             break;
    case 2:  fileRegisters[addr] &= 0xFB;
             break;
    case 3:  fileRegisters[addr] &= 0xF7;
             break;
    case 4:  fileRegisters[addr] &= 0xEF;
             break;
    case 5:  fileRegisters[addr] &= 0xDF;
             break;
    case 6:  fileRegisters[addr] &= 0xBF;
             break;
    case 7:  fileRegisters[addr] &= 0x7F;
             break;
  }
  pc += 2;
}

/*
Relative address    (n):  -128 to 127

Mnemonic: bn n
Opcode: 1110 0110 nnnn nnnn
*/

void bn() {
  uint8_t *codePtr = &codeMemory[pc];
  int8_t n = codePtr[0];

  if(status & 0x10)
    pc = pc + 2 + 2*n;
  else
    pc += 2;
}

/*
Mnemonic: bc n
Opcode: 1110 0010 nnnn nnnn
*/

void bc() {
  uint8_t *codePtr = &codeMemory[pc];
  int8_t n = codePtr[0];

  if(status & 0x01)
    pc = pc + 2 + 2*n;
  else
    pc += 2;
}

/*
Mnemonic: bnz n
Opcode: 1110 0001 nnnn nnnn
*/

void bnz() {
  uint8_t *codePtr = &codeMemory[pc];
  int8_t n = codePtr[0];

  if(status & 0x04)
    pc += 2;
  else
    pc = pc + 2 + 2*n;
}

/*
Mnemonic: bnov n
Opcode: 1110 0101 nnnn nnnn
*/

void bnov() {
  uint8_t *codePtr = &codeMemory[pc];
  int8_t n = codePtr[0];

  if(status & 0x08)
    pc += 2;
  else
    pc = pc + 2 + 2*n;
}

/*
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: movwf f, a
Opcode: 0110 111a ffff ffff
*/

void movwf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  fileRegisters[addr] = wreg;

  pc += 2;
}

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: andwf f, d, a
Opcode: 0001 01da ffff ffff
*/

void andwf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] &=  wreg;
    if(fileRegisters[addr] == 0x00)
      status |= STATUS_Z;
    if(fileRegisters[addr] & 0x80)
      status |= STATUS_N;
  }else {
    //d is a 0 ==> wreg
    wreg &= fileRegisters[addr];
    if(wreg == 0x00)
      status |= STATUS_Z;
    if(wreg & 0x80)
      status |= STATUS_N;
  }
  pc += 2;
}

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: rlcf f, d, a
Opcode: 0011 01da ffff ffff
*/

void rlcf() {
  uint8_t *codePtr = &codeMemory[pc];
  int address = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  int addr = adjustAddrForBankedOrAccess(a, address);

  int shiftL = fileRegisters[addr];

  if(d) {
    fileRegisters[addr] = shiftL << 1;
    if(status & 0x01)
      fileRegisters[addr] |= 0x01;
    if(fileRegisters[addr] == 0)
      status |= STATUS_Z;
    if(fileRegisters[addr] & 0x80)
      status |= STATUS_N;
  }
  else {
    wreg = shiftL << 1;
    if(status & 0x01)
      wreg |= 0x01;
    if(wreg == 0)
      status |= STATUS_Z;
    if(wreg & 0x80)
      status |= STATUS_N;
  }

  if(shiftL & 0x80)
       status |= STATUS_C;
  else
       status &= 0xFE;

  pc += 2;
}
