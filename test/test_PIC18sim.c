#include "unity.h"
#include "PIC18sim.h"
#include "_testHelper.h"

extern uint8_t fileRegisters[];

void setUp(void)
{
}

void tearDown(void)
{
}

void test_add_given_0x7A_and_0x8E_expect_N0_OV0_Z0_DC1_C1() {
  status = STATUS_N | STATUS_Z |STATUS_OV;
  int val = add(0x7A, 0x8E);
  TEST_ASSERT_EQUAL(0x7A + 0x8E, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_DC | STATUS_C, status);
}

void test_add_given_0x84_and_0x79_expect_N1_OV0_Z0_DC0_C0() {
  status = STATUS_DC | STATUS_Z | STATUS_C | STATUS_OV;
  int val = add(0x84, 0x79);
  TEST_ASSERT_EQUAL(0x84 + 0x79, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
}

void test_add_given_0x7C_and_0x64_expect_N1_OV1_Z0_DC1_C0() {
  status = STATUS_Z | STATUS_C;
  int val = add(0x7C, 0x64);
  TEST_ASSERT_EQUAL(0x7C + 0x64, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N | STATUS_DC | STATUS_OV, status);
}

void test_add_given_0x80_and_0x80_expect_N0_OV1_Z1_DC0_C1() {
  status = STATUS_N | STATUS_DC;
  int val = add(0x80, 0x80);
  TEST_ASSERT_EQUAL(0x80 + 0x80, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z | STATUS_C | STATUS_OV, status);
}

void test_add_given_0x00_and_0x00_expect_N0_OV0_Z1_DC0_C0() {
  status = STATUS_N | STATUS_DC | STATUS_OV | STATUS_C;
  int val = add(0x00, 0x00);
  TEST_ASSERT_EQUAL(0x00 + 0x00, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
}

void test_add_given_0xFD_and_0x9A_expect_N1_OV0_Z0_DC1_C1() {
  status = STATUS_OV | STATUS_Z;
  int val = add(0xFD, 0x9A);
  TEST_ASSERT_EQUAL(0xFD + 0x9A, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N | STATUS_C | STATUS_DC, status);
}

void test_add_given_0x05_and_0x0A_expect_N0_OV0_Z0_DC0_C0() {
  status = STATUS_OV | STATUS_Z | STATUS_DC | STATUS_N | STATUS_C;
  int val = add(0x05, 0x0A);
  TEST_ASSERT_EQUAL(0x05 + 0x0A, val);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
}

void test_add_given_0x7F_and_0x81_expect_N0_OV0_Z1_DC1_C1() {
  status = STATUS_OV | STATUS_N;
  int val = add(0x7F, 0x81);
  TEST_ASSERT_EQUAL(0x7F + 0x81, val);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C | STATUS_DC | STATUS_Z, status);
}

//----------------------------TEST ADDWF---------------------------------

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255
*/

//  0010 01da ffff ffff
//addwf   0x45, f, ACCESS   ==> 0010 0110 0100 0101(0x2645)
void test_executeInstruction_given_0x2645_expect_addwf_called_and_access_to_0x45_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x45, 0x26, 0x00, 0xff};
  //Set WREG
  wreg = 0x23;
  //Set content of target file reg
  fileRegisters[0x45] = 0x56;
  fileRegisters[0x245] = 0x00;
  fileRegisters[0xF45] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x38FE);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x79, fileRegisters[0x45]);
  TEST_ASSERT_EQUAL_HEX8(0x23, wreg);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
  TEST_ASSERT_EQUAL_PTR(0x38FE + 2, pc);
}

//  0010 01da ffff ffff
//addwf   0x23, w, ACCESS   ==> 0010 0100 0010 0011(0x2423)
void test_executeInstruction_given_0x2423_expect_addwf_called_and_access_to_0x23_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x23, 0x24, 0x00, 0xff};
  //Set WREG
  wreg = 0x38;
  //Set content of target file reg
  fileRegisters[0x23] = 0x6A;
  fileRegisters[0x723] = 0x00;
  fileRegisters[0xF23] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x1942);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x6A, fileRegisters[0x23]);
  TEST_ASSERT_EQUAL_HEX8(0xA2, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_OV | STATUS_N | STATUS_DC, status);
  TEST_ASSERT_EQUAL_PTR(0x1942 + 2, pc);
}

//  0010 01da ffff ffff
//addwf   0x67, f, ACCESS   ==> 0010 0110 0110 0111(0x2667)
void test_executeInstruction_given_0x2667_expect_addwf_called_and_access_to_0xf67_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x67, 0x26, 0x00, 0xff};
  //Set WREG
  wreg = 0x56;
  //Set content of target file reg
  fileRegisters[0x67]  = 0x00;
  fileRegisters[0x467] = 0x00;
  fileRegisters[0xf67] = 0x43;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xE890);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x99, fileRegisters[0xf67]);
  TEST_ASSERT_EQUAL_HEX8(0x56, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_OV | STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0xE890 + 2, pc);
}

//  0010 01da ffff ffff
//addwf   0x8F, w, ACCESS   ==> 0010 0100 1000 1111(0x248F)
void test_executeInstruction_given_0x248F_expect_addwf_called_and_access_to_0xf8f_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x8F, 0x24, 0x00, 0xff};
  //Set WREG
  wreg = 0x5B;
  //Set content of target file reg
  fileRegisters[0x8F]  = 0x00;
  fileRegisters[0x78F] = 0x00;
  fileRegisters[0xF8F] = 0x32;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6902);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x32, fileRegisters[0xF8F]);
  TEST_ASSERT_EQUAL_HEX8(0x8D, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_OV | STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0x6902 + 2, pc);
}


//  0010 01da ffff ffff
//addwf   0xA5, f, BANKED   ==> 0010 0111 1010 0101(0x27A5)
void test_executeInstruction_given_0x27A5_expect_addwf_called_and_access_to_0x8A5_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xA5, 0x27, 0x00, 0xff};
  //Set BSR
  bsr = 0x8;
  //Set WREG
  wreg = 0x9C;
  //Set content of target file reg
  fileRegisters[0xA5]  = 0x00;
  fileRegisters[0x8A5] = 0xA9;
  fileRegisters[0xFA5] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xA02C);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x45, fileRegisters[0x8A5]);
  TEST_ASSERT_EQUAL_HEX8(0x9C, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C | STATUS_DC | STATUS_OV, status);
  TEST_ASSERT_EQUAL_PTR(0xA02C + 2, pc);
}

//  0010 01da ffff ffff
//addwf   0x7D, w, BANKED   ==> 0010 0101 0111 1101(0x257D)
void test_executeInstruction_given_0x257D_expect_addwf_called_and_access_to_0xB7D_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x7D, 0x25, 0x00, 0xff};
  //Set BSR
  bsr = 0xB;
  //Set WREG
  wreg = 0xF9;
  //Set content of target file reg
  fileRegisters[0x7D]  = 0x00;
  fileRegisters[0xB7D] = 0xD8;
  fileRegisters[0xF7D] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x88DE);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xD8, fileRegisters[0xB7D]);
  TEST_ASSERT_EQUAL_HEX8(0xD1, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C | STATUS_N | STATUS_DC, status);
  TEST_ASSERT_EQUAL_PTR(0x88DE + 2, pc);
}

//----------------------------TEST ADDWFC---------------------------------

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255
*/

//  0010 00da ffff ffff
//addwfc   0x20, f, ACCESS   ==> 0010 0010 0010 0000(0x2220)
void test_executeInstruction_given_0x2220_expect_addwfc_called_and_access_to_0x20_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x20, 0x22, 0x00, 0xff};
  //Carry flag input(C is 1)
  status |= STATUS_C;
  //Set WREG
  wreg = 0xFF;
  //Set content of target file reg
  fileRegisters[0x20] = 0xFF;
  fileRegisters[0x220] = 0x00;
  fileRegisters[0xF20] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6612);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0x20]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C | STATUS_DC | STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0x6612 + 2, pc);
}

//  0010 00da ffff ffff
//addwfc   0xF4, w, BANKED   ==> 0010 0001 1111 0100(0x21F4)
void test_executeInstruction_given_0x21F4_expect_addwfc_called_and_access_to_0xCF4_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xF4, 0x21, 0x00, 0xff};
  //Carry flag input(C is 1)
  status |= STATUS_C;
  //Set BSR
  bsr = 0xC;
  //Set WREG
  wreg = 0x4D;
  //Set content of target file reg
  fileRegisters[0xF4] = 0x00;
  fileRegisters[0xCF4] = 0x02;
  fileRegisters[0xFF4] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xAA20);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x02, fileRegisters[0xCF4]);
  TEST_ASSERT_EQUAL_HEX8(0x50, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_DC, status);
  TEST_ASSERT_EQUAL_PTR(0xAA20 + 2, pc);
}

//  0010 00da ffff ffff
//addwfc   0x7A, w, ACCESS   ==> 0010 0000 0111 1010(0x207A)
void test_executeInstruction_given_0x207A_expect_addwfc_called_and_access_to_0xF7A_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x7A, 0x20, 0x00, 0xff};
  //Carry flag input(C is 0)
  status &= ~STATUS_C;
  //Set WREG
  wreg = 0xFE;
  //Set content of target file reg
  fileRegisters[0x7A] = 0x00;
  fileRegisters[0xD7A] = 0x00;
  fileRegisters[0xF7A] = 0x80;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x9B8A);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x80, fileRegisters[0xF7A]);
  TEST_ASSERT_EQUAL_HEX8(0x7E, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C | STATUS_OV, status);
  TEST_ASSERT_EQUAL_PTR(0x9B8A + 2, pc);
}

//----------------------------TEST INCF---------------------------------

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255
*/

//  0010 10da ffff ffff
//incf   0x10, f, ACCESS   ==> 0010 1010 0001 0000(0x2A10)
void test_executeInstruction_given_0x2A10_expect_incf_called_and_access_to_0x10_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x10, 0x2A, 0x00, 0xff};
  //Set WREG
  wreg = 0x22;
  //Set content of target file reg
  fileRegisters[0x10]  = 0xFF;
  fileRegisters[0xB10] = 0x00;
  fileRegisters[0xF10] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x0012);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0x10]);
  TEST_ASSERT_EQUAL_HEX8(0x22, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C | STATUS_DC | STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x0012 + 2, pc);
}

//  0010 10da ffff ffff
//incf   0x4D, w, ACCESS   ==> 0010 1000 0100 1101(0x284D)
void test_executeInstruction_given_0x284D_expect_incf_called_and_access_to_0x4D_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x4D, 0x28, 0x00, 0xff};
  //Set WREG
  wreg = 0x67;
  //Set content of target file reg
  fileRegisters[0x4D]  = 0xDA;
  fileRegisters[0xC4D] = 0x00;
  fileRegisters[0xF4D] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xF60A);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xDA, fileRegisters[0x4D]);
  TEST_ASSERT_EQUAL_HEX8(0xDB, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0xF60A + 2, pc);
}

//  0010 01da ffff ffff
//incf   0xA9, f, ACCESS   ==> 0010 1010 1010 1001(0x2AA9)
void test_executeInstruction_given_0x2AA9_expect_incf_called_and_access_to_0xFA9_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xA9, 0x2A, 0x00, 0xff};
  //Set WREG
  wreg = 0x9F;
  //Set content of target file reg
  fileRegisters[0xA9]  = 0x00;
  fileRegisters[0x4A9] = 0x00;
  fileRegisters[0xFA9] = 0xBF;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x53AA);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xC0, fileRegisters[0xFA9]);
  TEST_ASSERT_EQUAL_HEX8(0x9F, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_DC | STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0x53AA + 2, pc);
}

//  0010 01da ffff ffff
//incf   0xCD, w, ACCESS   ==> 0010 1000 1100 1101(0x28CD)
void test_executeInstruction_given_0x28CD_expect_incf_called_and_access_to_0xFCD_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xCD, 0x28, 0x00, 0xff};
  //Set WREG
  wreg = 0xE3;
  //Set content of target file reg
  fileRegisters[0xCD]  = 0x00;
  fileRegisters[0x4CD] = 0x00;
  fileRegisters[0xFCD] = 0xAA;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xDABC);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xAA, fileRegisters[0xFCD]);
  TEST_ASSERT_EQUAL_HEX8(0xAB, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0xDABC + 2, pc);
}

//  0010 01da ffff ffff
//incf   0x38, f, BANKED   ==> 0010 1011 0011 1000(0x2B38)
void test_executeInstruction_given_0x2B38_expect_incf_called_and_access_to_0x138_with_the_result_stored_in_fileRegister(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x38, 0x2B, 0x00, 0xff};
  //Set BSR
  bsr = 0x1;
  //Set WREG
  wreg = 0xFE;
  //Set content of target file reg
  fileRegisters[0x38]  = 0x00;
  fileRegisters[0x138] = 0x23;
  fileRegisters[0xF38] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xFE60);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x24, fileRegisters[0x138]);
  TEST_ASSERT_EQUAL_HEX8(0xFE, wreg);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
  TEST_ASSERT_EQUAL_PTR(0xFE60 + 2, pc);
}

//  0010 01da ffff ffff
//incf   0xFF, w, BANKED   ==> 0010 1001 1111 1111(0x29FF)
void test_executeInstruction_given_0x29FF_expect_incf_called_and_access_to_0xEFF_with_the_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xFF, 0x29, 0x00, 0xFE};
  //Set BSR
  bsr = 0xE;
  //Set WREG
  wreg = 0x2B;
  //Set content of target file reg
  fileRegisters[0xFF]  = 0x00;
  fileRegisters[0xEFF] = 0xD0;
  fileRegisters[0xFFF] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6666);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xD0, fileRegisters[0xEFF]);
  TEST_ASSERT_EQUAL_HEX8(0xD1, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0x6666 + 2, pc);
}

//----------------------------TEST CLRF---------------------------------

/*
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255
*/

//  0110 101a ffff ffff
//clrf   0x2E, ACCESS   ==> 0110 1010 0010 1110(0x6A2E)
void test_executeInstruction_given_0x6A2E_expect_clrf_called_and_access_to_0x2E_to_clear_the_data_stored(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x2E, 0x6A, 0x00, 0xff};
  //Set WREG
  wreg = 0x1F;
  //Set content of target file reg
  fileRegisters[0x2E]  = 0x8D;
  fileRegisters[0x12E] = 0x33;
  fileRegisters[0xF2E] = 0x44;
  //Clear the status register for testing
  status = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x3810);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0x2E]);
  TEST_ASSERT_EQUAL_HEX8(0x33, fileRegisters[0x12E]);       //test whether the clrf instruction clears the correct file register
  TEST_ASSERT_EQUAL_HEX8(0x44, fileRegisters[0xF2E]);
  TEST_ASSERT_EQUAL_HEX8(0x1F, wreg);                       //data stored in wreg is unaffected by clrf instruction
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x3810 + 2, pc);
}

//  0110 101a ffff ffff
//clrf   0x9F, ACCESS   ==> 0110 1010 1001 1111(0x6A9F)
void test_executeInstruction_given_0x6A9F_expect_clrf_called_and_access_to_0xF9F_to_clear_the_data_stored(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x9F, 0x6A, 0x00, 0xff};
  //Set WREG
  wreg = 0xF0;
  //Set content of target file reg
  fileRegisters[0x9F]  = 0x4D;
  fileRegisters[0x69F] = 0x33;
  fileRegisters[0xF9F] = 0x44;
  //Clear the status register for testing
  status = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6000);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xF9F]);
  TEST_ASSERT_EQUAL_HEX8(0x33, fileRegisters[0x69F]);
  TEST_ASSERT_EQUAL_HEX8(0x4D, fileRegisters[0x9F]);
  TEST_ASSERT_EQUAL_HEX8(0xF0, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x6000 + 2, pc);
}

//  0110 101a ffff ffff
//clrf   0x3A BANKED   ==> 0110 1011 0011 1010(0x6B3A)
void test_executeInstruction_given_0x6B3A_expect_clrf_called_and_access_to_0xA3A_to_clear_the_data_stored(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x3A, 0x6B, 0x00, 0xff};
  //Set BSR
  bsr = 0xA;
  //Set WREG
  wreg = 0x13;
  //Set content of target file reg
  fileRegisters[0x3A]  = 0x11;
  fileRegisters[0xA3A] = 0x23;
  fileRegisters[0xF3A] = 0x55;
  //Clear the status register for testing
  status = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x588E);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xA3A]);
  TEST_ASSERT_EQUAL_HEX8(0x11, fileRegisters[0x3A]);
  TEST_ASSERT_EQUAL_HEX8(0x55, fileRegisters[0xF3A]);
  TEST_ASSERT_EQUAL_HEX8(0x13, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x588E + 2, pc);
}

//----------------------------TEST BSF---------------------------------

/*
Bit           (b):    range from 0 to 7
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255
*/

//  1000 bbba ffff ffff
//bsf   0x3A, 5, ACCESS  ==> 1000 1010 0011 1010(0x8A3A)
//bit set 0 ==> 1
void test_executeInstruction_given_0x8A3A_expect_bsf_called_and_access_to_0x3A_to_set_bit_5(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x3A, 0x8A, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0x3A]  = 0x1F;
  fileRegisters[0x43A] = 0x33;
  fileRegisters[0xF3A] = 0x44;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x0968);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x3F, fileRegisters[0x3A]);
  TEST_ASSERT_EQUAL_HEX8(0x33, fileRegisters[0x43A]);         //test that the bsf instruction do not accidentally set bit on these file registers
  TEST_ASSERT_EQUAL_HEX8(0x44, fileRegisters[0xF3A]);
  TEST_ASSERT_EQUAL_PTR(0x0968 + 2, pc);
}

//  1000 bbba ffff ffff
//bsf   0x05, 1, ACCESS  ==> 1000 0010 0000 0101(0x8205)
//bit set 1 ==> 1
void test_executeInstruction_given_0x8205_expect_bsf_called_and_access_to_0x05_to_set_bit_1(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x05, 0x82, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0x05]  = 0xBA;
  fileRegisters[0x105] = 0x11;
  fileRegisters[0xF05] = 0x55;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x2004);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xBA, fileRegisters[0x05]);
  TEST_ASSERT_EQUAL_HEX8(0x11, fileRegisters[0x105]);
  TEST_ASSERT_EQUAL_HEX8(0x55, fileRegisters[0xF05]);
  TEST_ASSERT_EQUAL_PTR(0x2004 + 2, pc);
}

//  1000 bbba ffff ffff
//bsf   0xA0, 7, ACCESS  ==> 1000 1110 1010 0000(0x8EA0)
//bit set 0 ==> 1
void test_executeInstruction_given_0x8EA0_expect_bsf_called_and_access_to_0xFA0_to_set_bit_7(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xA0, 0x8E, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0xA0]  = 0xB2;
  fileRegisters[0x6A0] = 0xC5;
  fileRegisters[0xFA0] = 0x7D;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xE2D4);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xFD, fileRegisters[0xFA0]);
  TEST_ASSERT_EQUAL_HEX8(0xC5, fileRegisters[0x6A0]);
  TEST_ASSERT_EQUAL_HEX8(0xB2, fileRegisters[0xA0]);
  TEST_ASSERT_EQUAL_PTR(0xE2D4 + 2, pc);
}

//  1000 bbba ffff ffff
//bsf   0xDC, 0, ACCESS  ==> 1000 0000 1101 1100(0x80DC)
//bit set 1 ==> 1
void test_executeInstruction_given_0x80DC_expect_bsf_called_and_access_to_0xFDC_to_set_bit_0(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xDC, 0x80, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0xDC]  = 0x68;
  fileRegisters[0x7DC] = 0x20;
  fileRegisters[0xFDC] = 0x55;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x2ABC);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x55, fileRegisters[0xFDC]);
  TEST_ASSERT_EQUAL_HEX8(0x20, fileRegisters[0x7DC]);
  TEST_ASSERT_EQUAL_HEX8(0x68, fileRegisters[0xDC]);
  TEST_ASSERT_EQUAL_PTR(0x2ABC + 2, pc);
}

//  1000 bbba ffff ffff
//bsf   0x9A, 6, BANKED  ==> 1000 1101 1001 1010(0x8D9A)
//bit set 0 ==> 1
void test_executeInstruction_given_0x8D9A_expect_bsf_called_and_access_to_0xD9A_to_set_bit_6(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x9A, 0x8D, 0x00, 0xff};
  //Set BSR
  bsr = 0xD;
  //Set content of target file reg
  fileRegisters[0x9A]  = 0xF3;
  fileRegisters[0xD9A] = 0x9C;
  fileRegisters[0xF9A] = 0x82;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x4302);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xDC, fileRegisters[0xD9A]);
  TEST_ASSERT_EQUAL_HEX8(0xF3, fileRegisters[0x9A]);
  TEST_ASSERT_EQUAL_HEX8(0x82, fileRegisters[0xF9A]);
  TEST_ASSERT_EQUAL_PTR(0x4302 + 2, pc);
}

//  1000 bbba ffff ffff
//bsf   0xBD, 3, BANKED  ==> 1000 0111 1011 1101(0x87BD)
//bit set 1 ==> 1
void test_executeInstruction_given_0x87BD_expect_bsf_called_and_access_to_0x8BD_to_set_bit_3(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xBD, 0x87, 0x00, 0xff};
  //Set BSR
  bsr = 0x8;
  //Set content of target file reg
  fileRegisters[0xBD]  = 0x36;
  fileRegisters[0x8BD] = 0x1E;
  fileRegisters[0xFBD] = 0xF1;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6688);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x1E, fileRegisters[0x8BD]);
  TEST_ASSERT_EQUAL_HEX8(0xF1, fileRegisters[0xFBD]);
  TEST_ASSERT_EQUAL_HEX8(0x36, fileRegisters[0xBD]);
  TEST_ASSERT_EQUAL_PTR(0x6688 + 2, pc);
}

//----------------------------TEST BCF---------------------------------

/*
Bit           (b):    range from 0 to 7
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255
*/

//  1001 bbba ffff ffff
//bcf   0x56, 4, ACCESS  ==> 1001 1000 0101 0110(0x9856)
//bit clr 1 ==> 0
void test_executeInstruction_given_0x9856_expect_bcf_called_and_access_to_0x56_to_clear_bit_4(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x56, 0x98, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0x56]  = 0x3C;
  fileRegisters[0xA56] = 0x65;
  fileRegisters[0xF56] = 0xAA;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xFFFA);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x2C, fileRegisters[0x56]);
  TEST_ASSERT_EQUAL_HEX8(0x65, fileRegisters[0xA56]);
  TEST_ASSERT_EQUAL_HEX8(0xAA, fileRegisters[0xF56]);
  TEST_ASSERT_EQUAL_PTR(0xFFFA + 2, pc);
}

//  1001 bbba ffff ffff
//bcf   0x4F, 5, ACCESS  ==> 1001 1010 0100 1111(0x9A4F)
//bit clr 0 ==> 0
void test_executeInstruction_given_0x9A4F_expect_bcf_called_and_access_to_0x4F_to_clear_bit_5(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x4F, 0x9A, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0x4F]  = 0x8E;
  fileRegisters[0xD4F] = 0x01;
  fileRegisters[0xF4F] = 0x90;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x02EA);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x8E, fileRegisters[0x4F]);
  TEST_ASSERT_EQUAL_HEX8(0x01, fileRegisters[0xD4F]);
  TEST_ASSERT_EQUAL_HEX8(0x90, fileRegisters[0xF4F]);
  TEST_ASSERT_EQUAL_PTR(0x02EA + 2, pc);
}

//  1001 bbba ffff ffff
//bcf   0x78, 2, ACCESS  ==> 1001 0100 0111 1000(0x9478)
//bit clr 1 ==> 0
void test_executeInstruction_given_0x9478_expect_bcf_called_and_access_to_0xF78_to_clear_bit_2(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x78, 0x94, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0x78]  = 0xD2;
  fileRegisters[0x578] = 0x1B;
  fileRegisters[0xF78] = 0xC7;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x8AE2);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xC3, fileRegisters[0xF78]);
  TEST_ASSERT_EQUAL_HEX8(0x1B, fileRegisters[0x578]);
  TEST_ASSERT_EQUAL_HEX8(0xD2, fileRegisters[0x78]);
  TEST_ASSERT_EQUAL_PTR(0x8AE2 + 2, pc);
}

//  1001 bbba ffff ffff
//bcf   0x9A, 3, ACCESS  ==> 1001 0110 1001 1010(0x969A)
//bit clr 0 ==> 0
void test_executeInstruction_given_0x969A_expect_bcf_called_and_access_to_0xF9A_to_clear_bit_3(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x9A, 0x96, 0x00, 0xff};

  //Set content of target file reg
  fileRegisters[0x9A]  = 0x07;
  fileRegisters[0x99A] = 0xA3;
  fileRegisters[0xF9A] = 0xF7;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x008A);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xF7, fileRegisters[0xF9A]);
  TEST_ASSERT_EQUAL_HEX8(0xA3, fileRegisters[0x99A]);
  TEST_ASSERT_EQUAL_HEX8(0x07, fileRegisters[0x9A]);
  TEST_ASSERT_EQUAL_PTR(0x008A + 2, pc);
}

//  1001 bbba ffff ffff
//bcf   0x37, 7, BANKED  ==> 1001 1111 0011 0111(0x9F37)
//bit clr 1 ==> 0
void test_executeInstruction_given_0x9F37_expect_bcf_called_and_access_to_0xE37_to_clear_bit_7(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x37, 0x9F, 0x00, 0xff};
  //Set BSR
  bsr = 0xE;
  //Set content of target file reg
  fileRegisters[0x37]  = 0xEE;
  fileRegisters[0xE37] = 0xC6;
  fileRegisters[0xF37] = 0x0F;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x3668);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x46, fileRegisters[0xE37]);
  TEST_ASSERT_EQUAL_HEX8(0x0F, fileRegisters[0xF37]);
  TEST_ASSERT_EQUAL_HEX8(0xEE, fileRegisters[0x37]);
  TEST_ASSERT_EQUAL_PTR(0x3668 + 2, pc);
}

//  1001 bbba ffff ffff
//bcf   0xBE, 1, BANKED  ==> 1001 0011 1011 1110(0x93BE)
//bit clr 0 ==> 0
void test_executeInstruction_given_0x93BE_expect_bcf_called_and_access_to_0x1BE_to_clear_bit_1(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xBE, 0x93, 0x00, 0xff};
  //Set BSR
  bsr = 0x1;
  //Set content of target file reg
  fileRegisters[0xBE]  = 0xF4;
  fileRegisters[0x1BE] = 0x79;
  fileRegisters[0xFBE] = 0x32;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x2448);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x79, fileRegisters[0x1BE]);
  TEST_ASSERT_EQUAL_HEX8(0x32, fileRegisters[0xFBE]);
  TEST_ASSERT_EQUAL_HEX8(0xF4, fileRegisters[0xBE]);
  TEST_ASSERT_EQUAL_PTR(0x2448 + 2, pc);
}

//----------------------------TEST BN---------------------------------

/*
Relative address    (n):  -128 to 127

Mnemonic: bn n
Opcode: 1110 0110 nnnn nnnn

  n is the number of lines of instruction that need to be jumped

    PC        Opcode       Operands
|- 0x8454      E603       bn 0x845C
|  0x8456      ...          ...
|   .
|   .
|->0x845C

*/

//test for jumping forward
//  PC
// 0x8454   bn   0x845C   ==> 1110 0110 0000 0011(0xE603)
void test_executeInstruction_given_0xE603_and_PC_at_0x8454_and_NEGATIVE_bit_is_high_expect_bn_called_and_PC_is_0x845C(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x03, 0xE6, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x8454);
  //Set status flag (negative)
  status = STATUS_N;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x845C, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);     //test that bn do not affect status flags
}

//test for jumping forward (whole range)
//  PC
// 0x9900   bn   0x99FE   ==> 1110 0110 0111 1110(0xE67E)
void test_executeInstruction_given_0xE67E_and_PC_at_0x9900_and_NEGATIVE_bit_is_high_expect_bn_called_and_PC_is_0x99FE(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x7E, 0xE6, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x9900);
  //Set status flag (negative)
  status = STATUS_N;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x99FE, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);     //test that bn do not affect status flags
}

//test for jumping backwards
//  PC
// 0x236E   bn   0x2320   ==> 1110 0110 1101 1000(0xE6D8)
void test_executeInstruction_given_0xE6D8_and_PC_at_0x236E_and_NEGATIVE_bit_is_high_expect_bn_called_and_PC_is_0x2320(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xD8, 0xE6, 0x00, 0xff};
 //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x236E);
  //Set status flag (negative)
  status = STATUS_N;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x2320, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);     //test that bn do not affect status flags
}

//test for jumping backwards (whole range)
//  PC
// 0x10FE   bn   0x1000   ==> 1110 0110 1000 0000(0xE680)
void test_executeInstruction_given_0xE680_and_PC_at_0x10FE_and_NEGATIVE_bit_is_high_expect_bn_called_and_PC_is_0x1000(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x80, 0xE6, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x10FE);
  //Set status flag (negative)
  status = STATUS_N;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x1000, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);     //test that bn do not affect status flags
}

//test when NEGATIVE bit is low, BN will not jump to target address, it continues to the next instruction instead, which causes PC to +2
//  PC
// 0xFAB2   bn   0xFAC4   ==> 1110 0110 0000 1000(0xE608)
void test_executeInstruction_given_0xE608_and_PC_at_0xFAB2_and_NEGATIVE_bit_is_low_expect_bn_called_and_PC_is_0xFAB4(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x08, 0xE6, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xFAB2);
  //Clear status flag (negative)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0xFAB2 + 2, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bn do not affect status flags
}

//----------------------------TEST BNZ---------------------------------

/*
Relative address    (n):  -128 to 127

Mnemonic: bnz n
Opcode: 1110 0001 nnnn nnnn

  n is the number of lines of instruction that need to be jumped

*/

//test for jumping forward
//  PC
// 0x3A8E   bnz   0x3AA4   ==> 1110 0001 0000 1010(0xE10A)
void test_executeInstruction_given_0xE10A_and_PC_at_0x3A8E_and_ZERO_bit_is_low_expect_bnz_called_and_PC_is_0x3AA4(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x0A, 0xE1, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x3A8E);
  //Clear status flag (zero)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x3AA4, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnz do not affect status flags
}

//test for jumping forward (whole range)
//  PC
// 0x3E00   bnz   0x3EFE   ==> 1110 0001 0111 1110(0xE17E)
void test_executeInstruction_given_0xE17E_and_PC_at_0x3E00_and_ZERO_bit_is_low_expect_bnz_called_and_PC_is_0x3EFE(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x7E, 0xE1, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x3E00);
  //Clear status flag (zero)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x3EFE, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnz do not affect status flags
}

//test for jumping backwards
//  PC
// 0xBEFA   bnz   0xBEDC   ==> 1110 0001 1111 0000(0xE1F0)
void test_executeInstruction_given_0xE1F0_and_PC_at_0xBEFA_and_ZERO_bit_is_low_expect_bnz_called_and_PC_is_0xBEDC(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xF0, 0xE1, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xBEFA);
  //Clear status flag (zero)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0xBEDC, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnz do not affect status flags
}

//test for jumping backwards (whole range)
//  PC
// 0x11FE   bnz   0x1100   ==> 1110 0001 1000 0000(0xE180)
void test_executeInstruction_given_0xE180_and_PC_at_0x11FE_and_ZERO_bit_is_low_expect_bnz_called_and_PC_is_0x1100(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x80, 0xE1, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x11FE);
  //Clear status flag (zero)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x1100, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnz do not affect status flags
}

//test when ZERO bit is high, BNZ will not jump to target address, it continues to the next instruction instead, which causes PC to +2
//  PC
// 0xAAB2   bnz   0xAAC4   ==> 1110 0001 0000 1000(0xE108)
void test_executeInstruction_given_0xE108_and_PC_at_0xAAB2_and_ZERO_bit_is_high_expect_bnz_called_and_PC_is_0xAAB4(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x08, 0xE1, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xAAB2);
  //Set status flag (zero)
  status = STATUS_Z;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0xAAB2 + 2, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);     //test that bnz do not affect status flags
}

//----------------------------TEST BNOV---------------------------------

/*
Relative address    (n):  -128 to 127

Mnemonic: bnov n
Opcode: 1110 0101 nnnn nnnn

  n is the number of lines of instruction that need to be jumped

*/

//test for jumping forward
//  PC
// 0x1324   bnov   0x1370   ==> 1110 0101 0010 0101(0xE525)
void test_executeInstruction_given_0xE525_and_PC_at_0x1324_and_OV_bit_is_low_expect_bnov_called_and_PC_is_0x1370(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x25, 0xE5, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x1324);
  //Clear status flag (overflow)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x1370, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnov do not affect status flags
}

//test for jumping forward (whole range)
//  PC
// 0x6600   bnov   0x66FE   ==> 1110 0101 0111 1110(0xE57E)
void test_executeInstruction_given_0xE57E_and_PC_at_0x6600_and_OV_bit_is_low_expect_bnov_called_and_PC_is_0x66FE(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x7E, 0xE5, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6600);
  //Clear status flag (overflow)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x66FE, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnov do not affect status flags
}

//test for jumping backwards
//  PC
// 0x1A3A   bnov   0x1A06   ==> 1110 0101 1110 0101(0xE5E5)
void test_executeInstruction_given_0xE5E5_and_PC_at_0x1A3A_and_OV_bit_is_low_expect_bnov_called_and_PC_is_0x1A06(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xE5, 0xE5, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x1A3A);
  //Clear status flag (overflow)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x1A06, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnov do not affect status flags
}

//test for jumping backwards (whole range)
//  PC
// 0xFFFE   bnov   0xFF00   ==> 1110 0101 1000 0000(0xE580)
void test_executeInstruction_given_0xE580_and_PC_at_0xFFFE_and_OV_bit_is_low_expect_bnov_called_and_PC_is_0xFF00(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x80, 0xE5, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xFFFE);
  //Clear status flag (overflow)
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0xFF00, pc);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);         //test that bnov do not affect status flags
}

//test when OV bit is high, BNOV will not jump to target address, it continues to the next instruction instead, which causes PC to +2
//  PC
// 0x13B2   bnov   0x13C4   ==> 1110 0101 0000 1000(0xE508)
void test_executeInstruction_given_0xE508_and_PC_at_0x13B2_and_OV_bit_is_high_expect_bnov_called_and_PC_is_0x13B4(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x08, 0xE5, 0x00, 0xff};
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x13B2);
  //Set status flag (overflow)
  status = STATUS_OV;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_PTR(0x13B4, pc);
  TEST_ASSERT_EQUAL_HEX8(STATUS_OV, status);    //test that bnov do not affect status flags
}

//----------------------------TEST MOVWF---------------------------------

/*
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: movwf f, a
Opcode: 0110 111a ffff ffff
*/

//movwf   0x16, ACCESS   ==> 0110 1110 0001 0110(0x6E16)
void test_executeInstruction_given_0x6E16_expect_movwf_called_and_access_to_0x16_to_store_data_of_wreg_into_it(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x16, 0x6E, 0x00, 0xff};
  //Set WREG
  wreg = 0x5A;
  //Set content of target file reg
  fileRegisters[0x16] = 0xD3;
  fileRegisters[0x716] = 0x00;
  fileRegisters[0xF16] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x6780);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x5A, fileRegisters[0x16]);
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0x716]);       //check that the movwf instruction do not accidentally store the data in these file registers
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xF16]);
  TEST_ASSERT_EQUAL_HEX8(0x5A, wreg);                       //movwf instruction does not affect the data stored in wreg
  TEST_ASSERT_EQUAL_PTR(0x6780 + 2, pc);
}

//movwf   0xB9, ACCESS   ==> 0110 1110 1011 1001(0x6EB9)
void test_executeInstruction_given_0x6EB9_expect_movwf_called_and_access_to_0xFB9_to_store_data_of_wreg_into_it(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xB9, 0x6E, 0x00, 0xff};
  //Set WREG
  wreg = 0x66;
  //Set content of target file reg
  fileRegisters[0xB9] = 0x00;
  fileRegisters[0x3B9] = 0x00;
  fileRegisters[0xFB9] = 0x86;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x45FE);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x66, fileRegisters[0xFB9]);
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0x3B9]);
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xB9]);
  TEST_ASSERT_EQUAL_HEX8(0x66, wreg);
  TEST_ASSERT_EQUAL_PTR(0x45FE + 2, pc);
}

//movwf   0xEA, BANKED   ==> 0110 1111 1110 1010(0x6FEA)
void test_executeInstruction_given_0x6FEA_expect_movwf_called_and_access_to_0xDEA_to_store_data_of_wreg_into_it(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xEA, 0x6F, 0x00, 0xff};
  //Set BSR
  bsr = 0xD;
  //Set WREG
  wreg = 0x51;
  //Set content of target file reg
  fileRegisters[0xEA] = 0x00;
  fileRegisters[0xDEA] = 0x7B;
  fileRegisters[0xFEA] = 0x00;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x8052);
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x51, fileRegisters[0xDEA]);
  TEST_ASSERT_EQUAL_HEX8(0X00, fileRegisters[0xFEA]);
  TEST_ASSERT_EQUAL_HEX8(0X00, fileRegisters[0xEA]);
  TEST_ASSERT_EQUAL_HEX8(0x51, wreg);
  TEST_ASSERT_EQUAL_PTR(0x8052 + 2, pc);
}

//----------------------------TEST ANDWF---------------------------------

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: andwf f, d, a
Opcode: 0001 01da ffff ffff
*/

//  0001 01da ffff ffff
//andwf   0x02, f, ACCESS   ==> 0001 0110 0000 0010(0x1602)
void test_executeInstruction_given_0x1602_expect_andwf_called_and_access_to_0x02_and_the_anded_result_is_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x02, 0x16, 0x00, 0xff};
  //Set WREG
  wreg = 0x9E;
  //Set content of target file reg
  fileRegisters[0x02]  = 0xE7;
  fileRegisters[0xC02] = 0xFF;
  fileRegisters[0xF02] = 0xFF;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x2448);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x86, fileRegisters[0x02]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0xC02]);       //check that the andwf instruction does not accidentally AND wreg with these file register
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0xF02]);
  TEST_ASSERT_EQUAL_HEX8(0x9E, wreg);                       //check that result is not stored in wreg
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0x2448 + 2, pc);
}

//  0001 01da ffff ffff
//andwf   0x5F, w, ACCESS   ==> 0001 0100 0101 1111(0x145F)
void test_executeInstruction_given_0x145F_expect_andwf_called_and_access_to_0x5F_and_the_anded_result_is_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x5F, 0x14, 0x00, 0xff};
  //Set WREG
  wreg = 0x39;
  //Set content of target file reg
  fileRegisters[0x5F]  = 0x7C;
  fileRegisters[0x25F] = 0xFF;
  fileRegisters[0xF5F] = 0xFF;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x0330);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x7C, fileRegisters[0x5F]);        //check that the result is not stored in file register
  TEST_ASSERT_EQUAL_HEX8(0x38, wreg);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
  TEST_ASSERT_EQUAL_PTR(0x0330 + 2, pc);
}

//  0001 01da ffff ffff
//andwf   0x88, f, ACCESS   ==> 0001 0110 1000 1000(0x1688)
void test_executeInstruction_given_0x1688_expect_andwf_called_and_access_to_0xF88_and_the_anded_result_is_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x88, 0x16, 0x00, 0xff};
  //Set WREG
  wreg = 0xAA;
  //Set content of target file reg
  fileRegisters[0x88]  = 0xFF;
  fileRegisters[0x188] = 0xFF;
  fileRegisters[0xF88] = 0x55;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x0204);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xF88]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0x188]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0x88]);
  TEST_ASSERT_EQUAL_HEX8(0xAA, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x0204 + 2, pc);
}

//  0001 01da ffff ffff
//andwf   0xFF, w, ACCESS   ==> 0001 0100 1111 1111(0x14FF)
void test_executeInstruction_given_0x14FF_expect_andwf_called_and_access_to_0xFFF_and_the_anded_result_is_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xFF, 0x14, 0x00, 0xA2};
  //Set WREG
  wreg = 0xD2;
  //Set content of target file reg
  fileRegisters[0xFF]  = 0xFF;
  fileRegisters[0x9FF] = 0xFF;
  fileRegisters[0xFFF] = 0xED;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xEFFE);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xED, fileRegisters[0xFFF]);
  TEST_ASSERT_EQUAL_HEX8(0xC0, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0xEFFE + 2, pc);
}

//  0001 01da ffff ffff
//andwf   0x00, f, BANKED   ==> 0001 0111 0000 0000(0x1700)
void test_executeInstruction_given_0x1700_expect_andwf_called_and_access_to_0xE00_and_the_anded_result_is_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x00, 0x17, 0x09, 0xff};
  //Set BSR
  bsr = 0xE;
  //Set WREG
  wreg = 0x39;
  //Set content of target file reg
  fileRegisters[0x00]  = 0xFF;
  fileRegisters[0xE00] = 0xC6;
  fileRegisters[0xF00] = 0xFF;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x4058);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xE00]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0xF00]);
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0x00]);
  TEST_ASSERT_EQUAL_HEX8(0x39, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x4058 + 2, pc);
}

//  0001 01da ffff ffff
//andwf   0x48, w, BANKED   ==> 0001 0101 0100 1000(0x1548)
void test_executeInstruction_given_0x1548_expect_andwf_called_and_access_to_0x248_and_the_anded_result_is_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x48, 0x15, 0x00, 0xff};
  //Set BSR
  bsr = 0x2;
  //Set WREG
  wreg = 0x93;
  //Set content of target file reg
  fileRegisters[0x48]  = 0xFF;
  fileRegisters[0x248] = 0x1A;
  fileRegisters[0xF48] = 0xFF;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x3456);
  //Clear the status register for testing
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x1A, fileRegisters[0x248]);
  TEST_ASSERT_EQUAL_HEX8(0x12, wreg);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
  TEST_ASSERT_EQUAL_PTR(0x3456 + 2, pc);
}

//----------------------------TEST RLCF---------------------------------

/*
Direction     (d):    0 ==> WREG , 1 ==> file register
Bank          (a):    0 ==> ACCESS BANK, 1 ==> BANKED
File Register (f):    range from 0 to 255

Mnemonic: rlcf f, d, a
Opcode: 0011 01da ffff ffff
*/

/*
*            0x30      C    wreg
* before   10010111    0    0x86
* after    00101110    1    0x86
*/

//  0011 01da ffff ffff
//rlcf   0x30, f, ACCESS   ==> 0011 0110 0011 0000(0x3630)
void test_executeInstruction_given_0x3630_expect_rlcf_called_and_access_to_0x30_and_the_data_is_rotated_left_through_carry_and_result_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x30, 0x36, 0x00, 0xff};
  //Set WREG
  wreg = 0x86;
  //Set content of target file reg
  fileRegisters[0x30]  = 0x97;
  fileRegisters[0xA30] = 0x12;
  fileRegisters[0xF30] = 0x21;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x284A);
  //Clear the carry flag
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x2E, fileRegisters[0x30]);
  TEST_ASSERT_EQUAL_HEX8(0x12, fileRegisters[0xA30]);       //check that rlcf instruction do not accidentally rotate the wrong file register
  TEST_ASSERT_EQUAL_HEX8(0x21, fileRegisters[0xF30]);
  TEST_ASSERT_EQUAL_HEX8(0x86, wreg);                       //check that the result is not stored in wreg
  TEST_ASSERT_EQUAL_HEX8(STATUS_C, status);
  TEST_ASSERT_EQUAL_PTR(0x284A + 2, pc);
}

/*
*            0x4F      C    wreg
* before   00010111    0    0x98
* after    00010111    0    00101110
*/

//  0011 01da ffff ffff
//rlcf   0x4F, w, ACCESS   ==> 0011 0100 0100 1111(0x344F)
void test_executeInstruction_given_0x344F_expect_rlcf_called_and_access_to_0x4F_and_the_data_is_rotated_left_through_carry_and_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x4F, 0x34, 0x00, 0xff};
  //Set WREG
  wreg = 0x98;
  //Set content of target file reg
  fileRegisters[0x4F]  = 0x17;
  fileRegisters[0x24F] = 0x12;
  fileRegisters[0xF4F] = 0x21;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x26FA);
  //Clear the carry flag
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x17, fileRegisters[0x4F]);        //check that the result is not stored in file register
  TEST_ASSERT_EQUAL_HEX8(0x12, fileRegisters[0x24F]);
  TEST_ASSERT_EQUAL_HEX8(0x21, fileRegisters[0xF4F]);
  TEST_ASSERT_EQUAL_HEX8(0x2E, wreg);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
  TEST_ASSERT_EQUAL_PTR(0x26FA + 2, pc);
}

/*
*           0xF68      C    wreg
* before   10010111    1    0x01
* after    00101111    1    0X01
*/

//  0011 01da ffff ffff
//rlcf   0x68, f, ACCESS   ==> 0011 0110 0110 1000(0x3668)
void test_executeInstruction_given_0x3668_expect_rlcf_called_and_access_to_0xF68_and_the_data_is_rotated_left_through_carry_and_result_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x68, 0x36, 0x00, 0xff};
  //Set WREG
  wreg = 0x01;
  //Set content of target file reg
  fileRegisters[0x68]  = 0x08;
  fileRegisters[0x368] = 0x12;
  fileRegisters[0xF68] = 0x97;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xABCE);
  //Set the carry flag
  status = 0x01;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x2F, fileRegisters[0xF68]);
  TEST_ASSERT_EQUAL_HEX8(0x12, fileRegisters[0x368]);
  TEST_ASSERT_EQUAL_HEX8(0x08, fileRegisters[0x68]);
  TEST_ASSERT_EQUAL_HEX8(0x01, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_C, status);
  TEST_ASSERT_EQUAL_PTR(0xABCE + 2, pc);
}

/*
*           0xFF3      C    wreg
* before   00010111    1    0x50
* after    00010111    0    00101111
*/

//  0011 01da ffff ffff
//rlcf   0xF3, w, ACCESS   ==> 0011 0100 1111 0011(0x34F3)
void test_executeInstruction_given_0x34F3_expect_rlcf_called_and_access_to_0xFF3_and_the_data_is_rotated_left_through_carry_and_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0xF3, 0x34, 0x00, 0xff};
  //Set WREG
  wreg = 0x50;
  //Set content of target file reg
  fileRegisters[0xF3]  = 0x76;
  fileRegisters[0x7F3] = 0x10;
  fileRegisters[0xFF3] = 0x17;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xBCDA);
  //Set the carry flag
  status = 0x01;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x17, fileRegisters[0xFF3]);
  TEST_ASSERT_EQUAL_HEX8(0x10, fileRegisters[0x7F3]);
  TEST_ASSERT_EQUAL_HEX8(0x76, fileRegisters[0xF3]);
  TEST_ASSERT_EQUAL_HEX8(0x2F, wreg);
  TEST_ASSERT_EQUAL_HEX8(0x00, status);
  TEST_ASSERT_EQUAL_PTR(0xBCDA + 2, pc);
}

/*
*           0xC6D      C    wreg
* before   00000000    0    0x42
* after    00000000    0    0x42
*/

//  0011 01da ffff ffff
//rlcf   0x6D, f, BANKED   ==> 0011 0111 0110 1101(0x376D)
void test_executeInstruction_given_0x376D_expect_rlcf_called_and_access_to_0xC6D_and_the_data_is_rotated_left_through_carry_and_result_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x6D, 0x37, 0x00, 0xff};
  //Set BSR
  bsr = 0xC;
  //Set WREG
  wreg = 0x42;
  //Set content of target file reg
  fileRegisters[0x6D]  = 0x87;
  fileRegisters[0xC6D] = 0x00;
  fileRegisters[0xF6D] = 0x71;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x1234);
  //Clear the carry flag
  status = 0x00;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0x00, fileRegisters[0xC6D]);
  TEST_ASSERT_EQUAL_HEX8(0x71, fileRegisters[0xF6D]);
  TEST_ASSERT_EQUAL_HEX8(0x87, fileRegisters[0x6D]);
  TEST_ASSERT_EQUAL_HEX8(0x42, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_Z, status);
  TEST_ASSERT_EQUAL_PTR(0x1234 + 2, pc);
}

/*
*           0x123      C    wreg
* before   11111111    0    0x39
* after    11111111    1    11111110
*/

//  0011 01da ffff ffff
//rlcf   0x23, w, BANKED   ==> 0011 0101 0010 0011(0x3523)
void test_executeInstruction_given_0x3523_expect_rlcf_called_and_access_to_0x123_and_the_data_is_rotated_left_through_carry_and_result_stored_in_wreg(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x23, 0x35, 0x00, 0xff};
  //Set BSR
  bsr = 0x1;
  //Set WREG
  wreg = 0x39;
  //Set content of target file reg
  fileRegisters[0x23]  = 0x09;
  fileRegisters[0x123] = 0xFF;
  fileRegisters[0xF23] = 0x38;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0x8684);
  //Clear the carry flag
  status = 0x00;
  //Run the code under test
  executeInstruction(' ');
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0x123]);
  TEST_ASSERT_EQUAL_HEX8(0x38, fileRegisters[0xF23]);
  TEST_ASSERT_EQUAL_HEX8(0x09, fileRegisters[0x23]);
  TEST_ASSERT_EQUAL_HEX8(0xFE, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N | STATUS_C, status);
  TEST_ASSERT_EQUAL_PTR(0x8684 + 2, pc);
}

//EXTRA

/*
*           0xE00      C    wreg
* before   01111111    1    0x9A
* after    11111111    0    0x9A
*/

//  0011 01da ffff ffff
//rlcf   0x00, f, BANKED   ==> 0011 0111 0000 0000(0x3700)
void test_executeInstruction_given_0x3700_expect_rlcf_called_and_access_to_0xE00_and_the_data_is_rotated_left_through_carry_and_result_stored_in_file_register(void) {
  //Setup test fixture
  uint8_t machineCode[] = {0x00, 0x37, 0xE2, 0xff};
  //Set BSR
  bsr = 0xE;
  //Set WREG
  wreg = 0x9A;
  //Set content of target file reg
  fileRegisters[0x00]  = 0x5B;
  fileRegisters[0xE00] = 0x7F;
  fileRegisters[0xF00] = 0x11;
  //Copy instructions to the code memory
  copyCodeToCodeMemory(machineCode, pc = 0xA8B0);
  //Set the carry flag
  status = 0x01;
  //Run the code under test
  executeInstruction();
  //Verify the code has expected output
  TEST_ASSERT_EQUAL_HEX8(0xFF, fileRegisters[0xE00]);
  TEST_ASSERT_EQUAL_HEX8(0x11, fileRegisters[0xF00]);
  TEST_ASSERT_EQUAL_HEX8(0x5B, fileRegisters[0x00]);
  TEST_ASSERT_EQUAL_HEX8(0x9A, wreg);
  TEST_ASSERT_EQUAL_HEX8(STATUS_N, status);
  TEST_ASSERT_EQUAL_PTR(0xA8B0 + 2, pc);
}
