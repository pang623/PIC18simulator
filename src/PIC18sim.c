#include "PIC18sim.h"

InstructionExecutor pic18ExecutionTable[256] = {
 [0x24] = addwf, addwf, addwf, addwf,
 [0x28] = incf, incf, incf, incf,
 [0x6A] = clrf, clrf,
 [0x6E] = movwf, movwf,
 [0x90] = bcf, bcf, bcf, bcf, bcf, bcf, bcf, bcf,
          bcf, bcf, bcf, bcf, bcf, bcf, bcf, bcf,
 [0x80] = bsf, bsf, bsf, bsf, bsf, bsf, bsf, bsf,
          bsf, bsf, bsf, bsf, bsf, bsf, bsf, bsf,
 [0xE6] = bn,
 [0xE1] = bnz,
 [0xE5] = bnov,
 [0x14] = andwf, andwf, andwf, andwf,
 [0x34] = rlcf, rlcf, rlcf, rlcf,
};

// Range of file register is 0x000 to 0xFFF
int8_t fileRegisters[0x1000];

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
  int result;
  int nibbleAddLow;
  int nibbleAddHigh;
  int lowNibble_v1 = v1 & 0x0F;
  int lowNibble_v2 = v2 & 0x0F;
  int highNibble_v1 = (v1 & 0xF0) >> 4;
  int highNibble_v2 = (v2 & 0xF0) >> 4;
  result = v1 + v2;
  nibbleAddLow = lowNibble_v1 + lowNibble_v2;
  if(nibbleAddLow > 0xF)
    nibbleAddHigh = highNibble_v1 + highNibble_v2 + 1;
  else
    nibbleAddHigh = highNibble_v1 + highNibble_v2;
  //adjust status flags
  status = 0x00;

  if((int8_t)result == 0x00)
    status |= STATUS_Z;      //Set Z flag to 1
  if(result & 0x80)
    status |= STATUS_N;      //Set N flag to 1
  if(result > 0xFF || nibbleAddHigh > 0xF)
    status |= STATUS_C;      //Set C flag to 1
  if(nibbleAddLow > 0xF)
    status |= STATUS_DC;     //Set DC flag to 1

  //if both operands is positive, result must +ve, else set OV
  //if both operands is negative, result must -ve, else set OV

  if(v1 & 0x80 && v2 & 0x80){
    if(!(result & 0x80))     //if signed bit of result is 0, Set OV flag
      status |= STATUS_OV;
  }else if(!(v1 & 0x80) && !(v2 & 0x80)) {
    if(result & 0x80)
      status |= STATUS_OV;   //if signed bit of result is 1, Set OV flag
  }
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

uint8_t *executeInstruction(uint8_t *codePtr) {
  InstructionExecutor executor = pic18ExecutionTable[codePtr[1]];
  return executor(codePtr);
}


/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: addwf f, d, a
Opcode: 0010 01da ffff ffff
*/
uint8_t *addwf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xf00;
  }
  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] = add(fileRegisters[addr], wreg);
  }else {
    //d is a 0 ==> wreg
    wreg = add(fileRegisters[addr], wreg);
  }
  return NULL;
}

/*
Mnemonic: incf   f, d, a
Opcode: 0010 10da ffff ffff
*/
uint8_t *incf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xF00;
  }
  if(d) {
    //d is a 1 ==> store result in file register
    fileRegisters[addr] = add(fileRegisters[addr], 1);
  }else {
    //d is a 0 ==> wreg
    wreg = add(fileRegisters[addr], 1);
  }
  return NULL;
}

/*
Mnemonic: clrf   f, a
Opcode: 0110 101a ffff ffff
*/
uint8_t *clrf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xF00;
  }

  fileRegisters[addr] = 0x00;
  status |= STATUS_Z;

  return NULL;
}

/*
Bit           (b):    range from 0 to 7
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: bsf f, b, a
Opcode: 1000 bbba ffff ffff
*/
uint8_t *bsf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;
  int b = (codePtr[1] & 0x0E) >> 1;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xF00;
  }

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
  return NULL;
}

/*
Mnemonic: bcf f, b, a
Opcode: 1001 bbba ffff ffff
*/
uint8_t *bcf(uint8_t *codePtr) {
  //execute instruction
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;
  int b = (codePtr[1] & 0x0E) >> 1;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xF00;
  }

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
  return NULL;
}

/*
Relative address    (n):  -128 to 127

Mnemonic: bn n
Opcode: 1110 0110 nnnn nnnn
*/

uint8_t *bn(uint8_t *codePtr) {
  //execute instruction
  int8_t n = codePtr[0]; 
  
  if(status & 0x10)
    pcl = pcl + 2 + 2*n;
  else
    pcl = pcl + 2;
  
  return NULL;
}

/*
Mnemonic: bnz n
Opcode: 1110 0001 nnnn nnnn
*/

uint8_t *bnz(uint8_t *codePtr) {
  //execute instruction
  int8_t n = codePtr[0]; 
  
  if(status & 0x04)
    pcl = pcl + 2;
  else
    pcl = pcl + 2 + 2*n;
  
  return NULL;
}

/*
Mnemonic: bnov n
Opcode: 1110 0101 nnnn nnnn
*/

uint8_t *bnov(uint8_t *codePtr) {
  //execute instruction
  int8_t n = codePtr[0]; 
  
  if(status & 0x08)
    pcl = pcl + 2;
  else
    pcl = pcl + 2 + 2*n;
  
  return NULL;
}

/*
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: movwf f, a
Opcode: 0110 111a ffff ffff
*/

uint8_t *movwf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int a = codePtr[1] & 0x01;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xf00;
  }

  fileRegisters[addr] = wreg;

  return NULL;
}

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: andwf f, d, a
Opcode: 0001 01da ffff ffff
*/
uint8_t *andwf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xf00;
  }

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
  return NULL;
}

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: rlcf f, d, a
Opcode: 0011 01da ffff ffff
*/

uint8_t *rlcf(uint8_t *codePtr) {
  //execute instruction
  int addr = (uint8_t)codePtr[0];
  int d = codePtr[1] & 0x02;
  int a = codePtr[1] & 0x01;

  if(a) {
    //BANKED
    addr += (unsigned int)bsr*0x100;
  }else {
    //ACCESS
    if(addr > 0x5F)
      //Adjust the address if it is in SFR region
       addr += 0xf00;
  }

  int shiftL = (int8_t)fileRegisters[addr];
  
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
     
 return NULL;
}
