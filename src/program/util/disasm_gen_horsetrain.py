#!/usr/bin/env python3
if __name__ != "__main__":
    raise ImportError("[buffoon alert] just run the script bro :(") # (i just dont wanna indent main later, formatting concern)

from collections import defaultdict
import os
os.chdir(os.path.dirname(__file__))
OUTPUT_FILENAME = "disasm.cpp"


# This script maps each line into a cpp conditional+formatting block, and merges these
# into a routine that takes a u32 instruction and outputs a very rough mnemonic string.
# Thank you HorseTrain https://github.com/HorseTrain/Aarch64-Instruction-Table/blob/main/encodings.txt
horsetrain_src_text = """00000100--011000000-------------;ORR (vectors, predicated);SVE bitwise logical operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011001000-------------;EOR (vectors, predicated);SVE bitwise logical operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011010000-------------;AND (vectors, predicated);SVE bitwise logical operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011011000-------------;BIC (vectors, predicated);SVE bitwise logical operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--0111--000-------------;UNALLOCATED;SVE bitwise logical operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--000000000-------------;ADD (vectors, predicated);SVE integer add/subtract vectors (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--000001000-------------;SUB (vectors, predicated);SVE integer add/subtract vectors (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--000010000-------------;UNALLOCATED;SVE integer add/subtract vectors (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--000011000-------------;SUBR (vectors);SVE integer add/subtract vectors (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--0001--000-------------;UNALLOCATED;SVE integer add/subtract vectors (predicated);;size 22 2,opc 16 3,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010100000-------------;SDIV;SVE integer divide vectors (predicated);;size 22 2,R 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010101000-------------;UDIV;SVE integer divide vectors (predicated);;size 22 2,R 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010110000-------------;SDIVR;SVE integer divide vectors (predicated);;size 22 2,R 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010111000-------------;UDIVR;SVE integer divide vectors (predicated);;size 22 2,R 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--001000000-------------;SMAX (vectors);SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--001001000-------------;UMAX (vectors);SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--001010000-------------;SMIN (vectors);SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--001011000-------------;UMIN (vectors);SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--001100000-------------;SABD;SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--001101000-------------;UABD;SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--00111-000-------------;UNALLOCATED;SVE integer min/max/difference (predicated);;size 22 2,opc 17 2,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010000000-------------;MUL (vectors);SVE integer multiply vectors (predicated);;size 22 2,H 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010001000-------------;UNALLOCATED;SVE integer multiply vectors (predicated);;size 22 2,H 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010010000-------------;SMULH;SVE integer multiply vectors (predicated);;size 22 2,H 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010011000-------------;UMULH;SVE integer multiply vectors (predicated);;size 22 2,H 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011000001-------------;ORV;SVE bitwise logical reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--011001001-------------;EORV;SVE bitwise logical reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--011010001-------------;ANDV;SVE bitwise logical reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--011011001-------------;UNALLOCATED;SVE bitwise logical reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--01000-001-------------;MOVPRFX (predicated);SVE constructive prefix (predicated);;size 22 2,opc 17 2,M 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--01001-001-------------;UNALLOCATED;SVE constructive prefix (predicated);;size 22 2,opc 17 2,M 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--0101--001-------------;UNALLOCATED;SVE constructive prefix (predicated);;size 22 2,opc 17 2,M 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--000000001-------------;SADDV;SVE integer add reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--000001001-------------;UADDV;SVE integer add reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--00001-001-------------;UNALLOCATED;SVE integer add reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--001000001-------------;SMAXV;SVE integer min/max reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--001001001-------------;UMAXV;SVE integer min/max reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--001010001-------------;SMINV;SVE integer min/max reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--001011001-------------;UMINV;SVE integer min/max reduction (predicated);;size 22 2,op 17 1,U 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000100--000000100-------------;ASR (immediate, predicated);SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--000001100-------------;LSR (immediate, predicated);SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--000010100-------------;UNALLOCATED;SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--000011100-------------;LSL (immediate, predicated);SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--000100100-------------;ASRD;SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--000101100-------------;UNALLOCATED;SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--00011-100-------------;UNALLOCATED;SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--001---100-------------;UNALLOCATED;SVE bitwise shift by immediate (predicated);;tszh 22 2,opc 18 2,L 17 1,U 16 1,Pg 10 3,tszl 8 2,imm3 5 3,Zdn 0 5,;
00000100--010-10100-------------;UNALLOCATED;SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010000100-------------;ASR (vectors);SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010001100-------------;LSR (vectors);SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010011100-------------;LSL (vectors);SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010100100-------------;ASRR;SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010101100-------------;LSRR;SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--010111100-------------;LSLR;SVE bitwise shift by vector (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011000100-------------;ASR (wide elements, predicated);SVE bitwise shift by wide elements (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011001100-------------;LSR (wide elements, predicated);SVE bitwise shift by wide elements (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011010100-------------;UNALLOCATED;SVE bitwise shift by wide elements (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--011011100-------------;LSL (wide elements, predicated);SVE bitwise shift by wide elements (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000100--0111--100-------------;UNALLOCATED;SVE bitwise shift by wide elements (predicated);;size 22 2,R 18 1,L 17 1,U 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
11010101000000110011----000-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011----001-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,; Rt != 11111,
11010101000000110011----01011111;CLREX;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011----10011111;DSB;Barriers;Memory barrier;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011----10111111;DMB;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011----11011111;ISB;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011----111-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,; Rt != 11111,
11010101000000110011----11111111;SB;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011--0-00111111;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011--1000111111;DSB;Barriers;Memory nXS barrier;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011--1100111111;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
110101010000001100110001011-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110011001-011-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
1101010100000011001101--011-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
110101010000001100111---011-----;UNALLOCATED;Barriers;;CRm 8 4,op2 5 3,Rt 0 5,;
00110100------------------------;CBZ;Compare and branch (immediate);32-bit;sf 31 1,op 24 1,imm19 5 19,Rt 0 5,;
00110101------------------------;CBNZ;Compare and branch (immediate);32-bit;sf 31 1,op 24 1,imm19 5 19,Rt 0 5,;
10110100------------------------;CBZ;Compare and branch (immediate);64-bit;sf 31 1,op 24 1,imm19 5 19,Rt 0 5,;
10110101------------------------;CBNZ;Compare and branch (immediate);64-bit;sf 31 1,op 24 1,imm19 5 19,Rt 0 5,;
01010100-------------------0----;B.cond;Conditional branch (immediate);;o1 24 1,imm19 5 19,o0 4 1,cond 0 4,;
01010100-------------------1----;UNALLOCATED;Conditional branch (immediate);;o1 24 1,imm19 5 19,o0 4 1,cond 0 4,;
01010101------------------------;UNALLOCATED;Conditional branch (immediate);;o1 24 1,imm19 5 19,o0 4 1,cond 0 4,;
11010100-------------------001--;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100-------------------01---;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100-------------------1----;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100000----------------00000;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100000----------------00001;SVC;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100000----------------00010;HVC;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100000----------------00011;SMC;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100001----------------000-1;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100001----------------00000;BRK;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100001----------------0001-;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100010----------------000-1;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100010----------------00000;HLT;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100010----------------0001-;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100011----------------00001;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100011----------------0001-;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100100----------------000--;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100101----------------00000;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100101----------------00001;DCPS1;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100101----------------00010;DCPS2;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100101----------------00011;DCPS3;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100110----------------000--;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010100111----------------000--;UNALLOCATED;Exception generation;;opc 21 3,imm16 5 16,op2 2 3,LL 0 2,;
11010101000000110010-------11111;HINT;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000000011111;NOP;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000000111111;YIELD;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000001011111;WFE;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000001111111;WFI;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000010011111;SEV;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000010111111;SEVL;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000011011111;DGH;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010000011111111;XPACD, XPACI, XPACLRI;Hints;System;CRm 8 4,op2 5 3,;
11010101000000110010000100011111;PACIA, PACIA1716, PACIASP, PACIAZ, PACIZA;Hints;System, PACIA1716;CRm 8 4,op2 5 3,;
11010101000000110010000101011111;PACIB, PACIB1716, PACIBSP, PACIBZ, PACIZB;Hints;System, PACIB1716;CRm 8 4,op2 5 3,;
11010101000000110010000110011111;AUTIA, AUTIA1716, AUTIASP, AUTIAZ, AUTIZA;Hints;System, AUTIA1716;CRm 8 4,op2 5 3,;
11010101000000110010000111011111;AUTIB, AUTIB1716, AUTIBSP, AUTIBZ, AUTIZB;Hints;System, AUTIB1716;CRm 8 4,op2 5 3,;
11010101000000110010001000011111;ESB;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010001000111111;PSB CSYNC;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010001001011111;TSB CSYNC;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010001010011111;CSDB;Hints;;CRm 8 4,op2 5 3,;
11010101000000110010001100011111;PACIA, PACIA1716, PACIASP, PACIAZ, PACIZA;Hints;System, PACIAZ;CRm 8 4,op2 5 3,;
11010101000000110010001100111111;PACIA, PACIA1716, PACIASP, PACIAZ, PACIZA;Hints;System, PACIASP;CRm 8 4,op2 5 3,;
11010101000000110010001101011111;PACIB, PACIB1716, PACIBSP, PACIBZ, PACIZB;Hints;System, PACIBZ;CRm 8 4,op2 5 3,;
11010101000000110010001101111111;PACIB, PACIB1716, PACIBSP, PACIBZ, PACIZB;Hints;System, PACIBSP;CRm 8 4,op2 5 3,;
11010101000000110010001110011111;AUTIA, AUTIA1716, AUTIASP, AUTIAZ, AUTIZA;Hints;System, AUTIAZ;CRm 8 4,op2 5 3,;
11010101000000110010001110111111;AUTIA, AUTIA1716, AUTIASP, AUTIAZ, AUTIZA;Hints;System, AUTIASP;CRm 8 4,op2 5 3,;
11010101000000110010001111011111;AUTIB, AUTIB1716, AUTIBSP, AUTIBZ, AUTIZB;Hints;System, AUTIBZ;CRm 8 4,op2 5 3,;
11010101000000110010001111111111;AUTIB, AUTIB1716, AUTIBSP, AUTIBZ, AUTIZB;Hints;System, AUTIBSP;CRm 8 4,op2 5 3,;
110101010000001100100100--011111;BTI;Hints;;CRm 8 4,op2 5 3,;
1101010100000---0100------------;UNALLOCATED;PSTATE;;op1 16 3,CRm 8 4,op2 5 3,Rt 0 5,; Rt != 11111,
1101010100000---0100-------11111;MSR (immediate);PSTATE;;op1 16 3,CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000000100----00011111;CFINV;PSTATE;;op1 16 3,CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000000100----00111111;XAFLAG;PSTATE;;op1 16 3,CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000000100----01011111;AXFLAG;PSTATE;;op1 16 3,CRm 8 4,op2 5 3,Rt 0 5,;
1101010100001-------------------;SYS;System instructions;;L 21 1,op1 16 3,CRn 12 4,CRm 8 4,op2 5 3,Rt 0 5,;
1101010100101-------------------;SYSL;System instructions;;L 21 1,op1 16 3,CRn 12 4,CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110001------------;UNALLOCATED;System instructions with register argument;;CRm 8 4,op2 5 3,Rt 0 5,; CRm != 0000,
110101010000001100010000000-----;WFET;System instructions with register argument;;CRm 8 4,op2 5 3,Rt 0 5,;
110101010000001100010000001-----;WFIT;System instructions with register argument;;CRm 8 4,op2 5 3,Rt 0 5,;
11010101000000110001000001------;UNALLOCATED;System instructions with register argument;;CRm 8 4,op2 5 3,Rt 0 5,;
1101010100000011000100001-------;UNALLOCATED;System instructions with register argument;;CRm 8 4,op2 5 3,Rt 0 5,;
110101010001--------------------;MSR (register);System register move;;L 21 1,o0 19 1,op1 16 3,CRn 12 4,CRm 8 4,op2 5 3,Rt 0 5,;
110101010011--------------------;MRS;System register move;;L 21 1,o0 19 1,op1 16 3,CRn 12 4,CRm 8 4,op2 5 3,Rt 0 5,;
-0110110------------------------;TBZ;Test and branch (immediate);;b5 31 1,op 24 1,b40 19 5,imm14 5 14,Rt 0 5,;
-0110111------------------------;TBNZ;Test and branch (immediate);;b5 31 1,op 24 1,b40 19 5,imm14 5 14,Rt 0 5,;
000101--------------------------;B;Unconditional branch (immediate);;op 31 1,imm26 0 26,;
100101--------------------------;BL;Unconditional branch (immediate);;op 31 1,imm26 0 26,;
1101011-------------------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op2 != 11111,
1101011000011111000000----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 00000,
1101011000011111000000-----00000;BR;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000011111000001----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000011111000010----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 11111,
1101011000011111000010-----11111;BRAA, BRAAZ, BRAB, BRABZ;Unconditional branch (register);Key A, zero modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000011111000011----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 11111,
1101011000011111000011-----11111;BRAA, BRAAZ, BRAB, BRABZ;Unconditional branch (register);Key B, zero modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110000111110001------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000011111001-------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101100001111101--------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110000111111---------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000111111000000----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 00000,
1101011000111111000000-----00000;BLR;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000111111000001----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000111111000010----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 11111,
1101011000111111000010-----11111;BLRAA, BLRAAZ, BLRAB, BLRABZ;Unconditional branch (register);Key A, zero modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000111111000011----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 11111,
1101011000111111000011-----11111;BLRAA, BLRAAZ, BLRAB, BLRABZ;Unconditional branch (register);Key B, zero modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110001111110001------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011000111111001-------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101100011111101--------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110001111111---------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011001011111000000----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 00000,
1101011001011111000000-----00000;RET;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011001011111000001----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011001011111000010----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111, op4 != 11111,
11010110010111110000101111111111;RETAA, RETAB;Unconditional branch (register);RETAA;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011001011111000011----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111, op4 != 11111,
11010110010111110000111111111111;RETAA, RETAB;Unconditional branch (register);RETAB;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110010111110001------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011001011111001-------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101100101111101--------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110010111111---------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011001111111----------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011010011111000000----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111, op4 != 00000,
1101011010011111000000-----00000;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111,
110101101001111100000011111-----;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 00000,
11010110100111110000001111100000;ERET;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011010011111000001----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011010011111000010----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111, op4 != 11111,
1101011010011111000010-----11111;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111,
110101101001111100001011111-----;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 11111,
11010110100111110000101111111111;ERETAA, ERETAB;Unconditional branch (register);ERETAA;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011010011111000011----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111, op4 != 11111,
1101011010011111000011-----11111;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111,
110101101001111100001111111-----;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 11111,
11010110100111110000111111111111;ERETAA, ERETAB;Unconditional branch (register);ERETAB;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110100111110001------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011010011111001-------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101101001111101--------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010110100111111---------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011010111111----------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op3 != 000000,
1101011010111111000000----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111, op4 != 00000,
1101011010111111000000-----00000;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; Rn != 11111,
110101101011111100000011111-----;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,; op4 != 00000,
11010110101111110000001111100000;DRPS;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011011-11111----------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101110001111100000-----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011100011111000010----------;BRAA, BRAAZ, BRAB, BRABZ;Unconditional branch (register);Key A, register modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011100011111000011----------;BRAA, BRAAZ, BRAB, BRABZ;Unconditional branch (register);Key B, register modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010111000111110001------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011100011111001-------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101110001111101--------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010111000111111---------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101110011111100000-----------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011100111111000010----------;BLRAA, BLRAAZ, BLRAB, BLRABZ;Unconditional branch (register);Key A, register modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011100111111000011----------;BLRAA, BLRAAZ, BLRAB, BLRABZ;Unconditional branch (register);Key B, register modifier;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010111001111110001------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011100111111001-------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101110011111101--------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
11010111001111111---------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
1101011101-11111----------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
110101111--11111----------------;UNALLOCATED;Unconditional branch (register);;opc 21 4,op2 16 5,op3 10 6,Rn 5 5,op4 0 5,;
00000100--011000101-------------;CLS;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011001101-------------;CLZ;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011010101-------------;CNT;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011011101-------------;CNOT;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011100101-------------;FABS;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011101101-------------;FNEG;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011110101-------------;NOT (vector);SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--011111101-------------;UNALLOCATED;SVE bitwise unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010000101-------------;SXTB, SXTH, SXTW;SVE integer unary operations (predicated);Byte;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010001101-------------;UXTB, UXTH, UXTW;SVE integer unary operations (predicated);Byte;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010010101-------------;SXTB, SXTH, SXTW;SVE integer unary operations (predicated);Halfword;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010011101-------------;UXTB, UXTH, UXTW;SVE integer unary operations (predicated);Halfword;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010100101-------------;SXTB, SXTH, SXTW;SVE integer unary operations (predicated);Word;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010101101-------------;UXTB, UXTH, UXTW;SVE integer unary operations (predicated);Word;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010110101-------------;ABS;SVE integer unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
00000100--010111101-------------;NEG;SVE integer unary operations (predicated);;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
0-001100000000000000------------;ST4 (multiple structures);Advanced SIMD load/store multiple structures;No offset;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000001------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000010------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, Four registers;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000011------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000100------------;ST3 (multiple structures);Advanced SIMD load/store multiple structures;No offset;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000101------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000110------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, Three registers;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000000111------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, One register;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000001000------------;ST2 (multiple structures);Advanced SIMD load/store multiple structures;No offset;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000001001------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000001010------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, Two registers;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100000000001011------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-0011000000000011--------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000000------------;LD4 (multiple structures);Advanced SIMD load/store multiple structures;No offset;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000001------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000010------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, Four registers;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000011------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000100------------;LD3 (multiple structures);Advanced SIMD load/store multiple structures;No offset;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000101------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000110------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, Three registers;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000000111------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, One register;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000001000------------;LD2 (multiple structures);Advanced SIMD load/store multiple structures;No offset;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000001001------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000001010------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures;No offset, Two registers;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100010000001011------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-0011000100000011--------------;UNALLOCATED;Advanced SIMD load/store multiple structures;;Q 30 1,L 22 1,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----0001------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----0011------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----0101------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----1001------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----1011------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----11--------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100-----0000------------;ST4 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100-----0010------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Four registers, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100-----0100------------;ST3 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100-----0110------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Three registers, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100-----0111------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, One register, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100-----1000------------;ST2 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100-----1010------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Two registers, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100100111110000------------;ST4 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100111110010------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Four registers, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100111110100------------;ST3 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100111110110------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Three registers, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100111110111------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, One register, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100111111000------------;ST2 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100100111111010------------;ST1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Two registers, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----0001------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----0011------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----0101------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----1001------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----1011------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----11--------------;UNALLOCATED;Advanced SIMD load/store multiple structures (post-indexed);;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110-----0000------------;LD4 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110-----0010------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Four registers, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110-----0100------------;LD3 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110-----0110------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Three registers, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110-----0111------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, One register, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110-----1000------------;LD2 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110-----1010------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Two registers, register offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001100110111110000------------;LD4 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110111110010------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Four registers, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110111110100------------;LD3 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110111110110------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Three registers, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110111110111------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, One register, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110111111000------------;LD2 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-001100110111111010------------;LD1 (multiple structures);Advanced SIMD load/store multiple structures (post-indexed);Post-index, Two registers, immediate offset;Q 30 1,L 22 1,Rm 16 5,opcode 12 4,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100-0000011--------------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000000-------------;ST1 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000001-------------;ST3 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000010--0----------;ST1 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000010--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000011--0----------;ST3 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000011--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000100-00----------;ST1 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000100-1-----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000100001----------;ST1 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000100101----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000101-00----------;ST3 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000101-10----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000101001----------;ST3 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100000000101011----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101000000001011-1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000000-------------;ST2 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000001-------------;ST4 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000010--0----------;ST2 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000010--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000011--0----------;ST4 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000011--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000100-00----------;ST2 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000100-10----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000100001----------;ST2 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000100011----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101001000001001-1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000101-00----------;ST4 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000101-10----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000101001----------;ST4 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110100100000101011----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101001000001011-1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000000-------------;LD1 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000001-------------;LD3 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000010--0----------;LD1 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000010--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000011--0----------;LD3 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000011--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000100-00----------;LD1 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000100-1-----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000100001----------;LD1 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000100101----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000101-00----------;LD3 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000101-10----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000101001----------;LD3 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101000000101011----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101010000001011-1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101010000001100------------;LD1R;Advanced SIMD load/store single structure;No offset;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101010000001101------------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101010000001110------------;LD3R;Advanced SIMD load/store single structure;No offset;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101010000001111------------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000000-------------;LD2 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000001-------------;LD4 (single structure);Advanced SIMD load/store single structure;No offset, 8-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000010--0----------;LD2 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000010--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000011--0----------;LD4 (single structure);Advanced SIMD load/store single structure;No offset, 16-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000011--1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000100-00----------;LD2 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000100-10----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000100001----------;LD2 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000100011----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101011000001001-1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000101-00----------;LD4 (single structure);Advanced SIMD load/store single structure;No offset, 32-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000101-10----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000101001----------;LD4 (single structure);Advanced SIMD load/store single structure;No offset, 64-bit;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110101100000101011----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101011000001011-1----------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101011000001100------------;LD2R;Advanced SIMD load/store single structure;No offset;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101011000001101------------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101011000001110------------;LD4R;Advanced SIMD load/store single structure;No offset;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101011000001111------------;UNALLOCATED;Advanced SIMD load/store single structure;;Q 30 1,L 22 1,R 21 1,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110------11--------------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----010--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----011--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----100-1-----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----100101----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----101-10----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----101011----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----1011-1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101100-----000-------------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----001-------------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----010--0----------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----011--0----------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----100-00----------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----100001----------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----101-00----------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101100-----101001----------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-00110110011111000-------------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111001-------------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111010--0----------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111011--0----------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111100-00----------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111100001----------;ST1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111101-00----------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110011111101001----------;ST3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----010--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----011--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----100-10----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----100011----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----1001-1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----101-10----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----101011----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----1011-1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101101-----000-------------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----001-------------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----010--0----------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----011--0----------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----100-00----------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----100001----------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----101-00----------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101101-----101001----------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-00110110111111000-------------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111001-------------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111010--0----------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111011--0----------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111100-00----------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111100001----------;ST2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111101-00----------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110110111111101001----------;ST4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----010--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----011--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----100-1-----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----100101----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----101-10----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----101011----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----1011-1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----1101------------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----1111------------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110-----000-------------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----001-------------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----010--0----------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----011--0----------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----100-00----------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----100001----------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----101-00----------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----101001----------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----1100------------;LD1R;Advanced SIMD load/store single structure (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101110-----1110------------;LD3R;Advanced SIMD load/store single structure (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-00110111011111000-------------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111001-------------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111010--0----------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111011--0----------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111100-00----------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111100001----------;LD1 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111101-00----------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111011111101001----------;LD3 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110111111100------------;LD1R;Advanced SIMD load/store single structure (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101110111111110------------;LD3R;Advanced SIMD load/store single structure (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----010--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----011--1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----100-10----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----100011----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----1001-1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----101-10----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----101011----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----1011-1----------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----1101------------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----1111------------;UNALLOCATED;Advanced SIMD load/store single structure (post-indexed);;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111-----000-------------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----001-------------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----010--0----------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----011--0----------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----100-00----------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----100001----------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----101-00----------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----101001----------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----1100------------;LD2R;Advanced SIMD load/store single structure (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-001101111-----1110------------;LD4R;Advanced SIMD load/store single structure (post-indexed);Post-index, Register offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,; Rm != 11111,
0-00110111111111000-------------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111001-------------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 8-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111010--0----------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111011--0----------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 16-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111100-00----------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111100001----------;LD2 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111101-00----------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 32-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-00110111111111101001----------;LD4 (single structure);Advanced SIMD load/store single structure (post-indexed);Post-index, 64-bit, immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111111111100------------;LD2R;Advanced SIMD load/store single structure (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
0-001101111111111110------------;LD4R;Advanced SIMD load/store single structure (post-indexed);Post-index, Immediate offset;Q 30 1,L 22 1,R 21 1,Rm 16 5,opcode 13 3,S 12 1,size 10 2,Rn 5 5,Rt 0 5,;
--111000--1-----111-00----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--1110000-1-----110000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000011-----100100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000011-----101000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000011-----101100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000011-----110100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000101-----100100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000101-----101000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000101-----101100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000101-----110100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000111-----100100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000111-----101000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000111-----101100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000111-----110000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111000111-----110100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--111100--1---------00----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----000000----------;LDADDB, LDADDAB, LDADDALB, LDADDLB;Atomic memory operations;LDADDB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----000100----------;LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB;Atomic memory operations;LDCLRB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----001000----------;LDEORB, LDEORAB, LDEORALB, LDEORLB;Atomic memory operations;LDEORB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----001100----------;LDSETB, LDSETAB, LDSETALB, LDSETLB;Atomic memory operations;LDSETB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----010000----------;LDSMAXB, LDSMAXAB, LDSMAXALB, LDSMAXLB;Atomic memory operations;LDSMAXB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----010100----------;LDSMINB, LDSMINAB, LDSMINALB, LDSMINLB;Atomic memory operations;LDSMINB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----011000----------;LDUMAXB, LDUMAXAB, LDUMAXALB, LDUMAXLB;Atomic memory operations;LDUMAXB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----011100----------;LDUMINB, LDUMINAB, LDUMINALB, LDUMINLB;Atomic memory operations;LDUMINB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----100000----------;SWPB, SWPAB, SWPALB, SWPLB;Atomic memory operations;SWPB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----100100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----101000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----101100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000001-----110100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----000000----------;LDADDB, LDADDAB, LDADDALB, LDADDLB;Atomic memory operations;LDADDLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----000100----------;LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB;Atomic memory operations;LDCLRLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----001000----------;LDEORB, LDEORAB, LDEORALB, LDEORLB;Atomic memory operations;LDEORLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----001100----------;LDSETB, LDSETAB, LDSETALB, LDSETLB;Atomic memory operations;LDSETLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----010000----------;LDSMAXB, LDSMAXAB, LDSMAXALB, LDSMAXLB;Atomic memory operations;LDSMAXLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----010100----------;LDSMINB, LDSMINAB, LDSMINALB, LDSMINLB;Atomic memory operations;LDSMINLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----011000----------;LDUMAXB, LDUMAXAB, LDUMAXALB, LDUMAXLB;Atomic memory operations;LDUMAXLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----011100----------;LDUMINB, LDUMINAB, LDUMINALB, LDUMINLB;Atomic memory operations;LDUMINLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000011-----100000----------;SWPB, SWPAB, SWPALB, SWPLB;Atomic memory operations;SWPLB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----000000----------;LDADDB, LDADDAB, LDADDALB, LDADDLB;Atomic memory operations;LDADDAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----000100----------;LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB;Atomic memory operations;LDCLRAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----001000----------;LDEORB, LDEORAB, LDEORALB, LDEORLB;Atomic memory operations;LDEORAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----001100----------;LDSETB, LDSETAB, LDSETALB, LDSETLB;Atomic memory operations;LDSETAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----010000----------;LDSMAXB, LDSMAXAB, LDSMAXALB, LDSMAXLB;Atomic memory operations;LDSMAXAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----010100----------;LDSMINB, LDSMINAB, LDSMINALB, LDSMINLB;Atomic memory operations;LDSMINAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----011000----------;LDUMAXB, LDUMAXAB, LDUMAXALB, LDUMAXLB;Atomic memory operations;LDUMAXAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----011100----------;LDUMINB, LDUMINAB, LDUMINALB, LDUMINLB;Atomic memory operations;LDUMINAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----100000----------;SWPB, SWPAB, SWPALB, SWPLB;Atomic memory operations;SWPAB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000101-----110000----------;LDAPRB;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----000000----------;LDADDB, LDADDAB, LDADDALB, LDADDLB;Atomic memory operations;LDADDALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----000100----------;LDCLRB, LDCLRAB, LDCLRALB, LDCLRLB;Atomic memory operations;LDCLRALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----001000----------;LDEORB, LDEORAB, LDEORALB, LDEORLB;Atomic memory operations;LDEORALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----001100----------;LDSETB, LDSETAB, LDSETALB, LDSETLB;Atomic memory operations;LDSETALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----010000----------;LDSMAXB, LDSMAXAB, LDSMAXALB, LDSMAXLB;Atomic memory operations;LDSMAXALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----010100----------;LDSMINB, LDSMINAB, LDSMINALB, LDSMINLB;Atomic memory operations;LDSMINALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----011000----------;LDUMAXB, LDUMAXAB, LDUMAXALB, LDUMAXLB;Atomic memory operations;LDUMAXALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----011100----------;LDUMINB, LDUMINAB, LDUMINALB, LDUMINLB;Atomic memory operations;LDUMINALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
00111000111-----100000----------;SWPB, SWPAB, SWPALB, SWPLB;Atomic memory operations;SWPALB;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----000000----------;LDADDH, LDADDAH, LDADDALH, LDADDLH;Atomic memory operations;LDADDH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----000100----------;LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH;Atomic memory operations;LDCLRH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----001000----------;LDEORH, LDEORAH, LDEORALH, LDEORLH;Atomic memory operations;LDEORH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----001100----------;LDSETH, LDSETAH, LDSETALH, LDSETLH;Atomic memory operations;LDSETH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----010000----------;LDSMAXH, LDSMAXAH, LDSMAXALH, LDSMAXLH;Atomic memory operations;LDSMAXH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----010100----------;LDSMINH, LDSMINAH, LDSMINALH, LDSMINLH;Atomic memory operations;LDSMINH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----011000----------;LDUMAXH, LDUMAXAH, LDUMAXALH, LDUMAXLH;Atomic memory operations;LDUMAXH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----011100----------;LDUMINH, LDUMINAH, LDUMINALH, LDUMINLH;Atomic memory operations;LDUMINH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----100000----------;SWPH, SWPAH, SWPALH, SWPLH;Atomic memory operations;SWPH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----100100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----101000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----101100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000001-----110100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----000000----------;LDADDH, LDADDAH, LDADDALH, LDADDLH;Atomic memory operations;LDADDLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----000100----------;LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH;Atomic memory operations;LDCLRLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----001000----------;LDEORH, LDEORAH, LDEORALH, LDEORLH;Atomic memory operations;LDEORLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----001100----------;LDSETH, LDSETAH, LDSETALH, LDSETLH;Atomic memory operations;LDSETLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----010000----------;LDSMAXH, LDSMAXAH, LDSMAXALH, LDSMAXLH;Atomic memory operations;LDSMAXLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----010100----------;LDSMINH, LDSMINAH, LDSMINALH, LDSMINLH;Atomic memory operations;LDSMINLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----011000----------;LDUMAXH, LDUMAXAH, LDUMAXALH, LDUMAXLH;Atomic memory operations;LDUMAXLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----011100----------;LDUMINH, LDUMINAH, LDUMINALH, LDUMINLH;Atomic memory operations;LDUMINLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000011-----100000----------;SWPH, SWPAH, SWPALH, SWPLH;Atomic memory operations;SWPLH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----000000----------;LDADDH, LDADDAH, LDADDALH, LDADDLH;Atomic memory operations;LDADDAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----000100----------;LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH;Atomic memory operations;LDCLRAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----001000----------;LDEORH, LDEORAH, LDEORALH, LDEORLH;Atomic memory operations;LDEORAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----001100----------;LDSETH, LDSETAH, LDSETALH, LDSETLH;Atomic memory operations;LDSETAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----010000----------;LDSMAXH, LDSMAXAH, LDSMAXALH, LDSMAXLH;Atomic memory operations;LDSMAXAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----010100----------;LDSMINH, LDSMINAH, LDSMINALH, LDSMINLH;Atomic memory operations;LDSMINAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----011000----------;LDUMAXH, LDUMAXAH, LDUMAXALH, LDUMAXLH;Atomic memory operations;LDUMAXAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----011100----------;LDUMINH, LDUMINAH, LDUMINALH, LDUMINLH;Atomic memory operations;LDUMINAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----100000----------;SWPH, SWPAH, SWPALH, SWPLH;Atomic memory operations;SWPAH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000101-----110000----------;LDAPRH;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----000000----------;LDADDH, LDADDAH, LDADDALH, LDADDLH;Atomic memory operations;LDADDALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----000100----------;LDCLRH, LDCLRAH, LDCLRALH, LDCLRLH;Atomic memory operations;LDCLRALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----001000----------;LDEORH, LDEORAH, LDEORALH, LDEORLH;Atomic memory operations;LDEORALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----001100----------;LDSETH, LDSETAH, LDSETALH, LDSETLH;Atomic memory operations;LDSETALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----010000----------;LDSMAXH, LDSMAXAH, LDSMAXALH, LDSMAXLH;Atomic memory operations;LDSMAXALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----010100----------;LDSMINH, LDSMINAH, LDSMINALH, LDSMINLH;Atomic memory operations;LDSMINALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----011000----------;LDUMAXH, LDUMAXAH, LDUMAXALH, LDUMAXLH;Atomic memory operations;LDUMAXALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----011100----------;LDUMINH, LDUMINAH, LDUMINALH, LDUMINLH;Atomic memory operations;LDUMINALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
01111000111-----100000----------;SWPH, SWPAH, SWPALH, SWPLH;Atomic memory operations;SWPALH;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;32-bit LDADD;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;32-bit LDCLR;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;32-bit LDEOR;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;32-bit LDSET;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;32-bit LDSMAX;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;32-bit LDSMIN;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;32-bit LDUMAX;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;32-bit LDUMIN;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;32-bit SWP;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----100100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----101000----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----101100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000001-----110100----------;UNALLOCATED;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;32-bit LDADDL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;32-bit LDCLRL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;32-bit LDEORL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;32-bit LDSETL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;32-bit LDSMAXL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;32-bit LDSMINL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;32-bit LDUMAXL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;32-bit LDUMINL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000011-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;32-bit SWPL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;32-bit LDADDA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;32-bit LDCLRA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;32-bit LDEORA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;32-bit LDSETA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;32-bit LDSMAXA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;32-bit LDSMINA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;32-bit LDUMAXA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;32-bit LDUMINA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;32-bit SWPA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000101-----110000----------;LDAPR;Atomic memory operations;32-bit;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;32-bit LDADDAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;32-bit LDCLRAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;32-bit LDEORAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;32-bit LDSETAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;32-bit LDSMAXAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;32-bit LDSMINAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;32-bit LDUMAXAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;32-bit LDUMINAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
10111000111-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;32-bit SWPAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;64-bit LDADD;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;64-bit LDCLR;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;64-bit LDEOR;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;64-bit LDSET;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;64-bit LDSMAX;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;64-bit LDSMIN;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;64-bit LDUMAX;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;64-bit LDUMIN;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;64-bit SWP;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----101000----------;ST64BV0;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000001-----101100----------;ST64BV;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
1111100000111111100100----------;ST64B;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
1111100000111111110100----------;LD64B;Atomic memory operations;;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;64-bit LDADDL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;64-bit LDCLRL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;64-bit LDEORL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;64-bit LDSETL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;64-bit LDSMAXL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;64-bit LDSMINL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;64-bit LDUMAXL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;64-bit LDUMINL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000011-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;64-bit SWPL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;64-bit LDADDA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;64-bit LDCLRA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;64-bit LDEORA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;64-bit LDSETA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;64-bit LDSMAXA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;64-bit LDSMINA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;64-bit LDUMAXA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;64-bit LDUMINA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;64-bit SWPA;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000101-----110000----------;LDAPR;Atomic memory operations;64-bit;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----000000----------;LDADD, LDADDA, LDADDAL, LDADDL;Atomic memory operations;64-bit LDADDAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----000100----------;LDCLR, LDCLRA, LDCLRAL, LDCLRL;Atomic memory operations;64-bit LDCLRAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----001000----------;LDEOR, LDEORA, LDEORAL, LDEORL;Atomic memory operations;64-bit LDEORAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----001100----------;LDSET, LDSETA, LDSETAL, LDSETL;Atomic memory operations;64-bit LDSETAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----010000----------;LDSMAX, LDSMAXA, LDSMAXAL, LDSMAXL;Atomic memory operations;64-bit LDSMAXAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----010100----------;LDSMIN, LDSMINA, LDSMINAL, LDSMINL;Atomic memory operations;64-bit LDSMINAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----011000----------;LDUMAX, LDUMAXA, LDUMAXAL, LDUMAXL;Atomic memory operations;64-bit LDUMAXAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----011100----------;LDUMIN, LDUMINA, LDUMINAL, LDUMINL;Atomic memory operations;64-bit LDUMINAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
11111000111-----100000----------;SWP, SWPA, SWPAL, SWPL;Atomic memory operations;64-bit SWPAL;size 30 2,V 26 1,A 23 1,R 22 1,Rs 16 5,o3 15 1,opc 12 3,Rn 5 5,Rt 0 5,;
--0010001-1---------------------;UNALLOCATED;Compare and swap;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,; Rt2 != 11111,
00001000101-----011111----------;CASB, CASAB, CASALB, CASLB;Compare and swap;CASB;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000101-----111111----------;CASB, CASAB, CASALB, CASLB;Compare and swap;CASLB;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000111-----011111----------;CASB, CASAB, CASALB, CASLB;Compare and swap;CASAB;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000111-----111111----------;CASB, CASAB, CASALB, CASLB;Compare and swap;CASALB;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000101-----011111----------;CASH, CASAH, CASALH, CASLH;Compare and swap;CASH;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000101-----111111----------;CASH, CASAH, CASALH, CASLH;Compare and swap;CASLH;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000111-----011111----------;CASH, CASAH, CASALH, CASLH;Compare and swap;CASAH;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000111-----111111----------;CASH, CASAH, CASALH, CASLH;Compare and swap;CASALH;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000101-----011111----------;CAS, CASA, CASAL, CASL;Compare and swap;32-bit CAS;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000101-----111111----------;CAS, CASA, CASAL, CASL;Compare and swap;32-bit CASL;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000111-----011111----------;CAS, CASA, CASAL, CASL;Compare and swap;32-bit CASA;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000111-----111111----------;CAS, CASA, CASAL, CASL;Compare and swap;32-bit CASAL;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000101-----011111----------;CAS, CASA, CASAL, CASL;Compare and swap;64-bit CAS;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000101-----111111----------;CAS, CASA, CASAL, CASL;Compare and swap;64-bit CASL;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000111-----011111----------;CAS, CASA, CASAL, CASL;Compare and swap;64-bit CASA;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000111-----111111----------;CAS, CASA, CASAL, CASL;Compare and swap;64-bit CASAL;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
0-0010000-1---------------------;UNALLOCATED;Compare and swap pair;;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,; Rt2 != 11111,
00001000001-----011111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;32-bit CASP;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000001-----111111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;32-bit CASPL;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000011-----011111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;32-bit CASPA;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000011-----111111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;32-bit CASPAL;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000001-----011111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;64-bit CASP;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000001-----111111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;64-bit CASPL;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000011-----011111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;64-bit CASPA;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000011-----111111----------;CASP, CASPA, CASPAL, CASPL;Compare and swap pair;64-bit CASPAL;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00011001000---------00----------;STLURB;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00011001010---------00----------;LDAPURB;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00011001100---------00----------;LDAPURSB;LDAPR/STLR (unscaled immediate);64-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00011001110---------00----------;LDAPURSB;LDAPR/STLR (unscaled immediate);32-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01011001000---------00----------;STLURH;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01011001010---------00----------;LDAPURH;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01011001100---------00----------;LDAPURSH;LDAPR/STLR (unscaled immediate);64-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01011001110---------00----------;LDAPURSH;LDAPR/STLR (unscaled immediate);32-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10011001000---------00----------;STLUR;LDAPR/STLR (unscaled immediate);32-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10011001010---------00----------;LDAPUR;LDAPR/STLR (unscaled immediate);32-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10011001100---------00----------;LDAPURSW;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10011001110---------00----------;UNALLOCATED;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11011001000---------00----------;STLUR;LDAPR/STLR (unscaled immediate);64-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11011001010---------00----------;LDAPUR;LDAPR/STLR (unscaled immediate);64-bit;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11011001100---------00----------;UNALLOCATED;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11011001110---------00----------;UNALLOCATED;LDAPR/STLR (unscaled immediate);;size 30 2,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00011000------------------------;LDR (literal);Load register (literal);32-bit;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
00011100------------------------;LDR (literal, SIMD&FP);Load register (literal);32-bit;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
01011000------------------------;LDR (literal);Load register (literal);64-bit;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
01011100------------------------;LDR (literal, SIMD&FP);Load register (literal);64-bit;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
10011000------------------------;LDRSW (literal);Load register (literal);;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
10011100------------------------;LDR (literal, SIMD&FP);Load register (literal);128-bit;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
11011000------------------------;PRFM (literal);Load register (literal);;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
11011100------------------------;UNALLOCATED;Load register (literal);;opc 30 2,V 26 1,imm19 5 19,Rt 0 5,;
10001000001-----0---------------;STXP;Load/store exclusive pair;32-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000001-----1---------------;STLXP;Load/store exclusive pair;32-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000011-----0---------------;LDXP;Load/store exclusive pair;32-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000011-----1---------------;LDAXP;Load/store exclusive pair;32-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000001-----0---------------;STXP;Load/store exclusive pair;64-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000001-----1---------------;STLXP;Load/store exclusive pair;64-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000011-----0---------------;LDXP;Load/store exclusive pair;64-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000011-----1---------------;LDAXP;Load/store exclusive pair;64-bit;sz 30 1,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000000-----0---------------;STXRB;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000000-----1---------------;STLXRB;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000010-----0---------------;LDXRB;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000010-----1---------------;LDAXRB;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000000-----0---------------;STXRH;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000000-----1---------------;STLXRH;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000010-----0---------------;LDXRH;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000010-----1---------------;LDAXRH;Load/store exclusive register;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000000-----0---------------;STXR;Load/store exclusive register;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000000-----1---------------;STLXR;Load/store exclusive register;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000010-----0---------------;LDXR;Load/store exclusive register;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000010-----1---------------;LDAXR;Load/store exclusive register;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000000-----0---------------;STXR;Load/store exclusive register;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000000-----1---------------;STLXR;Load/store exclusive register;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000010-----0---------------;LDXR;Load/store exclusive register;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000010-----1---------------;LDAXR;Load/store exclusive register;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11011001001---------01----------;STG;Load/store memory tags;Post-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001001---------10----------;STG;Load/store memory tags;Signed offset;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001001---------11----------;STG;Load/store memory tags;Pre-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
1101100100100000000000----------;STZGM;Load/store memory tags;;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001011---------00----------;LDG;Load/store memory tags;;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001011---------01----------;STZG;Load/store memory tags;Post-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001011---------10----------;STZG;Load/store memory tags;Signed offset;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001011---------11----------;STZG;Load/store memory tags;Pre-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001101---------01----------;ST2G;Load/store memory tags;Post-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001101---------10----------;ST2G;Load/store memory tags;Signed offset;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001101---------11----------;ST2G;Load/store memory tags;Pre-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001101---------00----------;UNALLOCATED;Load/store memory tags;;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,; imm9 != 000000000,
1101100110100000000000----------;STGM;Load/store memory tags;;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001111---------01----------;STZ2G;Load/store memory tags;Post-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001111---------10----------;STZ2G;Load/store memory tags;Signed offset;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001111---------11----------;STZ2G;Load/store memory tags;Pre-index;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
11011001111---------00----------;UNALLOCATED;Load/store memory tags;;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,; imm9 != 000000000,
1101100111100000000000----------;LDGM;Load/store memory tags;;opc 22 2,imm9 12 9,op2 10 2,Rn 5 5,Rt 0 5,;
0010100000----------------------;STNP;Load/store no-allocate pair (offset);32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010100001----------------------;LDNP;Load/store no-allocate pair (offset);32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110000----------------------;STNP (SIMD&FP);Load/store no-allocate pair (offset);32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110001----------------------;LDNP (SIMD&FP);Load/store no-allocate pair (offset);32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
011010000-----------------------;UNALLOCATED;Load/store no-allocate pair (offset);;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110000----------------------;STNP (SIMD&FP);Load/store no-allocate pair (offset);64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110001----------------------;LDNP (SIMD&FP);Load/store no-allocate pair (offset);64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100000----------------------;STNP;Load/store no-allocate pair (offset);64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100001----------------------;LDNP;Load/store no-allocate pair (offset);64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110000----------------------;STNP (SIMD&FP);Load/store no-allocate pair (offset);128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110001----------------------;LDNP (SIMD&FP);Load/store no-allocate pair (offset);128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
11101-000-----------------------;UNALLOCATED;Load/store no-allocate pair (offset);;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000100-----0---------------;STLLRB;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000100-----1---------------;STLRB;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000110-----0---------------;LDLARB;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
00001000110-----1---------------;LDARB;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000100-----0---------------;STLLRH;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000100-----1---------------;STLRH;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000110-----0---------------;LDLARH;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
01001000110-----1---------------;LDARH;Load/store ordered;;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000100-----0---------------;STLLR;Load/store ordered;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000100-----1---------------;STLR;Load/store ordered;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000110-----0---------------;LDLAR;Load/store ordered;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
10001000110-----1---------------;LDAR;Load/store ordered;32-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000100-----0---------------;STLLR;Load/store ordered;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000100-----1---------------;STLR;Load/store ordered;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000110-----0---------------;LDLAR;Load/store ordered;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
11001000110-----1---------------;LDAR;Load/store ordered;64-bit;size 30 2,L 22 1,Rs 16 5,o0 15 1,Rt2 10 5,Rn 5 5,Rt 0 5,;
-11111001-0---------01----------;UNALLOCATED;Load/store register (immediate post-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000000---------01----------;STRB (immediate);Load/store register (immediate post-indexed);Post-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000010---------01----------;LDRB (immediate);Load/store register (immediate post-indexed);Post-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000100---------01----------;LDRSB (immediate);Load/store register (immediate post-indexed);Post-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000110---------01----------;LDRSB (immediate);Load/store register (immediate post-indexed);Post-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100000---------01----------;STR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 8-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100010---------01----------;LDR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 8-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100100---------01----------;STR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 128-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100110---------01----------;LDR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 128-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000000---------01----------;STRH (immediate);Load/store register (immediate post-indexed);Post-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000010---------01----------;LDRH (immediate);Load/store register (immediate post-indexed);Post-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000100---------01----------;LDRSH (immediate);Load/store register (immediate post-indexed);Post-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000110---------01----------;LDRSH (immediate);Load/store register (immediate post-indexed);Post-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111100000---------01----------;STR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 16-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111100010---------01----------;LDR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 16-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-111000110---------01----------;UNALLOCATED;Load/store register (immediate post-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-1111001-0---------01----------;UNALLOCATED;Load/store register (immediate post-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000000---------01----------;STR (immediate);Load/store register (immediate post-indexed);Post-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000010---------01----------;LDR (immediate);Load/store register (immediate post-indexed);Post-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000100---------01----------;LDRSW (immediate);Load/store register (immediate post-indexed);Post-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111100000---------01----------;STR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111100010---------01----------;LDR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000000---------01----------;STR (immediate);Load/store register (immediate post-indexed);Post-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000010---------01----------;LDR (immediate);Load/store register (immediate post-indexed);Post-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000100---------01----------;UNALLOCATED;Load/store register (immediate post-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111100000---------01----------;STR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111100010---------01----------;LDR (immediate, SIMD&FP);Load/store register (immediate post-indexed);Post-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
-11111001-0---------11----------;UNALLOCATED;Load/store register (immediate pre-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000000---------11----------;STRB (immediate);Load/store register (immediate pre-indexed);Pre-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000010---------11----------;LDRB (immediate);Load/store register (immediate pre-indexed);Pre-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000100---------11----------;LDRSB (immediate);Load/store register (immediate pre-indexed);Pre-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000110---------11----------;LDRSB (immediate);Load/store register (immediate pre-indexed);Pre-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100000---------11----------;STR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 8-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100010---------11----------;LDR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 8-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100100---------11----------;STR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 128-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100110---------11----------;LDR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 128-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000000---------11----------;STRH (immediate);Load/store register (immediate pre-indexed);Pre-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000010---------11----------;LDRH (immediate);Load/store register (immediate pre-indexed);Pre-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000100---------11----------;LDRSH (immediate);Load/store register (immediate pre-indexed);Pre-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000110---------11----------;LDRSH (immediate);Load/store register (immediate pre-indexed);Pre-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111100000---------11----------;STR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 16-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111100010---------11----------;LDR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 16-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-111000110---------11----------;UNALLOCATED;Load/store register (immediate pre-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-1111001-0---------11----------;UNALLOCATED;Load/store register (immediate pre-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000000---------11----------;STR (immediate);Load/store register (immediate pre-indexed);Pre-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000010---------11----------;LDR (immediate);Load/store register (immediate pre-indexed);Pre-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000100---------11----------;LDRSW (immediate);Load/store register (immediate pre-indexed);Pre-index;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111100000---------11----------;STR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111100010---------11----------;LDR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000000---------11----------;STR (immediate);Load/store register (immediate pre-indexed);Pre-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000010---------11----------;LDR (immediate);Load/store register (immediate pre-indexed);Pre-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000100---------11----------;UNALLOCATED;Load/store register (immediate pre-indexed);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111100000---------11----------;STR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111100010---------11----------;LDR (immediate, SIMD&FP);Load/store register (immediate pre-indexed);Pre-index, 64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
--111-00--1----------1----------;UNALLOCATED;Load/store register (pac);;size 30 2,V 26 1,M 23 1,S 22 1,imm9 12 9,W 11 1,Rn 5 5,Rt 0 5,; size != 11,
111110000-1---------01----------;LDRAA, LDRAB;Load/store register (pac);Key A, offset;size 30 2,V 26 1,M 23 1,S 22 1,imm9 12 9,W 11 1,Rn 5 5,Rt 0 5,;
111110000-1---------11----------;LDRAA, LDRAB;Load/store register (pac);Key A, pre-indexed;size 30 2,V 26 1,M 23 1,S 22 1,imm9 12 9,W 11 1,Rn 5 5,Rt 0 5,;
111110001-1---------01----------;LDRAA, LDRAB;Load/store register (pac);Key B, offset;size 30 2,V 26 1,M 23 1,S 22 1,imm9 12 9,W 11 1,Rn 5 5,Rt 0 5,;
111110001-1---------11----------;LDRAA, LDRAB;Load/store register (pac);Key B, pre-indexed;size 30 2,V 26 1,M 23 1,S 22 1,imm9 12 9,W 11 1,Rn 5 5,Rt 0 5,;
11111100--1----------1----------;UNALLOCATED;Load/store register (pac);;size 30 2,V 26 1,M 23 1,S 22 1,imm9 12 9,W 11 1,Rn 5 5,Rt 0 5,;
-11111001-1---------10----------;UNALLOCATED;Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111000001---------10----------;STRB (register);Load/store register (register offset);Extended register;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,; option != 011,
00111000001-----011-10----------;STRB (register);Load/store register (register offset);Shifted register;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111000011---------10----------;LDRB (register);Load/store register (register offset);Extended register;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,; option != 011,
00111000011-----011-10----------;LDRB (register);Load/store register (register offset);Shifted register;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111000101---------10----------;LDRSB (register);Load/store register (register offset);64-bit with extended register offset;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,; option != 011,
00111000101-----011-10----------;LDRSB (register);Load/store register (register offset);64-bit with shifted register offset;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111000111---------10----------;LDRSB (register);Load/store register (register offset);32-bit with extended register offset;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,; option != 011,
00111000111-----011-10----------;LDRSB (register);Load/store register (register offset);32-bit with shifted register offset;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111100001---------10----------;STR (register, SIMD&FP);Load/store register (register offset);8-fsreg,STR-8-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,; option != 011,
00111100001-----011-10----------;STR (register, SIMD&FP);Load/store register (register offset);8-fsreg,STR-8-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111100011---------10----------;LDR (register, SIMD&FP);Load/store register (register offset);8-fsreg,LDR-8-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,; option != 011,
00111100011-----011-10----------;LDR (register, SIMD&FP);Load/store register (register offset);8-fsreg,LDR-8-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111100101---------10----------;STR (register, SIMD&FP);Load/store register (register offset);128-fsreg,STR-128-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
00111100111---------10----------;LDR (register, SIMD&FP);Load/store register (register offset);128-fsreg,LDR-128-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
01111000001---------10----------;STRH (register);Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
01111000011---------10----------;LDRH (register);Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
01111000101---------10----------;LDRSH (register);Load/store register (register offset);64-bit;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
01111000111---------10----------;LDRSH (register);Load/store register (register offset);32-bit;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
01111100001---------10----------;STR (register, SIMD&FP);Load/store register (register offset);16-fsreg,STR-16-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
01111100011---------10----------;LDR (register, SIMD&FP);Load/store register (register offset);16-fsreg,LDR-16-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
1-111000111---------10----------;UNALLOCATED;Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
1-1111001-1---------10----------;UNALLOCATED;Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
10111000001---------10----------;STR (register);Load/store register (register offset);32-bit;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
10111000011---------10----------;LDR (register);Load/store register (register offset);32-bit;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
10111000101---------10----------;LDRSW (register);Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
10111100001---------10----------;STR (register, SIMD&FP);Load/store register (register offset);32-fsreg,STR-32-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
10111100011---------10----------;LDR (register, SIMD&FP);Load/store register (register offset);32-fsreg,LDR-32-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
11111000001---------10----------;STR (register);Load/store register (register offset);64-bit;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
11111000011---------10----------;LDR (register);Load/store register (register offset);64-bit;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
11111000101---------10----------;PRFM (register);Load/store register (register offset);;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
11111100001---------10----------;STR (register, SIMD&FP);Load/store register (register offset);64-fsreg,STR-64-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
11111100011---------10----------;LDR (register, SIMD&FP);Load/store register (register offset);64-fsreg,LDR-64-fsreg;size 30 2,V 26 1,opc 22 2,Rm 16 5,option 13 3,S 12 1,Rn 5 5,Rt 0 5,;
--111100--0---------10----------;UNALLOCATED;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000000---------10----------;STTRB;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000010---------10----------;LDTRB;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000100---------10----------;LDTRSB;Load/store register (unprivileged);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000110---------10----------;LDTRSB;Load/store register (unprivileged);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000000---------10----------;STTRH;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000010---------10----------;LDTRH;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000100---------10----------;LDTRSH;Load/store register (unprivileged);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000110---------10----------;LDTRSH;Load/store register (unprivileged);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-111000110---------10----------;UNALLOCATED;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000000---------10----------;STTR;Load/store register (unprivileged);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000010---------10----------;LDTR;Load/store register (unprivileged);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000100---------10----------;LDTRSW;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000000---------10----------;STTR;Load/store register (unprivileged);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000010---------10----------;LDTR;Load/store register (unprivileged);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000100---------10----------;UNALLOCATED;Load/store register (unprivileged);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
-11111001-0---------00----------;UNALLOCATED;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000000---------00----------;STURB;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000010---------00----------;LDURB;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000100---------00----------;LDURSB;Load/store register (unscaled immediate);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111000110---------00----------;LDURSB;Load/store register (unscaled immediate);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100000---------00----------;STUR (SIMD&FP);Load/store register (unscaled immediate);8-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100010---------00----------;LDUR (SIMD&FP);Load/store register (unscaled immediate);8-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100100---------00----------;STUR (SIMD&FP);Load/store register (unscaled immediate);128-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
00111100110---------00----------;LDUR (SIMD&FP);Load/store register (unscaled immediate);128-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000000---------00----------;STURH;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000010---------00----------;LDURH;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000100---------00----------;LDURSH;Load/store register (unscaled immediate);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111000110---------00----------;LDURSH;Load/store register (unscaled immediate);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111100000---------00----------;STUR (SIMD&FP);Load/store register (unscaled immediate);16-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
01111100010---------00----------;LDUR (SIMD&FP);Load/store register (unscaled immediate);16-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-111000110---------00----------;UNALLOCATED;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
1-1111001-0---------00----------;UNALLOCATED;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000000---------00----------;STUR;Load/store register (unscaled immediate);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000010---------00----------;LDUR;Load/store register (unscaled immediate);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111000100---------00----------;LDURSW;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111100000---------00----------;STUR (SIMD&FP);Load/store register (unscaled immediate);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
10111100010---------00----------;LDUR (SIMD&FP);Load/store register (unscaled immediate);32-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000000---------00----------;STUR;Load/store register (unscaled immediate);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000010---------00----------;LDUR;Load/store register (unscaled immediate);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111000100---------00----------;PRFUM;Load/store register (unscaled immediate);;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111100000---------00----------;STUR (SIMD&FP);Load/store register (unscaled immediate);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
11111100010---------00----------;LDUR (SIMD&FP);Load/store register (unscaled immediate);64-bit;size 30 2,V 26 1,opc 22 2,imm9 12 9,Rn 5 5,Rt 0 5,;
-11111011-----------------------;UNALLOCATED;Load/store register (unsigned immediate);;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011100100----------------------;STRB (immediate);Load/store register (unsigned immediate);Unsigned offset;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011100101----------------------;LDRB (immediate);Load/store register (unsigned immediate);Unsigned offset;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011100110----------------------;LDRSB (immediate);Load/store register (unsigned immediate);Unsigned offset, 64-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011100111----------------------;LDRSB (immediate);Load/store register (unsigned immediate);Unsigned offset, 32-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011110100----------------------;STR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 8-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011110101----------------------;LDR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 8-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011110110----------------------;STR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 128-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0011110111----------------------;LDR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 128-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0111100100----------------------;STRH (immediate);Load/store register (unsigned immediate);Unsigned offset;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0111100101----------------------;LDRH (immediate);Load/store register (unsigned immediate);Unsigned offset;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0111100110----------------------;LDRSH (immediate);Load/store register (unsigned immediate);Unsigned offset, 64-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0111100111----------------------;LDRSH (immediate);Load/store register (unsigned immediate);Unsigned offset, 32-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0111110100----------------------;STR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 16-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0111110101----------------------;LDR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 16-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1-11100111----------------------;UNALLOCATED;Load/store register (unsigned immediate);;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1-1111011-----------------------;UNALLOCATED;Load/store register (unsigned immediate);;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1011100100----------------------;STR (immediate);Load/store register (unsigned immediate);Unsigned offset, 32-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1011100101----------------------;LDR (immediate);Load/store register (unsigned immediate);Unsigned offset, 32-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1011100110----------------------;LDRSW (immediate);Load/store register (unsigned immediate);Unsigned offset;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1011110100----------------------;STR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 32-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1011110101----------------------;LDR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 32-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1111100100----------------------;STR (immediate);Load/store register (unsigned immediate);Unsigned offset, 64-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1111100101----------------------;LDR (immediate);Load/store register (unsigned immediate);Unsigned offset, 64-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1111100110----------------------;PRFM (immediate);Load/store register (unsigned immediate);;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1111110100----------------------;STR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 64-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
1111110101----------------------;LDR (immediate, SIMD&FP);Load/store register (unsigned immediate);Unsigned offset, 64-bit;size 30 2,V 26 1,opc 22 2,imm12 10 12,Rn 5 5,Rt 0 5,;
0010100100----------------------;STP;Load/store register pair (offset);Signed offset, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010100101----------------------;LDP;Load/store register pair (offset);Signed offset, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110100----------------------;STP (SIMD&FP);Load/store register pair (offset);Signed offset, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110101----------------------;LDP (SIMD&FP);Load/store register pair (offset);Signed offset, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110100100----------------------;STGP;Load/store register pair (offset);Signed offset;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110100101----------------------;LDPSW;Load/store register pair (offset);Signed offset;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110100----------------------;STP (SIMD&FP);Load/store register pair (offset);Signed offset, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110101----------------------;LDP (SIMD&FP);Load/store register pair (offset);Signed offset, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100100----------------------;STP;Load/store register pair (offset);Signed offset, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100101----------------------;LDP;Load/store register pair (offset);Signed offset, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110100----------------------;STP (SIMD&FP);Load/store register pair (offset);Signed offset, 128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110101----------------------;LDP (SIMD&FP);Load/store register pair (offset);Signed offset, 128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
11101-010-----------------------;UNALLOCATED;Load/store register pair (offset);;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010100010----------------------;STP;Load/store register pair (post-indexed);Post-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010100011----------------------;LDP;Load/store register pair (post-indexed);Post-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110010----------------------;STP (SIMD&FP);Load/store register pair (post-indexed);Post-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110011----------------------;LDP (SIMD&FP);Load/store register pair (post-indexed);Post-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110100010----------------------;STGP;Load/store register pair (post-indexed);Post-index;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110100011----------------------;LDPSW;Load/store register pair (post-indexed);Post-index;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110010----------------------;STP (SIMD&FP);Load/store register pair (post-indexed);Post-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110011----------------------;LDP (SIMD&FP);Load/store register pair (post-indexed);Post-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100010----------------------;STP;Load/store register pair (post-indexed);Post-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100011----------------------;LDP;Load/store register pair (post-indexed);Post-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110010----------------------;STP (SIMD&FP);Load/store register pair (post-indexed);Post-index, 128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110011----------------------;LDP (SIMD&FP);Load/store register pair (post-indexed);Post-index, 128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
11101-001-----------------------;UNALLOCATED;Load/store register pair (post-indexed);;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010100110----------------------;STP;Load/store register pair (pre-indexed);Pre-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010100111----------------------;LDP;Load/store register pair (pre-indexed);Pre-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110110----------------------;STP (SIMD&FP);Load/store register pair (pre-indexed);Pre-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0010110111----------------------;LDP (SIMD&FP);Load/store register pair (pre-indexed);Pre-index, 32-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110100110----------------------;STGP;Load/store register pair (pre-indexed);Pre-index;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110100111----------------------;LDPSW;Load/store register pair (pre-indexed);Pre-index;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110110----------------------;STP (SIMD&FP);Load/store register pair (pre-indexed);Pre-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
0110110111----------------------;LDP (SIMD&FP);Load/store register pair (pre-indexed);Pre-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100110----------------------;STP;Load/store register pair (pre-indexed);Pre-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010100111----------------------;LDP;Load/store register pair (pre-indexed);Pre-index, 64-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110110----------------------;STP (SIMD&FP);Load/store register pair (pre-indexed);Pre-index, 128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
1010110111----------------------;LDP (SIMD&FP);Load/store register pair (pre-indexed);Pre-index, 128-bit;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
11101-011-----------------------;UNALLOCATED;Load/store register pair (pre-indexed);;opc 30 2,V 26 1,L 22 1,imm7 15 7,Rt2 10 5,Rn 5 5,Rt 0 5,;
00000100--0-----010-------------;MLA;SVE integer multiply-accumulate writing addend (predicated);;size 22 2,Zm 16 5,op 13 1,Pg 10 3,Zn 5 5,Zda 0 5,;
00000100--0-----011-------------;MLS;SVE integer multiply-accumulate writing addend (predicated);;size 22 2,Zm 16 5,op 13 1,Pg 10 3,Zn 5 5,Zda 0 5,;
00000100--0-----110-------------;MAD;SVE integer multiply-add writing multiplicand (predicated);;size 22 2,Zm 16 5,op 13 1,Pg 10 3,Za 5 5,Zdn 0 5,;
00000100--0-----111-------------;MSB;SVE integer multiply-add writing multiplicand (predicated);;size 22 2,Zm 16 5,op 13 1,Pg 10 3,Za 5 5,Zdn 0 5,;
00000100--1-----000000----------;ADD (vectors, unpredicated);SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000100--1-----000001----------;SUB (vectors, unpredicated);SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000100--1-----00001-----------;UNALLOCATED;SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000100--1-----000100----------;SQADD (vectors);SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000100--1-----000101----------;UQADD (vectors);SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000100--1-----000110----------;SQSUB (vectors);SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000100--1-----000111----------;UQSUB (vectors);SVE integer add/subtract vectors (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
000100010-----------------------;ADD (immediate);Add/subtract (immediate);32-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
001100010-----------------------;ADDS (immediate);Add/subtract (immediate);32-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
010100010-----------------------;SUB (immediate);Add/subtract (immediate);32-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
011100010-----------------------;SUBS (immediate);Add/subtract (immediate);32-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
100100010-----------------------;ADD (immediate);Add/subtract (immediate);64-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
101100010-----------------------;ADDS (immediate);Add/subtract (immediate);64-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
110100010-----------------------;SUB (immediate);Add/subtract (immediate);64-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
111100010-----------------------;SUBS (immediate);Add/subtract (immediate);64-bit;sf 31 1,op 30 1,S 29 1,sh 22 1,imm12 10 12,Rn 5 5,Rd 0 5,;
---1000111----------------------;UNALLOCATED;Add/subtract (immediate, with tags);;sf 31 1,op 30 1,S 29 1,o2 22 1,uimm6 16 6,op3 14 2,uimm4 10 4,Rn 5 5,Rd 0 5,;
0--1000110----------------------;UNALLOCATED;Add/subtract (immediate, with tags);;sf 31 1,op 30 1,S 29 1,o2 22 1,uimm6 16 6,op3 14 2,uimm4 10 4,Rn 5 5,Rd 0 5,;
1-11000110----------------------;UNALLOCATED;Add/subtract (immediate, with tags);;sf 31 1,op 30 1,S 29 1,o2 22 1,uimm6 16 6,op3 14 2,uimm4 10 4,Rn 5 5,Rd 0 5,;
1001000110----------------------;ADDG;Add/subtract (immediate, with tags);;sf 31 1,op 30 1,S 29 1,o2 22 1,uimm6 16 6,op3 14 2,uimm4 10 4,Rn 5 5,Rd 0 5,;
1101000110----------------------;SUBG;Add/subtract (immediate, with tags);;sf 31 1,op 30 1,S 29 1,o2 22 1,uimm6 16 6,op3 14 2,uimm4 10 4,Rn 5 5,Rd 0 5,;
-11100110-----------------------;UNALLOCATED;Bitfield;;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0--1001101----------------------;UNALLOCATED;Bitfield;;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0001001100----------------------;SBFM;Bitfield;32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0011001100----------------------;BFM;Bitfield;32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0101001100----------------------;UBFM;Bitfield;32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
1--1001100----------------------;UNALLOCATED;Bitfield;;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
1001001101----------------------;SBFM;Bitfield;64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
1011001101----------------------;BFM;Bitfield;64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
1101001101----------------------;UBFM;Bitfield;64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
--1100111-----------------------;UNALLOCATED;Extract;;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
-00100111-1---------------------;UNALLOCATED;Extract;;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
-1-100111-----------------------;UNALLOCATED;Extract;;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
0--100111-------1---------------;UNALLOCATED;Extract;;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
0--1001111----------------------;UNALLOCATED;Extract;;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
00010011100-----0---------------;EXTR;Extract;32-bit;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
1--1001110----------------------;UNALLOCATED;Extract;;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
10010011110---------------------;EXTR;Extract;64-bit;sf 31 1,op21 29 2,N 22 1,o0 21 1,Rm 16 5,imms 10 6,Rn 5 5,Rd 0 5,;
0--1001001----------------------;UNALLOCATED;Logical (immediate);;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0001001000----------------------;AND (immediate);Logical (immediate);32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0011001000----------------------;ORR (immediate);Logical (immediate);32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0101001000----------------------;EOR (immediate);Logical (immediate);32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
0111001000----------------------;ANDS (immediate);Logical (immediate);32-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
100100100-----------------------;AND (immediate);Logical (immediate);64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
101100100-----------------------;ORR (immediate);Logical (immediate);64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
110100100-----------------------;EOR (immediate);Logical (immediate);64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
111100100-----------------------;ANDS (immediate);Logical (immediate);64-bit;sf 31 1,opc 29 2,N 22 1,immr 16 6,imms 10 6,Rn 5 5,Rd 0 5,;
-01100101-----------------------;UNALLOCATED;Move wide (immediate);;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
0--1001011----------------------;UNALLOCATED;Move wide (immediate);;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
0001001010----------------------;MOVN;Move wide (immediate);32-bit;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
0101001010----------------------;MOVZ;Move wide (immediate);32-bit;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
0111001010----------------------;MOVK;Move wide (immediate);32-bit;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
100100101-----------------------;MOVN;Move wide (immediate);64-bit;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
110100101-----------------------;MOVZ;Move wide (immediate);64-bit;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
111100101-----------------------;MOVK;Move wide (immediate);64-bit;sf 31 1,opc 29 2,hw 21 2,imm16 5 16,Rd 0 5,;
0--10000------------------------;ADR;PC-rel. addressing;;op 31 1,immlo 29 2,immhi 5 19,Rd 0 5,;
1--10000------------------------;ADRP;PC-rel. addressing;;op 31 1,immlo 29 2,immhi 5 19,Rd 0 5,;
00000100001-----001100----------;AND (vectors, unpredicated);SVE bitwise logical operations (unpredicated);;opc 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
00000100011-----001100----------;ORR (vectors, unpredicated);SVE bitwise logical operations (unpredicated);;opc 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
00000100101-----001100----------;EOR (vectors, unpredicated);SVE bitwise logical operations (unpredicated);;opc 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
00000100111-----001100----------;BIC (vectors, unpredicated);SVE bitwise logical operations (unpredicated);;opc 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
00000100--1-----010000----------;INDEX (immediates);SVE index generation (immediate start, immediate increment);;size 22 2,imm5b 16 5,imm5 5 5,Zd 0 5,;
00000100--1-----010010----------;INDEX (immediate, scalar);SVE index generation (immediate start, register increment);;size 22 2,Rm 16 5,imm5 5 5,Zd 0 5,;
00000100--1-----010001----------;INDEX (scalar, immediate);SVE index generation (register start, immediate increment);;size 22 2,imm5 16 5,Rn 5 5,Zd 0 5,;
00000100--1-----010011----------;INDEX (scalars);SVE index generation (register start, register increment);;size 22 2,Rm 16 5,Rn 5 5,Zd 0 5,;
00000100001-----01010-----------;ADDVL;SVE stack frame adjustment;;op 22 1,Rn 16 5,imm6 5 6,Rd 0 5,;
00000100011-----01010-----------;ADDPL;SVE stack frame adjustment;;op 22 1,Rn 16 5,imm6 5 6,Rd 0 5,;
000001001010----01010-----------;UNALLOCATED;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
0000010010110---01010-----------;UNALLOCATED;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
00000100101110--01010-----------;UNALLOCATED;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
000001001011110-01010-----------;UNALLOCATED;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
000001001011111001010-----------;UNALLOCATED;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
000001001011111101010-----------;RDVL;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
00000100111-----01010-----------;UNALLOCATED;SVE stack frame size;;op 22 1,opc2 16 5,imm6 5 6,Rd 0 5,;
---01011--1--------1-1----------;UNALLOCATED;Add/subtract (extended register);;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
---01011--1--------11-----------;UNALLOCATED;Add/subtract (extended register);;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
---01011-11---------------------;UNALLOCATED;Add/subtract (extended register);;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
---010111-1---------------------;UNALLOCATED;Add/subtract (extended register);;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
00001011001---------------------;ADD (extended register);Add/subtract (extended register);32-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
00101011001---------------------;ADDS (extended register);Add/subtract (extended register);32-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
01001011001---------------------;SUB (extended register);Add/subtract (extended register);32-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
01101011001---------------------;SUBS (extended register);Add/subtract (extended register);32-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
10001011001---------------------;ADD (extended register);Add/subtract (extended register);64-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
10101011001---------------------;ADDS (extended register);Add/subtract (extended register);64-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
11001011001---------------------;SUB (extended register);Add/subtract (extended register);64-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
11101011001---------------------;SUBS (extended register);Add/subtract (extended register);64-bit;sf 31 1,op 30 1,S 29 1,opt 22 2,Rm 16 5,option 13 3,imm3 10 3,Rn 5 5,Rd 0 5,;
---01011110---------------------;UNALLOCATED;Add/subtract (shifted register);;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
0--01011--0-----1---------------;UNALLOCATED;Add/subtract (shifted register);;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00001011--0---------------------;ADD (shifted register);Add/subtract (shifted register);32-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00101011--0---------------------;ADDS (shifted register);Add/subtract (shifted register);32-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01001011--0---------------------;SUB (shifted register);Add/subtract (shifted register);32-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01101011--0---------------------;SUBS (shifted register);Add/subtract (shifted register);32-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
10001011--0---------------------;ADD (shifted register);Add/subtract (shifted register);64-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
10101011--0---------------------;ADDS (shifted register);Add/subtract (shifted register);64-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
11001011--0---------------------;SUB (shifted register);Add/subtract (shifted register);64-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
11101011--0---------------------;SUBS (shifted register);Add/subtract (shifted register);64-bit;sf 31 1,op 30 1,S 29 1,shift 22 2,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00011010000-----000000----------;ADC;Add/subtract (with carry);32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
00111010000-----000000----------;ADCS;Add/subtract (with carry);32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
01011010000-----000000----------;SBC;Add/subtract (with carry);32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
01111010000-----000000----------;SBCS;Add/subtract (with carry);32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
10011010000-----000000----------;ADC;Add/subtract (with carry);64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
10111010000-----000000----------;ADCS;Add/subtract (with carry);64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
11011010000-----000000----------;SBC;Add/subtract (with carry);64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
11111010000-----000000----------;SBCS;Add/subtract (with carry);64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,Rn 5 5,Rd 0 5,;
---11010010---------1------1----;UNALLOCATED;Conditional compare (immediate);;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
---11010010---------11----------;UNALLOCATED;Conditional compare (immediate);;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
--011010010---------1-----------;UNALLOCATED;Conditional compare (immediate);;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
00111010010---------10-----0----;CCMN (immediate);Conditional compare (immediate);32-bit;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
01111010010---------10-----0----;CCMP (immediate);Conditional compare (immediate);32-bit;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
10111010010---------10-----0----;CCMN (immediate);Conditional compare (immediate);64-bit;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
11111010010---------10-----0----;CCMP (immediate);Conditional compare (immediate);64-bit;sf 31 1,op 30 1,S 29 1,imm5 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
---11010010---------0------1----;UNALLOCATED;Conditional compare (register);;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
---11010010---------01----------;UNALLOCATED;Conditional compare (register);;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
--011010010---------0-----------;UNALLOCATED;Conditional compare (register);;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
00111010010---------00-----0----;CCMN (register);Conditional compare (register);32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
01111010010---------00-----0----;CCMP (register);Conditional compare (register);32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
10111010010---------00-----0----;CCMN (register);Conditional compare (register);64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
11111010010---------00-----0----;CCMP (register);Conditional compare (register);64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,o2 10 1,Rn 5 5,o3 4 1,nzcv 0 4,;
---11010100---------1-----------;UNALLOCATED;Conditional select;;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
--111010100---------------------;UNALLOCATED;Conditional select;;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
00011010100---------00----------;CSEL;Conditional select;32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
00011010100---------01----------;CSINC;Conditional select;32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
01011010100---------00----------;CSINV;Conditional select;32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
01011010100---------01----------;CSNEG;Conditional select;32-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
10011010100---------00----------;CSEL;Conditional select;64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
10011010100---------01----------;CSINC;Conditional select;64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
11011010100---------00----------;CSINV;Conditional select;64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
11011010100---------01----------;CSNEG;Conditional select;64-bit;sf 31 1,op 30 1,S 29 1,Rm 16 5,cond 12 4,op2 10 2,Rn 5 5,Rd 0 5,;
-1-11010110-----1---------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-1-11010110---1-----------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-1-11010110--1------------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-1-11010110-1-------------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-1-110101101--------------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-10110101100000000011-----------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-101101011000000001-------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-10110101100000001--------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-1111010110---------------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
01-1101011000001----------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
0101101011000000000000----------;RBIT;Data-processing (1 source);32-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
0101101011000000000001----------;REV16;Data-processing (1 source);32-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
0101101011000000000010----------;REV;Data-processing (1 source);32-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
0101101011000000000011----------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
0101101011000000000100----------;CLZ;Data-processing (1 source);32-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
0101101011000000000101----------;CLS;Data-processing (1 source);32-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000000000000----------;RBIT;Data-processing (1 source);64-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000000000001----------;REV16;Data-processing (1 source);64-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000000000010----------;REV32;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000000000011----------;REV;Data-processing (1 source);64-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000000000100----------;CLZ;Data-processing (1 source);64-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000000000101----------;CLS;Data-processing (1 source);64-bit;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000000----------;PACIA, PACIA1716, PACIASP, PACIAZ, PACIZA;Data-processing (1 source);Integer, PACIA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000001----------;PACIB, PACIB1716, PACIBSP, PACIBZ, PACIZB;Data-processing (1 source);Integer, PACIB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000010----------;PACDA, PACDZA;Data-processing (1 source);PACDA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000011----------;PACDB, PACDZB;Data-processing (1 source);PACDB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000100----------;AUTIA, AUTIA1716, AUTIASP, AUTIAZ, AUTIZA;Data-processing (1 source);Integer, AUTIA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000101----------;AUTIB, AUTIB1716, AUTIBSP, AUTIBZ, AUTIZB;Data-processing (1 source);Integer, AUTIB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000110----------;AUTDA, AUTDZA;Data-processing (1 source);AUTDA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001000111----------;AUTDB, AUTDZB;Data-processing (1 source);AUTDB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100100011111-----;PACIA, PACIA1716, PACIASP, PACIAZ, PACIZA;Data-processing (1 source);Integer, PACIZA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100100111111-----;PACIB, PACIB1716, PACIBSP, PACIBZ, PACIZB;Data-processing (1 source);Integer, PACIZB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100101011111-----;PACDA, PACDZA;Data-processing (1 source);PACDZA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100101111111-----;PACDB, PACDZB;Data-processing (1 source);PACDZB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100110011111-----;AUTIA, AUTIA1716, AUTIASP, AUTIAZ, AUTIZA;Data-processing (1 source);Integer, AUTIZA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100110111111-----;AUTIB, AUTIB1716, AUTIBSP, AUTIBZ, AUTIZB;Data-processing (1 source);Integer, AUTIZB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100111011111-----;AUTDA, AUTDZA;Data-processing (1 source);AUTDZA;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000100111111111-----;AUTDB, AUTDZB;Data-processing (1 source);AUTDZB;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000101000011111-----;XPACD, XPACI, XPACLRI;Data-processing (1 source);Integer, XPACI;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000101000111111-----;XPACD, XPACI, XPACLRI;Data-processing (1 source);Integer, XPACD;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
110110101100000101001-----------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
11011010110000010101------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
1101101011000001011-------------;UNALLOCATED;Data-processing (1 source);;sf 31 1,S 29 1,opcode2 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0-11010110-----000001----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0-11010110-----011-------------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0-11010110-----1---------------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0011010110-----00011-----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0011010110-----001101----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0011010110-----00111-----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0111010110-----00001-----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0111010110-----0001------------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0111010110-----001-------------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0111010110-----01--------------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00-11010110-----000000----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----000010----------;UDIV;Data-processing (2 source);32-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----000011----------;SDIV;Data-processing (2 source);32-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----00010-----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----001000----------;LSLV;Data-processing (2 source);32-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----001001----------;LSRV;Data-processing (2 source);32-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----001010----------;ASRV;Data-processing (2 source);32-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----001011----------;RORV;Data-processing (2 source);32-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----001100----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010-11----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010000----------;CRC32B, CRC32H, CRC32W, CRC32X;Data-processing (2 source);CRC32B;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010001----------;CRC32B, CRC32H, CRC32W, CRC32X;Data-processing (2 source);CRC32H;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010010----------;CRC32B, CRC32H, CRC32W, CRC32X;Data-processing (2 source);CRC32W;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010100----------;CRC32CB, CRC32CH, CRC32CW, CRC32CX;Data-processing (2 source);CRC32CB;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010101----------;CRC32CB, CRC32CH, CRC32CW, CRC32CX;Data-processing (2 source);CRC32CH;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
00011010110-----010110----------;CRC32CB, CRC32CH, CRC32CW, CRC32CX;Data-processing (2 source);CRC32CW;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----000000----------;SUBP;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----000010----------;UDIV;Data-processing (2 source);64-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----000011----------;SDIV;Data-processing (2 source);64-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----000100----------;IRG;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----000101----------;GMI;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----001000----------;LSLV;Data-processing (2 source);64-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----001001----------;LSRV;Data-processing (2 source);64-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----001010----------;ASRV;Data-processing (2 source);64-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----001011----------;RORV;Data-processing (2 source);64-bit;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----001100----------;PACGA;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----010--0----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----010-0-----------;UNALLOCATED;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----010011----------;CRC32B, CRC32H, CRC32W, CRC32X;Data-processing (2 source);CRC32X;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10011010110-----010111----------;CRC32CB, CRC32CH, CRC32CW, CRC32CX;Data-processing (2 source);CRC32CX;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
10111010110-----000000----------;SUBPS;Data-processing (2 source);;sf 31 1,S 29 1,Rm 16 5,opcode 10 6,Rn 5 5,Rd 0 5,;
-0011011010-----1---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0011011011---------------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0011011100---------------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0011011110-----1---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0011011111---------------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0111011------------------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-1-11011------------------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011000-----0---------------;MADD;Data-processing (3 source);32-bit;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011000-----1---------------;MSUB;Data-processing (3 source);32-bit;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011001-----0---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011001-----1---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011010-----0---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011101-----0---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011101-----1---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011011110-----0---------------;UNALLOCATED;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011000-----0---------------;MADD;Data-processing (3 source);64-bit;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011000-----1---------------;MSUB;Data-processing (3 source);64-bit;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011001-----0---------------;SMADDL;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011001-----1---------------;SMSUBL;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011010-----0---------------;SMULH;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011101-----0---------------;UMADDL;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011101-----1---------------;UMSUBL;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10011011110-----0---------------;UMULH;Data-processing (3 source);;sf 31 1,op54 29 2,op31 21 3,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011010000-------0010----------;UNALLOCATED;Evaluate into flags;;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,;
00111010000-------0010----------;UNALLOCATED;Evaluate into flags;;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,; opcode2 != 000000,
00111010000000000-0010-----0----;UNALLOCATED;Evaluate into flags;;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,; mask != 1101,
00111010000000000-0010-----1----;UNALLOCATED;Evaluate into flags;;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,;
0011101000000000000010-----01101;SETF8, SETF16;Evaluate into flags;SETF8;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,;
0011101000000000010010-----01101;SETF8, SETF16;Evaluate into flags;SETF16;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,;
01-11010000-------0010----------;UNALLOCATED;Evaluate into flags;;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,;
1--11010000-------0010----------;UNALLOCATED;Evaluate into flags;;sf 31 1,op 30 1,S 29 1,opcode2 15 6,sz 14 1,Rn 5 5,o3 4 1,mask 0 4,;
0--01010--------1---------------;UNALLOCATED;Logical (shifted register);;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00001010--0---------------------;AND (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00001010--1---------------------;BIC (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00101010--0---------------------;ORR (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
00101010--1---------------------;ORN (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01001010--0---------------------;EOR (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01001010--1---------------------;EON (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01101010--0---------------------;ANDS (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01101010--1---------------------;BICS (shifted register);Logical (shifted register);32-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
10001010--0---------------------;AND (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
10001010--1---------------------;BIC (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
10101010--0---------------------;ORR (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
10101010--1---------------------;ORN (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
11001010--0---------------------;EOR (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
11001010--1---------------------;EON (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
11101010--0---------------------;ANDS (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
11101010--1---------------------;BICS (shifted register);Logical (shifted register);64-bit;sf 31 1,opc 29 2,shift 22 2,N 21 1,Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
0--11010000------00001----------;UNALLOCATED;Rotate right into flags;;sf 31 1,op 30 1,S 29 1,imm6 15 6,Rn 5 5,o2 4 1,mask 0 4,;
10011010000------00001----------;UNALLOCATED;Rotate right into flags;;sf 31 1,op 30 1,S 29 1,imm6 15 6,Rn 5 5,o2 4 1,mask 0 4,;
10111010000------00001-----0----;RMIF;Rotate right into flags;;sf 31 1,op 30 1,S 29 1,imm6 15 6,Rn 5 5,o2 4 1,mask 0 4,;
10111010000------00001-----1----;UNALLOCATED;Rotate right into flags;;sf 31 1,op 30 1,S 29 1,imm6 15 6,Rn 5 5,o2 4 1,mask 0 4,;
11-11010000------00001----------;UNALLOCATED;Rotate right into flags;;sf 31 1,op 30 1,S 29 1,imm6 15 6,Rn 5 5,o2 4 1,mask 0 4,;
00000100--1-----100100----------;ASR (immediate, unpredicated);SVE bitwise shift by immediate (unpredicated);;tszh 22 2,tszl 19 2,imm3 16 3,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100101----------;LSR (immediate, unpredicated);SVE bitwise shift by immediate (unpredicated);;tszh 22 2,tszl 19 2,imm3 16 3,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100110----------;UNALLOCATED;SVE bitwise shift by immediate (unpredicated);;tszh 22 2,tszl 19 2,imm3 16 3,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100111----------;LSL (immediate, unpredicated);SVE bitwise shift by immediate (unpredicated);;tszh 22 2,tszl 19 2,imm3 16 3,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100000----------;ASR (wide elements, unpredicated);SVE bitwise shift by wide elements (unpredicated);;size 22 2,Zm 16 5,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100001----------;LSR (wide elements, unpredicated);SVE bitwise shift by wide elements (unpredicated);;size 22 2,Zm 16 5,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100010----------;UNALLOCATED;SVE bitwise shift by wide elements (unpredicated);;size 22 2,Zm 16 5,opc 10 2,Zn 5 5,Zd 0 5,;
00000100--1-----100011----------;LSL (wide elements, unpredicated);SVE bitwise shift by wide elements (unpredicated);;size 22 2,Zm 16 5,opc 10 2,Zn 5 5,Zd 0 5,;
0--01110--110000000-10----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--110000001010----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--11000001--10----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--110000100-10----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--110000101110----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--110000110110----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--110000111010----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--1100010---10----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--110001100-10----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--11000111--10----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--110000001110----------;SADDLV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--110000101010----------;SMAXV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--110001101010----------;SMINV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--110001101110----------;ADDV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111000110000110010----------;FMAXNMV;Advanced SIMD across lanes;Half-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111000110000111110----------;FMAXV;Advanced SIMD across lanes;Half-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001110000110010----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001110000111110----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111010110000110010----------;FMINNMV;Advanced SIMD across lanes;Half-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111010110000111110----------;FMINV;Advanced SIMD across lanes;Half-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011110000110010----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011110000111110----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--110000001110----------;UADDLV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--110000101010----------;UMAXV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--110001101010----------;UMINV;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--110001101110----------;UNALLOCATED;Advanced SIMD across lanes;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-110000110010----------;FMAXNMV;Advanced SIMD across lanes;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-110000111110----------;FMAXV;Advanced SIMD across lanes;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-110000110010----------;FMINNMV;Advanced SIMD across lanes;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-110000111110----------;FMINV;Advanced SIMD across lanes;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110000-00000----1----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-001110000-----000001----------;DUP (element);Advanced SIMD copy;Vector;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-001110000-----000011----------;DUP (general);Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-001110000-----000101----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-001110000-----001001----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-001110000-----001101----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-001110000-----01---1----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
00001110000-----000111----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
00001110000-----001011----------;SMOV;Advanced SIMD copy;32-bit;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
00001110000-----001111----------;UMOV;Advanced SIMD copy;32-bit;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
00101110000-----0----1----------;UNALLOCATED;Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01001110000-----000111----------;INS (general);Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01001110000-----001011----------;SMOV;Advanced SIMD copy;64-reg,SMOV-64-reg;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01001110000-1000001111----------;UMOV;Advanced SIMD copy;64-reg,UMOV-64-reg;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01101110000-----0----1----------;INS (element);Advanced SIMD copy;;Q 30 1,op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-101110-10-----0----0----------;UNALLOCATED;Advanced SIMD extract;;Q 30 1,op2 22 2,Rm 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-101110000-----0----0----------;EXT;Advanced SIMD extract;;Q 30 1,op2 22 2,Rm 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-1011101-0-----0----0----------;UNALLOCATED;Advanced SIMD extract;;Q 30 1,op2 22 2,Rm 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
0-00111100000---0---11----------;UNALLOCATED;Advanced SIMD modified immediate;;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---0--001----------;MOVI;Advanced SIMD modified immediate;32-bit shifted immediate;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---0--101----------;ORR (vector, immediate);Advanced SIMD modified immediate;32-bit;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---10--11----------;UNALLOCATED;Advanced SIMD modified immediate;;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---10-001----------;MOVI;Advanced SIMD modified immediate;16-bit shifted immediate;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---10-101----------;ORR (vector, immediate);Advanced SIMD modified immediate;16-bit;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---110-01----------;MOVI;Advanced SIMD modified immediate;32-bit shifting ones;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---110-11----------;UNALLOCATED;Advanced SIMD modified immediate;;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---111001----------;MOVI;Advanced SIMD modified immediate;8-bit;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---111011----------;UNALLOCATED;Advanced SIMD modified immediate;;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---111101----------;FMOV (vector, immediate);Advanced SIMD modified immediate;Single-precision and double-precision, Single-precision;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-00111100000---111111----------;FMOV (vector, immediate);Advanced SIMD modified immediate;Half-precision;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-10111100000-------11----------;UNALLOCATED;Advanced SIMD modified immediate;;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-10111100000---0--001----------;MVNI;Advanced SIMD modified immediate;32-bit shifted immediate;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-10111100000---0--101----------;BIC (vector, immediate);Advanced SIMD modified immediate;32-bit;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-10111100000---10-001----------;MVNI;Advanced SIMD modified immediate;16-bit shifted immediate;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-10111100000---10-101----------;BIC (vector, immediate);Advanced SIMD modified immediate;16-bit;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-10111100000---110-01----------;MVNI;Advanced SIMD modified immediate;32-bit shifting ones;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0010111100000---111001----------;MOVI;Advanced SIMD modified immediate;64-bit scalar;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0010111100000---111101----------;UNALLOCATED;Advanced SIMD modified immediate;;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0110111100000---111001----------;MOVI;Advanced SIMD modified immediate;64-bit vector;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0110111100000---111101----------;FMOV (vector, immediate);Advanced SIMD modified immediate;Single-precision and double-precision, Double-precision;Q 30 1,op 29 1,a 18 1,b 17 1,c 16 1,cmode 12 4,o2 11 1,d 9 1,e 8 1,f 7 1,g 6 1,h 5 1,Rd 0 5,;
0-001110--0-----000010----------;UNALLOCATED;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----000110----------;UZP1;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----001010----------;TRN1;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----001110----------;ZIP1;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----010010----------;UNALLOCATED;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----010110----------;UZP2;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----011010----------;TRN2;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
0-001110--0-----011110----------;ZIP2;Advanced SIMD permute;;Q 30 1,size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----0---11----------;UNALLOCATED;Advanced SIMD scalar copy;;op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01011110000-----0--1-1----------;UNALLOCATED;Advanced SIMD scalar copy;;op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01011110000-----0-1--1----------;UNALLOCATED;Advanced SIMD scalar copy;;op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01011110000-----000001----------;DUP (element);Advanced SIMD scalar copy;Scalar;op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01011110000-----01---1----------;UNALLOCATED;Advanced SIMD scalar copy;;op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01111110000-----0----1----------;UNALLOCATED;Advanced SIMD scalar copy;;op 29 1,imm5 16 5,imm4 11 4,Rn 5 5,Rd 0 5,;
01-11110--1100000---10----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--11000010--10----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--110000111010----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--1100010---10----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--110001100-10----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--110001101010----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--11000111--10----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111101-110000110110----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--110001101110----------;ADDP (scalar);Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-110000110010----------;FMAXNMP (scalar);Advanced SIMD scalar pairwise;Half-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-110000110110----------;FADDP (scalar);Advanced SIMD scalar pairwise;Half-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-110000111110----------;FMAXP (scalar);Advanced SIMD scalar pairwise;Half-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-110000110010----------;FMINNMP (scalar);Advanced SIMD scalar pairwise;Half-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-110000111110----------;FMINP (scalar);Advanced SIMD scalar pairwise;Half-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--110001101110----------;UNALLOCATED;Advanced SIMD scalar pairwise;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-110000110010----------;FMAXNMP (scalar);Advanced SIMD scalar pairwise;Single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-110000110110----------;FADDP (scalar);Advanced SIMD scalar pairwise;Single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-110000111110----------;FMAXP (scalar);Advanced SIMD scalar pairwise;Single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-110000110010----------;FMINNMP (scalar);Advanced SIMD scalar pairwise;Single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-110000111110----------;FMINP (scalar);Advanced SIMD scalar pairwise;Single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111110-------000011----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------000111----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------001011----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------001111----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------010011----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------010111----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------011011----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------011111----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------101--1----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------110--1----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------111011----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-111110-------111101----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-1111100000--------1----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
010111110-------000001----------;SSHR;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------000101----------;SSRA;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------001001----------;SRSHR;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------001101----------;SRSRA;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------010001----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------010101----------;SHL;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------011001----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------011101----------;SQSHL (immediate);Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------100001----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------100011----------;UNALLOCATED;Advanced SIMD scalar shift by immediate;;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------100101----------;SQSHRN, SQSHRN2;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------100111----------;SQRSHRN, SQRSHRN2;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------111001----------;SCVTF (vector, fixed-point);Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
010111110-------111111----------;FCVTZS (vector, fixed-point);Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------000001----------;USHR;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------000101----------;USRA;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------001001----------;URSHR;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------001101----------;URSRA;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------010001----------;SRI;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------010101----------;SLI;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------011001----------;SQSHLU;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------011101----------;UQSHL (immediate);Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------100001----------;SQSHRUN, SQSHRUN2;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------100011----------;SQRSHRUN, SQRSHRUN2;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------100101----------;UQSHRN, UQSHRN2;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------100111----------;UQRSHRN, UQRSHRN2;Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------111001----------;UCVTF (vector, fixed-point);Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
011111110-------111111----------;FCVTZU (vector, fixed-point);Advanced SIMD scalar shift by immediate;Scalar;U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,; immh != 0000,
01-11110--1-----00--00----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01-11110--1-----01--00----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01-11110--1-----100000----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01-11110--1-----101000----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01-11110--1-----110000----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01-11110--1-----111-00----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01011110--1-----100100----------;SQDMLAL, SQDMLAL2 (vector);Advanced SIMD scalar three different;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01011110--1-----101100----------;SQDMLSL, SQDMLSL2 (vector);Advanced SIMD scalar three different;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01011110--1-----110100----------;SQDMULL, SQDMULL2 (vector);Advanced SIMD scalar three different;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01111110--1-----100100----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01111110--1-----101100----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01111110--1-----110100----------;UNALLOCATED;Advanced SIMD scalar three different;;U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
01-11110--1-----000001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01-11110--1-----0001-1----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01-11110--1-----001001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01-11110--1-----011--1----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01-11110--1-----1001-1----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01-111101-1-----110111----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----000011----------;SQADD;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----001011----------;SQSUB;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----001101----------;CMGT (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----001111----------;CMGE (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----010001----------;SSHL;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----010011----------;SQSHL (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----010101----------;SRSHL;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----010111----------;SQRSHL;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----100001----------;ADD (vector);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----100011----------;CMTST;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----101001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----101011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----101101----------;SQDMULH (vector);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01011110--1-----101111----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----110001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----110011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----110101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----110111----------;FMULX;Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----111001----------;FCMEQ (register);Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----111011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----111101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111100-1-----111111----------;FRECPS;Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----110001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----110011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----110101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----111001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----111011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----111101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
010111101-1-----111111----------;FRSQRTS;Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----000011----------;UQADD;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----001011----------;UQSUB;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----001101----------;CMHI (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----001111----------;CMHS (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----010001----------;USHL;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----010011----------;UQSHL (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----010101----------;URSHL;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----010111----------;UQRSHL;Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----100001----------;SUB (vector);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----100011----------;CMEQ (register);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----101001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----101011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----101101----------;SQRDMULH (vector);Advanced SIMD scalar three same;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01111110--1-----101111----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----110001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----110011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----110101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----110111----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----111001----------;FCMGE (register);Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----111011----------;FACGE;Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----111101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111100-1-----111111----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----110001----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----110011----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----110101----------;FABD;Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----111001----------;FCMGT (register);Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----111011----------;FACGT;Advanced SIMD scalar three same;Scalar single-precision and double-precision;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----111101----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
011111101-1-----111111----------;UNALLOCATED;Advanced SIMD scalar three same;;U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
01-11110-10-----001101----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01-11110110-----000111----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110010-----000111----------;FMULX;Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110010-----001001----------;FCMEQ (register);Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110010-----001011----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110010-----001111----------;FRECPS;Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110110-----001001----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110110-----001011----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01011110110-----001111----------;FRSQRTS;Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110010-----000111----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110010-----001001----------;FCMGE (register);Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110010-----001011----------;FACGE;Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110010-----001111----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110110-----000101----------;FABD;Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110110-----001001----------;FCMGT (register);Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110110-----001011----------;FACGT;Advanced SIMD scalar three same FP16;Scalar half precision;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01111110110-----001111----------;UNALLOCATED;Advanced SIMD scalar three same FP16;;U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
01-11110--0-----1001-1----------;UNALLOCATED;Advanced SIMD scalar three same extra;;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01-11110--0-----101--1----------;UNALLOCATED;Advanced SIMD scalar three same extra;;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01-11110--0-----11---1----------;UNALLOCATED;Advanced SIMD scalar three same extra;;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01011110--0-----100001----------;UNALLOCATED;Advanced SIMD scalar three same extra;;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01011110--0-----100011----------;UNALLOCATED;Advanced SIMD scalar three same extra;;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01111110--0-----100001----------;SQRDMLAH (vector);Advanced SIMD scalar three same extra;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01111110--0-----100011----------;SQRDMLSH (vector);Advanced SIMD scalar three same extra;Scalar;U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01-11110--100000000-10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100000001010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100000010-10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100000011010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100000111110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100001000-10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100001001110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100001010110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100001011110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100001100-10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110--100001111010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111100-10000011--10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111100-100001111110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111101-100001011010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111101-100001110010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100000001110----------;SUQADD;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100000011110----------;SQABS;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100000100010----------;CMGT (zero);Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100000100110----------;CMEQ (zero);Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100000101010----------;CMLT (zero);Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100000101110----------;ABS;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100001001010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--100001010010----------;SQXTN, SQXTN2;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-100001011010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-100001101010----------;FCVTNS (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-100001101110----------;FCVTMS (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-100001110010----------;FCVTAS (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111100-100001110110----------;SCVTF (vector, integer);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100000110010----------;FCMGT (zero);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100000110110----------;FCMEQ (zero);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100000111010----------;FCMLT (zero);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100001101010----------;FCVTPS (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100001101110----------;FCVTZS (vector, integer);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100001110110----------;FRECPE;Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-100001111110----------;FRECPX;Advanced SIMD scalar two-register miscellaneous;Single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100000001110----------;USQADD;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100000011110----------;SQNEG;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100000100010----------;CMGE (zero);Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100000100110----------;CMLE (zero);Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100000101010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100000101110----------;NEG (vector);Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100001001010----------;SQXTUN, SQXTUN2;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01111110--100001010010----------;UQXTN, UQXTN2;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-100001011010----------;FCVTXN, FCVTXN2;Advanced SIMD scalar two-register miscellaneous;Scalar;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-100001101010----------;FCVTNU (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-100001101110----------;FCVTMU (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-100001110010----------;FCVTAU (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111100-100001110110----------;UCVTF (vector, integer);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100000110010----------;FCMGE (zero);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100000110110----------;FCMLE (zero);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100000111010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100001101010----------;FCVTPU (vector);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100001101110----------;FCVTZU (vector, integer);Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100001110110----------;FRSQRTE;Advanced SIMD scalar two-register miscellaneous;Scalar single-precision and double-precision;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
011111101-100001111110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous;;U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110-11110000---10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110-111100010--10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110-11110010---10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110-1111001100-10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11110-1111001111010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-111100111100011--10----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-1111001111001111110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-1111011111000111110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-1111011111001110010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111001111001101010----------;FCVTNS (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111001111001101110----------;FCVTMS (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111001111001110010----------;FCVTAS (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111001111001110110----------;SCVTF (vector, integer);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111000110010----------;FCMGT (zero);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111000110110----------;FCMEQ (zero);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111000111010----------;FCMLT (zero);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111001101010----------;FCVTPS (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111001101110----------;FCVTZS (vector, integer);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111001110110----------;FRECPE;Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111011111001111110----------;FRECPX;Advanced SIMD scalar two-register miscellaneous FP16;Half-precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111001111001101010----------;FCVTNU (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111001111001101110----------;FCVTMU (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111001111001110010----------;FCVTAU (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111001111001110110----------;UCVTF (vector, integer);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111000110010----------;FCMGE (zero);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111000110110----------;FCMLE (zero);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111000111010----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111001101010----------;FCVTPU (vector);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111001101110----------;FCVTZU (vector, integer);Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111001110110----------;FRSQRTE;Advanced SIMD scalar two-register miscellaneous FP16;Scalar half precision;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0111111011111001111110----------;UNALLOCATED;Advanced SIMD scalar two-register miscellaneous FP16;;U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
01-11111--------0000-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-11111--------0010-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-11111--------0100-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-11111--------0110-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-11111--------1000-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-11111--------1010-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-11111--------1110-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-1111101------0001-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-1111101------0101-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01-1111101------1001-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01011111--------0011-0----------;SQDMLAL, SQDMLAL2 (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01011111--------0111-0----------;SQDMLSL, SQDMLSL2 (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01011111--------1011-0----------;SQDMULL, SQDMULL2 (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01011111--------1100-0----------;SQDMULH (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01011111--------1101-0----------;SQRDMULH (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01011111--------1111-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0101111100------0001-0----------;FMLA (by element);Advanced SIMD scalar x indexed element;Scalar, half-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0101111100------0101-0----------;FMLS (by element);Advanced SIMD scalar x indexed element;Scalar, half-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0101111100------1001-0----------;FMUL (by element);Advanced SIMD scalar x indexed element;Scalar, half-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
010111111-------0001-0----------;FMLA (by element);Advanced SIMD scalar x indexed element;Scalar, single-precision and double-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
010111111-------0101-0----------;FMLS (by element);Advanced SIMD scalar x indexed element;Scalar, single-precision and double-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
010111111-------1001-0----------;FMUL (by element);Advanced SIMD scalar x indexed element;Scalar, single-precision and double-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01111111--------0011-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01111111--------0111-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01111111--------1011-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01111111--------1100-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01111111--------1101-0----------;SQRDMLAH (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
01111111--------1111-0----------;SQRDMLSH (by element);Advanced SIMD scalar x indexed element;Scalar;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0111111100------0001-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0111111100------0101-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0111111100------1001-0----------;FMULX (by element);Advanced SIMD scalar x indexed element;Scalar, half-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
011111111-------0001-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
011111111-------0101-0----------;UNALLOCATED;Advanced SIMD scalar x indexed element;;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
011111111-------1001-0----------;FMULX (by element);Advanced SIMD scalar x indexed element;Scalar, single-precision and double-precision;U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0--011110-------000011----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------000111----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------001011----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------001111----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------010011----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------010111----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------011011----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------011111----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------101011----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------1011-1----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------110--1----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------111011----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0--011110-------111101----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------000001----------;SSHR;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------000101----------;SSRA;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------001001----------;SRSHR;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------001101----------;SRSRA;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------010001----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------010101----------;SHL;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------011001----------;UNALLOCATED;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------011101----------;SQSHL (immediate);Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------100001----------;SHRN, SHRN2;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------100011----------;RSHRN, RSHRN2;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------100101----------;SQSHRN, SQSHRN2;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------100111----------;SQRSHRN, SQRSHRN2;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------101001----------;SSHLL, SSHLL2;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------111001----------;SCVTF (vector, fixed-point);Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011110-------111111----------;FCVTZS (vector, fixed-point);Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------000001----------;USHR;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------000101----------;USRA;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------001001----------;URSHR;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------001101----------;URSRA;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------010001----------;SRI;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------010101----------;SLI;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------011001----------;SQSHLU;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------011101----------;UQSHL (immediate);Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------100001----------;SQSHRUN, SQSHRUN2;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------100011----------;SQRSHRUN, SQRSHRUN2;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------100101----------;UQSHRN, UQSHRN2;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------100111----------;UQRSHRN, UQRSHRN2;Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------101001----------;USHLL, USHLL2;Advanced SIMD shift by immediate;;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------111001----------;UCVTF (vector, fixed-point);Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011110-------111111----------;FCVTZU (vector, fixed-point);Advanced SIMD shift by immediate;Vector;Q 30 1,U 29 1,immh 19 4,immb 16 3,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110-10-----0---00----------;UNALLOCATED;Advanced SIMD table lookup;;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----000000----------;TBL;Advanced SIMD table lookup;Single register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----000100----------;TBX;Advanced SIMD table lookup;Single register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----001000----------;TBL;Advanced SIMD table lookup;Two register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----001100----------;TBX;Advanced SIMD table lookup;Two register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----010000----------;TBL;Advanced SIMD table lookup;Three register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----010100----------;TBX;Advanced SIMD table lookup;Three register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----011000----------;TBL;Advanced SIMD table lookup;Four register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-001110000-----011100----------;TBX;Advanced SIMD table lookup;Four register table;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0-0011101-0-----0---00----------;UNALLOCATED;Advanced SIMD table lookup;;Q 30 1,op2 22 2,Rm 16 5,len 13 2,op 12 1,Rn 5 5,Rd 0 5,;
0--01110--1-----111100----------;UNALLOCATED;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----000000----------;SADDL, SADDL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----000100----------;SADDW, SADDW2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----001000----------;SSUBL, SSUBL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----001100----------;SSUBW, SSUBW2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----010000----------;ADDHN, ADDHN2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----010100----------;SABAL, SABAL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----011000----------;SUBHN, SUBHN2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----011100----------;SABDL, SABDL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----100000----------;SMLAL, SMLAL2 (vector);Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----100100----------;SQDMLAL, SQDMLAL2 (vector);Advanced SIMD three different;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----101000----------;SMLSL, SMLSL2 (vector);Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----101100----------;SQDMLSL, SQDMLSL2 (vector);Advanced SIMD three different;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----110000----------;SMULL, SMULL2 (vector);Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----110100----------;SQDMULL, SQDMULL2 (vector);Advanced SIMD three different;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----111000----------;PMULL, PMULL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----000000----------;UADDL, UADDL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----000100----------;UADDW, UADDW2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----001000----------;USUBL, USUBL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----001100----------;USUBW, USUBW2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----010000----------;RADDHN, RADDHN2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----010100----------;UABAL, UABAL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----011000----------;RSUBHN, RSUBHN2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----011100----------;UABDL, UABDL2;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----100000----------;UMLAL, UMLAL2 (vector);Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----100100----------;UNALLOCATED;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----101000----------;UMLSL, UMLSL2 (vector);Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----101100----------;UNALLOCATED;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----110000----------;UMULL, UMULL2 (vector);Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----110100----------;UNALLOCATED;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-101110--1-----111000----------;UNALLOCATED;Advanced SIMD three different;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
0-001110--1-----000001----------;SHADD;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----000011----------;SQADD;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----000101----------;SRHADD;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----001001----------;SHSUB;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----001011----------;SQSUB;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----001101----------;CMGT (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----001111----------;CMGE (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----010001----------;SSHL;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----010011----------;SQSHL (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----010101----------;SRSHL;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----010111----------;SQRSHL;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----011001----------;SMAX;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----011011----------;SMIN;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----011101----------;SABD;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----011111----------;SABA;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----100001----------;ADD (vector);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----100011----------;CMTST;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----100101----------;MLA (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----100111----------;MUL (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----101001----------;SMAXP;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----101011----------;SMINP;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----101101----------;SQDMULH (vector);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110--1-----101111----------;ADDP (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----110001----------;FMAXNM (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----110011----------;FMLA (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----110101----------;FADD (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----110111----------;FMULX;Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----111001----------;FCMEQ (register);Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----111101----------;FMAX (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011100-1-----111111----------;FRECPS;Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110001-----000111----------;AND (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110001-----111011----------;FMLAL, FMLAL2 (vector);Advanced SIMD three same;FMLAL;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110011-----000111----------;BIC (vector, register);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110011-----111011----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----110001----------;FMINNM (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----110011----------;FMLS (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----110101----------;FSUB (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----110111----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----111001----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----111101----------;FMIN (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-0011101-1-----111111----------;FRSQRTS;Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110101-----000111----------;ORR (vector, register);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110101-----111011----------;FMLSL, FMLSL2 (vector);Advanced SIMD three same;FMLSL;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110111-----000111----------;ORN (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110111-----111011----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----000001----------;UHADD;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----000011----------;UQADD;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----000101----------;URHADD;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----001001----------;UHSUB;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----001011----------;UQSUB;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----001101----------;CMHI (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----001111----------;CMHS (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----010001----------;USHL;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----010011----------;UQSHL (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----010101----------;URSHL;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----010111----------;UQRSHL;Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----011001----------;UMAX;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----011011----------;UMIN;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----011101----------;UABD;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----011111----------;UABA;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----100001----------;SUB (vector);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----100011----------;CMEQ (register);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----100101----------;MLS (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----100111----------;PMUL;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----101001----------;UMAXP;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----101011----------;UMINP;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----101101----------;SQRDMULH (vector);Advanced SIMD three same;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110--1-----101111----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----110001----------;FMAXNMP (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----110101----------;FADDP (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----110111----------;FMUL (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----111001----------;FCMGE (register);Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----111011----------;FACGE;Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----111101----------;FMAXP (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011100-1-----111111----------;FDIV (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110001-----000111----------;EOR (vector);Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110001-----110011----------;FMLAL, FMLAL2 (vector);Advanced SIMD three same;FMLAL2;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110011-----000111----------;BSL;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110011-----110011----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----110001----------;FMINNMP (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----110101----------;FABD;Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----110111----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----111001----------;FCMGT (register);Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----111011----------;FACGT;Advanced SIMD three same;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----111101----------;FMINP (vector);Advanced SIMD three same;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-1011101-1-----111111----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110101-----000111----------;BIT;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110101-----110011----------;FMLSL, FMLSL2 (vector);Advanced SIMD three same;FMLSL2;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110111-----000111----------;BIF;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-101110111-----110011----------;UNALLOCATED;Advanced SIMD three same;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 5,Rn 5 5,Rd 0 5,;
0-001110010-----000001----------;FMAXNM (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----000011----------;FMLA (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----000101----------;FADD (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----000111----------;FMULX;Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----001001----------;FCMEQ (register);Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----001011----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----001101----------;FMAX (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110010-----001111----------;FRECPS;Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----000001----------;FMINNM (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----000011----------;FMLS (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----000101----------;FSUB (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----000111----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----001001----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----001011----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----001101----------;FMIN (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-001110110-----001111----------;FRSQRTS;Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----000001----------;FMAXNMP (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----000011----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----000101----------;FADDP (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----000111----------;FMUL (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----001001----------;FCMGE (register);Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----001011----------;FACGE;Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----001101----------;FMAXP (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110010-----001111----------;FDIV (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----000001----------;FMINNMP (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----000011----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----000101----------;FABD;Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----000111----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----001001----------;FCMGT (register);Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----001011----------;FACGT;Advanced SIMD three same (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----001101----------;FMINP (vector);Advanced SIMD three same (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0-101110110-----001111----------;UNALLOCATED;Advanced SIMD three same (FP16);;Q 30 1,U 29 1,a 23 1,Rm 16 5,opcode 11 3,Rn 5 5,Rd 0 5,;
0--011100-0-----100111----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0--01110110-----100111----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-001110--0-----100001----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-001110--0-----100011----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-001110--0-----100101----------;SDOT (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-001110--0-----11---1----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-001110100-----100111----------;USDOT (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110--0-----100001----------;SQRDMLAH (vector);Advanced SIMD three-register extension;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110--0-----100011----------;SQRDMLSH (vector);Advanced SIMD three-register extension;Vector;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110--0-----100101----------;UDOT (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110--0-----110--1----------;FCMLA;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110--0-----111-01----------;FCADD;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110000-----111011----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110000-----111111----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110010-----111111----------;BFDOT (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-1011101-0-----111011----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110100-----100111----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110100-----111111----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0-101110110-----111111----------;BFMLALB, BFMLALT (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
00-01110--0-----101--1----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
00101110010-----111011----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01-011100-0-----101--1----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01-011101-0-----1011-1----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01001110100-----101001----------;SMMLA (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01001110100-----101011----------;USMMLA (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01101110010-----111011----------;BFMMLA;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01101110100-----101001----------;UMMLA (vector);Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
01101110100-----101011----------;UNALLOCATED;Advanced SIMD three-register extension;;Q 30 1,U 29 1,size 22 2,Rm 16 5,opcode 11 4,Rn 5 5,Rd 0 5,;
0--01110--100001000-10----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110--100001010110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--011100-10000011--10----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--011101-100001011110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--011101-100001111010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--0111011100001011010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000000010----------;REV64;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000000110----------;REV16 (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000001010----------;SADDLP;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000001110----------;SUQADD;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000010010----------;CLS (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000010110----------;CNT;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000011010----------;SADALP;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000011110----------;SQABS;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000100010----------;CMGT (zero);Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000100110----------;CMEQ (zero);Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000101010----------;CMLT (zero);Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100000101110----------;ABS;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100001001010----------;XTN, XTN2;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100001001110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-001110--100001010010----------;SQXTN, SQXTN2;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001011010----------;FCVTN, FCVTN2;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001011110----------;FCVTL, FCVTL2;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001100010----------;FRINTN (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001100110----------;FRINTM (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001101010----------;FCVTNS (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001101110----------;FCVTMS (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001110010----------;FCVTAS (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001110110----------;SCVTF (vector, integer);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001111010----------;FRINT32Z (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011100-100001111110----------;FRINT64Z (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100000110010----------;FCMGT (zero);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100000110110----------;FCMEQ (zero);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100000111010----------;FCMLT (zero);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100000111110----------;FABS (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001100010----------;FRINTP (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001100110----------;FRINTZ (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001101010----------;FCVTPS (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001101110----------;FCVTZS (vector, integer);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001110010----------;URECPE;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001110110----------;FRECPE;Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-0011101-100001111110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111010100001011010----------;BFCVTN, BFCVTN2;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000000010----------;REV32 (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000000110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000001010----------;UADDLP;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000001110----------;USQADD;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000010010----------;CLZ (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000011010----------;UADALP;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000011110----------;SQNEG;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000100010----------;CMGE (zero);Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000100110----------;CMLE (zero);Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000101010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100000101110----------;NEG (vector);Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100001001010----------;SQXTUN, SQXTUN2;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100001001110----------;SHLL, SHLL2;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-101110--100001010010----------;UQXTN, UQXTN2;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001011010----------;FCVTXN, FCVTXN2;Advanced SIMD two-register miscellaneous;Vector;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001011110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001100010----------;FRINTA (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001100110----------;FRINTX (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001101010----------;FCVTNU (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001101110----------;FCVTMU (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001110010----------;FCVTAU (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001110110----------;UCVTF (vector, integer);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001111010----------;FRINT32X (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011100-100001111110----------;FRINT64X (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111000100000010110----------;NOT;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001100000010110----------;RBIT (vector);Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100000010110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100000110010----------;FCMGE (zero);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100000110110----------;FCMLE (zero);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100000111010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100000111110----------;FNEG (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001100010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001100110----------;FRINTI (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001101010----------;FCVTPU (vector);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001101110----------;FCVTZU (vector, integer);Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001110010----------;URSQRTE;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001110110----------;FRSQRTE;Advanced SIMD two-register miscellaneous;Vector single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-1011101-100001111110----------;FSQRT (vector);Advanced SIMD two-register miscellaneous;Single-precision and double-precision;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111010100001011010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous;;Q 30 1,U 29 1,size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110-11110000---10----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110-111100010--10----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110-11110010---10----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--01110-1111001111010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--011100111100011--10----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--0111001111001111110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--0111011111001110010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001111001100010----------;FRINTN (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001111001100110----------;FRINTM (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001111001101010----------;FCVTNS (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001111001101110----------;FCVTMS (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001111001110010----------;FCVTAS (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111001111001110110----------;SCVTF (vector, integer);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111000110010----------;FCMGT (zero);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111000110110----------;FCMEQ (zero);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111000111010----------;FCMLT (zero);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111000111110----------;FABS (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111001100010----------;FRINTP (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111001100110----------;FRINTZ (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111001101010----------;FCVTPS (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111001101110----------;FCVTZS (vector, integer);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111001110110----------;FRECPE;Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-00111011111001111110----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001111001100010----------;FRINTA (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001111001100110----------;FRINTX (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001111001101010----------;FCVTNU (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001111001101110----------;FCVTMU (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001111001110010----------;FCVTAU (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111001111001110110----------;UCVTF (vector, integer);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111000110010----------;FCMGE (zero);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111000110110----------;FCMLE (zero);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111000111010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111000111110----------;FNEG (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111001100010----------;UNALLOCATED;Advanced SIMD two-register miscellaneous (FP16);;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111001100110----------;FRINTI (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111001101010----------;FCVTPU (vector);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111001101110----------;FCVTZU (vector, integer);Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111001110110----------;FRSQRTE;Advanced SIMD two-register miscellaneous (FP16);Vector half precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0-10111011111001111110----------;FSQRT (vector);Advanced SIMD two-register miscellaneous (FP16);Half-precision;Q 30 1,U 29 1,a 23 1,opcode 12 5,Rn 5 5,Rd 0 5,;
0--0111101------1001-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------0010-0----------;SMLAL, SMLAL2 (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------0011-0----------;SQDMLAL, SQDMLAL2 (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------0110-0----------;SMLSL, SMLSL2 (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------0111-0----------;SQDMLSL, SQDMLSL2 (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------1000-0----------;MUL (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------1010-0----------;SMULL, SMULL2 (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------1011-0----------;SQDMULL, SQDMULL2 (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------1100-0----------;SQDMULH (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------1101-0----------;SQRDMULH (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-001111--------1110-0----------;SDOT (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-0011110-------0000-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-0011110-------0100-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111100------0001-0----------;FMLA (by element);Advanced SIMD vector x indexed element;Vector, half-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111100------0101-0----------;FMLS (by element);Advanced SIMD vector x indexed element;Vector, half-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111100------1001-0----------;FMUL (by element);Advanced SIMD vector x indexed element;Vector, half-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111100------1111-0----------;SUDOT (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111101------0001-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111101------0101-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111101------1111-0----------;BFDOT (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-0011111-------0001-0----------;FMLA (by element);Advanced SIMD vector x indexed element;Vector, single-precision and double-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-0011111-------0101-0----------;FMLS (by element);Advanced SIMD vector x indexed element;Vector, single-precision and double-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-0011111-------1001-0----------;FMUL (by element);Advanced SIMD vector x indexed element;Vector, single-precision and double-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111110------0000-0----------;FMLAL, FMLAL2 (by element);Advanced SIMD vector x indexed element;FMLAL;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111110------0100-0----------;FMLSL, FMLSL2 (by element);Advanced SIMD vector x indexed element;FMLSL;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111110------1111-0----------;USDOT (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111111------0000-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111111------0100-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-00111111------1111-0----------;BFMLALB, BFMLALT (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------0000-0----------;MLA (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------0010-0----------;UMLAL, UMLAL2 (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------0100-0----------;MLS (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------0110-0----------;UMLSL, UMLSL2 (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------1010-0----------;UMULL, UMULL2 (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------1011-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------1101-0----------;SQRDMLAH (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------1110-0----------;UDOT (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-101111--------1111-0----------;SQRDMLSH (by element);Advanced SIMD vector x indexed element;Vector;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-1011110-------1000-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-1011110-------1100-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111100------0001-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111100------0011-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111100------0101-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111100------0111-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111100------1001-0----------;FMULX (by element);Advanced SIMD vector x indexed element;Vector, half-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111101------0--1-0----------;FCMLA (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-1011111-------1001-0----------;FMULX (by element);Advanced SIMD vector x indexed element;Vector, single-precision and double-precision;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111110------0--1-0----------;FCMLA (by element);Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111110------1000-0----------;FMLAL, FMLAL2 (by element);Advanced SIMD vector x indexed element;FMLAL2;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111110------1100-0----------;FMLSL, FMLSL2 (by element);Advanced SIMD vector x indexed element;FMLSL2;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111111------0001-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111111------0011-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111111------0101-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111111------0111-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111111------1000-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
0-10111111------1100-0----------;UNALLOCATED;Advanced SIMD vector x indexed element;;Q 30 1,U 29 1,size 22 2,L 21 1,M 20 1,Rm 16 4,opcode 12 4,H 11 1,Rn 5 5,Rd 0 5,;
-0-11110--0--1------------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0-11110--0-000-----------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0-11110--0-101-----------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0-11110--00-00-----------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0-11110--01-01-----------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0-11110100---------------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0111110--0---------------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
00-11110--0-----0---------------;UNALLOCATED;Conversion between floating-point and fixed-point;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111000000010----------------;SCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;32-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111000000011----------------;UCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;32-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111000011000----------------;FCVTZS (scalar, fixed-point);Conversion between floating-point and fixed-point;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111000011001----------------;FCVTZU (scalar, fixed-point);Conversion between floating-point and fixed-point;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111001000010----------------;SCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;32-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111001000011----------------;UCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;32-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111001011000----------------;FCVTZS (scalar, fixed-point);Conversion between floating-point and fixed-point;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111001011001----------------;FCVTZU (scalar, fixed-point);Conversion between floating-point and fixed-point;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111011000010----------------;SCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;32-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111011000011----------------;UCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;32-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111011011000----------------;FCVTZS (scalar, fixed-point);Conversion between floating-point and fixed-point;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
0001111011011001----------------;FCVTZU (scalar, fixed-point);Conversion between floating-point and fixed-point;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111000000010----------------;SCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;64-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111000000011----------------;UCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;64-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111000011000----------------;FCVTZS (scalar, fixed-point);Conversion between floating-point and fixed-point;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111000011001----------------;FCVTZU (scalar, fixed-point);Conversion between floating-point and fixed-point;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111001000010----------------;SCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;64-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111001000011----------------;UCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;64-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111001011000----------------;FCVTZS (scalar, fixed-point);Conversion between floating-point and fixed-point;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111001011001----------------;FCVTZU (scalar, fixed-point);Conversion between floating-point and fixed-point;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111011000010----------------;SCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;64-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111011000011----------------;UCVTF (scalar, fixed-point);Conversion between floating-point and fixed-point;64-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111011011000----------------;FCVTZS (scalar, fixed-point);Conversion between floating-point and fixed-point;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
1001111011011001----------------;FCVTZU (scalar, fixed-point);Conversion between floating-point and fixed-point;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,scale 10 6,Rn 5 5,Rd 0 5,;
-0-11110--1-101-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
-0-11110--1-110-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
-0-11110--11-01-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
-0-11110--11-10-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
-0011110101--0--000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
-0011110101--10-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
-0111110--1-----000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
00011110001-111-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100000000000----------;FCVTNS (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100001000000----------;FCVTNU (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100010000000----------;SCVTF (scalar, integer);Conversion between floating-point and integer;32-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100011000000----------;UCVTF (scalar, integer);Conversion between floating-point and integer;32-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100100000000----------;FCVTAS (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100101000000----------;FCVTAU (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100110000000----------;FMOV (general);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000100111000000----------;FMOV (general);Conversion between floating-point and integer;32-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000101000000000----------;FCVTPS (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000101001000000----------;FCVTPU (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
000111100011-11-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000110000000000----------;FCVTMS (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000110001000000----------;FCVTMU (scalar);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000111000000000----------;FCVTZS (scalar, integer);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111000111001000000----------;FCVTZU (scalar, integer);Conversion between floating-point and integer;Single-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
000111100110-11-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001100000000000----------;FCVTNS (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001100001000000----------;FCVTNU (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001100010000000----------;SCVTF (scalar, integer);Conversion between floating-point and integer;32-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001100011000000----------;UCVTF (scalar, integer);Conversion between floating-point and integer;32-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001100100000000----------;FCVTAS (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001100101000000----------;FCVTAU (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001101000000000----------;FCVTPS (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001101001000000----------;FCVTPU (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001110000000000----------;FCVTMS (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001110001000000----------;FCVTMU (scalar);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
000111100111011-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001111000000000----------;FCVTZS (scalar, integer);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001111001000000----------;FCVTZU (scalar, integer);Conversion between floating-point and integer;Double-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001111110000000----------;FJCVTZS;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111001111111000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
00011110101--11-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100000000000----------;FCVTNS (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100001000000----------;FCVTNU (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100010000000----------;SCVTF (scalar, integer);Conversion between floating-point and integer;32-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100011000000----------;UCVTF (scalar, integer);Conversion between floating-point and integer;32-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100100000000----------;FCVTAS (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100101000000----------;FCVTAU (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100110000000----------;FMOV (general);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011100111000000----------;FMOV (general);Conversion between floating-point and integer;32-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011101000000000----------;FCVTPS (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011101001000000----------;FCVTPU (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011110000000000----------;FCVTMS (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011110001000000----------;FCVTMU (scalar);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011111000000000----------;FCVTZS (scalar, integer);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
0001111011111001000000----------;FCVTZU (scalar, integer);Conversion between floating-point and integer;Half-precision to 32-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
10011110001--11-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000100000000000----------;FCVTNS (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000100001000000----------;FCVTNU (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000100010000000----------;SCVTF (scalar, integer);Conversion between floating-point and integer;64-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000100011000000----------;UCVTF (scalar, integer);Conversion between floating-point and integer;64-bit to single-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000100100000000----------;FCVTAS (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000100101000000----------;FCVTAU (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000101000000000----------;FCVTPS (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000101001000000----------;FCVTPU (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000110000000000----------;FCVTMS (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000110001000000----------;FCVTMU (scalar);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000111000000000----------;FCVTZS (scalar, integer);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111000111001000000----------;FCVTZU (scalar, integer);Conversion between floating-point and integer;Single-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
10011110011-111-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100000000000----------;FCVTNS (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100001000000----------;FCVTNU (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100010000000----------;SCVTF (scalar, integer);Conversion between floating-point and integer;64-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100011000000----------;UCVTF (scalar, integer);Conversion between floating-point and integer;64-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100100000000----------;FCVTAS (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100101000000----------;FCVTAU (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100110000000----------;FMOV (general);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001100111000000----------;FMOV (general);Conversion between floating-point and integer;64-bit to double-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001101000000000----------;FCVTPS (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001101001000000----------;FCVTPU (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
100111100111-11-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001110000000000----------;FCVTMS (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001110001000000----------;FCVTMU (scalar);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001111000000000----------;FCVTZS (scalar, integer);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111001111001000000----------;FCVTZU (scalar, integer);Conversion between floating-point and integer;Double-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
10011110101-011-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111010101110000000----------;FMOV (general);Conversion between floating-point and integer;Top half of 128-bit to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111010101111000000----------;FMOV (general);Conversion between floating-point and integer;64-bit to top half of 128-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
100111101011-11-000000----------;UNALLOCATED;Conversion between floating-point and integer;;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100000000000----------;FCVTNS (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100001000000----------;FCVTNU (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100010000000----------;SCVTF (scalar, integer);Conversion between floating-point and integer;64-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100011000000----------;UCVTF (scalar, integer);Conversion between floating-point and integer;64-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100100000000----------;FCVTAS (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100101000000----------;FCVTAU (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100110000000----------;FMOV (general);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011100111000000----------;FMOV (general);Conversion between floating-point and integer;64-bit to half-precision;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011101000000000----------;FCVTPS (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011101001000000----------;FCVTPU (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011110000000000----------;FCVTMS (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011110001000000----------;FCVTMU (scalar);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011111000000000----------;FCVTZS (scalar, integer);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
1001111011111001000000----------;FCVTZU (scalar, integer);Conversion between floating-point and integer;Half-precision to 64-bit;sf 31 1,S 29 1,ptype 22 2,rmode 19 2,opcode 16 3,Rn 5 5,Rd 0 5,;
01001110--10100-1---10----------;UNALLOCATED;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01001110--10100000--10----------;UNALLOCATED;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01001110--101001----10----------;UNALLOCATED;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01001110-110100-----10----------;UNALLOCATED;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0100111000101000010010----------;AESE;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0100111000101000010110----------;AESD;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0100111000101000011010----------;AESMC;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0100111000101000011110----------;AESIMC;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010011101-10100-----10----------;UNALLOCATED;Cryptographic AES;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
11001110000-----0---------------;EOR3;Cryptographic four-register;;Op0 21 2,Rm 16 5,Ra 10 5,Rn 5 5,Rd 0 5,;
11001110001-----0---------------;BCAX;Cryptographic four-register;;Op0 21 2,Rm 16 5,Ra 10 5,Rn 5 5,Rd 0 5,;
11001110010-----0---------------;SM3SS1;Cryptographic four-register;;Op0 21 2,Rm 16 5,Ra 10 5,Rn 5 5,Rd 0 5,;
11001110011-----0---------------;UNALLOCATED;Cryptographic four-register;;Op0 21 2,Rm 16 5,Ra 10 5,Rn 5 5,Rd 0 5,;
01011110--0-----011100----------;UNALLOCATED;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110-10-----0---00----------;UNALLOCATED;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----000000----------;SHA1C;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----000100----------;SHA1P;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----001000----------;SHA1M;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----001100----------;SHA1SU0;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----010000----------;SHA256H;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----010100----------;SHA256H2;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
01011110000-----011000----------;SHA256SU1;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
010111101-0-----0---00----------;UNALLOCATED;Cryptographic three-register SHA;;size 22 2,Rm 16 5,opcode 12 3,Rn 5 5,Rd 0 5,;
11001110011-----100000----------;SHA512H;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----100001----------;SHA512H2;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----100010----------;SHA512SU1;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----100011----------;RAX1;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----110000----------;SM3PARTW1;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----110001----------;SM3PARTW2;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----110010----------;SM4EKEY;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110011-----110011----------;UNALLOCATED;Cryptographic three-register SHA 512;;Rm 16 5,O 14 1,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110010-----10--00----------;SM3TT1A;Cryptographic three-register, imm2;;Rm 16 5,imm2 12 2,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110010-----10--01----------;SM3TT1B;Cryptographic three-register, imm2;;Rm 16 5,imm2 12 2,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110010-----10--10----------;SM3TT2A;Cryptographic three-register, imm2;;Rm 16 5,imm2 12 2,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110010-----10--11----------;SM3TT2B;Cryptographic three-register, imm2;;Rm 16 5,imm2 12 2,opcode 10 2,Rn 5 5,Rd 0 5,;
11001110100---------------------;XAR;Cryptographic three-register, imm6;;Rm 16 5,imm6 10 6,Rn 5 5,Rd 0 5,;
01011110--10100--1--10----------;UNALLOCATED;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--10100-1---10----------;UNALLOCATED;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110--101001----10----------;UNALLOCATED;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
01011110-110100-----10----------;UNALLOCATED;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111000101000000010----------;SHA1H;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111000101000000110----------;SHA1SU1;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111000101000001010----------;SHA256SU0;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
0101111000101000001110----------;UNALLOCATED;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
010111101-10100-----10----------;UNALLOCATED;Cryptographic two-register SHA;;size 22 2,opcode 12 5,Rn 5 5,Rd 0 5,;
1100111011000000100000----------;SHA512SU0;Cryptographic two-register SHA 512;;opcode 10 2,Rn 5 5,Rd 0 5,;
1100111011000000100001----------;SM4E;Cryptographic two-register SHA 512;;opcode 10 2,Rn 5 5,Rd 0 5,;
110011101100000010001-----------;UNALLOCATED;Cryptographic two-register SHA 512;;opcode 10 2,Rn 5 5,Rd 0 5,;
-0-11110--1-------1000---------1;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0-11110--1-------1000--------1-;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0-11110--1-------1000-------1--;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0-11110--1------11000----------;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0-11110--1-----1-1000----------;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0-11110101-------1000----------;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0111110--1-------1000----------;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110001-----001000-----00000;FCMP;Floating-point compare;Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110001-----001000-----01000;FCMP;Floating-point compare;Single-precision, zero;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110001-----001000-----10000;FCMPE;Floating-point compare;Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110001-----001000-----11000;FCMPE;Floating-point compare;Single-precision, zero;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110011-----001000-----00000;FCMP;Floating-point compare;Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110011-----001000-----01000;FCMP;Floating-point compare;Double-precision, zero;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110011-----001000-----10000;FCMPE;Floating-point compare;Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110011-----001000-----11000;FCMPE;Floating-point compare;Double-precision, zero;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110111-----001000-----00000;FCMP;Floating-point compare;Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110111-----001000-----01000;FCMP;Floating-point compare;Half-precision, zero;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110111-----001000-----10000;FCMPE;Floating-point compare;Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
00011110111-----001000-----11000;FCMPE;Floating-point compare;Half-precision, zero;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
10-11110--1-------1000----------;UNALLOCATED;Floating-point compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,op 14 2,Rn 5 5,opcode2 0 5,;
-0-11110101---------01----------;UNALLOCATED;Floating-point conditional compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
-0111110--1---------01----------;UNALLOCATED;Floating-point conditional compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
00011110001---------01-----0----;FCCMP;Floating-point conditional compare;Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
00011110001---------01-----1----;FCCMPE;Floating-point conditional compare;Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
00011110011---------01-----0----;FCCMP;Floating-point conditional compare;Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
00011110011---------01-----1----;FCCMPE;Floating-point conditional compare;Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
00011110111---------01-----0----;FCCMP;Floating-point conditional compare;Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
00011110111---------01-----1----;FCCMPE;Floating-point conditional compare;Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
10-11110--1---------01----------;UNALLOCATED;Floating-point conditional compare;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,op 4 1,nzcv 0 4,;
-0-11110101---------11----------;UNALLOCATED;Floating-point conditional select;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,Rd 0 5,;
-0111110--1---------11----------;UNALLOCATED;Floating-point conditional select;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,Rd 0 5,;
00011110001---------11----------;FCSEL;Floating-point conditional select;Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,Rd 0 5,;
00011110011---------11----------;FCSEL;Floating-point conditional select;Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,Rd 0 5,;
00011110111---------11----------;FCSEL;Floating-point conditional select;Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,Rd 0 5,;
10-11110--1---------11----------;UNALLOCATED;Floating-point conditional select;;M 31 1,S 29 1,ptype 22 2,Rm 16 5,cond 12 4,Rn 5 5,Rd 0 5,;
-0-11110--11-----10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
-0111110--1------10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100000010000----------;FMOV (register);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100000110000----------;FABS (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100001010000----------;FNEG (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100001110000----------;FSQRT (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100010010000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100010110000----------;FCVT;Floating-point data-processing (1 source);Single-precision to double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100011010000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100011110000----------;FCVT;Floating-point data-processing (1 source);Single-precision to half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100100010000----------;FRINTN (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100100110000----------;FRINTP (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100101010000----------;FRINTM (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100101110000----------;FRINTZ (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100110010000----------;FRINTA (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100110110000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100111010000----------;FRINTX (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000100111110000----------;FRINTI (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000101000010000----------;FRINT32Z (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000101000110000----------;FRINT32X (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000101001010000----------;FRINT64Z (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111000101001110000----------;FRINT64X (scalar);Floating-point data-processing (1 source);Single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
000111100010101--10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
00011110001011---10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100000010000----------;FMOV (register);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100000110000----------;FABS (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100001010000----------;FNEG (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100001110000----------;FSQRT (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100010010000----------;FCVT;Floating-point data-processing (1 source);Double-precision to single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100010110000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100011010000----------;BFCVT;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100011110000----------;FCVT;Floating-point data-processing (1 source);Double-precision to half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100100010000----------;FRINTN (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100100110000----------;FRINTP (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100101010000----------;FRINTM (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100101110000----------;FRINTZ (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100110010000----------;FRINTA (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100110110000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100111010000----------;FRINTX (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001100111110000----------;FRINTI (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001101000010000----------;FRINT32Z (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001101000110000----------;FRINT32X (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001101001010000----------;FRINT64Z (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111001101001110000----------;FRINT64X (scalar);Floating-point data-processing (1 source);Double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
000111100110101--10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
00011110011011---10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
000111101010-----10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100000010000----------;FMOV (register);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100000110000----------;FABS (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100001010000----------;FNEG (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100001110000----------;FSQRT (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100010010000----------;FCVT;Floating-point data-processing (1 source);Half-precision to single-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100010110000----------;FCVT;Floating-point data-processing (1 source);Half-precision to double-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100011-10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100100010000----------;FRINTN (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100100110000----------;FRINTP (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100101010000----------;FRINTM (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100101110000----------;FRINTZ (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100110010000----------;FRINTA (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100110110000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100111010000----------;FRINTX (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011100111110000----------;FRINTI (scalar);Floating-point data-processing (1 source);Half-precision;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
0001111011101----10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
10-11110--1------10000----------;UNALLOCATED;Floating-point data-processing (1 source);;M 31 1,S 29 1,ptype 22 2,opcode 15 6,Rn 5 5,Rd 0 5,;
-0-11110--1-----1--110----------;UNALLOCATED;Floating-point data-processing (2 source);;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
-0-11110--1-----1-1-10----------;UNALLOCATED;Floating-point data-processing (2 source);;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
-0-11110--1-----11--10----------;UNALLOCATED;Floating-point data-processing (2 source);;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
-0-11110101---------10----------;UNALLOCATED;Floating-point data-processing (2 source);;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
-0111110--1---------10----------;UNALLOCATED;Floating-point data-processing (2 source);;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----000010----------;FMUL (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----000110----------;FDIV (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----001010----------;FADD (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----001110----------;FSUB (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----010010----------;FMAX (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----010110----------;FMIN (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----011010----------;FMAXNM (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----011110----------;FMINNM (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110001-----100010----------;FNMUL (scalar);Floating-point data-processing (2 source);Single-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----000010----------;FMUL (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----000110----------;FDIV (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----001010----------;FADD (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----001110----------;FSUB (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----010010----------;FMAX (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----010110----------;FMIN (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----011010----------;FMAXNM (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----011110----------;FMINNM (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110011-----100010----------;FNMUL (scalar);Floating-point data-processing (2 source);Double-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----000010----------;FMUL (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----000110----------;FDIV (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----001010----------;FADD (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----001110----------;FSUB (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----010010----------;FMAX (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----010110----------;FMIN (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----011010----------;FMAXNM (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----011110----------;FMINNM (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
00011110111-----100010----------;FNMUL (scalar);Floating-point data-processing (2 source);Half-precision;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
10-11110--1---------10----------;UNALLOCATED;Floating-point data-processing (2 source);;M 31 1,S 29 1,ptype 22 2,Rm 16 5,opcode 12 4,Rn 5 5,Rd 0 5,;
-0-1111110----------------------;UNALLOCATED;Floating-point data-processing (3 source);;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0111111------------------------;UNALLOCATED;Floating-point data-processing (3 source);;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111000-----0---------------;FMADD;Floating-point data-processing (3 source);Single-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111000-----1---------------;FMSUB;Floating-point data-processing (3 source);Single-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111001-----0---------------;FNMADD;Floating-point data-processing (3 source);Single-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111001-----1---------------;FNMSUB;Floating-point data-processing (3 source);Single-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111010-----0---------------;FMADD;Floating-point data-processing (3 source);Double-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111010-----1---------------;FMSUB;Floating-point data-processing (3 source);Double-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111011-----0---------------;FNMADD;Floating-point data-processing (3 source);Double-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111011-----1---------------;FNMSUB;Floating-point data-processing (3 source);Double-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111110-----0---------------;FMADD;Floating-point data-processing (3 source);Half-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111110-----1---------------;FMSUB;Floating-point data-processing (3 source);Half-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111111-----0---------------;FNMADD;Floating-point data-processing (3 source);Half-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
00011111111-----1---------------;FNMSUB;Floating-point data-processing (3 source);Half-precision;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
10-11111------------------------;UNALLOCATED;Floating-point data-processing (3 source);;M 31 1,S 29 1,ptype 22 2,o1 21 1,Rm 16 5,o0 15 1,Ra 10 5,Rn 5 5,Rd 0 5,;
-0-11110--1--------100----1-----;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
-0-11110--1--------100---1------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
-0-11110--1--------100--1-------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
-0-11110--1--------100-1--------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
-0-11110--1--------1001---------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
-0-11110101--------100----------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
-0111110--1--------100----------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
00011110001--------10000000-----;FMOV (scalar, immediate);Floating-point immediate;Single-precision;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
00011110011--------10000000-----;FMOV (scalar, immediate);Floating-point immediate;Double-precision;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
00011110111--------10000000-----;FMOV (scalar, immediate);Floating-point immediate;Half-precision;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
10-11110--1--------100----------;UNALLOCATED;Floating-point immediate;;M 31 1,S 29 1,ptype 22 2,imm8 13 8,imm5 5 5,Rd 0 5,;
00000100001-----1010------------;ADR;SVE address generation;Unpacked 32-bit signed offsets;opc 22 2,Zm 16 5,msz 10 2,Zn 5 5,Zd 0 5,;
00000100011-----1010------------;ADR;SVE address generation;Unpacked 32-bit unsigned offsets;opc 22 2,Zm 16 5,msz 10 2,Zn 5 5,Zd 0 5,;
000001001-1-----1010------------;ADR;SVE address generation;Packed offsets;opc 22 2,Zm 16 5,msz 10 2,Zn 5 5,Zd 0 5,;
0000000000000000----------------;UDF;Reserved;;imm16 0 16,;
0000010000100000101111----------;MOVPRFX (unpredicated);SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
0000010000100001101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
000001000010001-101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
00000100001001--101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
0000010000101---101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
000001000011----101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
00000100011-----101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
000001001-1-----101111----------;UNALLOCATED;SVE constructive prefix (unpredicated);;opc 22 2,opc2 16 5,Zn 5 5,Zd 0 5,;
00000100--100000101110----------;FEXPA;SVE floating-point exponential accelerator;;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
00000100--100001101110----------;UNALLOCATED;SVE floating-point exponential accelerator;;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
00000100--10001-101110----------;UNALLOCATED;SVE floating-point exponential accelerator;;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
00000100--1001--101110----------;UNALLOCATED;SVE floating-point exponential accelerator;;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
00000100--101---101110----------;UNALLOCATED;SVE floating-point exponential accelerator;;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
00000100--11----101110----------;UNALLOCATED;SVE floating-point exponential accelerator;;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
00000100--1-----101100----------;FTSSEL;SVE floating-point trig select coefficient;;size 22 2,Zm 16 5,op 10 1,Zn 5 5,Zd 0 5,;
00000100--1-----101101----------;UNALLOCATED;SVE floating-point trig select coefficient;;size 22 2,Zm 16 5,op 10 1,Zn 5 5,Zd 0 5,;
00000100--10----111001----------;UNALLOCATED;SVE element count;;size 22 2,imm4 16 4,op 10 1,pattern 5 5,Rd 0 5,;
000001000010----111000----------;CNTB, CNTD, CNTH, CNTW;SVE element count;Byte;size 22 2,imm4 16 4,op 10 1,pattern 5 5,Rd 0 5,;
000001000110----111000----------;CNTB, CNTD, CNTH, CNTW;SVE element count;Halfword;size 22 2,imm4 16 4,op 10 1,pattern 5 5,Rd 0 5,;
000001001010----111000----------;CNTB, CNTD, CNTH, CNTW;SVE element count;Word;size 22 2,imm4 16 4,op 10 1,pattern 5 5,Rd 0 5,;
000001001110----111000----------;CNTB, CNTD, CNTH, CNTW;SVE element count;Doubleword;size 22 2,imm4 16 4,op 10 1,pattern 5 5,Rd 0 5,;
000001000011----111000----------;INCB, INCD, INCH, INCW (scalar);SVE inc/dec register by element count;Byte;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001000011----111001----------;DECB, DECD, DECH, DECW (scalar);SVE inc/dec register by element count;Byte;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001000111----111000----------;INCB, INCD, INCH, INCW (scalar);SVE inc/dec register by element count;Halfword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001000111----111001----------;DECB, DECD, DECH, DECW (scalar);SVE inc/dec register by element count;Halfword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001001011----111000----------;INCB, INCD, INCH, INCW (scalar);SVE inc/dec register by element count;Word;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001001011----111001----------;DECB, DECD, DECH, DECW (scalar);SVE inc/dec register by element count;Word;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001001111----111000----------;INCB, INCD, INCH, INCW (scalar);SVE inc/dec register by element count;Doubleword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001001111----111001----------;DECB, DECD, DECH, DECW (scalar);SVE inc/dec register by element count;Doubleword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Rdn 0 5,;
000001000011----11000-----------;UNALLOCATED;SVE inc/dec vector by element count;;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001000111----110000----------;INCD, INCH, INCW (vector);SVE inc/dec vector by element count;Halfword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001000111----110001----------;DECD, DECH, DECW (vector);SVE inc/dec vector by element count;Halfword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001001011----110000----------;INCD, INCH, INCW (vector);SVE inc/dec vector by element count;Word;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001001011----110001----------;DECD, DECH, DECW (vector);SVE inc/dec vector by element count;Word;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001001111----110000----------;INCD, INCH, INCW (vector);SVE inc/dec vector by element count;Doubleword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001001111----110001----------;DECD, DECH, DECW (vector);SVE inc/dec vector by element count;Doubleword;size 22 2,imm4 16 4,D 10 1,pattern 5 5,Zdn 0 5,;
000001000010----111100----------;SQINCB;SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000010----111101----------;UQINCB;SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000010----111110----------;SQDECB;SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000010----111111----------;UQDECB;SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000011----111100----------;SQINCB;SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000011----111101----------;UQINCB;SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000011----111110----------;SQDECB;SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000011----111111----------;UQDECB;SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000110----111100----------;SQINCH (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000110----111101----------;UQINCH (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000110----111110----------;SQDECH (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000110----111111----------;UQDECH (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000111----111100----------;SQINCH (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000111----111101----------;UQINCH (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000111----111110----------;SQDECH (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000111----111111----------;UQDECH (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001010----111100----------;SQINCW (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001010----111101----------;UQINCW (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001010----111110----------;SQDECW (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001010----111111----------;UQDECW (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001011----111100----------;SQINCW (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001011----111101----------;UQINCW (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001011----111110----------;SQDECW (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001011----111111----------;UQDECW (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001110----111100----------;SQINCD (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001110----111101----------;UQINCD (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001110----111110----------;SQDECD (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001110----111111----------;UQDECD (scalar);SVE saturating inc/dec register by element count;32-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001111----111100----------;SQINCD (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001111----111101----------;UQINCD (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001111----111110----------;SQDECD (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001001111----111111----------;UQDECD (scalar);SVE saturating inc/dec register by element count;64-bit;size 22 2,sf 20 1,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Rdn 0 5,;
000001000010----1100------------;UNALLOCATED;SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001000110----110000----------;SQINCH (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001000110----110001----------;UQINCH (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001000110----110010----------;SQDECH (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001000110----110011----------;UQDECH (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001010----110000----------;SQINCW (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001010----110001----------;UQINCW (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001010----110010----------;SQDECW (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001010----110011----------;UQDECW (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001110----110000----------;SQINCD (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001110----110001----------;UQINCD (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001110----110010----------;SQDECD (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
000001001110----110011----------;UQDECD (vector);SVE saturating inc/dec vector by element count;;size 22 2,imm4 16 4,D 11 1,U 10 1,pattern 5 5,Zdn 0 5,;
00000101001-----000-------------;EXT;SVE extract vector (immediate offset, destructive);;imm8h 16 5,imm8l 10 3,Zm 5 5,Zdn 0 5,;
00000101101-----000000----------;ZIP1, ZIP2 (vectors);SVE permute vector segments;Low halves (quadwords);op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101101-----000001----------;ZIP1, ZIP2 (vectors);SVE permute vector segments;High halves (quadwords);op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101101-----000010----------;UZP1, UZP2 (vectors);SVE permute vector segments;Even (quadwords);op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101101-----000011----------;UZP1, UZP2 (vectors);SVE permute vector segments;Odd (quadwords);op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101101-----00010-----------;UNALLOCATED;SVE permute vector segments;;op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101101-----000110----------;TRN1, TRN2 (vectors);SVE permute vector segments;Even (quadwords);op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101101-----000111----------;TRN1, TRN2 (vectors);SVE permute vector segments;Odd (quadwords);op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101111-----000-------------;UNALLOCATED;SVE permute vector segments;;op 22 1,Zm 16 5,opc2 10 3,Zn 5 5,Zd 0 5,;
00000101000000------------------;ORR (immediate);SVE bitwise logical with immediate (unpredicated);;opc 22 2,imm13 5 13,Zdn 0 5,;
00000101010000------------------;EOR (immediate);SVE bitwise logical with immediate (unpredicated);;opc 22 2,imm13 5 13,Zdn 0 5,;
00000101100000------------------;AND (immediate);SVE bitwise logical with immediate (unpredicated);;opc 22 2,imm13 5 13,Zdn 0 5,;
00000101110000------------------;DUPM;SVE broadcast bitmask immediate;;imm13 5 13,Zd 0 5,;
00000101--01----110-------------;FCPY;SVE copy floating-point immediate (predicated);;size 22 2,Pg 16 4,imm8 5 8,Zd 0 5,;
00000101--01----00--------------;CPY (immediate, zeroing);SVE copy integer immediate (predicated);;size 22 2,Pg 16 4,M 14 1,sh 13 1,imm8 5 8,Zd 0 5,;
00000101--01----01--------------;CPY (immediate, merging);SVE copy integer immediate (predicated);;size 22 2,Pg 16 4,M 14 1,sh 13 1,imm8 5 8,Zd 0 5,;
00000101--1-----001000----------;DUP (indexed);SVE broadcast indexed element;;imm2 22 2,tsz 16 5,Zn 5 5,Zd 0 5,;
00000101--1-----001100----------;TBL;SVE table lookup;;size 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
00000101--100000001110----------;DUP (scalar);SVE broadcast general register;;size 22 2,Rn 5 5,Zd 0 5,;
00000101--110100001110----------;INSR (SIMD&FP scalar);SVE insert SIMD&FP scalar register;;size 22 2,Vm 5 5,Zdn 0 5,;
00000101--100100001110----------;INSR (scalar);SVE insert general register;;size 22 2,Rm 5 5,Zdn 0 5,;
00000101--111000001110----------;REV (vector);SVE reverse vector elements;;size 22 2,Zn 5 5,Zd 0 5,;
00000101--110000001110----------;SUNPKHI, SUNPKLO;SVE unpack vector elements;Low half;size 22 2,U 17 1,H 16 1,Zn 5 5,Zd 0 5,;
00000101--110001001110----------;SUNPKHI, SUNPKLO;SVE unpack vector elements;High half;size 22 2,U 17 1,H 16 1,Zn 5 5,Zd 0 5,;
00000101--110010001110----------;UUNPKHI, UUNPKLO;SVE unpack vector elements;Low half;size 22 2,U 17 1,H 16 1,Zn 5 5,Zd 0 5,;
00000101--110011001110----------;UUNPKHI, UUNPKLO;SVE unpack vector elements;High half;size 22 2,U 17 1,H 16 1,Zn 5 5,Zd 0 5,;
00000101--10----0100000----0----;ZIP1, ZIP2 (predicates);SVE permute predicate elements;Low halves;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--10----0100010----0----;ZIP1, ZIP2 (predicates);SVE permute predicate elements;High halves;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--10----0100100----0----;UZP1, UZP2 (predicates);SVE permute predicate elements;Even;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--10----0100110----0----;UZP1, UZP2 (predicates);SVE permute predicate elements;Odd;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--10----0101000----0----;TRN1, TRN2 (predicates);SVE permute predicate elements;Even;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--10----0101010----0----;TRN1, TRN2 (predicates);SVE permute predicate elements;Odd;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--10----01011-0----0----;UNALLOCATED;SVE permute predicate elements;;size 22 2,Pm 16 4,opc 11 2,H 10 1,Pn 5 4,Pd 0 4,;
00000101--1101000100000----0----;REV (predicate);SVE reverse predicate elements;;size 22 2,Pn 5 4,Pd 0 4,;
00000101001100000100000----0----;PUNPKHI, PUNPKLO;SVE unpack predicate elements;Low half;H 16 1,Pn 5 4,Pd 0 4,;
00000101001100010100000----0----;PUNPKHI, PUNPKLO;SVE unpack predicate elements;High half;H 16 1,Pn 5 4,Pd 0 4,;
00000101--1-----011000----------;ZIP1, ZIP2 (vectors);SVE permute vector elements;Low halves;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--1-----011001----------;ZIP1, ZIP2 (vectors);SVE permute vector elements;High halves;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--1-----011010----------;UZP1, UZP2 (vectors);SVE permute vector elements;Even;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--1-----011011----------;UZP1, UZP2 (vectors);SVE permute vector elements;Odd;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--1-----011100----------;TRN1, TRN2 (vectors);SVE permute vector elements;Even;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--1-----011101----------;TRN1, TRN2 (vectors);SVE permute vector elements;Odd;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--1-----01111-----------;UNALLOCATED;SVE permute vector elements;;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
00000101--100001100-------------;COMPACT;SVE compress active elements;;size 22 2,Pg 10 3,Zn 5 5,Zd 0 5,;
00000101--101000100-------------;CLASTA (vectors);SVE conditionally broadcast element to vector;;size 22 2,B 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000101--101001100-------------;CLASTB (vectors);SVE conditionally broadcast element to vector;;size 22 2,B 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000101--101010100-------------;CLASTA (SIMD&FP scalar);SVE conditionally extract element to SIMD&FP scalar;;size 22 2,B 16 1,Pg 10 3,Zm 5 5,Vdn 0 5,;
00000101--101011100-------------;CLASTB (SIMD&FP scalar);SVE conditionally extract element to SIMD&FP scalar;;size 22 2,B 16 1,Pg 10 3,Zm 5 5,Vdn 0 5,;
00000101--110000101-------------;CLASTA (scalar);SVE conditionally extract element to general register;;size 22 2,B 16 1,Pg 10 3,Zm 5 5,Rdn 0 5,;
00000101--110001101-------------;CLASTB (scalar);SVE conditionally extract element to general register;;size 22 2,B 16 1,Pg 10 3,Zm 5 5,Rdn 0 5,;
00000101--100000100-------------;CPY (SIMD&FP scalar);SVE copy SIMD&FP scalar register to vector (predicated);;size 22 2,Pg 10 3,Vn 5 5,Zd 0 5,;
00000101--101000101-------------;CPY (scalar);SVE copy general register to vector (predicated);;size 22 2,Pg 10 3,Rn 5 5,Zd 0 5,;
00000101--100010100-------------;LASTA (SIMD&FP scalar);SVE extract element to SIMD&FP scalar register;;size 22 2,B 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000101--100011100-------------;LASTB (SIMD&FP scalar);SVE extract element to SIMD&FP scalar register;;size 22 2,B 16 1,Pg 10 3,Zn 5 5,Vd 0 5,;
00000101--100000101-------------;LASTA (scalar);SVE extract element to general register;;size 22 2,B 16 1,Pg 10 3,Zn 5 5,Rd 0 5,;
00000101--100001101-------------;LASTB (scalar);SVE extract element to general register;;size 22 2,B 16 1,Pg 10 3,Zn 5 5,Rd 0 5,;
00000101--100100100-------------;REVB, REVH, REVW;SVE reverse within elements;Byte;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
00000101--100101100-------------;REVB, REVH, REVW;SVE reverse within elements;Halfword;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
00000101--100110100-------------;REVB, REVH, REVW;SVE reverse within elements;Word;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
00000101--100111100-------------;RBIT;SVE reverse within elements;;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
00000101--101100100-------------;SPLICE;SVE vector splice (destructive);;size 22 2,Pg 10 3,Zm 5 5,Zdn 0 5,;
00000101--1-----11--------------;SEL (vectors);SVE select vector elements (predicated);;size 22 2,Zm 16 5,Pg 10 4,Zn 5 5,Zd 0 5,;
00100100--0-----000--------0----;CMP<cc> (vectors);SVE integer compare vectors;Higher or same;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----000--------1----;CMP<cc> (vectors);SVE integer compare vectors;Higher;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----001--------0----;CMP<cc> (wide elements);SVE integer compare vectors;Equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----001--------1----;CMP<cc> (wide elements);SVE integer compare vectors;Not equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----100--------0----;CMP<cc> (vectors);SVE integer compare vectors;Greater than or equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----100--------1----;CMP<cc> (vectors);SVE integer compare vectors;Greater than;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----101--------0----;CMP<cc> (vectors);SVE integer compare vectors;Equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----101--------1----;CMP<cc> (vectors);SVE integer compare vectors;Not equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----010--------0----;CMP<cc> (wide elements);SVE integer compare with wide elements;Greater than or equal;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----010--------1----;CMP<cc> (wide elements);SVE integer compare with wide elements;Greater than;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----011--------0----;CMP<cc> (wide elements);SVE integer compare with wide elements;Less than;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----011--------1----;CMP<cc> (wide elements);SVE integer compare with wide elements;Less than or equal;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----110--------0----;CMP<cc> (wide elements);SVE integer compare with wide elements;Higher or same;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----110--------1----;CMP<cc> (wide elements);SVE integer compare with wide elements;Higher;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----111--------0----;CMP<cc> (wide elements);SVE integer compare with wide elements;Lower;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--0-----111--------1----;CMP<cc> (wide elements);SVE integer compare with wide elements;Lower or same;size 22 2,Zm 16 5,U 15 1,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--1-------0--------0----;CMP<cc> (immediate);SVE integer compare with unsigned immediate;Higher or same;size 22 2,imm7 14 7,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--1-------0--------1----;CMP<cc> (immediate);SVE integer compare with unsigned immediate;Higher;size 22 2,imm7 14 7,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--1-------1--------0----;CMP<cc> (immediate);SVE integer compare with unsigned immediate;Lower;size 22 2,imm7 14 7,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100100--1-------1--------1----;CMP<cc> (immediate);SVE integer compare with unsigned immediate;Lower or same;size 22 2,imm7 14 7,lt 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
001001010000----01----0----0----;AND, ANDS (predicates);SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010000----01----0----1----;BIC, BICS (predicates);SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010000----01----1----0----;EOR, EORS (predicates);SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010000----01----1----1----;SEL (predicates);SVE predicate logical operations;;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010100----01----0----0----;AND, ANDS (predicates);SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010100----01----0----1----;BIC, BICS (predicates);SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010100----01----1----0----;EOR, EORS (predicates);SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010100----01----1----1----;UNALLOCATED;SVE predicate logical operations;;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011000----01----0----0----;ORR, ORRS (predicates);SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011000----01----0----1----;ORN, ORNS (predicates);SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011000----01----1----0----;NOR, NORS;SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011000----01----1----1----;NAND, NANDS;SVE predicate logical operations;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011100----01----0----0----;ORR, ORRS (predicates);SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011100----01----0----1----;ORN, ORNS (predicates);SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011100----01----1----0----;NOR, NORS;SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001011100----01----1----1----;NAND, NANDS;SVE predicate logical operations;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,o2 9 1,Pn 5 4,o3 4 1,Pd 0 4,;
001001010000----11----0----0----;BRKPA, BRKPAS;SVE propagate break from previous partition;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,Pn 5 4,B 4 1,Pd 0 4,;
001001010000----11----0----1----;BRKPB, BRKPBS;SVE propagate break from previous partition;Not setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,Pn 5 4,B 4 1,Pd 0 4,;
001001010100----11----0----0----;BRKPA, BRKPAS;SVE propagate break from previous partition;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,Pn 5 4,B 4 1,Pd 0 4,;
001001010100----11----0----1----;BRKPB, BRKPBS;SVE propagate break from previous partition;Setting the condition flags;op 23 1,S 22 1,Pm 16 4,Pg 10 4,Pn 5 4,B 4 1,Pd 0 4,;
001001011-00----11----0---------;UNALLOCATED;SVE propagate break from previous partition;;op 23 1,S 22 1,Pm 16 4,Pg 10 4,Pn 5 4,B 4 1,Pd 0 4,;
00100101-101000001----0----1----;UNALLOCATED;SVE partition break condition;;B 23 1,S 22 1,Pg 10 4,Pn 5 4,M 4 1,Pd 0 4,;
001001010001000001----0---------;BRKA, BRKAS;SVE partition break condition;Not setting the condition flags;B 23 1,S 22 1,Pg 10 4,Pn 5 4,M 4 1,Pd 0 4,;
001001010101000001----0----0----;BRKA, BRKAS;SVE partition break condition;Setting the condition flags;B 23 1,S 22 1,Pg 10 4,Pn 5 4,M 4 1,Pd 0 4,;
001001011001000001----0---------;BRKB, BRKBS;SVE partition break condition;Not setting the condition flags;B 23 1,S 22 1,Pg 10 4,Pn 5 4,M 4 1,Pd 0 4,;
001001011101000001----0----0----;BRKB, BRKBS;SVE partition break condition;Setting the condition flags;B 23 1,S 22 1,Pg 10 4,Pn 5 4,M 4 1,Pd 0 4,;
001001010001100001----0----0----;BRKN, BRKNS;SVE propagate break to next partition;Not setting the condition flags;S 22 1,Pg 10 4,Pn 5 4,Pdm 0 4,;
001001010101100001----0----0----;BRKN, BRKNS;SVE propagate break to next partition;Setting the condition flags;S 22 1,Pg 10 4,Pn 5 4,Pdm 0 4,;
00100101000110001100000----0----;UNALLOCATED;SVE predicate first active;;op 23 1,S 22 1,Pg 5 4,Pdn 0 4,;
00100101010110001100000----0----;PFIRST;SVE predicate first active;;op 23 1,S 22 1,Pg 5 4,Pdn 0 4,;
001001011-0110001100000----0----;UNALLOCATED;SVE predicate first active;;op 23 1,S 22 1,Pg 5 4,Pdn 0 4,;
00100101--011000111000-----0----;PTRUE, PTRUES;SVE predicate initialize;Not setting the condition flags;size 22 2,S 16 1,pattern 5 5,Pd 0 4,;
00100101--011001111000-----0----;PTRUE, PTRUES;SVE predicate initialize;Setting the condition flags;size 22 2,S 16 1,pattern 5 5,Pd 0 4,;
00100101--0110011100010----0----;PNEXT;SVE predicate next active;;size 22 2,Pg 5 4,Pdn 0 4,;
00100101000110001111000----0----;RDFFR, RDFFRS (predicated);SVE predicate read from FFR (predicated);Not setting the condition flags;op 23 1,S 22 1,Pg 5 4,Pd 0 4,;
00100101010110001111000----0----;RDFFR, RDFFRS (predicated);SVE predicate read from FFR (predicated);Setting the condition flags;op 23 1,S 22 1,Pg 5 4,Pd 0 4,;
001001011-0110001111000----0----;UNALLOCATED;SVE predicate read from FFR (predicated);;op 23 1,S 22 1,Pg 5 4,Pd 0 4,;
0010010100011001111100000000----;RDFFR (unpredicated);SVE predicate read from FFR (unpredicated);;op 23 1,S 22 1,Pd 0 4,;
0010010101011001111100000000----;UNALLOCATED;SVE predicate read from FFR (unpredicated);;op 23 1,S 22 1,Pd 0 4,;
001001011-011001111100000000----;UNALLOCATED;SVE predicate read from FFR (unpredicated);;op 23 1,S 22 1,Pd 0 4,;
001001010001000011----0----0----;UNALLOCATED;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
001001010101000011----0----00000;PTEST;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
001001010101000011----0----00001;UNALLOCATED;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
001001010101000011----0----0001-;UNALLOCATED;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
001001010101000011----0----001--;UNALLOCATED;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
001001010101000011----0----01---;UNALLOCATED;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
001001011-01000011----0----0----;UNALLOCATED;SVE predicate test;;op 23 1,S 22 1,Pg 10 4,Pn 5 4,opc2 0 4,;
0010010100011000111001000000----;PFALSE;SVE predicate zero;;op 23 1,S 22 1,Pd 0 4,;
0010010101011000111001000000----;UNALLOCATED;SVE predicate zero;;op 23 1,S 22 1,Pd 0 4,;
001001011-011000111001000000----;UNALLOCATED;SVE predicate zero;;op 23 1,S 22 1,Pd 0 4,;
00100101--0-----000--------0----;CMP<cc> (immediate);SVE integer compare with signed immediate;Greater than or equal;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--0-----000--------1----;CMP<cc> (immediate);SVE integer compare with signed immediate;Greater than;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--0-----001--------0----;CMP<cc> (immediate);SVE integer compare with signed immediate;Less than;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--0-----001--------1----;CMP<cc> (immediate);SVE integer compare with signed immediate;Less than or equal;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--0-----100--------0----;CMP<cc> (immediate);SVE integer compare with signed immediate;Equal;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--0-----100--------1----;CMP<cc> (immediate);SVE integer compare with signed immediate;Not equal;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--0-----101-------------;UNALLOCATED;SVE integer compare with signed immediate;;size 22 2,imm5 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
00100101--10000010----0---------;CNTP;SVE predicate count;;size 22 2,opc 16 3,Pg 10 4,Pn 5 4,Rd 0 5,;
00100101--10000110----0---------;UNALLOCATED;SVE predicate count;;size 22 2,opc 16 3,Pg 10 4,Pn 5 4,Rd 0 5,;
00100101--10001-10----0---------;UNALLOCATED;SVE predicate count;;size 22 2,opc 16 3,Pg 10 4,Pn 5 4,Rd 0 5,;
00100101--1001--10----0---------;UNALLOCATED;SVE predicate count;;size 22 2,opc 16 3,Pg 10 4,Pn 5 4,Rd 0 5,;
00100101--10110-1000101---------;UNALLOCATED;SVE inc/dec register by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Rdn 0 5,;
00100101--10110-100011----------;UNALLOCATED;SVE inc/dec register by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Rdn 0 5,;
00100101--1011001000100---------;INCP (scalar);SVE inc/dec register by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Rdn 0 5,;
00100101--1011011000100---------;DECP (scalar);SVE inc/dec register by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Rdn 0 5,;
00100101--10111-10001-----------;UNALLOCATED;SVE inc/dec register by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Rdn 0 5,;
00100101--10110-1000001---------;UNALLOCATED;SVE inc/dec vector by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Zdn 0 5,;
00100101--10110-100001----------;UNALLOCATED;SVE inc/dec vector by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Zdn 0 5,;
00100101--1011001000000---------;INCP (vector);SVE inc/dec vector by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Zdn 0 5,;
00100101--1011011000000---------;DECP (vector);SVE inc/dec vector by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Zdn 0 5,;
00100101--10111-10000-----------;UNALLOCATED;SVE inc/dec vector by predicate count;;size 22 2,op 17 1,D 16 1,opc2 9 2,Pm 5 4,Zdn 0 5,;
00100101--1010--10001-1---------;UNALLOCATED;SVE saturating inc/dec register by predicate count;;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010001000100---------;SQINCP (scalar);SVE saturating inc/dec register by predicate count;32-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010001000110---------;SQINCP (scalar);SVE saturating inc/dec register by predicate count;64-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010011000100---------;UQINCP (scalar);SVE saturating inc/dec register by predicate count;32-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010011000110---------;UQINCP (scalar);SVE saturating inc/dec register by predicate count;64-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010101000100---------;SQDECP (scalar);SVE saturating inc/dec register by predicate count;32-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010101000110---------;SQDECP (scalar);SVE saturating inc/dec register by predicate count;64-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010111000100---------;UQDECP (scalar);SVE saturating inc/dec register by predicate count;32-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010111000110---------;UQDECP (scalar);SVE saturating inc/dec register by predicate count;64-bit;size 22 2,D 17 1,U 16 1,sf 10 1,op 9 1,Pm 5 4,Rdn 0 5,;
00100101--1010--1000001---------;UNALLOCATED;SVE saturating inc/dec vector by predicate count;;size 22 2,D 17 1,U 16 1,opc 9 2,Pm 5 4,Zdn 0 5,;
00100101--1010--100001----------;UNALLOCATED;SVE saturating inc/dec vector by predicate count;;size 22 2,D 17 1,U 16 1,opc 9 2,Pm 5 4,Zdn 0 5,;
00100101--1010001000000---------;SQINCP (vector);SVE saturating inc/dec vector by predicate count;;size 22 2,D 17 1,U 16 1,opc 9 2,Pm 5 4,Zdn 0 5,;
00100101--1010011000000---------;UQINCP (vector);SVE saturating inc/dec vector by predicate count;;size 22 2,D 17 1,U 16 1,opc 9 2,Pm 5 4,Zdn 0 5,;
00100101--1010101000000---------;SQDECP (vector);SVE saturating inc/dec vector by predicate count;;size 22 2,D 17 1,U 16 1,opc 9 2,Pm 5 4,Zdn 0 5,;
00100101--1010111000000---------;UQDECP (vector);SVE saturating inc/dec vector by predicate count;;size 22 2,D 17 1,U 16 1,opc 9 2,Pm 5 4,Zdn 0 5,;
00100101001011001001000000000000;SETFFR;SVE FFR initialise;;opc 22 2,;
00100101011011001001000000000000;UNALLOCATED;SVE FFR initialise;;opc 22 2,;
001001011-1011001001000000000000;UNALLOCATED;SVE FFR initialise;;opc 22 2,;
00100101001010001001000----00000;WRFFR;SVE FFR write from predicate;;opc 22 2,Pn 5 4,;
00100101011010001001000----00000;UNALLOCATED;SVE FFR write from predicate;;opc 22 2,Pn 5 4,;
001001011-1010001001000----00000;UNALLOCATED;SVE FFR write from predicate;;opc 22 2,Pn 5 4,;
001001010-1-----001000------0000;UNALLOCATED;SVE conditionally terminate scalars;;op 23 1,sz 22 1,Rm 16 5,Rn 5 5,ne 4 1,;
001001011-1-----001000-----00000;CTERMEQ, CTERMNE;SVE conditionally terminate scalars;Equal;op 23 1,sz 22 1,Rm 16 5,Rn 5 5,ne 4 1,;
001001011-1-----001000-----10000;CTERMEQ, CTERMNE;SVE conditionally terminate scalars;Not equal;op 23 1,sz 22 1,Rm 16 5,Rn 5 5,ne 4 1,;
00100101--1-----000--0----------;UNALLOCATED;SVE integer compare scalar count and limit;;size 22 2,Rm 16 5,sf 12 1,U 11 1,lt 10 1,Rn 5 5,eq 4 1,Pd 0 4,;
00100101--1-----000-01-----0----;WHILELT;SVE integer compare scalar count and limit;;size 22 2,Rm 16 5,sf 12 1,U 11 1,lt 10 1,Rn 5 5,eq 4 1,Pd 0 4,;
00100101--1-----000-01-----1----;WHILELE;SVE integer compare scalar count and limit;;size 22 2,Rm 16 5,sf 12 1,U 11 1,lt 10 1,Rn 5 5,eq 4 1,Pd 0 4,;
00100101--1-----000-11-----0----;WHILELO;SVE integer compare scalar count and limit;;size 22 2,Rm 16 5,sf 12 1,U 11 1,lt 10 1,Rn 5 5,eq 4 1,Pd 0 4,;
00100101--1-----000-11-----1----;WHILELS;SVE integer compare scalar count and limit;;size 22 2,Rm 16 5,sf 12 1,U 11 1,lt 10 1,Rn 5 5,eq 4 1,Pd 0 4,;
00100101--111001110-------------;FDUP;SVE broadcast floating-point immediate (unpredicated);;size 22 2,opc 17 2,o2 13 1,imm8 5 8,Zd 0 5,;
00100101--111001111-------------;UNALLOCATED;SVE broadcast floating-point immediate (unpredicated);;size 22 2,opc 17 2,o2 13 1,imm8 5 8,Zd 0 5,;
00100101--11101111--------------;UNALLOCATED;SVE broadcast floating-point immediate (unpredicated);;size 22 2,opc 17 2,o2 13 1,imm8 5 8,Zd 0 5,;
00100101--1111-111--------------;UNALLOCATED;SVE broadcast floating-point immediate (unpredicated);;size 22 2,opc 17 2,o2 13 1,imm8 5 8,Zd 0 5,;
00100101--11100011--------------;DUP (immediate);SVE broadcast integer immediate (unpredicated);;size 22 2,opc 17 2,sh 13 1,imm8 5 8,Zd 0 5,;
00100101--11101011--------------;UNALLOCATED;SVE broadcast integer immediate (unpredicated);;size 22 2,opc 17 2,sh 13 1,imm8 5 8,Zd 0 5,;
00100101--1111-011--------------;UNALLOCATED;SVE broadcast integer immediate (unpredicated);;size 22 2,opc 17 2,sh 13 1,imm8 5 8,Zd 0 5,;
00100101--10000011--------------;ADD (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10000111--------------;SUB (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10001011--------------;UNALLOCATED;SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10001111--------------;SUBR (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10010011--------------;SQADD (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10010111--------------;UQADD (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10011011--------------;SQSUB (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--10011111--------------;UQSUB (immediate);SVE integer add/subtract immediate (unpredicated);;size 22 2,opc 16 3,sh 13 1,imm8 5 8,Zdn 0 5,;
00100101--1010--111-------------;UNALLOCATED;SVE integer min/max immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--101000110-------------;SMAX (immediate);SVE integer min/max immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--101001110-------------;UMAX (immediate);SVE integer min/max immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--101010110-------------;SMIN (immediate);SVE integer min/max immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--101011110-------------;UMIN (immediate);SVE integer min/max immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--1011--11--------------;UNALLOCATED;SVE integer min/max immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--110000110-------------;MUL (immediate);SVE integer multiply immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--110000111-------------;UNALLOCATED;SVE integer multiply immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--11000111--------------;UNALLOCATED;SVE integer multiply immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--11001-11--------------;UNALLOCATED;SVE integer multiply immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
00100101--1101--11--------------;UNALLOCATED;SVE integer multiply immediate (unpredicated);;size 22 2,opc 16 3,o2 13 1,imm8 5 8,Zdn 0 5,;
01000100--0-----000000----------;SDOT (vectors);SVE integer dot product (unpredicated);;size 22 2,Zm 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100--0-----000001----------;UDOT (vectors);SVE integer dot product (unpredicated);;size 22 2,Zm 16 5,U 10 1,Zn 5 5,Zda 0 5,;
010001000-0-----011110----------;UNALLOCATED;SVE mixed sign dot product;;size 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
01000100100-----011110----------;USDOT (vectors);SVE mixed sign dot product;;size 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
01000100110-----011110----------;UNALLOCATED;SVE mixed sign dot product;;size 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
010001000-1-----00000-----------;UNALLOCATED;SVE integer dot product (indexed);;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100101-----000000----------;SDOT (indexed);SVE integer dot product (indexed);32-bit;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100101-----000001----------;UDOT (indexed);SVE integer dot product (indexed);32-bit;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100111-----000000----------;SDOT (indexed);SVE integer dot product (indexed);64-bit;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100111-----000001----------;UDOT (indexed);SVE integer dot product (indexed);64-bit;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
010001000-1-----00011-----------;UNALLOCATED;SVE mixed sign dot product (indexed);;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100101-----000110----------;USDOT (indexed);SVE mixed sign dot product (indexed);;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100101-----000111----------;SUDOT;SVE mixed sign dot product (indexed);;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000100111-----00011-----------;UNALLOCATED;SVE mixed sign dot product (indexed);;size 22 2,opc 16 5,U 10 1,Zn 5 5,Zda 0 5,;
01000101000-----100110----------;SMMLA;SVE integer matrix multiply accumulate;;uns 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
01000101010-----100110----------;UNALLOCATED;SVE integer matrix multiply accumulate;;uns 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
01000101100-----100110----------;USMMLA;SVE integer matrix multiply accumulate;;uns 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
01000101110-----100110----------;UMMLA;SVE integer matrix multiply accumulate;;uns 22 2,Zm 16 5,Zn 5 5,Zd 0 5,;
01100100--00000-100-------------;FCADD;SVE floating-point complex add (predicated);;size 22 2,rot 16 1,Pg 10 3,Zm 5 5,Zdn 0 5,;
011001000-0010--101-------------;UNALLOCATED;SVE floating-point convert precision odd elements;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
011001001000100-101-------------;UNALLOCATED;SVE floating-point convert precision odd elements;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010010001010101-------------;BFCVTNT;SVE floating-point convert precision odd elements;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010010001011101-------------;UNALLOCATED;SVE floating-point convert precision odd elements;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
01100100110010--101-------------;UNALLOCATED;SVE floating-point convert precision odd elements;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
01100100--0-----0---------------;FCMLA (vectors);SVE floating-point complex multiply-add (predicated);;size 22 2,Zm 16 5,rot 13 2,Pg 10 3,Zn 5 5,Zda 0 5,;
011001000-1-----000000----------;FMLA (indexed);SVE floating-point multiply-add (indexed);Half-precision;size 22 2,opc 16 5,op 10 1,Zn 5 5,Zda 0 5,;
011001000-1-----000001----------;FMLS (indexed);SVE floating-point multiply-add (indexed);Half-precision;size 22 2,opc 16 5,op 10 1,Zn 5 5,Zda 0 5,;
01100100101-----000000----------;FMLA (indexed);SVE floating-point multiply-add (indexed);Single-precision;size 22 2,opc 16 5,op 10 1,Zn 5 5,Zda 0 5,;
01100100101-----000001----------;FMLS (indexed);SVE floating-point multiply-add (indexed);Single-precision;size 22 2,opc 16 5,op 10 1,Zn 5 5,Zda 0 5,;
01100100111-----000000----------;FMLA (indexed);SVE floating-point multiply-add (indexed);Double-precision;size 22 2,opc 16 5,op 10 1,Zn 5 5,Zda 0 5,;
01100100111-----000001----------;FMLS (indexed);SVE floating-point multiply-add (indexed);Double-precision;size 22 2,opc 16 5,op 10 1,Zn 5 5,Zda 0 5,;
011001000-1-----0001------------;UNALLOCATED;SVE floating-point complex multiply-add (indexed);;size 22 2,opc 16 5,rot 10 2,Zn 5 5,Zda 0 5,;
01100100101-----0001------------;FCMLA (indexed);SVE floating-point complex multiply-add (indexed);Half-precision;size 22 2,opc 16 5,rot 10 2,Zn 5 5,Zda 0 5,;
01100100111-----0001------------;FCMLA (indexed);SVE floating-point complex multiply-add (indexed);Single-precision;size 22 2,opc 16 5,rot 10 2,Zn 5 5,Zda 0 5,;
011001000-1-----001000----------;FMUL (indexed);SVE floating-point multiply (indexed);Half-precision;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
01100100101-----001000----------;FMUL (indexed);SVE floating-point multiply (indexed);Single-precision;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
01100100111-----001000----------;FMUL (indexed);SVE floating-point multiply (indexed);Double-precision;size 22 2,opc 16 5,Zn 5 5,Zd 0 5,;
01100100001-----010000----------;UNALLOCATED;SVE BFloat16 floating-point dot product (indexed);;op 22 1,i2 19 2,Zm 16 3,Zn 5 5,Zda 0 5,;
01100100011-----010000----------;BFDOT (indexed);SVE BFloat16 floating-point dot product (indexed);;op 22 1,i2 19 2,Zm 16 3,Zn 5 5,Zda 0 5,;
01100100101-----01-0------------;UNALLOCATED;SVE floating-point multiply-add long (indexed);;o2 22 1,i3h 19 2,Zm 16 3,op 13 1,i3l 11 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100111-----0100-0----------;BFMLALB (indexed);SVE floating-point multiply-add long (indexed);;o2 22 1,i3h 19 2,Zm 16 3,op 13 1,i3l 11 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100111-----0100-1----------;BFMLALT (indexed);SVE floating-point multiply-add long (indexed);;o2 22 1,i3h 19 2,Zm 16 3,op 13 1,i3l 11 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100111-----0110------------;UNALLOCATED;SVE floating-point multiply-add long (indexed);;o2 22 1,i3h 19 2,Zm 16 3,op 13 1,i3l 11 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100001-----100000----------;UNALLOCATED;SVE BFloat16 floating-point dot product;;op 22 1,Zm 16 5,Zn 5 5,Zda 0 5,;
01100100011-----100000----------;BFDOT (vectors);SVE BFloat16 floating-point dot product;;op 22 1,Zm 16 5,Zn 5 5,Zda 0 5,;
01100100101-----10-00-----------;UNALLOCATED;SVE floating-point multiply-add long;;o2 22 1,Zm 16 5,op 13 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100111-----100000----------;BFMLALB (vectors);SVE floating-point multiply-add long;;o2 22 1,Zm 16 5,op 13 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100111-----100001----------;BFMLALT (vectors);SVE floating-point multiply-add long;;o2 22 1,Zm 16 5,op 13 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100111-----10100-----------;UNALLOCATED;SVE floating-point multiply-add long;;o2 22 1,Zm 16 5,op 13 1,T 10 1,Zn 5 5,Zda 0 5,;
01100100001-----111001----------;UNALLOCATED;SVE floating point matrix multiply accumulate;;opc 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
01100100011-----111001----------;BFMMLA;SVE floating point matrix multiply accumulate;;opc 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
01100100101-----111001----------;FMMLA;SVE floating point matrix multiply accumulate;32-bit element;opc 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
01100100111-----111001----------;FMMLA;SVE floating point matrix multiply accumulate;64-bit element;opc 22 2,Zm 16 5,Zn 5 5,Zda 0 5,;
01100101--000000001-------------;FADDV;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--000001001-------------;UNALLOCATED;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--00001-001-------------;UNALLOCATED;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--000100001-------------;FMAXNMV;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--000101001-------------;FMINNMV;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--000110001-------------;FMAXV;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--000111001-------------;FMINV;SVE floating-point recursive reduction;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Vd 0 5,;
01100101--0010--001100----------;UNALLOCATED;SVE floating-point reciprocal estimate (unpredicated);;size 22 2,opc 16 3,Zn 5 5,Zd 0 5,;
01100101--00110-001100----------;UNALLOCATED;SVE floating-point reciprocal estimate (unpredicated);;size 22 2,opc 16 3,Zn 5 5,Zd 0 5,;
01100101--001110001100----------;FRECPE;SVE floating-point reciprocal estimate (unpredicated);;size 22 2,opc 16 3,Zn 5 5,Zd 0 5,;
01100101--001111001100----------;FRSQRTE;SVE floating-point reciprocal estimate (unpredicated);;size 22 2,opc 16 3,Zn 5 5,Zd 0 5,;
01100101--010000001--------0----;FCM<cc> (zero);SVE floating-point compare with zero;Greater than or equal;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--010000001--------1----;FCM<cc> (zero);SVE floating-point compare with zero;Greater than;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--010001001--------0----;FCM<cc> (zero);SVE floating-point compare with zero;Less than;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--010001001--------1----;FCM<cc> (zero);SVE floating-point compare with zero;Less than or equal;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--01001-001--------1----;UNALLOCATED;SVE floating-point compare with zero;;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--010010001--------0----;FCM<cc> (zero);SVE floating-point compare with zero;Equal;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--010011001--------0----;FCM<cc> (zero);SVE floating-point compare with zero;Not equal;size 22 2,eq 17 1,lt 16 1,Pg 10 3,Zn 5 5,ne 4 1,Pd 0 4,;
01100101--011000001-------------;FADDA;SVE floating-point serial reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zm 5 5,Vdn 0 5,;
01100101--011001001-------------;UNALLOCATED;SVE floating-point serial reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zm 5 5,Vdn 0 5,;
01100101--01101-001-------------;UNALLOCATED;SVE floating-point serial reduction (predicated);;size 22 2,opc 16 2,Pg 10 3,Zm 5 5,Vdn 0 5,;
01100101--0-----000000----------;FADD (vectors, unpredicated);SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----000001----------;FSUB (vectors, unpredicated);SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----000010----------;FMUL (vectors, unpredicated);SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----000011----------;FTSMUL;SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----00010-----------;UNALLOCATED;SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----000110----------;FRECPS;SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----000111----------;FRSQRTS;SVE floating-point arithmetic (unpredicated);;size 22 2,Zm 16 5,opc 10 3,Zn 5 5,Zd 0 5,;
01100101--000000100-------------;FADD (vectors, predicated);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000001100-------------;FSUB (vectors, predicated);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000010100-------------;FMUL (vectors, predicated);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000011100-------------;FSUBR (vectors);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000100100-------------;FMAXNM (vectors);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000101100-------------;FMINNM (vectors);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000110100-------------;FMAX (vectors);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--000111100-------------;FMIN (vectors);SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--001000100-------------;FABD;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--001001100-------------;FSCALE;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--001010100-------------;FMULX;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--001011100-------------;UNALLOCATED;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--001100100-------------;FDIVR;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--001101100-------------;FDIV;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--00111-100-------------;UNALLOCATED;SVE floating-point arithmetic (predicated);;size 22 2,opc 16 4,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--011000100---0000------;FADD (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011001100---0000------;FSUB (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011010100---0000------;FMUL (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011011100---0000------;FSUBR (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011100100---0000------;FMAXNM (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011101100---0000------;FMINNM (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011110100---0000------;FMAX (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--011111100---0000------;FMIN (immediate);SVE floating-point arithmetic with immediate (predicated);;size 22 2,opc 16 3,Pg 10 3,i1 5 1,Zdn 0 5,;
01100101--010---100000----------;FTMAD;SVE floating-point trig multiply-add coefficient;;size 22 2,imm3 16 3,Zm 5 5,Zdn 0 5,;
011001010-0010--101-------------;UNALLOCATED;SVE floating-point convert precision;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110001000101-------------;FCVT;SVE floating-point convert precision;Single-precision to half-precision;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110001001101-------------;FCVT;SVE floating-point convert precision;Half-precision to single-precision;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110001010101-------------;BFCVT;SVE floating-point convert precision;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110001011101-------------;UNALLOCATED;SVE floating-point convert precision;;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111001000101-------------;FCVT;SVE floating-point convert precision;Double-precision to half-precision;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111001001101-------------;FCVT;SVE floating-point convert precision;Half-precision to double-precision;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111001010101-------------;FCVT;SVE floating-point convert precision;Double-precision to single-precision;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111001011101-------------;FCVT;SVE floating-point convert precision;Single-precision to double-precision;opc 22 2,opc2 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010100011---101-------------;UNALLOCATED;SVE floating-point convert to integer;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
011001010101100-101-------------;UNALLOCATED;SVE floating-point convert to integer;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101011010101-------------;FCVTZS;SVE floating-point convert to integer;Half-precision to 16-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101011011101-------------;FCVTZU;SVE floating-point convert to integer;Half-precision to 16-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101011100101-------------;FCVTZS;SVE floating-point convert to integer;Half-precision to 32-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101011101101-------------;FCVTZU;SVE floating-point convert to integer;Half-precision to 32-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101011110101-------------;FCVTZS;SVE floating-point convert to integer;Half-precision to 64-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101011111101-------------;FCVTZU;SVE floating-point convert to integer;Half-precision to 64-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101100110--101-------------;UNALLOCATED;SVE floating-point convert to integer;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110011100101-------------;FCVTZS;SVE floating-point convert to integer;Single-precision to 32-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110011101101-------------;FCVTZU;SVE floating-point convert to integer;Single-precision to 32-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
011001011001111-101-------------;UNALLOCATED;SVE floating-point convert to integer;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111011000101-------------;FCVTZS;SVE floating-point convert to integer;Double-precision to 32-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111011001101-------------;FCVTZU;SVE floating-point convert to integer;Double-precision to 32-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
011001011101101-101-------------;UNALLOCATED;SVE floating-point convert to integer;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111011100101-------------;FCVTZS;SVE floating-point convert to integer;Single-precision to 64-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111011101101-------------;FCVTZU;SVE floating-point convert to integer;Single-precision to 64-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111011110101-------------;FCVTZS;SVE floating-point convert to integer;Double-precision to 64-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111011111101-------------;FCVTZU;SVE floating-point convert to integer;Double-precision to 64-bit;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000000101-------------;FRINT<r>;SVE floating-point round to integral value;Nearest with ties to even;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000001101-------------;FRINT<r>;SVE floating-point round to integral value;Toward plus infinity;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000010101-------------;FRINT<r>;SVE floating-point round to integral value;Toward minus infinity;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000011101-------------;FRINT<r>;SVE floating-point round to integral value;Toward zero;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000100101-------------;FRINT<r>;SVE floating-point round to integral value;Nearest with ties to away;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000101101-------------;UNALLOCATED;SVE floating-point round to integral value;;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000110101-------------;FRINT<r>;SVE floating-point round to integral value;Current mode signalling inexact;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--000111101-------------;FRINT<r>;SVE floating-point round to integral value;Current mode;size 22 2,opc 16 3,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--001100101-------------;FRECPX;SVE floating-point unary operations;;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--001101101-------------;FSQRT;SVE floating-point unary operations;;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--00111-101-------------;UNALLOCATED;SVE floating-point unary operations;;size 22 2,opc 16 2,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010100010---101-------------;UNALLOCATED;SVE integer convert to floating-point;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
011001010101000-101-------------;UNALLOCATED;SVE integer convert to floating-point;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101010010101-------------;SCVTF;SVE integer convert to floating-point;16-bit to half-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101010011101-------------;UCVTF;SVE integer convert to floating-point;16-bit to half-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101010100101-------------;SCVTF;SVE integer convert to floating-point;32-bit to half-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101010101101-------------;UCVTF;SVE integer convert to floating-point;32-bit to half-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101010110101-------------;SCVTF;SVE integer convert to floating-point;64-bit to half-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010101010111101-------------;UCVTF;SVE integer convert to floating-point;64-bit to half-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101100100--101-------------;UNALLOCATED;SVE integer convert to floating-point;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110010100101-------------;SCVTF;SVE integer convert to floating-point;32-bit to single-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010110010101101-------------;UCVTF;SVE integer convert to floating-point;32-bit to single-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
011001011001011-101-------------;UNALLOCATED;SVE integer convert to floating-point;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111010000101-------------;SCVTF;SVE integer convert to floating-point;32-bit to double-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111010001101-------------;UCVTF;SVE integer convert to floating-point;32-bit to double-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
011001011101001-101-------------;UNALLOCATED;SVE integer convert to floating-point;;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111010100101-------------;SCVTF;SVE integer convert to floating-point;64-bit to single-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111010101101-------------;UCVTF;SVE integer convert to floating-point;64-bit to single-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111010110101-------------;SCVTF;SVE integer convert to floating-point;64-bit to double-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
0110010111010111101-------------;UCVTF;SVE integer convert to floating-point;64-bit to double-precision;opc 22 2,opc2 17 2,U 16 1,Pg 10 3,Zn 5 5,Zd 0 5,;
01100101--0-----010--------0----;FCM<cc> (vectors);SVE floating-point compare vectors;Greater than or equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----010--------1----;FCM<cc> (vectors);SVE floating-point compare vectors;Greater than;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----011--------0----;FCM<cc> (vectors);SVE floating-point compare vectors;Equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----011--------1----;FCM<cc> (vectors);SVE floating-point compare vectors;Not equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----110--------0----;FCM<cc> (vectors);SVE floating-point compare vectors;Unordered;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----110--------1----;FAC<cc>;SVE floating-point compare vectors;Greater than or equal;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----111--------0----;UNALLOCATED;SVE floating-point compare vectors;;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--0-----111--------1----;FAC<cc>;SVE floating-point compare vectors;Greater than;size 22 2,Zm 16 5,op 15 1,o2 13 1,Pg 10 3,Zn 5 5,o3 4 1,Pd 0 4,;
01100101--1-----000-------------;FMLA (vectors);SVE floating-point multiply-accumulate writing addend;;size 22 2,Zm 16 5,opc 13 2,Pg 10 3,Zn 5 5,Zda 0 5,;
01100101--1-----001-------------;FMLS (vectors);SVE floating-point multiply-accumulate writing addend;;size 22 2,Zm 16 5,opc 13 2,Pg 10 3,Zn 5 5,Zda 0 5,;
01100101--1-----010-------------;FNMLA;SVE floating-point multiply-accumulate writing addend;;size 22 2,Zm 16 5,opc 13 2,Pg 10 3,Zn 5 5,Zda 0 5,;
01100101--1-----011-------------;FNMLS;SVE floating-point multiply-accumulate writing addend;;size 22 2,Zm 16 5,opc 13 2,Pg 10 3,Zn 5 5,Zda 0 5,;
01100101--1-----100-------------;FMAD;SVE floating-point multiply-accumulate writing multiplicand;;size 22 2,Za 16 5,opc 13 2,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--1-----101-------------;FMSB;SVE floating-point multiply-accumulate writing multiplicand;;size 22 2,Za 16 5,opc 13 2,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--1-----110-------------;FNMAD;SVE floating-point multiply-accumulate writing multiplicand;;size 22 2,Za 16 5,opc 13 2,Pg 10 3,Zm 5 5,Zdn 0 5,;
01100101--1-----111-------------;FNMSB;SVE floating-point multiply-accumulate writing multiplicand;;size 22 2,Za 16 5,opc 13 2,Pg 10 3,Zm 5 5,Zdn 0 5,;
100001000-0-----000-------------;LD1SB (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001000-0-----001-------------;LDFF1SB (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001000-0-----010-------------;LD1B (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001000-0-----011-------------;LDFF1B (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-0-----000-------------;LD1SH (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-0-----001-------------;LDFF1SH (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-0-----010-------------;LD1H (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-0-----011-------------;LDFF1H (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001010-0-----00--------------;UNALLOCATED;SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001010-0-----010-------------;LD1W (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001010-0-----011-------------;LDFF1W (scalar plus vector);SVE 32-bit gather load (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
10000100001-----100-------------;LD1SB (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100001-----101-------------;LDFF1SB (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100001-----110-------------;LD1B (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100001-----111-------------;LDFF1B (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100101-----100-------------;LD1SH (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100101-----101-------------;LDFF1SH (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100101-----110-------------;LD1H (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000100101-----111-------------;LDFF1H (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000101001-----10--------------;UNALLOCATED;SVE 32-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000101001-----110-------------;LD1W (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000101001-----111-------------;LDFF1W (vector plus immediate);SVE 32-bit gather load (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
10000101101-----1---------------;UNALLOCATED;SVE 32-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
100001001-1-----000-------------;LD1SH (scalar plus vector);SVE 32-bit gather load halfwords (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-1-----001-------------;LDFF1SH (scalar plus vector);SVE 32-bit gather load halfwords (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-1-----010-------------;LD1H (scalar plus vector);SVE 32-bit gather load halfwords (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001001-1-----011-------------;LDFF1H (scalar plus vector);SVE 32-bit gather load halfwords (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001010-1-----00--------------;UNALLOCATED;SVE 32-bit gather load words (scalar plus 32-bit scaled offsets);;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001010-1-----010-------------;LD1W (scalar plus vector);SVE 32-bit gather load words (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001010-1-----011-------------;LDFF1W (scalar plus vector);SVE 32-bit gather load words (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
100001000-1-----000--------0----;PRFB (scalar plus vector);SVE 32-bit gather prefetch (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
100001000-1-----001--------0----;PRFH (scalar plus vector);SVE 32-bit gather prefetch (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
100001000-1-----010--------0----;PRFW (scalar plus vector);SVE 32-bit gather prefetch (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
100001000-1-----011--------0----;PRFD (scalar plus vector);SVE 32-bit gather prefetch (scalar plus 32-bit scaled offsets);32-bit scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
10000100000-----111--------0----;PRFB (vector plus immediate);SVE 32-bit gather prefetch (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
10000100100-----111--------0----;PRFH (vector plus immediate);SVE 32-bit gather prefetch (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
10000101000-----111--------0----;PRFW (vector plus immediate);SVE 32-bit gather prefetch (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
10000101100-----111--------0----;PRFD (vector plus immediate);SVE 32-bit gather prefetch (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
1000010111------000--------0----;PRFB (scalar plus immediate);SVE contiguous prefetch (scalar plus immediate);;imm6 16 6,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
1000010111------001--------0----;PRFH (scalar plus immediate);SVE contiguous prefetch (scalar plus immediate);;imm6 16 6,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
1000010111------010--------0----;PRFW (scalar plus immediate);SVE contiguous prefetch (scalar plus immediate);;imm6 16 6,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
1000010111------011--------0----;PRFD (scalar plus immediate);SVE contiguous prefetch (scalar plus immediate);;imm6 16 6,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
10000100000-----110--------0----;PRFB (scalar plus scalar);SVE contiguous prefetch (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,prfop 0 4,;
10000100100-----110--------0----;PRFH (scalar plus scalar);SVE contiguous prefetch (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,prfop 0 4,;
10000101000-----110--------0----;PRFW (scalar plus scalar);SVE contiguous prefetch (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,prfop 0 4,;
10000101100-----110--------0----;PRFD (scalar plus scalar);SVE contiguous prefetch (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,prfop 0 4,;
1000010001------100-------------;LD1RB;SVE load and broadcast element;8-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010001------101-------------;LD1RB;SVE load and broadcast element;16-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010001------110-------------;LD1RB;SVE load and broadcast element;32-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010001------111-------------;LD1RB;SVE load and broadcast element;64-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010011------100-------------;LD1RSW;SVE load and broadcast element;;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010011------101-------------;LD1RH;SVE load and broadcast element;16-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010011------110-------------;LD1RH;SVE load and broadcast element;32-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010011------111-------------;LD1RH;SVE load and broadcast element;64-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010101------100-------------;LD1RSH;SVE load and broadcast element;64-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010101------101-------------;LD1RSH;SVE load and broadcast element;32-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010101------110-------------;LD1RW;SVE load and broadcast element;32-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010101------111-------------;LD1RW;SVE load and broadcast element;64-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010111------100-------------;LD1RSB;SVE load and broadcast element;64-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010111------101-------------;LD1RSB;SVE load and broadcast element;32-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010111------110-------------;LD1RSB;SVE load and broadcast element;16-bit element;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010111------111-------------;LD1RD;SVE load and broadcast element;;dtypeh 23 2,imm6 16 6,dtypel 13 2,Pg 10 3,Rn 5 5,Zt 0 5,;
1000010110------000--------0----;LDR (predicate);SVE load predicate register;;imm9h 16 6,imm9l 10 3,Rn 5 5,Pt 0 4,;
1000010110------010-------------;LDR (vector);SVE load vector register;;imm9h 16 6,imm9l 10 3,Rn 5 5,Zt 0 5,;
10100100000-----011-------------;LDFF1B (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);8-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100001-----011-------------;LDFF1B (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);16-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100010-----011-------------;LDFF1B (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100011-----011-------------;LDFF1B (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100100-----011-------------;LDFF1SW (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100101-----011-------------;LDFF1H (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);16-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100110-----011-------------;LDFF1H (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100111-----011-------------;LDFF1H (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101000-----011-------------;LDFF1SH (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101001-----011-------------;LDFF1SH (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101010-----011-------------;LDFF1W (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101011-----011-------------;LDFF1W (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101100-----011-------------;LDFF1SB (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101101-----011-------------;LDFF1SB (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101110-----011-------------;LDFF1SB (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);16-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101111-----011-------------;LDFF1D (scalar plus scalar);SVE contiguous first-fault load (scalar plus scalar);;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000000----101-------------;LD1B (scalar plus immediate);SVE contiguous load (scalar plus immediate);8-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000010----101-------------;LD1B (scalar plus immediate);SVE contiguous load (scalar plus immediate);16-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000100----101-------------;LD1B (scalar plus immediate);SVE contiguous load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000110----101-------------;LD1B (scalar plus immediate);SVE contiguous load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001000----101-------------;LD1SW (scalar plus immediate);SVE contiguous load (scalar plus immediate);;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001010----101-------------;LD1H (scalar plus immediate);SVE contiguous load (scalar plus immediate);16-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001100----101-------------;LD1H (scalar plus immediate);SVE contiguous load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001110----101-------------;LD1H (scalar plus immediate);SVE contiguous load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010000----101-------------;LD1SH (scalar plus immediate);SVE contiguous load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010010----101-------------;LD1SH (scalar plus immediate);SVE contiguous load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010100----101-------------;LD1W (scalar plus immediate);SVE contiguous load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010110----101-------------;LD1W (scalar plus immediate);SVE contiguous load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011000----101-------------;LD1SB (scalar plus immediate);SVE contiguous load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011010----101-------------;LD1SB (scalar plus immediate);SVE contiguous load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011100----101-------------;LD1SB (scalar plus immediate);SVE contiguous load (scalar plus immediate);16-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011110----101-------------;LD1D (scalar plus immediate);SVE contiguous load (scalar plus immediate);;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100000-----010-------------;LD1B (scalar plus scalar);SVE contiguous load (scalar plus scalar);8-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100001-----010-------------;LD1B (scalar plus scalar);SVE contiguous load (scalar plus scalar);16-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100010-----010-------------;LD1B (scalar plus scalar);SVE contiguous load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100011-----010-------------;LD1B (scalar plus scalar);SVE contiguous load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100100-----010-------------;LD1SW (scalar plus scalar);SVE contiguous load (scalar plus scalar);;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100101-----010-------------;LD1H (scalar plus scalar);SVE contiguous load (scalar plus scalar);16-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100110-----010-------------;LD1H (scalar plus scalar);SVE contiguous load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100111-----010-------------;LD1H (scalar plus scalar);SVE contiguous load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101000-----010-------------;LD1SH (scalar plus scalar);SVE contiguous load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101001-----010-------------;LD1SH (scalar plus scalar);SVE contiguous load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101010-----010-------------;LD1W (scalar plus scalar);SVE contiguous load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101011-----010-------------;LD1W (scalar plus scalar);SVE contiguous load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101100-----010-------------;LD1SB (scalar plus scalar);SVE contiguous load (scalar plus scalar);64-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101101-----010-------------;LD1SB (scalar plus scalar);SVE contiguous load (scalar plus scalar);32-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101110-----010-------------;LD1SB (scalar plus scalar);SVE contiguous load (scalar plus scalar);16-bit element;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101111-----010-------------;LD1D (scalar plus scalar);SVE contiguous load (scalar plus scalar);;dtype 21 4,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000001----101-------------;LDNF1B;SVE contiguous non-fault load (scalar plus immediate);8-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000011----101-------------;LDNF1B;SVE contiguous non-fault load (scalar plus immediate);16-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000101----101-------------;LDNF1B;SVE contiguous non-fault load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000111----101-------------;LDNF1B;SVE contiguous non-fault load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001001----101-------------;LDNF1SW;SVE contiguous non-fault load (scalar plus immediate);;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001011----101-------------;LDNF1H;SVE contiguous non-fault load (scalar plus immediate);16-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001101----101-------------;LDNF1H;SVE contiguous non-fault load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001111----101-------------;LDNF1H;SVE contiguous non-fault load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010001----101-------------;LDNF1SH;SVE contiguous non-fault load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010011----101-------------;LDNF1SH;SVE contiguous non-fault load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010101----101-------------;LDNF1W;SVE contiguous non-fault load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010111----101-------------;LDNF1W;SVE contiguous non-fault load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011001----101-------------;LDNF1SB;SVE contiguous non-fault load (scalar plus immediate);64-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011011----101-------------;LDNF1SB;SVE contiguous non-fault load (scalar plus immediate);32-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011101----101-------------;LDNF1SB;SVE contiguous non-fault load (scalar plus immediate);16-bit element;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011111----101-------------;LDNF1D;SVE contiguous non-fault load (scalar plus immediate);;dtype 21 4,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000000----111-------------;LDNT1B (scalar plus immediate);SVE contiguous non-temporal load (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001000----111-------------;LDNT1H (scalar plus immediate);SVE contiguous non-temporal load (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010000----111-------------;LDNT1W (scalar plus immediate);SVE contiguous non-temporal load (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011000----111-------------;LDNT1D (scalar plus immediate);SVE contiguous non-temporal load (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100000-----110-------------;LDNT1B (scalar plus scalar);SVE contiguous non-temporal load (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100100-----110-------------;LDNT1H (scalar plus scalar);SVE contiguous non-temporal load (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101000-----110-------------;LDNT1W (scalar plus scalar);SVE contiguous non-temporal load (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101100-----110-------------;LDNT1D (scalar plus scalar);SVE contiguous non-temporal load (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
1010010--1-0----001-------------;UNALLOCATED;SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000000----001-------------;LD1RQB (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000010----001-------------;LD1ROB (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001000----001-------------;LD1RQH (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001010----001-------------;LD1ROH (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010000----001-------------;LD1RQW (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010010----001-------------;LD1ROW (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011000----001-------------;LD1RQD (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011010----001-------------;LD1ROD (scalar plus immediate);SVE load and broadcast quadword (scalar plus immediate);;msz 23 2,ssz 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
1010010--1------000-------------;UNALLOCATED;SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100000-----000-------------;LD1RQB (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100001-----000-------------;LD1ROB (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100100-----000-------------;LD1RQH (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100101-----000-------------;LD1ROH (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101000-----000-------------;LD1RQW (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101001-----000-------------;LD1ROW (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101100-----000-------------;LD1RQD (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101101-----000-------------;LD1ROD (scalar plus scalar);SVE load and broadcast quadword (scalar plus scalar);;msz 23 2,ssz 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000010----111-------------;LD2B (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000100----111-------------;LD3B (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001000110----111-------------;LD4B (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001010----111-------------;LD2H (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001100----111-------------;LD3H (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001001110----111-------------;LD4H (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010010----111-------------;LD2W (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010100----111-------------;LD3W (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001010110----111-------------;LD4W (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011010----111-------------;LD2D (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011100----111-------------;LD3D (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
101001011110----111-------------;LD4D (scalar plus immediate);SVE load multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100001-----110-------------;LD2B (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100010-----110-------------;LD3B (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100011-----110-------------;LD4B (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100101-----110-------------;LD2H (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100110-----110-------------;LD3H (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100100111-----110-------------;LD4H (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101001-----110-------------;LD2W (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101010-----110-------------;LD3W (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101011-----110-------------;LD4W (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101101-----110-------------;LD2D (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101110-----110-------------;LD3D (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
10100101111-----110-------------;LD4D (scalar plus scalar);SVE load multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-1-----000-------------;LD1SH (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-1-----001-------------;LDFF1SH (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-1-----010-------------;LD1H (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-1-----011-------------;LDFF1H (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-1-----000-------------;LD1SW (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-1-----001-------------;LDFF1SW (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-1-----010-------------;LD1W (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-1-----011-------------;LDFF1W (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001011-1-----00--------------;UNALLOCATED;SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001011-1-----010-------------;LD1D (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001011-1-----011-------------;LDFF1D (scalar plus vector);SVE 64-bit gather load (scalar plus 32-bit unpacked scaled offsets);32-bit unpacked scaled offset;opc 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100111-----100-------------;LD1SH (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100111-----101-------------;LDFF1SH (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100111-----110-------------;LD1H (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100111-----111-------------;LDFF1H (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101011-----100-------------;LD1SW (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101011-----101-------------;LDFF1SW (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101011-----110-------------;LD1W (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101011-----111-------------;LDFF1W (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101111-----10--------------;UNALLOCATED;SVE 64-bit gather load (scalar plus 64-bit scaled offsets);;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101111-----110-------------;LD1D (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101111-----111-------------;LDFF1D (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit scaled offsets);64-bit scaled offset;opc 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100010-----100-------------;LD1SB (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100010-----101-------------;LDFF1SB (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100010-----110-------------;LD1B (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100010-----111-------------;LDFF1B (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100110-----100-------------;LD1SH (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100110-----101-------------;LDFF1SH (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100110-----110-------------;LD1H (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100110-----111-------------;LDFF1H (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101010-----100-------------;LD1SW (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101010-----101-------------;LDFF1SW (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101010-----110-------------;LD1W (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101010-----111-------------;LDFF1W (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101110-----10--------------;UNALLOCATED;SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101110-----110-------------;LD1D (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000101110-----111-------------;LDFF1D (scalar plus vector);SVE 64-bit gather load (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001000-0-----000-------------;LD1SB (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001000-0-----001-------------;LDFF1SB (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001000-0-----010-------------;LD1B (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001000-0-----011-------------;LDFF1B (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-0-----000-------------;LD1SH (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-0-----001-------------;LDFF1SH (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-0-----010-------------;LD1H (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001001-0-----011-------------;LDFF1H (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-0-----000-------------;LD1SW (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-0-----001-------------;LDFF1SW (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-0-----010-------------;LD1W (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001010-0-----011-------------;LDFF1W (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001011-0-----00--------------;UNALLOCATED;SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001011-0-----010-------------;LD1D (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
110001011-0-----011-------------;LDFF1D (scalar plus vector);SVE 64-bit gather load (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,xs 22 1,Zm 16 5,U 14 1,ff 13 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11000100001-----100-------------;LD1SB (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100001-----101-------------;LDFF1SB (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100001-----110-------------;LD1B (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100001-----111-------------;LDFF1B (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100101-----100-------------;LD1SH (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100101-----101-------------;LDFF1SH (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100101-----110-------------;LD1H (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100101-----111-------------;LDFF1H (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101001-----100-------------;LD1SW (vector plus immediate);SVE 64-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101001-----101-------------;LDFF1SW (vector plus immediate);SVE 64-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101001-----110-------------;LD1W (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101001-----111-------------;LDFF1W (vector plus immediate);SVE 64-bit gather load (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101101-----10--------------;UNALLOCATED;SVE 64-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101101-----110-------------;LD1D (vector plus immediate);SVE 64-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000101101-----111-------------;LDFF1D (vector plus immediate);SVE 64-bit gather load (vector plus immediate);;msz 23 2,imm5 16 5,U 14 1,ff 13 1,Pg 10 3,Zn 5 5,Zt 0 5,;
11000100011-----100--------0----;PRFB (scalar plus vector);SVE 64-bit gather prefetch (scalar plus 64-bit scaled offsets);64-bit scaled offset;Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
11000100011-----101--------0----;PRFH (scalar plus vector);SVE 64-bit gather prefetch (scalar plus 64-bit scaled offsets);64-bit scaled offset;Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
11000100011-----110--------0----;PRFW (scalar plus vector);SVE 64-bit gather prefetch (scalar plus 64-bit scaled offsets);64-bit scaled offset;Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
11000100011-----111--------0----;PRFD (scalar plus vector);SVE 64-bit gather prefetch (scalar plus 64-bit scaled offsets);64-bit scaled offset;Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
110001000-1-----000--------0----;PRFB (scalar plus vector);SVE 64-bit gather prefetch (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
110001000-1-----001--------0----;PRFH (scalar plus vector);SVE 64-bit gather prefetch (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
110001000-1-----010--------0----;PRFW (scalar plus vector);SVE 64-bit gather prefetch (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
110001000-1-----011--------0----;PRFD (scalar plus vector);SVE 64-bit gather prefetch (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;xs 22 1,Zm 16 5,msz 13 2,Pg 10 3,Rn 5 5,prfop 0 4,;
11000100000-----111--------0----;PRFB (vector plus immediate);SVE 64-bit gather prefetch (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
11000100100-----111--------0----;PRFH (vector plus immediate);SVE 64-bit gather prefetch (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
11000101000-----111--------0----;PRFW (vector plus immediate);SVE 64-bit gather prefetch (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
11000101100-----111--------0----;PRFD (vector plus immediate);SVE 64-bit gather prefetch (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,prfop 0 4,;
111001000-------010-------------;ST1B (scalar plus scalar);SVE contiguous store (scalar plus scalar);;opc 22 3,o2 21 1,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
111001001-------010-------------;ST1H (scalar plus scalar);SVE contiguous store (scalar plus scalar);;opc 22 3,o2 21 1,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
111001010-------010-------------;ST1W (scalar plus scalar);SVE contiguous store (scalar plus scalar);;opc 22 3,o2 21 1,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101110-----010-------------;UNALLOCATED;SVE contiguous store (scalar plus scalar);;opc 22 3,o2 21 1,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101111-----010-------------;ST1D (scalar plus scalar);SVE contiguous store (scalar plus scalar);;opc 22 3,o2 21 1,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
1110010110------000--------0----;STR (predicate);SVE store predicate register;;imm9h 16 6,imm9l 10 3,Rn 5 5,Pt 0 4,;
1110010110------010-------------;STR (vector);SVE store vector register;;imm9h 16 6,imm9l 10 3,Rn 5 5,Zt 0 5,;
11100100000-----011-------------;STNT1B (scalar plus scalar);SVE contiguous non-temporal store (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100100-----011-------------;STNT1H (scalar plus scalar);SVE contiguous non-temporal store (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101000-----011-------------;STNT1W (scalar plus scalar);SVE contiguous non-temporal store (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101100-----011-------------;STNT1D (scalar plus scalar);SVE contiguous non-temporal store (scalar plus scalar);;msz 23 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100001-----011-------------;ST2B (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100010-----011-------------;ST3B (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100011-----011-------------;ST4B (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100101-----011-------------;ST2H (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100110-----011-------------;ST3H (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100111-----011-------------;ST4H (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101001-----011-------------;ST2W (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101010-----011-------------;ST3W (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101011-----011-------------;ST4W (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101101-----011-------------;ST2D (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101110-----011-------------;ST3D (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101111-----011-------------;ST4D (scalar plus scalar);SVE store multiple structures (scalar plus scalar);;msz 23 2,opc 21 2,Rm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100011-----101-------------;ST1B (vector plus immediate);SVE 32-bit scatter store (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100100111-----101-------------;ST1H (vector plus immediate);SVE 32-bit scatter store (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100101011-----101-------------;ST1W (vector plus immediate);SVE 32-bit scatter store (vector plus immediate);32-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100101111-----101-------------;UNALLOCATED;SVE 32-bit scatter store (vector plus immediate);;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100100001-----101-------------;UNALLOCATED;SVE 64-bit scatter store (scalar plus 64-bit scaled offsets);;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100101-----101-------------;ST1H (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit scaled offsets);64-bit scaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101001-----101-------------;ST1W (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit scaled offsets);64-bit scaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101101-----101-------------;ST1D (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit scaled offsets);64-bit scaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100000-----101-------------;ST1B (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100100-----101-------------;ST1H (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101000-----101-------------;ST1W (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101100-----101-------------;ST1D (scalar plus vector);SVE 64-bit scatter store (scalar plus 64-bit unscaled offsets);64-bit unscaled offset;msz 23 2,Zm 16 5,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100010-----101-------------;ST1B (vector plus immediate);SVE 64-bit scatter store (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100100110-----101-------------;ST1H (vector plus immediate);SVE 64-bit scatter store (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100101010-----101-------------;ST1W (vector plus immediate);SVE 64-bit scatter store (vector plus immediate);64-bit element;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
11100101110-----101-------------;ST1D (vector plus immediate);SVE 64-bit scatter store (vector plus immediate);;msz 23 2,imm5 16 5,Pg 10 3,Zn 5 5,Zt 0 5,;
111001000001----111-------------;STNT1B (scalar plus immediate);SVE contiguous non-temporal store (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001001001----111-------------;STNT1H (scalar plus immediate);SVE contiguous non-temporal store (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001010001----111-------------;STNT1W (scalar plus immediate);SVE contiguous non-temporal store (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001011001----111-------------;STNT1D (scalar plus immediate);SVE contiguous non-temporal store (scalar plus immediate);;msz 23 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001000--0----111-------------;ST1B (scalar plus immediate);SVE contiguous store (scalar plus immediate);;msz 23 2,size 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001001--0----111-------------;ST1H (scalar plus immediate);SVE contiguous store (scalar plus immediate);;msz 23 2,size 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001010--0----111-------------;ST1W (scalar plus immediate);SVE contiguous store (scalar plus immediate);;msz 23 2,size 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001011--0----111-------------;ST1D (scalar plus immediate);SVE contiguous store (scalar plus immediate);;msz 23 2,size 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001000011----111-------------;ST2B (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001000101----111-------------;ST3B (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001000111----111-------------;ST4B (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001001011----111-------------;ST2H (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001001101----111-------------;ST3H (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001001111----111-------------;ST4H (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001010011----111-------------;ST2W (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001010101----111-------------;ST3W (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001010111----111-------------;ST4W (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001011011----111-------------;ST2D (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001011101----111-------------;ST3D (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
111001011111----111-------------;ST4D (scalar plus immediate);SVE store multiple structures (scalar plus immediate);;msz 23 2,opc 21 2,imm4 16 4,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100011-----1-0-------------;UNALLOCATED;SVE 32-bit scatter store (scalar plus 32-bit scaled offsets);;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100111-----1-0-------------;ST1H (scalar plus vector);SVE 32-bit scatter store (scalar plus 32-bit scaled offsets);32-bit scaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101011-----1-0-------------;ST1W (scalar plus vector);SVE 32-bit scatter store (scalar plus 32-bit scaled offsets);32-bit scaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101111-----1-0-------------;UNALLOCATED;SVE 32-bit scatter store (scalar plus 32-bit scaled offsets);;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100010-----1-0-------------;ST1B (scalar plus vector);SVE 32-bit scatter store (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100110-----1-0-------------;ST1H (scalar plus vector);SVE 32-bit scatter store (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101010-----1-0-------------;ST1W (scalar plus vector);SVE 32-bit scatter store (scalar plus 32-bit unscaled offsets);32-bit unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101110-----1-0-------------;UNALLOCATED;SVE 32-bit scatter store (scalar plus 32-bit unscaled offsets);;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100001-----1-0-------------;UNALLOCATED;SVE 64-bit scatter store (scalar plus unpacked 32-bit scaled offsets);;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100101-----1-0-------------;ST1H (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101001-----1-0-------------;ST1W (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101101-----1-0-------------;ST1D (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit scaled offsets);32-bit unpacked scaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100000-----1-0-------------;ST1B (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100100100-----1-0-------------;ST1H (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101000-----1-0-------------;ST1W (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;
11100101100-----1-0-------------;ST1D (scalar plus vector);SVE 64-bit scatter store (scalar plus unpacked 32-bit unscaled offsets);32-bit unpacked unscaled offset;msz 23 2,Zm 16 5,xs 14 1,Pg 10 3,Rn 5 5,Zt 0 5,;"""



class Encoding:
    MNM_OUT_MAX_SIZE = 64

    def __init__(self, src_line):
        self.src_line = src_line
        self.src_cols = self.src_line.split(";")
        self.bits_str = self.src_cols[0] # 32 chars representing inst bits from left side
        self.encoding_name = self.src_cols[1] # usually mnemonic name and some comment
        self.inst_class_name = self.src_cols[2]
        self.tag_name = self.src_cols[3]
        self.src_operands = self.src_cols[4].strip(", ").split(",")
        self.src_dne_cases = list(filter(bool, self.src_cols[5].strip(", ").split(",")))
        self._prep_operands()

    def __repr__(self):
        return f'Encoding("{self.src_line}")'

    def _prep_operands(self):
        self.operand_u32_masks = dict()
        self.operand_mask_is_rev = dict()
        self.operand_mask_shift = dict()
        self.operand_mask_bottom_top = dict()

        for operand in self.src_operands:
            op_name, op_bottom, op_top = operand.split(" ")
            op_bottom = int(op_bottom)
            op_top = int(op_top)

            is_rev = op_bottom > op_top
            self.operand_mask_is_rev[op_name] = is_rev
            if is_rev:
                t = op_top;
                op_top = op_bottom
                op_bottom = t
                self.operand_mask_shift[op_name] = 31 - op_top # to shift right after masking then bit reversal
            else:
                self.operand_mask_shift[op_name] = op_bottom # to shift right after masking

            self.operand_mask_bottom_top[op_name] = (op_bottom, op_top)

            self.operand_u32_masks[op_name] = 0
            i = op_bottom
            while True:
                self.operand_u32_masks[op_name] += 2**i
                i += 1
                if i > op_top:
                    break

    @property
    def u32_always(self):
        out = 0

        out += 2**0 if self.bits_str[-1] == '1' else 0 # last src byte = least significant
        out += 2**1 if self.bits_str[-2] == '1' else 0
        out += 2**2 if self.bits_str[-3] == '1' else 0
        out += 2**3 if self.bits_str[-4] == '1' else 0
        out += 2**4 if self.bits_str[-5] == '1' else 0
        out += 2**5 if self.bits_str[-6] == '1' else 0
        out += 2**6 if self.bits_str[-7] == '1' else 0
        out += 2**7 if self.bits_str[-8] == '1' else 0

        out += 2**8  if self.bits_str[ -9] == '1' else 0
        out += 2**9  if self.bits_str[-10] == '1' else 0
        out += 2**10 if self.bits_str[-11] == '1' else 0
        out += 2**11 if self.bits_str[-12] == '1' else 0
        out += 2**12 if self.bits_str[-13] == '1' else 0
        out += 2**13 if self.bits_str[-14] == '1' else 0
        out += 2**14 if self.bits_str[-15] == '1' else 0
        out += 2**15 if self.bits_str[-16] == '1' else 0

        out += 2**16 if self.bits_str[-17] == '1' else 0
        out += 2**17 if self.bits_str[-18] == '1' else 0
        out += 2**18 if self.bits_str[-19] == '1' else 0
        out += 2**19 if self.bits_str[-20] == '1' else 0
        out += 2**20 if self.bits_str[-21] == '1' else 0
        out += 2**21 if self.bits_str[-22] == '1' else 0
        out += 2**22 if self.bits_str[-23] == '1' else 0
        out += 2**23 if self.bits_str[-24] == '1' else 0

        out += 2**24 if self.bits_str[-25] == '1' else 0 # first src byte = most significant
        out += 2**25 if self.bits_str[-26] == '1' else 0
        out += 2**26 if self.bits_str[-27] == '1' else 0
        out += 2**27 if self.bits_str[-28] == '1' else 0
        out += 2**28 if self.bits_str[-29] == '1' else 0
        out += 2**29 if self.bits_str[-30] == '1' else 0
        out += 2**30 if self.bits_str[-31] == '1' else 0
        out += 2**31 if self.bits_str[-32] == '1' else 0

        return out

    @property
    def u32_never(self):
        out = 0

        out += 2**0 if self.bits_str[-1] == '0' else 0 # last src byte = least significant
        out += 2**1 if self.bits_str[-2] == '0' else 0
        out += 2**2 if self.bits_str[-3] == '0' else 0
        out += 2**3 if self.bits_str[-4] == '0' else 0
        out += 2**4 if self.bits_str[-5] == '0' else 0
        out += 2**5 if self.bits_str[-6] == '0' else 0
        out += 2**6 if self.bits_str[-7] == '0' else 0
        out += 2**7 if self.bits_str[-8] == '0' else 0

        out += 2**8  if self.bits_str[ -9] == '0' else 0
        out += 2**9  if self.bits_str[-10] == '0' else 0
        out += 2**10 if self.bits_str[-11] == '0' else 0
        out += 2**11 if self.bits_str[-12] == '0' else 0
        out += 2**12 if self.bits_str[-13] == '0' else 0
        out += 2**13 if self.bits_str[-14] == '0' else 0
        out += 2**14 if self.bits_str[-15] == '0' else 0
        out += 2**15 if self.bits_str[-16] == '0' else 0

        out += 2**16 if self.bits_str[-17] == '0' else 0
        out += 2**17 if self.bits_str[-18] == '0' else 0
        out += 2**18 if self.bits_str[-19] == '0' else 0
        out += 2**19 if self.bits_str[-20] == '0' else 0
        out += 2**20 if self.bits_str[-21] == '0' else 0
        out += 2**21 if self.bits_str[-22] == '0' else 0
        out += 2**22 if self.bits_str[-23] == '0' else 0
        out += 2**23 if self.bits_str[-24] == '0' else 0

        out += 2**24 if self.bits_str[-25] == '0' else 0 # first src byte = most significant
        out += 2**25 if self.bits_str[-26] == '0' else 0
        out += 2**26 if self.bits_str[-27] == '0' else 0
        out += 2**27 if self.bits_str[-28] == '0' else 0
        out += 2**28 if self.bits_str[-29] == '0' else 0
        out += 2**29 if self.bits_str[-30] == '0' else 0
        out += 2**30 if self.bits_str[-31] == '0' else 0
        out += 2**31 if self.bits_str[-32] == '0' else 0

        return out

    '''
    def cpp_u32_endian_flip(self, v):
        out = 0
        out += ((v & 0x000000ff)      ) * 0x01000000
        out += ((v & 0x0000ff00) >>  8) * 0x00010000
        out += ((v & 0x00ff0000) >> 16) * 0x00000100
        out += ((v & 0xff000000) >> 24) * 0x00000001
        return f"((u32){hex(out)})"
    '''

    def cpp_is_match(self, identifier="inst"):
        always = hex(self.u32_always)
        never = hex(self.u32_never)
        return f"""(({identifier} & {always}) == {always} && (~{identifier} & {never}) == {never})"""

encodings = [Encoding(l) for l in horsetrain_src_text.splitlines()]
cpp_gen_out = open(OUTPUT_FILENAME, "w")

cpp_gen_out.write('#include "util/disasm.hpp"\n')
cpp_gen_out.write('#include <nn/util.h>\n')
cpp_gen_out.write("namespace lotuskit::util::disasm {\n")
cpp_gen_out.write(f"    size_t MNM_OUT_MAX_SIZE = {Encoding.MNM_OUT_MAX_SIZE};\n")
cpp_gen_out.write("    void u32_to_str_mnemonic(u32 inst, char* output) {\n")
cpp_gen_out.write("        if (inst == 0) { output[0] = 0; return; } // dont draw anything for null space\n")

for enc in encodings:
    if enc.inst_class_name.startswith("SVE"):
        continue # unsupported? we have ARMv8.0-A?

    mnm_trash = enc.encoding_name.split(" ")
    if mnm_trash[0] == "UNALLOCATED" or mnm_trash[0] == "UDF":
        continue
    mnm_trash[0] = mnm_trash[0].lower()
    mnm_trash = " ".join(mnm_trash) # wip display info

    cpp_gen_out.write(f"""        if ({enc.cpp_is_match()}) {{ // {mnm_trash}\n""")

    for op_name, op_mask in enc.operand_u32_masks.items():
        shift = enc.operand_mask_shift[op_name]
        if enc.operand_mask_is_rev[op_name]:
            cpp_gen_out.write(f"""            u32 {op_name}_rev = (inst & {op_mask});\n""")
            cpp_gen_out.write(f"""            u32 {op_name} = 0;\n""")
            cpp_gen_out.write(f"""            asm("rbit %0,%1" : "=r"({op_name}) : "r"({op_name}_rev));\n""")
            cpp_gen_out.write(f"""            {op_name} >>= {shift};\n""")
        else:
            cpp_gen_out.write(f"""            u32 {op_name} = (inst & {op_mask}) >> {shift};\n""")

    pass_cond = "true"
    if len(enc.src_dne_cases) > 0:
        pass_cond = " && ".join(enc.src_dne_cases)

    cpp_gen_out.write(f"            if ({pass_cond}) {{\n")
    op_names = list(enc.operand_u32_masks.keys())
    if len(op_names):
        op_fmt = ", ".join(f"{op}=%x" for op in op_names)
        op_fmt_args = ", ".join(op_names)
        cpp_gen_out.write(f"""                nn::util::SNPrintf(output, {Encoding.MNM_OUT_MAX_SIZE}, "{mnm_trash}: {op_fmt}", {op_fmt_args});\n""")
    else:
        # nothing to format
        cpp_gen_out.write(f"""                strcpy(output, "{mnm_trash}");\n""")
    cpp_gen_out.write("                return;\n")
    cpp_gen_out.write("            }\n")
    cpp_gen_out.write("}\n")

cpp_gen_out.write('        strcpy(output, "???"); // cannot decode\n')
cpp_gen_out.write("    }\n")
cpp_gen_out.write("} // ns\n")
cpp_gen_out.close()

