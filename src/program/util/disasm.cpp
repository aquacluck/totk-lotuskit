#include "util/disasm.hpp"
namespace lotuskit::util::disasm {
    size_t MNM_OUT_MAX_SIZE = 64;
    void u32_to_str_mnemonic(u32 inst, char* output) {
        //if (inst == 0) { output[0] = 0; return; } // dont draw anything for null space
        if (((inst & 505460736) == 505460736 && (~inst & 553731072) == 553731072)) { // abs     Sd, Sn
            strcpy(output, "abs     Sd, Sn");
            return;
        }
        if (((inst & 237025280) == 237025280 && (~inst & 822166528) == 822166528)) { // abs     Vd, Vn
            strcpy(output, "abs     Vd, Vn");
            return;
        }
        if (((inst & 436207616) == 436207616 && (~inst & 1709181952) == 1709181952)) { // adc     Rd, Rn, Rm
            strcpy(output, "adc     Rd, Rn, Rm");
            return;
        }
        if (((inst & 973078528) == 973078528 && (~inst & 1172311040) == 1172311040)) { // adcs    Rd, Rn, Rm
            strcpy(output, "adcs    Rd, Rn, Rm");
            return;
        }
        if (((inst & 1310736384) == 1310736384 && (~inst & 822131712) == 822131712)) { // addhn2  Vd, Vn, Vm
            strcpy(output, "addhn2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 236994560) == 236994560 && (~inst & 1895873536) == 1895873536)) { // addhn   Vd, Vn, Vm
            strcpy(output, "addhn   Vd, Vn, Vm");
            return;
        }
        if (((inst & 506574848) == 506574848 && (~inst & 16794624) == 16794624)) { // addp    Sd, Vn
            strcpy(output, "addp    Sd, Vn");
            return;
        }
        if (((inst & 237026304) == 237026304 && (~inst & 285229056) == 285229056)) { // addp    Vd, Vn, Vm
            strcpy(output, "addp    Vd, Vn, Vm");
            return;
        }
        if (((inst & 184549376) == 184549376 && (~inst & 1948254208) == 1948254208)) { // add     Rd, Rn, Rm_SFT
            strcpy(output, "add     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 16777216) == 16777216 && (~inst & 1845493760) == 1845493760)) { // add     Rd_SP, Rn_SP, AIMM
            strcpy(output, "add     Rd_SP, Rn_SP, AIMM");
            return;
        }
        if (((inst & 186646528) == 186646528 && (~inst & 1954545664) == 1954545664)) { // add     Rd_SP, Rn_SP, Rm_EXT
            strcpy(output, "add     Rd_SP, Rn_SP, Rm_EXT");
            return;
        }
        if (((inst & 1579156480) == 1579156480 && (~inst & 553678848) == 553678848)) { // add     Sd, Sn, Sm
            strcpy(output, "add     Sd, Sn, Sm");
            return;
        }
        if (((inst & 721420288) == 721420288 && (~inst & 1411383296) == 1411383296)) { // adds    Rd, Rn, Rm_SFT
            strcpy(output, "adds    Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 553648128) == 553648128 && (~inst & 1308622848) == 1308622848)) { // adds    Rd, Rn_SP, AIMM
            strcpy(output, "adds    Rd, Rn_SP, AIMM");
            return;
        }
        if (((inst & 723517440) == 723517440 && (~inst & 1417674752) == 1417674752)) { // adds    Rd, Rn_SP, Rm_EXT
            strcpy(output, "adds    Rd, Rn_SP, Rm_EXT");
            return;
        }
        if (((inst & 237011968) == 237011968 && (~inst & 822114304) == 822114304)) { // add     Vd, Vn, Vm
            strcpy(output, "add     Vd, Vn, Vm");
            return;
        }
        if (((inst & 238139392) == 238139392 && (~inst & 285230080) == 285230080)) { // addv    Fd, Vn
            strcpy(output, "addv    Fd, Vn");
            return;
        }
        if (((inst & 2147483648) == 2147483648 && (~inst & 251658240) == 251658240)) { // adrp    Rd, ADDR_ADRP
            strcpy(output, "adrp    Rd, ADDR_ADRP");
            return;
        }
        if (((inst & 0) == 0 && (~inst & 2399141888) == 2399141888)) { // adr     Rd, ADDR_PCREL21
            strcpy(output, "adr     Rd, ADDR_PCREL21");
            return;
        }
        if (((inst & 237524992) == 237524992 && (~inst & 285254656) == 285254656)) { // aesd    Vd, Vn
            strcpy(output, "aesd    Vd, Vn");
            return;
        }
        if (((inst & 237520896) == 237520896 && (~inst & 285324288) == 285324288)) { // aese    Vd, Vn
            strcpy(output, "aese    Vd, Vn");
            return;
        }
        if (((inst & 237533184) == 237533184 && (~inst & 285312000) == 285312000)) { // aesimc  Vd, Vn
            strcpy(output, "aesimc  Vd, Vn");
            return;
        }
        if (((inst & 237529088) == 237529088 && (~inst & 285316096) == 285316096)) { // aesmc   Vd, Vn
            strcpy(output, "aesmc   Vd, Vn");
            return;
        }
        if (((inst & 167772160) == 167772160 && (~inst & 1965031424) == 1965031424)) { // and     Rd, Rn, Rm_SFT
            strcpy(output, "and     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 33554432) == 33554432 && (~inst & 1837105152) == 1837105152)) { // and     Rd_SP, Rn, LIMM
            strcpy(output, "and     Rd_SP, Rn, LIMM");
            return;
        }
        if (((inst & 1644167168) == 1644167168 && (~inst & 226492416) == 226492416)) { // ands    Rd, Rn, LIMM
            strcpy(output, "ands    Rd, Rn, LIMM");
            return;
        }
        if (((inst & 1778384896) == 1778384896 && (~inst & 354418688) == 354418688)) { // ands    Rd, Rn, Rm_SFT
            strcpy(output, "ands    Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 236985344) == 236985344 && (~inst & 834723840) == 834723840)) { // and     Vd, Vn, Vm
            strcpy(output, "and     Vd, Vn, Vm");
            return;
        }
        if (((inst & 444606464) == 444606464 && (~inst & 85984256) == 85984256)) { // asrv    Rd, Rn, Rm
            strcpy(output, "asrv    Rd, Rn, Rm");
            return;
        }
        if (((inst & 67108864) == 67108864 && (~inst & 3892314112) == 3892314112)) { // b       ADDR_PCREL26
            strcpy(output, "b       ADDR_PCREL26");
            return;
        }
        if (((inst & 1140850688) == 1140850688 && (~inst & 2868903936) == 2868903936)) { // b.c     ADDR_PCREL19
            strcpy(output, "b.c     ADDR_PCREL19");
            return;
        }
        if (((inst & 587202560) == 587202560 && (~inst & 209715200) == 209715200)) { // bfm     Rd, Rn, IMMR, IMMS
            strcpy(output, "bfm     Rd, Rn, IMMR, IMMS");
            return;
        }
        if (((inst & 169869312) == 169869312 && (~inst & 1694498816) == 1694498816)) { // bic     Rd, Rn, Rm_SFT
            strcpy(output, "bic     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 1780482048) == 1780482048 && (~inst & 83886080) == 83886080)) { // bics    Rd, Rn, Rm_SFT
            strcpy(output, "bics    Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 788534272) == 788534272 && (~inst & 268468224) == 268468224)) { // bic     Vd, SIMD_IMM_SFT
            strcpy(output, "bic     Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 788567040) == 788567040 && (~inst & 268453888) == 268453888)) { // bic     Vd, SIMD_IMM_SFT
            strcpy(output, "bic     Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 241179648) == 241179648 && (~inst & 830529536) == 830529536)) { // bic     Vd, Vn, Vm
            strcpy(output, "bic     Vd, Vn, Vm");
            return;
        }
        if (((inst & 786439168) == 786439168 && (~inst & 285270016) == 285270016)) { // bif     Vd, Vn, Vm
            strcpy(output, "bif     Vd, Vn, Vm");
            return;
        }
        if (((inst & 782244864) == 782244864 && (~inst & 289464320) == 289464320)) { // bit     Vd, Vn, Vm
            strcpy(output, "bit     Vd, Vn, Vm");
            return;
        }
        if (((inst & 2214592512) == 2214592512 && (~inst & 1744830464) == 1744830464)) { // bl      ADDR_PCREL26
            strcpy(output, "bl      ADDR_PCREL26");
            return;
        }
        if (((inst & 1176502272) == 1176502272 && (~inst & 696254464) == 696254464)) { // blr     Rn
            strcpy(output, "blr     Rn");
            return;
        }
        if (((inst & 3290431488) == 3290431488 && (~inst & 721420291) == 721420291)) { // brk     EXCEPTION
            strcpy(output, "brk     EXCEPTION");
            return;
        }
        if (((inst & 1174405120) == 1174405120 && (~inst & 702545920) == 702545920)) { // br      Rn
            strcpy(output, "br      Rn");
            return;
        }
        if (((inst & 778050560) == 778050560 && (~inst & 293658624) == 293658624)) { // bsl     Vd, Vn, Vm
            strcpy(output, "bsl     Vd, Vn, Vm");
            return;
        }
        if (((inst & 620756992) == 620756992 && (~inst & 167772160) == 167772160)) { // cbnz    Rt, ADDR_PCREL19
            strcpy(output, "cbnz    Rt, ADDR_PCREL19");
            return;
        }
        if (((inst & 603979776) == 603979776 && (~inst & 184549376) == 184549376)) { // cbz     Rt, ADDR_PCREL19
            strcpy(output, "cbz     Rt, ADDR_PCREL19");
            return;
        }
        if (((inst & 436209664) == 436209664 && (~inst & 1168114688) == 1168114688)) { // ccmn    Rn, CCMP_IMM, NZCV, COND
            strcpy(output, "ccmn    Rn, CCMP_IMM, NZCV, COND");
            return;
        }
        if (((inst & 440401920) == 440401920 && (~inst & 1168116736) == 1168116736)) { // ccmn    Rn, Rm, NZCV, COND
            strcpy(output, "ccmn    Rn, Rm, NZCV, COND");
            return;
        }
        if (((inst & 1509951488) == 1509951488 && (~inst & 94372864) == 94372864)) { // ccmp    Rn, CCMP_IMM, NZCV, COND
            strcpy(output, "ccmp    Rn, CCMP_IMM, NZCV, COND");
            return;
        }
        if (((inst & 1514143744) == 1514143744 && (~inst & 94374912) == 94374912)) { // ccmp    Rn, Rm, NZCV, COND
            strcpy(output, "ccmp    Rn, Rm, NZCV, COND");
            return;
        }
        if (((inst & 1157632064) == 1157632064 && (~inst & 672661664) == 672661664)) { // clrex   UIMM4
            strcpy(output, "clrex   UIMM4");
            return;
        }
        if (((inst & 440407040) == 440407040 && (~inst & 85985280) == 85985280)) { // cls     Rd, Rn
            strcpy(output, "cls     Rd, Rn");
            return;
        }
        if (((inst & 236996608) == 236996608 && (~inst & 822719488) == 822719488)) { // cls     Vd, Vn
            strcpy(output, "cls     Vd, Vn");
            return;
        }
        if (((inst & 448794624) == 448794624 && (~inst & 85986304) == 85986304)) { // clz     Rd, Rn
            strcpy(output, "clz     Rd, Rn");
            return;
        }
        if (((inst & 773867520) == 773867520 && (~inst & 285848576) == 285848576)) { // clz     Vd, Vn
            strcpy(output, "clz     Vd, Vn");
            return;
        }
        if (((inst & 505452544) == 505452544 && (~inst & 553739264) == 553739264)) { // cmeq    Sd, Sn, IMM0
            strcpy(output, "cmeq    Sd, Sn, IMM0");
            return;
        }
        if (((inst & 1042320384) == 1042320384 && (~inst & 16805888) == 16805888)) { // cmeq    Sd, Sn, Sm
            strcpy(output, "cmeq    Sd, Sn, Sm");
            return;
        }
        if (((inst & 237017088) == 237017088 && (~inst & 822174720) == 822174720)) { // cmeq    Vd, Vn, IMM0
            strcpy(output, "cmeq    Vd, Vn, IMM0");
            return;
        }
        if (((inst & 773884928) == 773884928 && (~inst & 285241344) == 285241344)) { // cmeq    Vd, Vn, Vm
            strcpy(output, "cmeq    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042319360) == 1042319360 && (~inst & 16806912) == 16806912)) { // cmge    Sd, Sn, IMM0
            strcpy(output, "cmge    Sd, Sn, IMM0");
            return;
        }
        if (((inst & 1579170816) == 1579170816 && (~inst & 553664512) == 553664512)) { // cmge    Sd, Sn, Sm
            strcpy(output, "cmge    Sd, Sn, Sm");
            return;
        }
        if (((inst & 773883904) == 773883904 && (~inst & 285307904) == 285307904)) { // cmge    Vd, Vn, IMM0
            strcpy(output, "cmge    Vd, Vn, IMM0");
            return;
        }
        if (((inst & 236993536) == 236993536 && (~inst & 822132736) == 822132736)) { // cmge    Vd, Vn, Vm
            strcpy(output, "cmge    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1579190272) == 1579190272 && (~inst & 553677824) == 553677824)) { // cmgt    Sd, Sn, IMM0
            strcpy(output, "cmgt    Sd, Sn, IMM0");
            return;
        }
        if (((inst & 1579168768) == 1579168768 && (~inst & 553682944) == 553682944)) { // cmgt    Sd, Sn, Sm
            strcpy(output, "cmgt    Sd, Sn, Sm");
            return;
        }
        if (((inst & 237012992) == 237012992 && (~inst & 822178816) == 822178816)) { // cmgt    Vd, Vn, IMM0
            strcpy(output, "cmgt    Vd, Vn, IMM0");
            return;
        }
        if (((inst & 236991488) == 236991488 && (~inst & 822134784) == 822134784)) { // cmgt    Vd, Vn, Vm
            strcpy(output, "cmgt    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042297856) == 1042297856 && (~inst & 16812032) == 16812032)) { // cmhi    Sd, Sn, Sm
            strcpy(output, "cmhi    Sd, Sn, Sm");
            return;
        }
        if (((inst & 773862400) == 773862400 && (~inst & 285263872) == 285263872)) { // cmhi    Vd, Vn, Vm
            strcpy(output, "cmhi    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042299904) == 1042299904 && (~inst & 16777216) == 16777216)) { // cmhs    Sd, Sn, Sm
            strcpy(output, "cmhs    Sd, Sn, Sm");
            return;
        }
        if (((inst & 773864448) == 773864448 && (~inst & 285261824) == 285261824)) { // cmhs    Vd, Vn, Vm
            strcpy(output, "cmhs    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042323456) == 1042323456 && (~inst & 16868352) == 16868352)) { // cmle    Sd, Sn, IMM0
            strcpy(output, "cmle    Sd, Sn, IMM0");
            return;
        }
        if (((inst & 773888000) == 773888000 && (~inst & 285303808) == 285303808)) { // cmle    Vd, Vn, IMM0
            strcpy(output, "cmle    Vd, Vn, IMM0");
            return;
        }
        if (((inst & 505456640) == 505456640 && (~inst & 17912832) == 17912832)) { // cmlt    Sd, Sn, IMM0
            strcpy(output, "cmlt    Sd, Sn, IMM0");
            return;
        }
        if (((inst & 237021184) == 237021184 && (~inst & 286348288) == 286348288)) { // cmlt    Vd, Vn, IMM0
            strcpy(output, "cmlt    Vd, Vn, IMM0");
            return;
        }
        if (((inst & 1579191296) == 1579191296 && (~inst & 553676800) == 553676800)) { // cmtst   Sd, Sn, Sm
            strcpy(output, "cmtst   Sd, Sn, Sm");
            return;
        }
        if (((inst & 237014016) == 237014016 && (~inst & 822112256) == 822112256)) { // cmtst   Vd, Vn, Vm
            strcpy(output, "cmtst   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237000704) == 237000704 && (~inst & 822649856) == 822649856)) { // cnt     Vd, Vn
            strcpy(output, "cnt     Vd, Vn");
            return;
        }
        if (((inst & 444596224) == 444596224 && (~inst & 1163922432) == 1163922432)) { // csel    Rd, Rn, Rm, COND
            strcpy(output, "csel    Rd, Rn, Rm, COND");
            return;
        }
        if (((inst & 436208640) == 436208640 && (~inst & 1163921408) == 1163921408)) { // csinc   Rd, Rn, Rm, COND
            strcpy(output, "csinc   Rd, Rn, Rm, COND");
            return;
        }
        if (((inst & 1518338048) == 1518338048 && (~inst & 90180608) == 90180608)) { // csinv   Rd, Rn, Rm, COND
            strcpy(output, "csinv   Rd, Rn, Rm, COND");
            return;
        }
        if (((inst & 1509950464) == 1509950464 && (~inst & 90179584) == 90179584)) { // csneg   Rd, Rn, Rm, COND
            strcpy(output, "csneg   Rd, Rn, Rm, COND");
            return;
        }
        if (((inst & 3290431489) == 3290431489 && (~inst & 721420290) == 721420290)) { // dcps1   EXCEPTION
            strcpy(output, "dcps1   EXCEPTION");
            return;
        }
        if (((inst & 3290431490) == 3290431490 && (~inst & 721420289) == 721420289)) { // dcps2   EXCEPTION
            strcpy(output, "dcps2   EXCEPTION");
            return;
        }
        if (((inst & 3290431491) == 3290431491 && (~inst & 721420288) == 721420288)) { // dcps3   EXCEPTION
            strcpy(output, "dcps3   EXCEPTION");
            return;
        }
        if (((inst & 1157632032) == 1157632032 && (~inst & 672661504) == 672661504)) { // dmb     BARRIER
            strcpy(output, "dmb     BARRIER");
            return;
        }
        if (((inst & 1184890880) == 1184890880 && (~inst & 687865856) == 687865856)) { // drps    
            strcpy(output, "drps    ");
            return;
        }
        if (((inst & 1157632000) == 1157632000 && (~inst & 672661600) == 672661600)) { // dsb     BARRIER
            strcpy(output, "dsb     BARRIER");
            return;
        }
        if (((inst & 1577059328) == 1577059328 && (~inst & 18874368) == 18874368)) { // dup     Sd, En
            strcpy(output, "dup     Sd, En");
            return;
        }
        if (((inst & 234882048) == 234882048 && (~inst & 824182784) == 824182784)) { // dup     Vd, En
            strcpy(output, "dup     Vd, En");
            return;
        }
        if (((inst & 234884096) == 234884096 && (~inst & 824193024) == 824193024)) { // dup     Vd, Rn
            strcpy(output, "dup     Vd, Rn");
            return;
        }
        if (((inst & 1243611136) == 1243611136 && (~inst & 620756992) == 620756992)) { // eon     Rd, Rn, Rm_SFT
            strcpy(output, "eon     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 1241513984) == 1241513984 && (~inst & 891289600) == 891289600)) { // eor     Rd, Rn, Rm_SFT
            strcpy(output, "eor     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 1107296256) == 1107296256 && (~inst & 763363328) == 763363328)) { // eor     Rd_SP, Rn, LIMM
            strcpy(output, "eor     Rd_SP, Rn, LIMM");
            return;
        }
        if (((inst & 773856256) == 773856256 && (~inst & 297852928) == 297852928)) { // eor     Vd, Vn, Vm
            strcpy(output, "eor     Vd, Vn, Vm");
            return;
        }
        if (((inst & 1182793728) == 1182793728 && (~inst & 694157312) == 694157312)) { // eret    
            strcpy(output, "eret    ");
            return;
        }
        if (((inst & 58720256) == 58720256 && (~inst & 201326592) == 201326592)) { // extr    Rd, Rn, Rm, IMMS
            strcpy(output, "extr    Rd, Rn, Rm, IMMS");
            return;
        }
        if (((inst & 771751936) == 771751936 && (~inst & 287310848) == 287310848)) { // ext     Vd, Vn, Vm, IDX
            strcpy(output, "ext     Vd, Vn, Vm, IDX");
            return;
        }
        if (((inst & 1042322432) == 1042322432 && (~inst & 16787456) == 16787456)) { // fabd    Sd, Sn, Sm
            strcpy(output, "fabd    Sd, Sn, Sm");
            return;
        }
        if (((inst & 782291968) == 782291968 && (~inst & 285222912) == 285222912)) { // fabd    Vd, Vn, Vm
            strcpy(output, "fabd    Vd, Vn, Vm");
            return;
        }
        if (((inst & 505462784) == 505462784 && (~inst & 17251328) == 17251328)) { // fabs    Fd, Fn
            strcpy(output, "fabs    Fd, Fn");
            return;
        }
        if (((inst & 237041664) == 237041664 && (~inst & 554763264) == 554763264)) { // fabs    Vd, Vn
            strcpy(output, "fabs    Vd, Vn");
            return;
        }
        if (((inst & 1042312192) == 1042312192 && (~inst & 25169920) == 25169920)) { // facge   Sd, Sn, Sm
            strcpy(output, "facge   Sd, Sn, Sm");
            return;
        }
        if (((inst & 237038592) == 237038592 && (~inst & 293605376) == 293605376)) { // facge   Vd, Vn, Vm
            strcpy(output, "facge   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1050700800) == 1050700800 && (~inst & 16781312) == 16781312)) { // facgt   Sd, Sn, Sm
            strcpy(output, "facgt   Sd, Sn, Sm");
            return;
        }
        if (((inst & 245427200) == 245427200 && (~inst & 285216768) == 285216768)) { // facgt   Vd, Vn, Vm
            strcpy(output, "facgt   Vd, Vn, Vm");
            return;
        }
        if (((inst & 505423872) == 505423872 && (~inst & 1627444224) == 1627444224)) { // fadd    Fd, Fn, Fm
            strcpy(output, "fadd    Fd, Fn, Fm");
            return;
        }
        if (((inst & 238082048) == 238082048 && (~inst & 16851968) == 16851968)) { // faddp   Sd, Vn
            strcpy(output, "faddp   Sd, Vn");
            return;
        }
        if (((inst & 773903360) == 773903360 && (~inst & 293611520) == 293611520)) { // faddp   Vd, Vn, Vm
            strcpy(output, "faddp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237032448) == 237032448 && (~inst & 830482432) == 830482432)) { // fadd    Vd, Vn, Vm
            strcpy(output, "fadd    Vd, Vn, Vm");
            return;
        }
        if (((inst & 505414672) == 505414672 && (~inst & 1627392000) == 1627392000)) { // fccmpe  Fn, Fm, NZCV, COND
            strcpy(output, "fccmpe  Fn, Fm, NZCV, COND");
            return;
        }
        if (((inst & 505414656) == 505414656 && (~inst & 1627392016) == 1627392016)) { // fccmp   Fn, Fm, NZCV, COND
            strcpy(output, "fccmp   Fn, Fm, NZCV, COND");
            return;
        }
        if (((inst & 505468928) == 505468928 && (~inst & 554771456) == 554771456)) { // fcmeq   Sd, Sn, IMM0
            strcpy(output, "fcmeq   Sd, Sn, IMM0");
            return;
        }
        if (((inst & 1579164672) == 1579164672 && (~inst & 553654272) == 553654272)) { // fcmeq   Sd, Sn, Sm
            strcpy(output, "fcmeq   Sd, Sn, Sm");
            return;
        }
        if (((inst & 237033472) == 237033472 && (~inst & 823206912) == 823206912)) { // fcmeq   Vd, Vn, IMM0
            strcpy(output, "fcmeq   Vd, Vn, IMM0");
            return;
        }
        if (((inst & 237036544) == 237036544 && (~inst & 830478336) == 830478336)) { // fcmeq   Vd, Vn, Vm
            strcpy(output, "fcmeq   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042335744) == 1042335744 && (~inst & 17904640) == 17904640)) { // fcmge   Sd, Sn, IMM0
            strcpy(output, "fcmge   Sd, Sn, IMM0");
            return;
        }
        if (((inst & 1042293760) == 1042293760 && (~inst & 25171968) == 25171968)) { // fcmge   Sd, Sn, Sm
            strcpy(output, "fcmge   Sd, Sn, Sm");
            return;
        }
        if (((inst & 773900288) == 773900288 && (~inst & 286340096) == 286340096)) { // fcmge   Vd, Vn, IMM0
            strcpy(output, "fcmge   Vd, Vn, IMM0");
            return;
        }
        if (((inst & 773907456) == 773907456 && (~inst & 293607424) == 293607424)) { // fcmge   Vd, Vn, Vm
            strcpy(output, "fcmge   Vd, Vn, Vm");
            return;
        }
        if (((inst & 505464832) == 505464832 && (~inst & 554775552) == 554775552)) { // fcmgt   Sd, Sn, IMM0
            strcpy(output, "fcmgt   Sd, Sn, IMM0");
            return;
        }
        if (((inst & 1050682368) == 1050682368 && (~inst & 16783360) == 16783360)) { // fcmgt   Sd, Sn, Sm
            strcpy(output, "fcmgt   Sd, Sn, Sm");
            return;
        }
        if (((inst & 237029376) == 237029376 && (~inst & 823211008) == 823211008)) { // fcmgt   Vd, Vn, IMM0
            strcpy(output, "fcmgt   Vd, Vn, IMM0");
            return;
        }
        if (((inst & 245425152) == 245425152 && (~inst & 285218816) == 285218816)) { // fcmgt   Vd, Vn, Vm
            strcpy(output, "fcmgt   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042339840) == 1042339840 && (~inst & 17900544) == 17900544)) { // fcmle   Sd, Sn, IMM0
            strcpy(output, "fcmle   Sd, Sn, IMM0");
            return;
        }
        if (((inst & 773904384) == 773904384 && (~inst & 286336000) == 286336000)) { // fcmle   Vd, Vn, IMM0
            strcpy(output, "fcmle   Vd, Vn, IMM0");
            return;
        }
        if (((inst & 505473024) == 505473024 && (~inst & 16782336) == 16782336)) { // fcmlt   Sd, Sn, IMM0
            strcpy(output, "fcmlt   Sd, Sn, IMM0");
            return;
        }
        if (((inst & 237037568) == 237037568 && (~inst & 285217792) == 285217792)) { // fcmlt   Vd, Vn, IMM0
            strcpy(output, "fcmlt   Vd, Vn, IMM0");
            return;
        }
        if (((inst & 505421840) == 505421840 && (~inst & 16833544) == 16833544)) { // fcmpe   Fn, Fm
            strcpy(output, "fcmpe   Fn, Fm");
            return;
        }
        if (((inst & 505421848) == 505421848 && (~inst & 16833536) == 16833536)) { // fcmpe   Fn, FPIMM0
            strcpy(output, "fcmpe   Fn, FPIMM0");
            return;
        }
        if (((inst & 505421824) == 505421824 && (~inst & 16833560) == 16833560)) { // fcmp    Fn, Fm
            strcpy(output, "fcmp    Fn, Fm");
            return;
        }
        if (((inst & 505421832) == 505421832 && (~inst & 16833552) == 16833552)) { // fcmp    Fn, FPIMM0
            strcpy(output, "fcmp    Fn, FPIMM0");
            return;
        }
        if (((inst & 505416704) == 505416704 && (~inst & 1627389952) == 1627389952)) { // fcsel   Fd, Fn, Fm, COND
            strcpy(output, "fcsel   Fd, Fn, Fm, COND");
            return;
        }
        if (((inst & 505675776) == 505675776 && (~inst & 17038336) == 17038336)) { // fcvtas  Rd, Fn
            strcpy(output, "fcvtas  Rd, Fn");
            return;
        }
        if (((inst & 505530368) == 505530368 && (~inst & 562050048) == 562050048)) { // fcvtas  Sd, Sn
            strcpy(output, "fcvtas  Sd, Sn");
            return;
        }
        if (((inst & 237094912) == 237094912 && (~inst & 830485504) == 830485504)) { // fcvtas  Vd, Vn
            strcpy(output, "fcvtas  Vd, Vn");
            return;
        }
        if (((inst & 505741312) == 505741312 && (~inst & 16972800) == 16972800)) { // fcvtau  Rd, Fn
            strcpy(output, "fcvtau  Rd, Fn");
            return;
        }
        if (((inst & 1042401280) == 1042401280 && (~inst & 25179136) == 25179136)) { // fcvtau  Sd, Sn
            strcpy(output, "fcvtau  Sd, Sn");
            return;
        }
        if (((inst & 773965824) == 773965824 && (~inst & 293614592) == 293614592)) { // fcvtau  Vd, Vn
            strcpy(output, "fcvtau  Vd, Vn");
            return;
        }
        if (((inst & 505561088) == 505561088 && (~inst & 17054720) == 17054720)) { // fcvt    Fd, Fn
            strcpy(output, "fcvt    Fd, Fn");
            return;
        }
        if (((inst & 1310816256) == 1310816256 && (~inst & 285246464) == 285246464)) { // fcvtl2  Vd, Vn
            strcpy(output, "fcvtl2  Vd, Vn");
            return;
        }
        if (((inst & 237074432) == 237074432 && (~inst & 1358988288) == 1358988288)) { // fcvtl   Vd, Vn
            strcpy(output, "fcvtl   Vd, Vn");
            return;
        }
        if (((inst & 506462208) == 506462208 && (~inst & 17824768) == 17824768)) { // fcvtms  Rd, Fn
            strcpy(output, "fcvtms  Rd, Fn");
            return;
        }
        if (((inst & 505526272) == 505526272 && (~inst & 563102720) == 563102720)) { // fcvtms  Sd, Sn
            strcpy(output, "fcvtms  Sd, Sn");
            return;
        }
        if (((inst & 237090816) == 237090816 && (~inst & 831538176) == 831538176)) { // fcvtms  Vd, Vn
            strcpy(output, "fcvtms  Vd, Vn");
            return;
        }
        if (((inst & 506527744) == 506527744 && (~inst & 17759232) == 17759232)) { // fcvtmu  Rd, Fn
            strcpy(output, "fcvtmu  Rd, Fn");
            return;
        }
        if (((inst & 1042397184) == 1042397184 && (~inst & 26231808) == 26231808)) { // fcvtmu  Sd, Sn
            strcpy(output, "fcvtmu  Sd, Sn");
            return;
        }
        if (((inst & 773961728) == 773961728 && (~inst & 294667264) == 294667264)) { // fcvtmu  Vd, Vn
            strcpy(output, "fcvtmu  Vd, Vn");
            return;
        }
        if (((inst & 1310812160) == 1310812160 && (~inst & 822121472) == 822121472)) { // fcvtn2  Vd, Vn
            strcpy(output, "fcvtn2  Vd, Vn");
            return;
        }
        if (((inst & 505413632) == 505413632 && (~inst & 18873344) == 18873344)) { // fcvtns  Rd, Fn
            strcpy(output, "fcvtns  Rd, Fn");
            return;
        }
        if (((inst & 505522176) == 505522176 && (~inst & 563106816) == 563106816)) { // fcvtns  Sd, Sn
            strcpy(output, "fcvtns  Sd, Sn");
            return;
        }
        if (((inst & 237086720) == 237086720 && (~inst & 831542272) == 831542272)) { // fcvtns  Vd, Vn
            strcpy(output, "fcvtns  Vd, Vn");
            return;
        }
        if (((inst & 505479168) == 505479168 && (~inst & 18807808) == 18807808)) { // fcvtnu  Rd, Fn
            strcpy(output, "fcvtnu  Rd, Fn");
            return;
        }
        if (((inst & 1042393088) == 1042393088 && (~inst & 26235904) == 26235904)) { // fcvtnu  Sd, Sn
            strcpy(output, "fcvtnu  Sd, Sn");
            return;
        }
        if (((inst & 773957632) == 773957632 && (~inst & 294671360) == 294671360)) { // fcvtnu  Vd, Vn
            strcpy(output, "fcvtnu  Vd, Vn");
            return;
        }
        if (((inst & 237070336) == 237070336 && (~inst & 1895863296) == 1895863296)) { // fcvtn   Vd, Vn
            strcpy(output, "fcvtn   Vd, Vn");
            return;
        }
        if (((inst & 505937920) == 505937920 && (~inst & 18349056) == 18349056)) { // fcvtps  Rd, Fn
            strcpy(output, "fcvtps  Rd, Fn");
            return;
        }
        if (((inst & 513910784) == 513910784 && (~inst & 554718208) == 554718208)) { // fcvtps  Sd, Sn
            strcpy(output, "fcvtps  Sd, Sn");
            return;
        }
        if (((inst & 245475328) == 245475328 && (~inst & 823153664) == 823153664)) { // fcvtps  Vd, Vn
            strcpy(output, "fcvtps  Vd, Vn");
            return;
        }
        if (((inst & 506003456) == 506003456 && (~inst & 18283520) == 18283520)) { // fcvtpu  Rd, Fn
            strcpy(output, "fcvtpu  Rd, Fn");
            return;
        }
        if (((inst & 1050781696) == 1050781696 && (~inst & 17847296) == 17847296)) { // fcvtpu  Sd, Sn
            strcpy(output, "fcvtpu  Sd, Sn");
            return;
        }
        if (((inst & 782346240) == 782346240 && (~inst & 286282752) == 286282752)) { // fcvtpu  Vd, Vn
            strcpy(output, "fcvtpu  Vd, Vn");
            return;
        }
        if (((inst & 1847683072) == 1847683072 && (~inst & 285250560) == 285250560)) { // fcvtxn2 Vd, Vn
            strcpy(output, "fcvtxn2 Vd, Vn");
            return;
        }
        if (((inst & 1042311168) == 1042311168 && (~inst & 16815104) == 16815104)) { // fcvtxn  Sd, Sn
            strcpy(output, "fcvtxn  Sd, Sn");
            return;
        }
        if (((inst & 773941248) == 773941248 && (~inst & 1358992384) == 1358992384)) { // fcvtxn  Vd, Vn
            strcpy(output, "fcvtxn  Vd, Vn");
            return;
        }
        if (((inst & 506986496) == 506986496 && (~inst & 17300480) == 17300480)) { // fcvtzs  Rd, Fn
            strcpy(output, "fcvtzs  Rd, Fn");
            return;
        }
        if (((inst & 503316480) == 503316480 && (~inst & 1092812800) == 1092812800)) { // fcvtzs  Rd, Fn, FBITS
            strcpy(output, "fcvtzs  Rd, Fn, FBITS");
            return;
        }
        if (((inst & 513914880) == 513914880 && (~inst & 554714112) == 554714112)) { // fcvtzs  Sd, Sn
            strcpy(output, "fcvtzs  Sd, Sn");
            return;
        }
        if (((inst & 1593879552) == 1593879552 && (~inst & 536870912) == 536870912)) { // fcvtzs  Sd, Sn, IMM_VLSR
            strcpy(output, "fcvtzs  Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 245479424) == 245479424 && (~inst & 823149568) == 823149568)) { // fcvtzs  Vd, Vn
            strcpy(output, "fcvtzs  Vd, Vn");
            return;
        }
        if (((inst & 251706368) == 251706368 && (~inst & 805306368) == 805306368)) { // fcvtzs  Vd, Vn, IMM_VLSR
            strcpy(output, "fcvtzs  Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 507052032) == 507052032 && (~inst & 17234944) == 17234944)) { // fcvtzu  Rd, Fn
            strcpy(output, "fcvtzu  Rd, Fn");
            return;
        }
        if (((inst & 503382016) == 503382016 && (~inst & 1092747264) == 1092747264)) { // fcvtzu  Rd, Fn, FBITS
            strcpy(output, "fcvtzu  Rd, Fn, FBITS");
            return;
        }
        if (((inst & 1050785792) == 1050785792 && (~inst & 17843200) == 17843200)) { // fcvtzu  Sd, Sn
            strcpy(output, "fcvtzu  Sd, Sn");
            return;
        }
        if (((inst & 1057012736) == 1057012736 && (~inst & 0) == 0)) { // fcvtzu  Sd, Sn, IMM_VLSR
            strcpy(output, "fcvtzu  Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 782350336) == 782350336 && (~inst & 286278656) == 286278656)) { // fcvtzu  Vd, Vn
            strcpy(output, "fcvtzu  Vd, Vn");
            return;
        }
        if (((inst & 788577280) == 788577280 && (~inst & 268435456) == 268435456)) { // fcvtzu  Vd, Vn, IMM_VLSR
            strcpy(output, "fcvtzu  Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 505419776) == 505419776 && (~inst & 1090577408) == 1090577408)) { // fdiv    Fd, Fn, Fm
            strcpy(output, "fdiv    Fd, Fn, Fm");
            return;
        }
        if (((inst & 773913600) == 773913600 && (~inst & 293601280) == 293601280)) { // fdiv    Vd, Vn, Vm
            strcpy(output, "fdiv    Vd, Vn, Vm");
            return;
        }
        if (((inst & 520093696) == 520093696 && (~inst & 1612742656) == 1612742656)) { // fmadd   Fd, Fn, Fm, Fa
            strcpy(output, "fmadd   Fd, Fn, Fm, Fa");
            return;
        }
        if (((inst & 505432064) == 505432064 && (~inst & 1627436032) == 1627436032)) { // fmax    Fd, Fn, Fm
            strcpy(output, "fmax    Fd, Fn, Fm");
            return;
        }
        if (((inst & 505440256) == 505440256 && (~inst & 553686016) == 553686016)) { // fmaxnm  Fd, Fn, Fm
            strcpy(output, "fmaxnm  Fd, Fn, Fm");
            return;
        }
        if (((inst & 506513408) == 506513408 && (~inst & 25244672) == 25244672)) { // fmaxnmp Sd, Vn
            strcpy(output, "fmaxnmp Sd, Vn");
            return;
        }
        if (((inst & 773899264) == 773899264 && (~inst & 293615616) == 293615616)) { // fmaxnmp Vd, Vn, Vm
            strcpy(output, "fmaxnmp Vd, Vn, Vm");
            return;
        }
        if (((inst & 237028352) == 237028352 && (~inst & 830486528) == 830486528)) { // fmaxnm  Vd, Vn, Vm
            strcpy(output, "fmaxnm  Vd, Vn, Vm");
            return;
        }
        if (((inst & 238077952) == 238077952 && (~inst & 293680128) == 293680128)) { // fmaxnmv Fd, Vn
            strcpy(output, "fmaxnmv Fd, Vn");
            return;
        }
        if (((inst & 506525696) == 506525696 && (~inst & 25232384) == 25232384)) { // fmaxp   Sd, Vn
            strcpy(output, "fmaxp   Sd, Vn");
            return;
        }
        if (((inst & 773911552) == 773911552 && (~inst & 293603328) == 293603328)) { // fmaxp   Vd, Vn, Vm
            strcpy(output, "fmaxp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237040640) == 237040640 && (~inst & 830474240) == 830474240)) { // fmax    Vd, Vn, Vm
            strcpy(output, "fmax    Vd, Vn, Vm");
            return;
        }
        if (((inst & 238090240) == 238090240 && (~inst & 293667840) == 293667840)) { // fmaxv   Fd, Vn
            strcpy(output, "fmaxv   Fd, Vn");
            return;
        }
        if (((inst & 505436160) == 505436160 && (~inst & 16819200) == 16819200)) { // fmin    Fd, Fn, Fm
            strcpy(output, "fmin    Fd, Fn, Fm");
            return;
        }
        if (((inst & 505444352) == 505444352 && (~inst & 1627423744) == 1627423744)) { // fminnm  Fd, Fn, Fm
            strcpy(output, "fminnm  Fd, Fn, Fm");
            return;
        }
        if (((inst & 514902016) == 514902016 && (~inst & 16856064) == 16856064)) { // fminnmp Sd, Vn
            strcpy(output, "fminnmp Sd, Vn");
            return;
        }
        if (((inst & 782287872) == 782287872 && (~inst & 285227008) == 285227008)) { // fminnmp Vd, Vn, Vm
            strcpy(output, "fminnmp Vd, Vn, Vm");
            return;
        }
        if (((inst & 245416960) == 245416960 && (~inst & 822097920) == 822097920)) { // fminnm  Vd, Vn, Vm
            strcpy(output, "fminnm  Vd, Vn, Vm");
            return;
        }
        if (((inst & 246466560) == 246466560 && (~inst & 285291520) == 285291520)) { // fminnmv Fd, Vn
            strcpy(output, "fminnmv Fd, Vn");
            return;
        }
        if (((inst & 514914304) == 514914304 && (~inst & 16843776) == 16843776)) { // fminp   Sd, Vn
            strcpy(output, "fminp   Sd, Vn");
            return;
        }
        if (((inst & 782300160) == 782300160 && (~inst & 285214720) == 285214720)) { // fminp   Vd, Vn, Vm
            strcpy(output, "fminp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 245429248) == 245429248 && (~inst & 822085632) == 822085632)) { // fmin    Vd, Vn, Vm
            strcpy(output, "fmin    Vd, Vn, Vm");
            return;
        }
        if (((inst & 246478848) == 246478848 && (~inst & 285279232) == 285279232)) { // fminv   Fd, Vn
            strcpy(output, "fminv   Fd, Vn");
            return;
        }
        if (((inst & 1593835520) == 1593835520 && (~inst & 536929280) == 536929280)) { // fmla    Sd, Sn, Em
            strcpy(output, "fmla    Sd, Sn, Em");
            return;
        }
        if (((inst & 251662336) == 251662336 && (~inst & 268493824) == 268493824)) { // fmla    Vd, Vn, Em
            strcpy(output, "fmla    Vd, Vn, Em");
            return;
        }
        if (((inst & 237030400) == 237030400 && (~inst & 293613568) == 293613568)) { // fmla    Vd, Vn, Vm
            strcpy(output, "fmla    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593851904) == 1593851904 && (~inst & 536912896) == 536912896)) { // fmls    Sd, Sn, Em
            strcpy(output, "fmls    Sd, Sn, Em");
            return;
        }
        if (((inst & 251678720) == 251678720 && (~inst & 268477440) == 268477440)) { // fmls    Vd, Vn, Em
            strcpy(output, "fmls    Vd, Vn, Em");
            return;
        }
        if (((inst & 245419008) == 245419008 && (~inst & 285224960) == 285224960)) { // fmls    Vd, Vn, Vm
            strcpy(output, "fmls    Vd, Vn, Vm");
            return;
        }
        if (((inst & 505430016) == 505430016 && (~inst & 17284096) == 17284096)) { // fmov    Fd, Fn
            strcpy(output, "fmov    Fd, Fn");
            return;
        }
        if (((inst & 505417728) == 505417728 && (~inst & 1090522112) == 1090522112)) { // fmov    Fd, FPIMM
            strcpy(output, "fmov    Fd, FPIMM");
            return;
        }
        if (((inst & 505872384) == 505872384 && (~inst & 17366016) == 17366016)) { // fmov    Fd, Rn
            strcpy(output, "fmov    Fd, Rn");
            return;
        }
        if (((inst & 505806848) == 505806848 && (~inst & 17431552) == 17431552)) { // fmov    Rd, Fn
            strcpy(output, "fmov    Rd, Fn");
            return;
        }
        if (((inst & 506331136) == 506331136 && (~inst & 16907264) == 16907264)) { // fmov    Rd, VnD1
            strcpy(output, "fmov    Rd, VnD1");
            return;
        }
        if (((inst & 506396672) == 506396672 && (~inst & 16841728) == 16841728)) { // fmov    VdD1, Rn
            strcpy(output, "fmov    VdD1, Rn");
            return;
        }
        if (((inst & 251720704) == 251720704 && (~inst & 805308416) == 805308416)) { // fmov    Vd, SIMD_FPIMM
            strcpy(output, "fmov    Vd, SIMD_FPIMM");
            return;
        }
        if (((inst & 788591616) == 788591616 && (~inst & 268437504) == 268437504)) { // fmov    Vd, SIMD_FPIMM
            strcpy(output, "fmov    Vd, SIMD_FPIMM");
            return;
        }
        if (((inst & 520126464) == 520126464 && (~inst & 1612709888) == 1612709888)) { // fmsub   Fd, Fn, Fm, Fa
            strcpy(output, "fmsub   Fd, Fn, Fm, Fa");
            return;
        }
        if (((inst & 505415680) == 505415680 && (~inst & 1090581504) == 1090581504)) { // fmul    Fd, Fn, Fm
            strcpy(output, "fmul    Fd, Fn, Fm");
            return;
        }
        if (((inst & 1593872384) == 1593872384 && (~inst & 536896512) == 536896512)) { // fmul    Sd, Sn, Em
            strcpy(output, "fmul    Sd, Sn, Em");
            return;
        }
        if (((inst & 251695104) == 251695104 && (~inst & 805331968) == 805331968)) { // fmul    Vd, Vn, Em
            strcpy(output, "fmul    Vd, Vn, Em");
            return;
        }
        if (((inst & 773905408) == 773905408 && (~inst & 285220864) == 285220864)) { // fmul    Vd, Vn, Vm
            strcpy(output, "fmul    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1056997376) == 1056997376 && (~inst & 1024) == 1024)) { // fmulx   Sd, Sn, Em
            strcpy(output, "fmulx   Sd, Sn, Em");
            return;
        }
        if (((inst & 1579195392) == 1579195392 && (~inst & 553656320) == 553656320)) { // fmulx   Sd, Sn, Sm
            strcpy(output, "fmulx   Sd, Sn, Sm");
            return;
        }
        if (((inst & 788566016) == 788566016 && (~inst & 268461056) == 268461056)) { // fmulx   Vd, Vn, Em
            strcpy(output, "fmulx   Vd, Vn, Em");
            return;
        }
        if (((inst & 237034496) == 237034496 && (~inst & 822091776) == 822091776)) { // fmulx   Vd, Vn, Vm
            strcpy(output, "fmulx   Vd, Vn, Vm");
            return;
        }
        if (((inst & 505495552) == 505495552 && (~inst & 17218560) == 17218560)) { // fneg    Fd, Fn
            strcpy(output, "fneg    Fd, Fn");
            return;
        }
        if (((inst & 773912576) == 773912576 && (~inst & 17892352) == 17892352)) { // fneg    Vd, Vn
            strcpy(output, "fneg    Vd, Vn");
            return;
        }
        if (((inst & 522190848) == 522190848 && (~inst & 1610645504) == 1610645504)) { // fnmadd  Fd, Fn, Fm, Fa
            strcpy(output, "fnmadd  Fd, Fn, Fm, Fa");
            return;
        }
        if (((inst & 522223616) == 522223616 && (~inst & 1610612736) == 1610612736)) { // fnmsub  Fd, Fn, Fm, Fa
            strcpy(output, "fnmsub  Fd, Fn, Fm, Fa");
            return;
        }
        if (((inst & 505448448) == 505448448 && (~inst & 1627419648) == 1627419648)) { // fnmul   Fd, Fn, Fm
            strcpy(output, "fnmul   Fd, Fn, Fm");
            return;
        }
        if (((inst & 513923072) == 513923072 && (~inst & 553657344) == 553657344)) { // frecpe  Sd, Sn
            strcpy(output, "frecpe  Sd, Sn");
            return;
        }
        if (((inst & 245487616) == 245487616 && (~inst & 822092800) == 822092800)) { // frecpe  Vd, Vn
            strcpy(output, "frecpe  Vd, Vn");
            return;
        }
        if (((inst & 1579187200) == 1579187200 && (~inst & 562036736) == 562036736)) { // frecps  Sd, Sn, Sm
            strcpy(output, "frecps  Sd, Sn, Sm");
            return;
        }
        if (((inst & 237042688) == 237042688 && (~inst & 830472192) == 830472192)) { // frecps  Vd, Vn, Vm
            strcpy(output, "frecps  Vd, Vn, Vm");
            return;
        }
        if (((inst & 505542656) == 505542656 && (~inst & 16778240) == 16778240)) { // frecpx  Sd, Sn
            strcpy(output, "frecpx  Sd, Sn");
            return;
        }
        if (((inst & 505823232) == 505823232 && (~inst & 16890880) == 16890880)) { // frinta  Fd, Fn
            strcpy(output, "frinta  Fd, Fn");
            return;
        }
        if (((inst & 773949440) == 773949440 && (~inst & 293630976) == 293630976)) { // frinta  Vd, Vn
            strcpy(output, "frinta  Vd, Vn");
            return;
        }
        if (((inst & 505856000) == 505856000 && (~inst & 16792576) == 16792576)) { // frinti  Fd, Fn
            strcpy(output, "frinti  Fd, Fn");
            return;
        }
        if (((inst & 782342144) == 782342144 && (~inst & 16802816) == 16802816)) { // frinti  Vd, Vn
            strcpy(output, "frinti  Vd, Vn");
            return;
        }
        if (((inst & 505757696) == 505757696 && (~inst & 16956416) == 16956416)) { // frintm  Fd, Fn
            strcpy(output, "frintm  Fd, Fn");
            return;
        }
        if (((inst & 237082624) == 237082624 && (~inst & 562062336) == 562062336)) { // frintm  Vd, Vn
            strcpy(output, "frintm  Vd, Vn");
            return;
        }
        if (((inst & 505692160) == 505692160 && (~inst & 17021952) == 17021952)) { // frintn  Fd, Fn
            strcpy(output, "frintn  Fd, Fn");
            return;
        }
        if (((inst & 237078528) == 237078528 && (~inst & 830501888) == 830501888)) { // frintn  Vd, Vn
            strcpy(output, "frintn  Vd, Vn");
            return;
        }
        if (((inst & 505724928) == 505724928 && (~inst & 16989184) == 16989184)) { // frintp  Fd, Fn
            strcpy(output, "frintp  Fd, Fn");
            return;
        }
        if (((inst & 245467136) == 245467136 && (~inst & 285242368) == 285242368)) { // frintp  Vd, Vn
            strcpy(output, "frintp  Vd, Vn");
            return;
        }
        if (((inst & 505888768) == 505888768 && (~inst & 16825344) == 16825344)) { // frintx  Fd, Fn
            strcpy(output, "frintx  Fd, Fn");
            return;
        }
        if (((inst & 773953536) == 773953536 && (~inst & 25191424) == 25191424)) { // frintx  Vd, Vn
            strcpy(output, "frintx  Vd, Vn");
            return;
        }
        if (((inst & 505790464) == 505790464 && (~inst & 16923648) == 16923648)) { // frintz  Fd, Fn
            strcpy(output, "frintz  Fd, Fn");
            return;
        }
        if (((inst & 245471232) == 245471232 && (~inst & 553673728) == 553673728)) { // frintz  Vd, Vn
            strcpy(output, "frintz  Vd, Vn");
            return;
        }
        if (((inst & 1050793984) == 1050793984 && (~inst & 16786432) == 16786432)) { // frsqrte Sd, Sn
            strcpy(output, "frsqrte Sd, Sn");
            return;
        }
        if (((inst & 782358528) == 782358528 && (~inst & 285221888) == 285221888)) { // frsqrte Vd, Vn
            strcpy(output, "frsqrte Vd, Vn");
            return;
        }
        if (((inst & 1587575808) == 1587575808 && (~inst & 553648128) == 553648128)) { // frsqrts Sd, Sn, Sm
            strcpy(output, "frsqrts Sd, Sn, Sm");
            return;
        }
        if (((inst & 245431296) == 245431296 && (~inst & 285212672) == 285212672)) { // frsqrts Vd, Vn, Vm
            strcpy(output, "frsqrts Vd, Vn, Vm");
            return;
        }
        if (((inst & 505528320) == 505528320 && (~inst & 17185792) == 17185792)) { // fsqrt   Fd, Fn
            strcpy(output, "fsqrt   Fd, Fn");
            return;
        }
        if (((inst & 237107200) == 237107200 && (~inst & 285213696) == 285213696)) { // fsqrt   Vd, Vn
            strcpy(output, "fsqrt   Vd, Vn");
            return;
        }
        if (((inst & 505427968) == 505427968 && (~inst & 1627440128) == 1627440128)) { // fsub    Fd, Fn, Fm
            strcpy(output, "fsub    Fd, Fn, Fm");
            return;
        }
        if (((inst & 245421056) == 245421056 && (~inst & 822093824) == 822093824)) { // fsub    Vd, Vn, Vm
            strcpy(output, "fsub    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1157636096) == 1157636096 && (~inst & 672665600) == 672665600)) { // hint    UIMM7
            strcpy(output, "hint    UIMM7");
            return;
        }
        if (((inst & 3288334336) == 3288334336 && (~inst & 723517443) == 723517443)) { // hlt     EXCEPTION
            strcpy(output, "hlt     EXCEPTION");
            return;
        }
        if (((inst & 3288334338) == 3288334338 && (~inst & 723517441) == 723517441)) { // hvc     EXCEPTION
            strcpy(output, "hvc     EXCEPTION");
            return;
        }
        if (((inst & 771752960) == 771752960 && (~inst & 287309824) == 287309824)) { // ins     Ed, En
            strcpy(output, "ins     Ed, En");
            return;
        }
        if (((inst & 234888192) == 234888192 && (~inst & 824188928) == 824188928)) { // ins     Ed, Rn
            strcpy(output, "ins     Ed, Rn");
            return;
        }
        if (((inst & 1157632192) == 1157632192 && (~inst & 672661536) == 672661536)) { // isb     BARRIER_ISB
            strcpy(output, "isb     BARRIER_ISB");
            return;
        }
        if (((inst & 230686720) == 230686720 && (~inst & 840966144) == 840966144)) { // ld1     LEt, SIMD_ADDR_POST
            strcpy(output, "ld1     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 222298112) == 222298112 && (~inst & 849354752) == 849354752)) { // ld1     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "ld1     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 216006656) == 216006656 && (~inst & 838868992) == 838868992)) { // ld2     LEt, SIMD_ADDR_POST
            strcpy(output, "ld2     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 224395264) == 224395264 && (~inst & 847257600) == 847257600)) { // ld2     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "ld2     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 230694912) == 230694912 && (~inst & 840957952) == 840957952)) { // ld3     LEt, SIMD_ADDR_POST
            strcpy(output, "ld3     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 222306304) == 222306304 && (~inst & 849346560) == 849346560)) { // ld3     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "ld3     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 216014848) == 216014848 && (~inst & 838860800) == 838860800)) { // ld4     LEt, SIMD_ADDR_POST
            strcpy(output, "ld4     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 224403456) == 224403456 && (~inst & 847249408) == 847249408)) { // ld4     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "ld4     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 213909504) == 213909504 && (~inst & 857735168) == 857735168)) { // ld4     LVt, SIMD_ADDR_POST
            strcpy(output, "ld4     LVt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 205520896) == 205520896 && (~inst & 864026624) == 864026624)) { // ld4     LVt, SIMD_ADDR_SIMPLE
            strcpy(output, "ld4     LVt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 146800640) == 146800640 && (~inst & 4127195136) == 4127195136)) { // ldarb   Rt, ADDR_SIMPLE
            strcpy(output, "ldarb   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1220542464) == 1220542464 && (~inst & 3053453312) == 3053453312)) { // ldarh   Rt, ADDR_SIMPLE
            strcpy(output, "ldarh   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 2294284288) == 2294284288 && (~inst & 905969664) == 905969664)) { // ldar    Rt, ADDR_SIMPLE
            strcpy(output, "ldar    Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 140541952) == 140541952 && (~inst & 914358272) == 914358272)) { // ldaxp   Rt, Rt2, ADDR_SIMPLE
            strcpy(output, "ldaxp   Rt, Rt2, ADDR_SIMPLE");
            return;
        }
        if (((inst & 138444800) == 138444800 && (~inst & 4137680896) == 4137680896)) { // ldaxrb  Rt, ADDR_SIMPLE
            strcpy(output, "ldaxrb  Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1212186624) == 1212186624 && (~inst & 3063939072) == 3063939072)) { // ldaxrh  Rt, ADDR_SIMPLE
            strcpy(output, "ldaxrh  Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 2285928448) == 2285928448 && (~inst & 916455424) == 916455424)) { // ldaxr   Rt, ADDR_SIMPLE
            strcpy(output, "ldaxr   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 742391808) == 742391808 && (~inst & 310378496) == 310378496)) { // ldnp    Ft, Ft2, ADDR_SIMM7
            strcpy(output, "ldnp    Ft, Ft2, ADDR_SIMM7");
            return;
        }
        if (((inst & 675282944) == 675282944 && (~inst & 1451229184) == 1451229184)) { // ldnp    Rt, Rt2, ADDR_SIMM7
            strcpy(output, "ldnp    Rt, Rt2, ADDR_SIMM7");
            return;
        }
        if (((inst & 742391808) == 742391808 && (~inst & 310378496) == 310378496)) { // ldp     Ft, Ft2, ADDR_SIMM7
            strcpy(output, "ldp     Ft, Ft2, ADDR_SIMM7");
            return;
        }
        if (((inst & 750780416) == 750780416 && (~inst & 301989888) == 301989888)) { // ldp     Ft, Ft2, ADDR_SIMM7
            strcpy(output, "ldp     Ft, Ft2, ADDR_SIMM7");
            return;
        }
        if (((inst & 683671552) == 683671552 && (~inst & 1442840576) == 1442840576)) { // ldp     Rt, Rt2, ADDR_SIMM7
            strcpy(output, "ldp     Rt, Rt2, ADDR_SIMM7");
            return;
        }
        if (((inst & 1749024768) == 1749024768 && (~inst & 377487360) == 377487360)) { // ldpsw   Rt, Rt2, ADDR_SIMM7
            strcpy(output, "ldpsw   Rt, Rt2, ADDR_SIMM7");
            return;
        }
        if (((inst & 1757413376) == 1757413376 && (~inst & 369098752) == 369098752)) { // ldpsw   Rt, Rt2, ADDR_SIMM7
            strcpy(output, "ldpsw   Rt, Rt2, ADDR_SIMM7");
            return;
        }
        if (((inst & 945817600) == 945817600 && (~inst & 3347055616) == 3347055616)) { // ldrb    Rt, ADDR_REGOFF
            strcpy(output, "ldrb    Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 943719424) == 943719424 && (~inst & 3347054592) == 3347054592)) { // ldrb    Rt, ADDR_SIMM9
            strcpy(output, "ldrb    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 423624704) == 423624704 && (~inst & 3330277376) == 3330277376)) { // ldrb    Rt, ADDR_UIMM12
            strcpy(output, "ldrb    Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 469762048) == 469762048 && (~inst & 587202560) == 587202560)) { // ldr     Ft, ADDR_PCREL19
            strcpy(output, "ldr     Ft, ADDR_PCREL19");
            return;
        }
        if (((inst & 1010829312) == 1010829312 && (~inst & 50332672) == 50332672)) { // ldr     Ft, ADDR_REGOFF
            strcpy(output, "ldr     Ft, ADDR_REGOFF");
            return;
        }
        if (((inst & 1010828288) == 1010828288 && (~inst & 50331648) == 50331648)) { // ldr     Ft, ADDR_SIMM9
            strcpy(output, "ldr     Ft, ADDR_SIMM9");
            return;
        }
        if (((inst & 490733568) == 490733568 && (~inst & 33554432) == 33554432)) { // ldr     Ft, ADDR_UIMM12
            strcpy(output, "ldr     Ft, ADDR_UIMM12");
            return;
        }
        if (((inst & 2019559424) == 2019559424 && (~inst & 2273313792) == 2273313792)) { // ldrh    Rt, ADDR_REGOFF
            strcpy(output, "ldrh    Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 2017461248) == 2017461248 && (~inst & 2273312768) == 2273312768)) { // ldrh    Rt, ADDR_SIMM9
            strcpy(output, "ldrh    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 1497366528) == 1497366528 && (~inst & 2256535552) == 2256535552)) { // ldrh    Rt, ADDR_UIMM12
            strcpy(output, "ldrh    Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 402653184) == 402653184 && (~inst & 2801795072) == 2801795072)) { // ldr     Rt, ADDR_PCREL19
            strcpy(output, "ldr     Rt, ADDR_PCREL19");
            return;
        }
        if (((inst & 3093301248) == 3093301248 && (~inst & 125830144) == 125830144)) { // ldr     Rt, ADDR_REGOFF
            strcpy(output, "ldr     Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 3091203072) == 3091203072 && (~inst & 125829120) == 125829120)) { // ldr     Rt, ADDR_SIMM9
            strcpy(output, "ldr     Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2571108352) == 2571108352 && (~inst & 109051904) == 109051904)) { // ldr     Rt, ADDR_UIMM12
            strcpy(output, "ldr     Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 950011904) == 950011904 && (~inst & 3338667008) == 3338667008)) { // ldrsb   Rt, ADDR_REGOFF
            strcpy(output, "ldrsb   Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 947913728) == 947913728 && (~inst & 3338665984) == 3338665984)) { // ldrsb   Rt, ADDR_SIMM9
            strcpy(output, "ldrsb   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 427819008) == 427819008 && (~inst & 3321888768) == 3321888768)) { // ldrsb   Rt, ADDR_UIMM12
            strcpy(output, "ldrsb   Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 2023753728) == 2023753728 && (~inst & 2264925184) == 2264925184)) { // ldrsh   Rt, ADDR_REGOFF
            strcpy(output, "ldrsh   Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 2021655552) == 2021655552 && (~inst & 117440512) == 117440512)) { // ldrsh   Rt, ADDR_SIMM9
            strcpy(output, "ldrsh   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 1501560832) == 1501560832 && (~inst & 2248146944) == 2248146944)) { // ldrsh   Rt, ADDR_UIMM12
            strcpy(output, "ldrsh   Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 2550136832) == 2550136832 && (~inst & 1728053248) == 1728053248)) { // ldrsw   Rt, ADDR_PCREL19
            strcpy(output, "ldrsw   Rt, ADDR_PCREL19");
            return;
        }
        if (((inst & 3097495552) == 3097495552 && (~inst & 1191183360) == 1191183360)) { // ldrsw   Rt, ADDR_REGOFF
            strcpy(output, "ldrsw   Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 3095397376) == 3095397376 && (~inst & 1191182336) == 1191182336)) { // ldrsw   Rt, ADDR_SIMM9
            strcpy(output, "ldrsw   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2575302656) == 2575302656 && (~inst & 1174405120) == 1174405120)) { // ldrsw   Rt, ADDR_UIMM12
            strcpy(output, "ldrsw   Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 943718400) == 943718400 && (~inst & 3349152768) == 3349152768)) { // ldtrb   Rt, ADDR_SIMM9
            strcpy(output, "ldtrb   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2017460224) == 2017460224 && (~inst & 2275410944) == 2275410944)) { // ldtrh   Rt, ADDR_SIMM9
            strcpy(output, "ldtrh   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 3091202048) == 3091202048 && (~inst & 127927296) == 127927296)) { // ldtr    Rt, ADDR_SIMM9
            strcpy(output, "ldtr    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 947912704) == 947912704 && (~inst & 3340764160) == 3340764160)) { // ldtrsb  Rt, ADDR_SIMM9
            strcpy(output, "ldtrsb  Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2021654528) == 2021654528 && (~inst & 119538688) == 119538688)) { // ldtrsh  Rt, ADDR_SIMM9
            strcpy(output, "ldtrsh  Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 3095396352) == 3095396352 && (~inst & 1193280512) == 1193280512)) { // ldtrsw  Rt, ADDR_SIMM9
            strcpy(output, "ldtrsw  Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 943718400) == 943718400 && (~inst & 3347055616) == 3347055616)) { // ldurb   Rt, ADDR_SIMM9
            strcpy(output, "ldurb   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 1010827264) == 1010827264 && (~inst & 50332672) == 50332672)) { // ldur    Ft, ADDR_SIMM9
            strcpy(output, "ldur    Ft, ADDR_SIMM9");
            return;
        }
        if (((inst & 2017460224) == 2017460224 && (~inst & 2273313792) == 2273313792)) { // ldurh   Rt, ADDR_SIMM9
            strcpy(output, "ldurh   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 3091202048) == 3091202048 && (~inst & 125830144) == 125830144)) { // ldur    Rt, ADDR_SIMM9
            strcpy(output, "ldur    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 947912704) == 947912704 && (~inst & 3338667008) == 3338667008)) { // ldursb  Rt, ADDR_SIMM9
            strcpy(output, "ldursb  Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2021654528) == 2021654528 && (~inst & 2264925184) == 2264925184)) { // ldursh  Rt, ADDR_SIMM9
            strcpy(output, "ldursh  Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 3095396352) == 3095396352 && (~inst & 1191183360) == 1191183360)) { // ldursw  Rt, ADDR_SIMM9
            strcpy(output, "ldursw  Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 140509184) == 140509184 && (~inst & 914391040) == 914391040)) { // ldxp    Rt, Rt2, ADDR_SIMPLE
            strcpy(output, "ldxp    Rt, Rt2, ADDR_SIMPLE");
            return;
        }
        if (((inst & 138412032) == 138412032 && (~inst & 4137713664) == 4137713664)) { // ldxrb   Rt, ADDR_SIMPLE
            strcpy(output, "ldxrb   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1212153856) == 1212153856 && (~inst & 3063971840) == 3063971840)) { // ldxrh   Rt, ADDR_SIMPLE
            strcpy(output, "ldxrh   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 2285895680) == 2285895680 && (~inst & 916488192) == 916488192)) { // ldxr    Rt, ADDR_SIMPLE
            strcpy(output, "ldxr    Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 448798720) == 448798720 && (~inst & 85990400) == 85990400)) { // lslv    Rd, Rn, Rm
            strcpy(output, "lslv    Rd, Rn, Rm");
            return;
        }
        if (((inst & 440411136) == 440411136 && (~inst & 85989376) == 85989376)) { // lsrv    Rd, Rn, Rm
            strcpy(output, "lsrv    Rd, Rn, Rm");
            return;
        }
        if (((inst & 452984832) == 452984832 && (~inst & 73433088) == 73433088)) { // madd    Rd, Rn, Rm, Ra
            strcpy(output, "madd    Rd, Rn, Rm, Ra");
            return;
        }
        if (((inst & 251658240) == 251658240 && (~inst & 268497920) == 268497920)) { // mla     Vd, Vn, Em
            strcpy(output, "mla     Vd, Vn, Em");
            return;
        }
        if (((inst & 237016064) == 237016064 && (~inst & 822110208) == 822110208)) { // mla     Vd, Vn, Vm
            strcpy(output, "mla     Vd, Vn, Vm");
            return;
        }
        if (((inst & 251674624) == 251674624 && (~inst & 268481536) == 268481536)) { // mls     Vd, Vn, Em
            strcpy(output, "mls     Vd, Vn, Em");
            return;
        }
        if (((inst & 773886976) == 773886976 && (~inst & 285239296) == 285239296)) { // mls     Vd, Vn, Vm
            strcpy(output, "mls     Vd, Vn, Vm");
            return;
        }
        if (((inst & 788587520) == 788587520 && (~inst & 268441600) == 268441600)) { // movi    Sd, SIMD_IMM
            strcpy(output, "movi    Sd, SIMD_IMM");
            return;
        }
        if (((inst & 251716608) == 251716608 && (~inst & 805312512) == 805312512)) { // movi    Vd, SIMD_IMM
            strcpy(output, "movi    Vd, SIMD_IMM");
            return;
        }
        if (((inst & 251659264) == 251659264 && (~inst & 805343232) == 805343232)) { // movi    Vd, SIMD_IMM_SFT
            strcpy(output, "movi    Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 251692032) == 251692032 && (~inst & 805328896) == 805328896)) { // movi    Vd, SIMD_IMM_SFT
            strcpy(output, "movi    Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 251708416) == 251708416 && (~inst & 805316608) == 805316608)) { // movi    Vd, SIMD_IMM_SFT
            strcpy(output, "movi    Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 578813952) == 578813952 && (~inst & 218103808) == 218103808)) { // movk    Rd, HALF
            strcpy(output, "movk    Rd, HALF");
            return;
        }
        if (((inst & 41943040) == 41943040 && (~inst & 1828716544) == 1828716544)) { // movn    Rd, HALF
            strcpy(output, "movn    Rd, HALF");
            return;
        }
        if (((inst & 1115684864) == 1115684864 && (~inst & 754974720) == 754974720)) { // movz    Rd, HALF
            strcpy(output, "movz    Rd, HALF");
            return;
        }
        if (((inst & 1160773632) == 1160773632 && (~inst & 671088640) == 671088640)) { // mrs     Rt, SYSREG
            strcpy(output, "mrs     Rt, SYSREG");
            return;
        }
        if (((inst & 1157627904) == 1157627904 && (~inst & 672673792) == 672673792)) { // msr     PSTATEFIELD, UIMM4
            strcpy(output, "msr     PSTATEFIELD, UIMM4");
            return;
        }
        if (((inst & 1158676480) == 1158676480 && (~inst & 673185792) == 673185792)) { // msr     SYSREG, Rt
            strcpy(output, "msr     SYSREG, Rt");
            return;
        }
        if (((inst & 453017600) == 453017600 && (~inst & 69206016) == 69206016)) { // msub    Rd, Rn, Rm, Ra
            strcpy(output, "msub    Rd, Rn, Rm, Ra");
            return;
        }
        if (((inst & 251691008) == 251691008 && (~inst & 268465152) == 268465152)) { // mul     Vd, Vn, Em
            strcpy(output, "mul     Vd, Vn, Em");
            return;
        }
        if (((inst & 237018112) == 237018112 && (~inst & 822108160) == 822108160)) { // mul     Vd, Vn, Vm
            strcpy(output, "mul     Vd, Vn, Vm");
            return;
        }
        if (((inst & 788530176) == 788530176 && (~inst & 268472320) == 268472320)) { // mvni    Vd, SIMD_IMM_SFT
            strcpy(output, "mvni    Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 788562944) == 788562944 && (~inst & 268457984) == 268457984)) { // mvni    Vd, SIMD_IMM_SFT
            strcpy(output, "mvni    Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 788579328) == 788579328 && (~inst & 268445696) == 268445696)) { // mvni    Vd, SIMD_IMM_SFT
            strcpy(output, "mvni    Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 1042331648) == 1042331648 && (~inst & 16860160) == 16860160)) { // neg     Sd, Sn
            strcpy(output, "neg     Sd, Sn");
            return;
        }
        if (((inst & 773896192) == 773896192 && (~inst & 285295616) == 285295616)) { // neg     Vd, Vn
            strcpy(output, "neg     Vd, Vn");
            return;
        }
        if (((inst & 773871616) == 773871616 && (~inst & 289973248) == 289973248)) { // not     Vd, Vn
            strcpy(output, "not     Vd, Vn");
            return;
        }
        if (((inst & 706740224) == 706740224 && (~inst & 1157627904) == 1157627904)) { // orn     Rd, Rn, Rm_SFT
            strcpy(output, "orn     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 249568256) == 249568256 && (~inst & 822140928) == 822140928)) { // orn     Vd, Vn, Vm
            strcpy(output, "orn     Vd, Vn, Vm");
            return;
        }
        if (((inst & 704643072) == 704643072 && (~inst & 1428160512) == 1428160512)) { // orr     Rd, Rn, Rm_SFT
            strcpy(output, "orr     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 570425344) == 570425344 && (~inst & 1300234240) == 1300234240)) { // orr     Rd_SP, Rn, LIMM
            strcpy(output, "orr     Rd_SP, Rn, LIMM");
            return;
        }
        if (((inst & 251663360) == 251663360 && (~inst & 805339136) == 805339136)) { // orr     Vd, SIMD_IMM_SFT
            strcpy(output, "orr     Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 251696128) == 251696128 && (~inst & 805324800) == 805324800)) { // orr     Vd, SIMD_IMM_SFT
            strcpy(output, "orr     Vd, SIMD_IMM_SFT");
            return;
        }
        if (((inst & 245373952) == 245373952 && (~inst & 826335232) == 826335232)) { // orr     Vd, Vn, Vm
            strcpy(output, "orr     Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310777344) == 1310777344 && (~inst & 20978688) == 20978688)) { // pmull2  Vd, Vn, Vm
            strcpy(output, "pmull2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 1314971648) == 1314971648 && (~inst & 16784384) == 16784384)) { // pmull2  Vd, Vn, Vm
            strcpy(output, "pmull2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 237035520) == 237035520 && (~inst & 1094720512) == 1094720512)) { // pmull   Vd, Vn, Vm
            strcpy(output, "pmull   Vd, Vn, Vm");
            return;
        }
        if (((inst & 241229824) == 241229824 && (~inst & 1090526208) == 1090526208)) { // pmull   Vd, Vn, Vm
            strcpy(output, "pmull   Vd, Vn, Vm");
            return;
        }
        if (((inst & 773889024) == 773889024 && (~inst & 285237248) == 285237248)) { // pmul    Vd, Vn, Vm
            strcpy(output, "pmul    Vd, Vn, Vm");
            return;
        }
        if (((inst & 3623878656) == 3623878656 && (~inst & 654311424) == 654311424)) { // prfm    PRFOP, ADDR_PCREL19
            strcpy(output, "prfm    PRFOP, ADDR_PCREL19");
            return;
        }
        if (((inst & 4171237376) == 4171237376 && (~inst & 117441536) == 117441536)) { // prfm    PRFOP, ADDR_REGOFF
            strcpy(output, "prfm    PRFOP, ADDR_REGOFF");
            return;
        }
        if (((inst & 3649044480) == 3649044480 && (~inst & 100663296) == 100663296)) { // prfm    PRFOP, ADDR_UIMM12
            strcpy(output, "prfm    PRFOP, ADDR_UIMM12");
            return;
        }
        if (((inst & 4169138176) == 4169138176 && (~inst & 117441536) == 117441536)) { // prfum   PRFOP, ADDR_SIMM9
            strcpy(output, "prfum   PRFOP, ADDR_SIMM9");
            return;
        }
        if (((inst & 1847607296) == 1847607296 && (~inst & 285260800) == 285260800)) { // raddhn2 Vd, Vn, Vm
            strcpy(output, "raddhn2 Vd, Vn, Vm");
            return;
        }
        if (((inst & 773865472) == 773865472 && (~inst & 1359002624) == 1359002624)) { // raddhn  Vd, Vn, Vm
            strcpy(output, "raddhn  Vd, Vn, Vm");
            return;
        }
        if (((inst & 448790528) == 448790528 && (~inst & 85998592) == 85998592)) { // rbit    Rd, Rn
            strcpy(output, "rbit    Rd, Rn");
            return;
        }
        if (((inst & 778065920) == 778065920 && (~inst & 285778944) == 285778944)) { // rbit    Vd, Vn
            strcpy(output, "rbit    Vd, Vn");
            return;
        }
        if (((inst & 1178599424) == 1178599424 && (~inst & 689963008) == 689963008)) { // ret     Rn
            strcpy(output, "ret     Rn");
            return;
        }
        if (((inst & 440402944) == 440402944 && (~inst & 85997568) == 85997568)) { // rev16   Rd, Rn
            strcpy(output, "rev16   Rd, Rn");
            return;
        }
        if (((inst & 236984320) == 236984320 && (~inst & 285271040) == 285271040)) { // rev16   Vd, Vn
            strcpy(output, "rev16   Vd, Vn");
            return;
        }
        if (((inst & 3665823744) == 3665823744 && (~inst & 85992448) == 85992448)) { // rev32   Rd, Rn
            strcpy(output, "rev32   Rd, Rn");
            return;
        }
        if (((inst & 773851136) == 773851136 && (~inst & 285275136) == 285275136)) { // rev32   Vd, Vn
            strcpy(output, "rev32   Vd, Vn");
            return;
        }
        if (((inst & 236980224) == 236980224 && (~inst & 822146048) == 822146048)) { // rev64   Vd, Vn
            strcpy(output, "rev64   Vd, Vn");
            return;
        }
        if (((inst & 1518340096) == 1518340096 && (~inst & 2233476096) == 2233476096)) { // rev     Rd, Rn
            strcpy(output, "rev     Rd, Rn");
            return;
        }
        if (((inst & 1509952512) == 1509952512 && (~inst & 85991424) == 85991424)) { // rev     Rd, Rn
            strcpy(output, "rev     Rd, Rn");
            return;
        }
        if (((inst & 436218880) == 436218880 && (~inst & 85983232) == 85983232)) { // rorv    Rd, Rn, Rm
            strcpy(output, "rorv    Rd, Rn, Rm");
            return;
        }
        if (((inst & 1325435904) == 1325435904 && (~inst & 805310464) == 805310464)) { // rshrn2  Vd, Vn, IMM_VLSR
            strcpy(output, "rshrn2  Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 251694080) == 251694080 && (~inst & 1879052288) == 1879052288)) { // rshrn   Vd, Vn, IMM_VLSR
            strcpy(output, "rshrn   Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1847615488) == 1847615488 && (~inst & 16817152) == 16817152)) { // rsubhn2 Vd, Vn, Vm
            strcpy(output, "rsubhn2 Vd, Vn, Vm");
            return;
        }
        if (((inst & 773873664) == 773873664 && (~inst & 1090558976) == 1090558976)) { // rsubhn  Vd, Vn, Vm
            strcpy(output, "rsubhn  Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310740480) == 1310740480 && (~inst & 822127616) == 822127616)) { // sabal2  Vd, Vn, Vm
            strcpy(output, "sabal2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 236998656) == 236998656 && (~inst & 1895869440) == 1895869440)) { // sabal   Vd, Vn, Vm
            strcpy(output, "sabal   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237009920) == 237009920 && (~inst & 822116352) == 822116352)) { // saba    Vd, Vn, Vm
            strcpy(output, "saba    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310748672) == 1310748672 && (~inst & 822086656) == 822086656)) { // sabdl2  Vd, Vn, Vm
            strcpy(output, "sabdl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 237006848) == 237006848 && (~inst & 1895828480) == 1895828480)) { // sabdl   Vd, Vn, Vm
            strcpy(output, "sabdl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237007872) == 237007872 && (~inst & 822118400) == 822118400)) { // sabd    Vd, Vn, Vm
            strcpy(output, "sabd    Vd, Vn, Vm");
            return;
        }
        if (((inst & 237004800) == 237004800 && (~inst & 822711296) == 822711296)) { // sadalp  Vd, Vn
            strcpy(output, "sadalp  Vd, Vn");
            return;
        }
        if (((inst & 1310720000) == 1310720000 && (~inst & 822148096) == 822148096)) { // saddl2  Vd, Vn, Vm
            strcpy(output, "saddl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 236988416) == 236988416 && (~inst & 822203392) == 822203392)) { // saddlp  Vd, Vn
            strcpy(output, "saddlp  Vd, Vn");
            return;
        }
        if (((inst & 236978176) == 236978176 && (~inst & 1895889920) == 1895889920)) { // saddl   Vd, Vn, Vm
            strcpy(output, "saddl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 238041088) == 238041088 && (~inst & 822199296) == 822199296)) { // saddlv  Fd, Vn
            strcpy(output, "saddlv  Fd, Vn");
            return;
        }
        if (((inst & 1310724096) == 1310724096 && (~inst & 822144000) == 822144000)) { // saddw2  Vd, Vn, Vm
            strcpy(output, "saddw2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 236982272) == 236982272 && (~inst & 1895885824) == 1895885824)) { // saddw   Vd, Vn, Vm
            strcpy(output, "saddw   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1509949440) == 1509949440 && (~inst & 635440128) == 635440128)) { // sbc     Rd, Rn, Rm
            strcpy(output, "sbc     Rd, Rn, Rm");
            return;
        }
        if (((inst & 2046820352) == 2046820352 && (~inst & 98569216) == 98569216)) { // sbcs    Rd, Rn, Rm
            strcpy(output, "sbcs    Rd, Rn, Rm");
            return;
        }
        if (((inst & 50331648) == 50331648 && (~inst & 1820327936) == 1820327936)) { // sbfm    Rd, Rn, IMMR, IMMS
            strcpy(output, "sbfm    Rd, Rn, IMMR, IMMS");
            return;
        }
        if (((inst & 505544704) == 505544704 && (~inst & 17169408) == 17169408)) { // scvtf   Fd, Rn
            strcpy(output, "scvtf   Fd, Rn");
            return;
        }
        if (((inst & 503447552) == 503447552 && (~inst & 1092681728) == 1092681728)) { // scvtf   Fd, Rn, FBITS
            strcpy(output, "scvtf   Fd, Rn, FBITS");
            return;
        }
        if (((inst & 505534464) == 505534464 && (~inst & 562045952) == 562045952)) { // scvtf   Sd, Sn
            strcpy(output, "scvtf   Sd, Sn");
            return;
        }
        if (((inst & 1593869312) == 1593869312 && (~inst & 536877056) == 536877056)) { // scvtf   Sd, Sn, IMM_VLSR
            strcpy(output, "scvtf   Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 237099008) == 237099008 && (~inst & 830481408) == 830481408)) { // scvtf   Vd, Vn
            strcpy(output, "scvtf   Vd, Vn");
            return;
        }
        if (((inst & 436210688) == 436210688 && (~inst & 1159733248) == 1159733248)) { // sdiv    Rd, Rn, Rm
            strcpy(output, "sdiv    Rd, Rn, Rm");
            return;
        }
        if (((inst & 1577058304) == 1577058304 && (~inst & 18904064) == 18904064)) { // sha1c   Fd, Fn, Vm
            strcpy(output, "sha1c   Fd, Fn, Vm");
            return;
        }
        if (((inst & 1579157504) == 1579157504 && (~inst & 16839680) == 16839680)) { // sha1h   Fd, Fn
            strcpy(output, "sha1h   Fd, Fn");
            return;
        }
        if (((inst & 1577066496) == 1577066496 && (~inst & 18895872) == 18895872)) { // sha1m   Fd, Fn, Vm
            strcpy(output, "sha1m   Fd, Fn, Vm");
            return;
        }
        if (((inst & 1577062400) == 1577062400 && (~inst & 18899968) == 18899968)) { // sha1p   Fd, Fn, Vm
            strcpy(output, "sha1p   Fd, Fn, Vm");
            return;
        }
        if (((inst & 1577070592) == 1577070592 && (~inst & 18875392) == 18875392)) { // sha1su0 Vd, Vn, Vm
            strcpy(output, "sha1su0 Vd, Vn, Vm");
            return;
        }
        if (((inst & 1579161600) == 1579161600 && (~inst & 16835584) == 16835584)) { // sha1su1 Vd, Vn
            strcpy(output, "sha1su1 Vd, Vn");
            return;
        }
        if (((inst & 1577078784) == 1577078784 && (~inst & 18883584) == 18883584)) { // sha256h2 Fd, Fn, Vm
            strcpy(output, "sha256h2 Fd, Fn, Vm");
            return;
        }
        if (((inst & 1577074688) == 1577074688 && (~inst & 18887680) == 18887680)) { // sha256h Fd, Fn, Vm
            strcpy(output, "sha256h Fd, Fn, Vm");
            return;
        }
        if (((inst & 1579165696) == 1579165696 && (~inst & 553702400) == 553702400)) { // sha256su0 Vd, Vn
            strcpy(output, "sha256su0 Vd, Vn");
            return;
        }
        if (((inst & 1577082880) == 1577082880 && (~inst & 18879488) == 18879488)) { // sha256su1 Vd, Vn, Vm
            strcpy(output, "sha256su1 Vd, Vn, Vm");
            return;
        }
        if (((inst & 236979200) == 236979200 && (~inst & 822147072) == 822147072)) { // shadd   Vd, Vn, Vm
            strcpy(output, "shadd   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310799872) == 1310799872 && (~inst & 285262848) == 285262848)) { // shll2   Vd, Vn, SHLL_IMM
            strcpy(output, "shll2   Vd, Vn, SHLL_IMM");
            return;
        }
        if (((inst & 237058048) == 237058048 && (~inst & 1359004672) == 1359004672)) { // shll    Vd, Vn, SHLL_IMM
            strcpy(output, "shll    Vd, Vn, SHLL_IMM");
            return;
        }
        if (((inst & 1593857024) == 1593857024 && (~inst & 536911872) == 536911872)) { // shl     Sd, Sn, IMM_VLSL
            strcpy(output, "shl     Sd, Sn, IMM_VLSL");
            return;
        }
        if (((inst & 236987392) == 236987392 && (~inst & 822138880) == 822138880)) { // shsub   Vd, Vn, Vm
            strcpy(output, "shsub   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1056985088) == 1056985088 && (~inst & 40960) == 40960)) { // sli     Sd, Sn, IMM_VLSL
            strcpy(output, "sli     Sd, Sn, IMM_VLSL");
            return;
        }
        if (((inst & 455081984) == 455081984 && (~inst & 75530240) == 75530240)) { // smaddl  Rd, Rn, Rm, Ra
            strcpy(output, "smaddl  Rd, Rn, Rm, Ra");
            return;
        }
        if (((inst & 237020160) == 237020160 && (~inst & 822106112) == 822106112)) { // smaxp   Vd, Vn, Vm
            strcpy(output, "smaxp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237003776) == 237003776 && (~inst & 822122496) == 822122496)) { // smax    Vd, Vn, Vm
            strcpy(output, "smax    Vd, Vn, Vm");
            return;
        }
        if (((inst & 238069760) == 238069760 && (~inst & 553735168) == 553735168)) { // smaxv   Fd, Vn
            strcpy(output, "smaxv   Fd, Vn");
            return;
        }
        if (((inst & 3288334339) == 3288334339 && (~inst & 723517440) == 723517440)) { // smc     EXCEPTION
            strcpy(output, "smc     EXCEPTION");
            return;
        }
        if (((inst & 237022208) == 237022208 && (~inst & 822104064) == 822104064)) { // sminp   Vd, Vn, Vm
            strcpy(output, "sminp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 237005824) == 237005824 && (~inst & 822120448) == 822120448)) { // smin    Vd, Vn, Vm
            strcpy(output, "smin    Vd, Vn, Vm");
            return;
        }
        if (((inst & 238135296) == 238135296 && (~inst & 553669632) == 553669632)) { // sminv   Fd, Vn
            strcpy(output, "sminv   Fd, Vn");
            return;
        }
        if (((inst & 1325408256) == 1325408256 && (~inst & 805360640) == 805360640)) { // smlal2  Vd, Vn, Em
            strcpy(output, "smlal2  Vd, Vn, Em");
            return;
        }
        if (((inst & 1310752768) == 1310752768 && (~inst & 553679872) == 553679872)) { // smlal2  Vd, Vn, Vm
            strcpy(output, "smlal2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 251666432) == 251666432 && (~inst & 1879102464) == 1879102464)) { // smlal   Vd, Vn, Em
            strcpy(output, "smlal   Vd, Vn, Em");
            return;
        }
        if (((inst & 237010944) == 237010944 && (~inst & 1627421696) == 1627421696)) { // smlal   Vd, Vn, Vm
            strcpy(output, "smlal   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1325424640) == 1325424640 && (~inst & 805344256) == 805344256)) { // smlsl2  Vd, Vn, Em
            strcpy(output, "smlsl2  Vd, Vn, Em");
            return;
        }
        if (((inst & 1310760960) == 1310760960 && (~inst & 553671680) == 553671680)) { // smlsl2  Vd, Vn, Vm
            strcpy(output, "smlsl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 251682816) == 251682816 && (~inst & 1879086080) == 1879086080)) { // smlsl   Vd, Vn, Em
            strcpy(output, "smlsl   Vd, Vn, Em");
            return;
        }
        if (((inst & 237019136) == 237019136 && (~inst & 1627413504) == 1627413504)) { // smlsl   Vd, Vn, Vm
            strcpy(output, "smlsl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 234892288) == 234892288 && (~inst & 824184832) == 824184832)) { // smov    Rd, En
            strcpy(output, "smov    Rd, En");
            return;
        }
        if (((inst & 455114752) == 455114752 && (~inst & 75497472) == 75497472)) { // smsubl  Rd, Rn, Rm, Ra
            strcpy(output, "smsubl  Rd, Rn, Rm, Ra");
            return;
        }
        if (((inst & 457179136) == 457179136 && (~inst & 77627392) == 77627392)) { // smulh   Rd, Rn, Rm
            strcpy(output, "smulh   Rd, Rn, Rm");
            return;
        }
        if (((inst & 1325441024) == 1325441024 && (~inst & 805311488) == 805311488)) { // smull2  Vd, Vn, Em
            strcpy(output, "smull2  Vd, Vn, Em");
            return;
        }
        if (((inst & 1310769152) == 1310769152 && (~inst & 822098944) == 822098944)) { // smull2  Vd, Vn, Vm
            strcpy(output, "smull2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 251699200) == 251699200 && (~inst & 1879053312) == 1879053312)) { // smull   Vd, Vn, Em
            strcpy(output, "smull   Vd, Vn, Em");
            return;
        }
        if (((inst & 237027328) == 237027328 && (~inst & 1895840768) == 1895840768)) { // smull   Vd, Vn, Vm
            strcpy(output, "smull   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1579186176) == 1579186176 && (~inst & 553681920) == 553681920)) { // sqabs   Sd, Sn
            strcpy(output, "sqabs   Sd, Sn");
            return;
        }
        if (((inst & 237008896) == 237008896 && (~inst & 822707200) == 822707200)) { // sqabs   Vd, Vn
            strcpy(output, "sqabs   Vd, Vn");
            return;
        }
        if (((inst & 1579158528) == 1579158528 && (~inst & 553709568) == 553709568)) { // sqadd   Sd, Sn, Sm
            strcpy(output, "sqadd   Sd, Sn, Sm");
            return;
        }
        if (((inst & 236981248) == 236981248 && (~inst & 822145024) == 822145024)) { // sqadd   Vd, Vn, Vm
            strcpy(output, "sqadd   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1325412352) == 1325412352 && (~inst & 268485632) == 268485632)) { // sqdmlal2 Vd, Vn, Em
            strcpy(output, "sqdmlal2 Vd, Vn, Em");
            return;
        }
        if (((inst & 1310756864) == 1310756864 && (~inst & 285240320) == 285240320)) { // sqdmlal2 Vd, Vn, Vm
            strcpy(output, "sqdmlal2 Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593843712) == 1593843712 && (~inst & 536921088) == 536921088)) { // sqdmlal Sd, Sn, Em
            strcpy(output, "sqdmlal Sd, Sn, Em");
            return;
        }
        if (((inst & 1579159552) == 1579159552 && (~inst & 16804864) == 16804864)) { // sqdmlal Sd, Sn, Sm
            strcpy(output, "sqdmlal Sd, Sn, Sm");
            return;
        }
        if (((inst & 251670528) == 251670528 && (~inst & 1342227456) == 1342227456)) { // sqdmlal Vd, Vn, Em
            strcpy(output, "sqdmlal Vd, Vn, Em");
            return;
        }
        if (((inst & 237015040) == 237015040 && (~inst & 1358982144) == 1358982144)) { // sqdmlal Vd, Vn, Vm
            strcpy(output, "sqdmlal Vd, Vn, Vm");
            return;
        }
        if (((inst & 1325428736) == 1325428736 && (~inst & 268469248) == 268469248)) { // sqdmlsl2 Vd, Vn, Em
            strcpy(output, "sqdmlsl2 Vd, Vn, Em");
            return;
        }
        if (((inst & 1310765056) == 1310765056 && (~inst & 285232128) == 285232128)) { // sqdmlsl2 Vd, Vn, Vm
            strcpy(output, "sqdmlsl2 Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593860096) == 1593860096 && (~inst & 536904704) == 536904704)) { // sqdmlsl Sd, Sn, Em
            strcpy(output, "sqdmlsl Sd, Sn, Em");
            return;
        }
        if (((inst & 1579167744) == 1579167744 && (~inst & 16780288) == 16780288)) { // sqdmlsl Sd, Sn, Sm
            strcpy(output, "sqdmlsl Sd, Sn, Sm");
            return;
        }
        if (((inst & 251686912) == 251686912 && (~inst & 1342211072) == 1342211072)) { // sqdmlsl Vd, Vn, Em
            strcpy(output, "sqdmlsl Vd, Vn, Em");
            return;
        }
        if (((inst & 237023232) == 237023232 && (~inst & 1358973952) == 1358973952)) { // sqdmlsl Vd, Vn, Vm
            strcpy(output, "sqdmlsl Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593868288) == 1593868288 && (~inst & 536876032) == 536876032)) { // sqdmulh Sd, Sn, Em
            strcpy(output, "sqdmulh Sd, Sn, Em");
            return;
        }
        if (((inst & 1579201536) == 1579201536 && (~inst & 553650176) == 553650176)) { // sqdmulh Sd, Sn, Sm
            strcpy(output, "sqdmulh Sd, Sn, Sm");
            return;
        }
        if (((inst & 251707392) == 251707392 && (~inst & 268448768) == 268448768)) { // sqdmulh Vd, Vn, Em
            strcpy(output, "sqdmulh Vd, Vn, Em");
            return;
        }
        if (((inst & 237024256) == 237024256 && (~inst & 822102016) == 822102016)) { // sqdmulh Vd, Vn, Vm
            strcpy(output, "sqdmulh Vd, Vn, Vm");
            return;
        }
        if (((inst & 1325445120) == 1325445120 && (~inst & 268436480) == 268436480)) { // sqdmull2 Vd, Vn, Em
            strcpy(output, "sqdmull2 Vd, Vn, Em");
            return;
        }
        if (((inst & 1310773248) == 1310773248 && (~inst & 285223936) == 285223936)) { // sqdmull2 Vd, Vn, Vm
            strcpy(output, "sqdmull2 Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593880576) == 1593880576 && (~inst & 536871936) == 536871936)) { // sqdmull Sd, Sn, Em
            strcpy(output, "sqdmull Sd, Sn, Em");
            return;
        }
        if (((inst & 1579175936) == 1579175936 && (~inst & 16788480) == 16788480)) { // sqdmull Sd, Sn, Sm
            strcpy(output, "sqdmull Sd, Sn, Sm");
            return;
        }
        if (((inst & 251703296) == 251703296 && (~inst & 1342178304) == 1342178304)) { // sqdmull Vd, Vn, Em
            strcpy(output, "sqdmull Vd, Vn, Em");
            return;
        }
        if (((inst & 237031424) == 237031424 && (~inst & 1358965760) == 1358965760)) { // sqdmull Vd, Vn, Vm
            strcpy(output, "sqdmull Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042315264) == 1042315264 && (~inst & 16811008) == 16811008)) { // sqneg   Sd, Sn
            strcpy(output, "sqneg   Sd, Sn");
            return;
        }
        if (((inst & 773879808) == 773879808 && (~inst & 285836288) == 285836288)) { // sqneg   Vd, Vn
            strcpy(output, "sqneg   Vd, Vn");
            return;
        }
        if (((inst & 1593888768) == 1593888768 && (~inst & 536880128) == 536880128)) { // sqrdmulh Sd, Sn, Em
            strcpy(output, "sqrdmulh Sd, Sn, Em");
            return;
        }
        if (((inst & 1042330624) == 1042330624 && (~inst & 16779264) == 16779264)) { // sqrdmulh Sd, Sn, Sm
            strcpy(output, "sqrdmulh Sd, Sn, Sm");
            return;
        }
        if (((inst & 251711488) == 251711488 && (~inst & 268444672) == 268444672)) { // sqrdmulh Vd, Vn, Em
            strcpy(output, "sqrdmulh Vd, Vn, Em");
            return;
        }
        if (((inst & 773895168) == 773895168 && (~inst & 285231104) == 285231104)) { // sqrdmulh Vd, Vn, Vm
            strcpy(output, "sqrdmulh Vd, Vn, Vm");
            return;
        }
        if (((inst & 1579162624) == 1579162624 && (~inst & 553689088) == 553689088)) { // sqrshl  Sd, Sn, Sm
            strcpy(output, "sqrshl  Sd, Sn, Sm");
            return;
        }
        if (((inst & 237001728) == 237001728 && (~inst & 822124544) == 822124544)) { // sqrshl  Vd, Vn, Vm
            strcpy(output, "sqrshl  Vd, Vn, Vm");
            return;
        }
        if (((inst & 1325440000) == 1325440000 && (~inst & 805314560) == 805314560)) { // sqrshrn2 Vd, Vn, IMM_VLSR
            strcpy(output, "sqrshrn2 Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1593871360) == 1593871360 && (~inst & 536879104) == 536879104)) { // sqrshrn Sd, Sn, IMM_VLSR
            strcpy(output, "sqrshrn Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 251698176) == 251698176 && (~inst & 1879056384) == 1879056384)) { // sqrshrn Vd, Vn, IMM_VLSR
            strcpy(output, "sqrshrn Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1862306816) == 1862306816 && (~inst & 268439552) == 268439552)) { // sqrshrun2 Vd, Vn, IMM_VLSR
            strcpy(output, "sqrshrun2 Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1057000448) == 1057000448 && (~inst & 4096) == 4096)) { // sqrshrun Sd, Sn, IMM_VLSR
            strcpy(output, "sqrshrun Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 788564992) == 788564992 && (~inst & 1342181376) == 1342181376)) { // sqrshrun Vd, Vn, IMM_VLSR
            strcpy(output, "sqrshrun Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1593865216) == 1593865216 && (~inst & 536903680) == 536903680)) { // sqshl   Sd, Sn, IMM_VLSL
            strcpy(output, "sqshl   Sd, Sn, IMM_VLSL");
            return;
        }
        if (((inst & 1579174912) == 1579174912 && (~inst & 553660416) == 553660416)) { // sqshl   Sd, Sn, Sm
            strcpy(output, "sqshl   Sd, Sn, Sm");
            return;
        }
        if (((inst & 1056989184) == 1056989184 && (~inst & 36864) == 36864)) { // sqshlu  Sd, Sn, IMM_VLSL
            strcpy(output, "sqshlu  Sd, Sn, IMM_VLSL");
            return;
        }
        if (((inst & 236997632) == 236997632 && (~inst & 822128640) == 822128640)) { // sqshl   Vd, Vn, Vm
            strcpy(output, "sqshl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593873408) == 1593873408 && (~inst & 536872960) == 536872960)) { // sqshrn  Sd, Sn, IMM_VLSR
            strcpy(output, "sqshrn  Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1056998400) == 1056998400 && (~inst & 14336) == 14336)) { // sqshrun Sd, Sn, IMM_VLSR
            strcpy(output, "sqshrun Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1579166720) == 1579166720 && (~inst & 553652224) == 553652224)) { // sqsub   Sd, Sn, Sm
            strcpy(output, "sqsub   Sd, Sn, Sm");
            return;
        }
        if (((inst & 236989440) == 236989440 && (~inst & 822136832) == 822136832)) { // sqsub   Vd, Vn, Vm
            strcpy(output, "sqsub   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310803968) == 1310803968 && (~inst & 822129664) == 822129664)) { // sqxtn2  Vd, Vn
            strcpy(output, "sqxtn2  Vd, Vn");
            return;
        }
        if (((inst & 1579173888) == 1579173888 && (~inst & 553694208) == 553694208)) { // sqxtn   Sd, Sn
            strcpy(output, "sqxtn   Sd, Sn");
            return;
        }
        if (((inst & 237062144) == 237062144 && (~inst & 1895871488) == 1895871488)) { // sqxtn   Vd, Vn
            strcpy(output, "sqxtn   Vd, Vn");
            return;
        }
        if (((inst & 1847666688) == 1847666688 && (~inst & 285266944) == 285266944)) { // sqxtun2 Vd, Vn
            strcpy(output, "sqxtun2 Vd, Vn");
            return;
        }
        if (((inst & 1042294784) == 1042294784 && (~inst & 16831488) == 16831488)) { // sqxtun  Sd, Sn
            strcpy(output, "sqxtun  Sd, Sn");
            return;
        }
        if (((inst & 773924864) == 773924864 && (~inst & 1359008768) == 1359008768)) { // sqxtun  Vd, Vn
            strcpy(output, "sqxtun  Vd, Vn");
            return;
        }
        if (((inst & 236983296) == 236983296 && (~inst & 822142976) == 822142976)) { // srhadd  Vd, Vn, Vm
            strcpy(output, "srhadd  Vd, Vn, Vm");
            return;
        }
        if (((inst & 1056980992) == 1056980992 && (~inst & 45056) == 45056)) { // sri     Sd, Sn, IMM_VLSR
            strcpy(output, "sri     Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1579160576) == 1579160576 && (~inst & 553658368) == 553658368)) { // srshl   Sd, Sn, Sm
            strcpy(output, "srshl   Sd, Sn, Sm");
            return;
        }
        if (((inst & 236999680) == 236999680 && (~inst & 822126592) == 822126592)) { // srshl   Vd, Vn, Vm
            strcpy(output, "srshl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593844736) == 1593844736 && (~inst & 536907776) == 536907776)) { // srshr   Sd, Sn, IMM_VLSR
            strcpy(output, "srshr   Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1593848832) == 1593848832 && (~inst & 536920064) == 536920064)) { // srsra   Sd, Sn, IMM_VLSR
            strcpy(output, "srsra   Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1579172864) == 1579172864 && (~inst & 553662464) == 553662464)) { // sshl    Sd, Sn, Sm
            strcpy(output, "sshl    Sd, Sn, Sm");
            return;
        }
        if (((inst & 236995584) == 236995584 && (~inst & 822130688) == 822130688)) { // sshl    Vd, Vn, Vm
            strcpy(output, "sshl    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1593836544) == 1593836544 && (~inst & 536915968) == 536915968)) { // sshr    Sd, Sn, IMM_VLSR
            strcpy(output, "sshr    Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1593840640) == 1593840640 && (~inst & 536928256) == 536928256)) { // ssra    Sd, Sn, IMM_VLSR
            strcpy(output, "ssra    Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1310728192) == 1310728192 && (~inst & 822139904) == 822139904)) { // ssubl2  Vd, Vn, Vm
            strcpy(output, "ssubl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 236986368) == 236986368 && (~inst & 1895881728) == 1895881728)) { // ssubl   Vd, Vn, Vm
            strcpy(output, "ssubl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310732288) == 1310732288 && (~inst & 822135808) == 822135808)) { // ssubw2  Vd, Vn, Vm
            strcpy(output, "ssubw2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 236990464) == 236990464 && (~inst & 1895877632) == 1895877632)) { // ssubw   Vd, Vn, Vm
            strcpy(output, "ssubw   Vd, Vn, Vm");
            return;
        }
        if (((inst & 226492416) == 226492416 && (~inst & 845160448) == 845160448)) { // st1     LEt, SIMD_ADDR_POST
            strcpy(output, "st1     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 218103808) == 218103808 && (~inst & 853549056) == 853549056)) { // st1     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "st1     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 211812352) == 211812352 && (~inst & 843063296) == 843063296)) { // st2     LEt, SIMD_ADDR_POST
            strcpy(output, "st2     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 220200960) == 220200960 && (~inst & 851451904) == 851451904)) { // st2     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "st2     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 226500608) == 226500608 && (~inst & 845152256) == 845152256)) { // st3     LEt, SIMD_ADDR_POST
            strcpy(output, "st3     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 218112000) == 218112000 && (~inst & 853540864) == 853540864)) { // st3     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "st3     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 211820544) == 211820544 && (~inst & 843055104) == 843055104)) { // st4     LEt, SIMD_ADDR_POST
            strcpy(output, "st4     LEt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 220209152) == 220209152 && (~inst & 851443712) == 851443712)) { // st4     LEt, SIMD_ADDR_SIMPLE
            strcpy(output, "st4     LEt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 209715200) == 209715200 && (~inst & 861929472) == 861929472)) { // st4     LVt, SIMD_ADDR_POST
            strcpy(output, "st4     LVt, SIMD_ADDR_POST");
            return;
        }
        if (((inst & 201326592) == 201326592 && (~inst & 868220928) == 868220928)) { // st4     LVt, SIMD_ADDR_SIMPLE
            strcpy(output, "st4     LVt, SIMD_ADDR_SIMPLE");
            return;
        }
        if (((inst & 142606336) == 142606336 && (~inst & 4131389440) == 4131389440)) { // stlrb   Rt, ADDR_SIMPLE
            strcpy(output, "stlrb   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1216348160) == 1216348160 && (~inst & 3057647616) == 3057647616)) { // stlrh   Rt, ADDR_SIMPLE
            strcpy(output, "stlrh   Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 2290089984) == 2290089984 && (~inst & 910163968) == 910163968)) { // stlr    Rt, ADDR_SIMPLE
            strcpy(output, "stlr    Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 136347648) == 136347648 && (~inst & 918552576) == 918552576)) { // stlxp   Rs, Rt, Rt2, ADDR_SIMPLE
            strcpy(output, "stlxp   Rs, Rt, Rt2, ADDR_SIMPLE");
            return;
        }
        if (((inst & 134250496) == 134250496 && (~inst & 4141875200) == 4141875200)) { // stlxrb  Rs, Rt, ADDR_SIMPLE
            strcpy(output, "stlxrb  Rs, Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1207992320) == 1207992320 && (~inst & 3068133376) == 3068133376)) { // stlxrh  Rs, Rt, ADDR_SIMPLE
            strcpy(output, "stlxrh  Rs, Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 2281734144) == 2281734144 && (~inst & 920649728) == 920649728)) { // stlxr   Rs, Rt, ADDR_SIMPLE
            strcpy(output, "stlxr   Rs, Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 738197504) == 738197504 && (~inst & 314572800) == 314572800)) { // stnp    Ft, Ft2, ADDR_SIMM7
            strcpy(output, "stnp    Ft, Ft2, ADDR_SIMM7");
            return;
        }
        if (((inst & 671088640) == 671088640 && (~inst & 381681664) == 381681664)) { // stnp    Rt, Rt2, ADDR_SIMM7
            strcpy(output, "stnp    Rt, Rt2, ADDR_SIMM7");
            return;
        }
        if (((inst & 738197504) == 738197504 && (~inst & 314572800) == 314572800)) { // stp     Ft, Ft2, ADDR_SIMM7
            strcpy(output, "stp     Ft, Ft2, ADDR_SIMM7");
            return;
        }
        if (((inst & 746586112) == 746586112 && (~inst & 306184192) == 306184192)) { // stp     Ft, Ft2, ADDR_SIMM7
            strcpy(output, "stp     Ft, Ft2, ADDR_SIMM7");
            return;
        }
        if (((inst & 679477248) == 679477248 && (~inst & 373293056) == 373293056)) { // stp     Rt, Rt2, ADDR_SIMM7
            strcpy(output, "stp     Rt, Rt2, ADDR_SIMM7");
            return;
        }
        if (((inst & 941623296) == 941623296 && (~inst & 3351249920) == 3351249920)) { // strb    Rt, ADDR_REGOFF
            strcpy(output, "strb    Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 939525120) == 939525120 && (~inst & 3351248896) == 3351248896)) { // strb    Rt, ADDR_SIMM9
            strcpy(output, "strb    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 419430400) == 419430400 && (~inst & 3334471680) == 3334471680)) { // strb    Rt, ADDR_UIMM12
            strcpy(output, "strb    Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 1006635008) == 1006635008 && (~inst & 54526976) == 54526976)) { // str     Ft, ADDR_REGOFF
            strcpy(output, "str     Ft, ADDR_REGOFF");
            return;
        }
        if (((inst & 1006633984) == 1006633984 && (~inst & 54525952) == 54525952)) { // str     Ft, ADDR_SIMM9
            strcpy(output, "str     Ft, ADDR_SIMM9");
            return;
        }
        if (((inst & 486539264) == 486539264 && (~inst & 37748736) == 37748736)) { // str     Ft, ADDR_UIMM12
            strcpy(output, "str     Ft, ADDR_UIMM12");
            return;
        }
        if (((inst & 2015365120) == 2015365120 && (~inst & 2277508096) == 2277508096)) { // strh    Rt, ADDR_REGOFF
            strcpy(output, "strh    Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 2013266944) == 2013266944 && (~inst & 2277507072) == 2277507072)) { // strh    Rt, ADDR_SIMM9
            strcpy(output, "strh    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 1493172224) == 1493172224 && (~inst & 2260729856) == 2260729856)) { // strh    Rt, ADDR_UIMM12
            strcpy(output, "strh    Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 3089106944) == 3089106944 && (~inst & 130024448) == 130024448)) { // str     Rt, ADDR_REGOFF
            strcpy(output, "str     Rt, ADDR_REGOFF");
            return;
        }
        if (((inst & 3087008768) == 3087008768 && (~inst & 130023424) == 130023424)) { // str     Rt, ADDR_SIMM9
            strcpy(output, "str     Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2566914048) == 2566914048 && (~inst & 113246208) == 113246208)) { // str     Rt, ADDR_UIMM12
            strcpy(output, "str     Rt, ADDR_UIMM12");
            return;
        }
        if (((inst & 939524096) == 939524096 && (~inst & 3353347072) == 3353347072)) { // sttrb   Rt, ADDR_SIMM9
            strcpy(output, "sttrb   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 2013265920) == 2013265920 && (~inst & 2279605248) == 2279605248)) { // sttrh   Rt, ADDR_SIMM9
            strcpy(output, "sttrh   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 3087007744) == 3087007744 && (~inst & 132121600) == 132121600)) { // sttr    Rt, ADDR_SIMM9
            strcpy(output, "sttr    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 939524096) == 939524096 && (~inst & 3351249920) == 3351249920)) { // sturb   Rt, ADDR_SIMM9
            strcpy(output, "sturb   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 1006632960) == 1006632960 && (~inst & 54526976) == 54526976)) { // stur    Ft, ADDR_SIMM9
            strcpy(output, "stur    Ft, ADDR_SIMM9");
            return;
        }
        if (((inst & 2013265920) == 2013265920 && (~inst & 2277508096) == 2277508096)) { // sturh   Rt, ADDR_SIMM9
            strcpy(output, "sturh   Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 3087007744) == 3087007744 && (~inst & 130024448) == 130024448)) { // stur    Rt, ADDR_SIMM9
            strcpy(output, "stur    Rt, ADDR_SIMM9");
            return;
        }
        if (((inst & 136314880) == 136314880 && (~inst & 918585344) == 918585344)) { // stxp    Rs, Rt, Rt2, ADDR_SIMPLE
            strcpy(output, "stxp    Rs, Rt, Rt2, ADDR_SIMPLE");
            return;
        }
        if (((inst & 134217728) == 134217728 && (~inst & 4141907968) == 4141907968)) { // stxrb   Rs, Rt, ADDR_SIMPLE
            strcpy(output, "stxrb   Rs, Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1207959552) == 1207959552 && (~inst & 3068166144) == 3068166144)) { // stxrh   Rs, Rt, ADDR_SIMPLE
            strcpy(output, "stxrh   Rs, Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 2281701376) == 2281701376 && (~inst & 920682496) == 920682496)) { // stxr    Rs, Rt, ADDR_SIMPLE
            strcpy(output, "stxr    Rs, Rt, ADDR_SIMPLE");
            return;
        }
        if (((inst & 1310744576) == 1310744576 && (~inst & 553688064) == 553688064)) { // subhn2  Vd, Vn, Vm
            strcpy(output, "subhn2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 237002752) == 237002752 && (~inst & 1627429888) == 1627429888)) { // subhn   Vd, Vn, Vm
            strcpy(output, "subhn   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1258291200) == 1258291200 && (~inst & 874512384) == 874512384)) { // sub     Rd, Rn, Rm_SFT
            strcpy(output, "sub     Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 1090519040) == 1090519040 && (~inst & 771751936) == 771751936)) { // sub     Rd_SP, Rn_SP, AIMM
            strcpy(output, "sub     Rd_SP, Rn_SP, AIMM");
            return;
        }
        if (((inst & 1260388352) == 1260388352 && (~inst & 880803840) == 880803840)) { // sub     Rd_SP, Rn_SP, Rm_EXT
            strcpy(output, "sub     Rd_SP, Rn_SP, Rm_EXT");
            return;
        }
        if (((inst & 1042285568) == 1042285568 && (~inst & 16807936) == 16807936)) { // sub     Sd, Sn, Sm
            strcpy(output, "sub     Sd, Sn, Sm");
            return;
        }
        if (((inst & 1795162112) == 1795162112 && (~inst & 337641472) == 337641472)) { // subs    Rd, Rn, Rm_SFT
            strcpy(output, "subs    Rd, Rn, Rm_SFT");
            return;
        }
        if (((inst & 1627389952) == 1627389952 && (~inst & 234881024) == 234881024)) { // subs    Rd, Rn_SP, AIMM
            strcpy(output, "subs    Rd, Rn_SP, AIMM");
            return;
        }
        if (((inst & 1797259264) == 1797259264 && (~inst & 343932928) == 343932928)) { // subs    Rd, Rn_SP, Rm_EXT
            strcpy(output, "subs    Rd, Rn_SP, Rm_EXT");
            return;
        }
        if (((inst & 773882880) == 773882880 && (~inst & 285243392) == 285243392)) { // sub     Vd, Vn, Vm
            strcpy(output, "sub     Vd, Vn, Vm");
            return;
        }
        if (((inst & 1579169792) == 1579169792 && (~inst & 553698304) == 553698304)) { // suqadd  Sd, Sn
            strcpy(output, "suqadd  Sd, Sn");
            return;
        }
        if (((inst & 236992512) == 236992512 && (~inst & 823247872) == 823247872)) { // suqadd  Vd, Vn
            strcpy(output, "suqadd  Vd, Vn");
            return;
        }
        if (((inst & 3288334337) == 3288334337 && (~inst & 723517442) == 723517442)) { // svc     EXCEPTION
            strcpy(output, "svc     EXCEPTION");
            return;
        }
        if (((inst & 1160249344) == 1160249344 && (~inst & 672137216) == 672137216)) { // sysl    Rt, UIMM3_OP1, Cn, Cm, UIMM3_OP2
            strcpy(output, "sysl    Rt, UIMM3_OP1, Cn, Cm, UIMM3_OP2");
            return;
        }
        if (((inst & 1158152192) == 1158152192 && (~inst & 674234368) == 674234368)) { // sys     UIMM3_OP1, Cn, Cm, UIMM3_OP2, Rt
            strcpy(output, "sys     UIMM3_OP1, Cn, Cm, UIMM3_OP2, Rt");
            return;
        }
        if (((inst & 234881024) == 234881024 && (~inst & 824187904) == 824187904)) { // tbl     Vd, LVn, Vm
            strcpy(output, "tbl     Vd, LVn, Vm");
            return;
        }
        if (((inst & 654311424) == 654311424 && (~inst & 134217728) == 134217728)) { // tbnz    Rt, BIT_NUM, ADDR_PCREL14
            strcpy(output, "tbnz    Rt, BIT_NUM, ADDR_PCREL14");
            return;
        }
        if (((inst & 234885120) == 234885120 && (~inst & 824183808) == 824183808)) { // tbx     Vd, LVn, Vm
            strcpy(output, "tbx     Vd, LVn, Vm");
            return;
        }
        if (((inst & 637534208) == 637534208 && (~inst & 150994944) == 150994944)) { // tbz     Rt, BIT_NUM, ADDR_PCREL14
            strcpy(output, "tbz     Rt, BIT_NUM, ADDR_PCREL14");
            return;
        }
        if (((inst & 234883072) == 234883072 && (~inst & 824202240) == 824202240)) { // trn1    Vd, Vn, Vm
            strcpy(output, "trn1    Vd, Vn, Vm");
            return;
        }
        if (((inst & 234899456) == 234899456 && (~inst & 824185856) == 824185856)) { // trn2    Vd, Vn, Vm
            strcpy(output, "trn2    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1847611392) == 1847611392 && (~inst & 285256704) == 285256704)) { // uabal2  Vd, Vn, Vm
            strcpy(output, "uabal2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 773869568) == 773869568 && (~inst & 1358998528) == 1358998528)) { // uabal   Vd, Vn, Vm
            strcpy(output, "uabal   Vd, Vn, Vm");
            return;
        }
        if (((inst & 773880832) == 773880832 && (~inst & 285245440) == 285245440)) { // uaba    Vd, Vn, Vm
            strcpy(output, "uaba    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1847619584) == 1847619584 && (~inst & 285215744) == 285215744)) { // uabdl2  Vd, Vn, Vm
            strcpy(output, "uabdl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 773877760) == 773877760 && (~inst & 1358957568) == 1358957568)) { // uabdl   Vd, Vn, Vm
            strcpy(output, "uabdl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 773878784) == 773878784 && (~inst & 285247488) == 285247488)) { // uabd    Vd, Vn, Vm
            strcpy(output, "uabd    Vd, Vn, Vm");
            return;
        }
        if (((inst & 773875712) == 773875712 && (~inst & 285840384) == 285840384)) { // uadalp  Vd, Vn
            strcpy(output, "uadalp  Vd, Vn");
            return;
        }
        if (((inst & 1847590912) == 1847590912 && (~inst & 285277184) == 285277184)) { // uaddl2  Vd, Vn, Vm
            strcpy(output, "uaddl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 773859328) == 773859328 && (~inst & 285332480) == 285332480)) { // uaddlp  Vd, Vn
            strcpy(output, "uaddlp  Vd, Vn");
            return;
        }
        if (((inst & 773849088) == 773849088 && (~inst & 1359019008) == 1359019008)) { // uaddl   Vd, Vn, Vm
            strcpy(output, "uaddl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 774912000) == 774912000 && (~inst & 285328384) == 285328384)) { // uaddlv  Fd, Vn
            strcpy(output, "uaddlv  Fd, Vn");
            return;
        }
        if (((inst & 1847595008) == 1847595008 && (~inst & 285273088) == 285273088)) { // uaddw2  Vd, Vn, Vm
            strcpy(output, "uaddw2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 773853184) == 773853184 && (~inst & 1359014912) == 1359014912)) { // uaddw   Vd, Vn, Vm
            strcpy(output, "uaddw   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1124073472) == 1124073472 && (~inst & 746586112) == 746586112)) { // ubfm    Rd, Rn, IMMR, IMMS
            strcpy(output, "ubfm    Rd, Rn, IMMR, IMMS");
            return;
        }
        if (((inst & 505610240) == 505610240 && (~inst & 17103872) == 17103872)) { // ucvtf   Fd, Rn
            strcpy(output, "ucvtf   Fd, Rn");
            return;
        }
        if (((inst & 503513088) == 503513088 && (~inst & 1092616192) == 1092616192)) { // ucvtf   Fd, Rn, FBITS
            strcpy(output, "ucvtf   Fd, Rn, FBITS");
            return;
        }
        if (((inst & 1042405376) == 1042405376 && (~inst & 25175040) == 25175040)) { // ucvtf   Sd, Sn
            strcpy(output, "ucvtf   Sd, Sn");
            return;
        }
        if (((inst & 1057006592) == 1057006592 && (~inst & 6144) == 6144)) { // ucvtf   Sd, Sn, IMM_VLSR
            strcpy(output, "ucvtf   Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 773969920) == 773969920 && (~inst & 293610496) == 293610496)) { // ucvtf   Vd, Vn
            strcpy(output, "ucvtf   Vd, Vn");
            return;
        }
        if (((inst & 444598272) == 444598272 && (~inst & 1159734272) == 1159734272)) { // udiv    Rd, Rn, Rm
            strcpy(output, "udiv    Rd, Rn, Rm");
            return;
        }
        if (((inst & 773850112) == 773850112 && (~inst & 285276160) == 285276160)) { // uhadd   Vd, Vn, Vm
            strcpy(output, "uhadd   Vd, Vn, Vm");
            return;
        }
        if (((inst & 773858304) == 773858304 && (~inst & 285267968) == 285267968)) { // uhsub   Vd, Vn, Vm
            strcpy(output, "uhsub   Vd, Vn, Vm");
            return;
        }
        if (((inst & 195035136) == 195035136 && (~inst & 67141632) == 67141632)) { // umaddl  Rd, Rn, Rm, Ra
            strcpy(output, "umaddl  Rd, Rn, Rm, Ra");
            return;
        }
        if (((inst & 773891072) == 773891072 && (~inst & 285235200) == 285235200)) { // umaxp   Vd, Vn, Vm
            strcpy(output, "umaxp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 773874688) == 773874688 && (~inst & 285251584) == 285251584)) { // umax    Vd, Vn, Vm
            strcpy(output, "umax    Vd, Vn, Vm");
            return;
        }
        if (((inst & 774940672) == 774940672 && (~inst & 16864256) == 16864256)) { // umaxv   Fd, Vn
            strcpy(output, "umaxv   Fd, Vn");
            return;
        }
        if (((inst & 773893120) == 773893120 && (~inst & 285233152) == 285233152)) { // uminp   Vd, Vn, Vm
            strcpy(output, "uminp   Vd, Vn, Vm");
            return;
        }
        if (((inst & 773876736) == 773876736 && (~inst & 285249536) == 285249536)) { // umin    Vd, Vn, Vm
            strcpy(output, "umin    Vd, Vn, Vm");
            return;
        }
        if (((inst & 775006208) == 775006208 && (~inst & 16798720) == 16798720)) { // uminv   Fd, Vn
            strcpy(output, "uminv   Fd, Vn");
            return;
        }
        if (((inst & 1862279168) == 1862279168 && (~inst & 268489728) == 268489728)) { // umlal2  Vd, Vn, Em
            strcpy(output, "umlal2  Vd, Vn, Em");
            return;
        }
        if (((inst & 1847623680) == 1847623680 && (~inst & 16808960) == 16808960)) { // umlal2  Vd, Vn, Vm
            strcpy(output, "umlal2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 788537344) == 788537344 && (~inst & 1342231552) == 1342231552)) { // umlal   Vd, Vn, Em
            strcpy(output, "umlal   Vd, Vn, Em");
            return;
        }
        if (((inst & 773881856) == 773881856 && (~inst & 1090550784) == 1090550784)) { // umlal   Vd, Vn, Vm
            strcpy(output, "umlal   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1862295552) == 1862295552 && (~inst & 268473344) == 268473344)) { // umlsl2  Vd, Vn, Em
            strcpy(output, "umlsl2  Vd, Vn, Em");
            return;
        }
        if (((inst & 1847631872) == 1847631872 && (~inst & 16800768) == 16800768)) { // umlsl2  Vd, Vn, Vm
            strcpy(output, "umlsl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 788553728) == 788553728 && (~inst & 1342215168) == 1342215168)) { // umlsl   Vd, Vn, Em
            strcpy(output, "umlsl   Vd, Vn, Em");
            return;
        }
        if (((inst & 773890048) == 773890048 && (~inst & 1090542592) == 1090542592)) { // umlsl   Vd, Vn, Vm
            strcpy(output, "umlsl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 234896384) == 234896384 && (~inst & 824180736) == 824180736)) { // umov    Rd, En
            strcpy(output, "umov    Rd, En");
            return;
        }
        if (((inst & 195067904) == 195067904 && (~inst & 67108864) == 67108864)) { // umsubl  Rd, Rn, Rm, Ra
            strcpy(output, "umsubl  Rd, Rn, Rm, Ra");
            return;
        }
        if (((inst & 465567744) == 465567744 && (~inst & 69238784) == 69238784)) { // umulh   Rd, Rn, Rm
            strcpy(output, "umulh   Rd, Rn, Rm");
            return;
        }
        if (((inst & 1862311936) == 1862311936 && (~inst & 268440576) == 268440576)) { // umull2  Vd, Vn, Em
            strcpy(output, "umull2  Vd, Vn, Em");
            return;
        }
        if (((inst & 1847640064) == 1847640064 && (~inst & 285228032) == 285228032)) { // umull2  Vd, Vn, Vm
            strcpy(output, "umull2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 788570112) == 788570112 && (~inst & 1342182400) == 1342182400)) { // umull   Vd, Vn, Em
            strcpy(output, "umull   Vd, Vn, Em");
            return;
        }
        if (((inst & 773898240) == 773898240 && (~inst & 1358969856) == 1358969856)) { // umull   Vd, Vn, Vm
            strcpy(output, "umull   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042287616) == 1042287616 && (~inst & 16838656) == 16838656)) { // uqadd   Sd, Sn, Sm
            strcpy(output, "uqadd   Sd, Sn, Sm");
            return;
        }
        if (((inst & 773852160) == 773852160 && (~inst & 285274112) == 285274112)) { // uqadd   Vd, Vn, Vm
            strcpy(output, "uqadd   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042291712) == 1042291712 && (~inst & 16785408) == 16785408)) { // uqrshl  Sd, Sn, Sm
            strcpy(output, "uqrshl  Sd, Sn, Sm");
            return;
        }
        if (((inst & 773872640) == 773872640 && (~inst & 285253632) == 285253632)) { // uqrshl  Vd, Vn, Vm
            strcpy(output, "uqrshl  Vd, Vn, Vm");
            return;
        }
        if (((inst & 1862310912) == 1862310912 && (~inst & 268443648) == 268443648)) { // uqrshrn2 Vd, Vn, IMM_VLSR
            strcpy(output, "uqrshrn2 Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1057004544) == 1057004544 && (~inst & 8192) == 8192)) { // uqrshrn Sd, Sn, IMM_VLSR
            strcpy(output, "uqrshrn Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 788569088) == 788569088 && (~inst & 1342185472) == 1342185472)) { // uqrshrn Vd, Vn, IMM_VLSR
            strcpy(output, "uqrshrn Vd, Vn, IMM_VLSR");
            return;
        }
        if (((inst & 1056993280) == 1056993280 && (~inst & 32768) == 32768)) { // uqshl   Sd, Sn, IMM_VLSL
            strcpy(output, "uqshl   Sd, Sn, IMM_VLSL");
            return;
        }
        if (((inst & 1042304000) == 1042304000 && (~inst & 16789504) == 16789504)) { // uqshl   Sd, Sn, Sm
            strcpy(output, "uqshl   Sd, Sn, Sm");
            return;
        }
        if (((inst & 773868544) == 773868544 && (~inst & 285257728) == 285257728)) { // uqshl   Vd, Vn, Vm
            strcpy(output, "uqshl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1057002496) == 1057002496 && (~inst & 2048) == 2048)) { // uqshrn  Sd, Sn, IMM_VLSR
            strcpy(output, "uqshrn  Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1042295808) == 1042295808 && (~inst & 16797696) == 16797696)) { // uqsub   Sd, Sn, Sm
            strcpy(output, "uqsub   Sd, Sn, Sm");
            return;
        }
        if (((inst & 773860352) == 773860352 && (~inst & 285265920) == 285265920)) { // uqsub   Vd, Vn, Vm
            strcpy(output, "uqsub   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1847674880) == 1847674880 && (~inst & 285258752) == 285258752)) { // uqxtn2  Vd, Vn
            strcpy(output, "uqxtn2  Vd, Vn");
            return;
        }
        if (((inst & 1042302976) == 1042302976 && (~inst & 16823296) == 16823296)) { // uqxtn   Sd, Sn
            strcpy(output, "uqxtn   Sd, Sn");
            return;
        }
        if (((inst & 773933056) == 773933056 && (~inst & 1359000576) == 1359000576)) { // uqxtn   Vd, Vn
            strcpy(output, "uqxtn   Vd, Vn");
            return;
        }
        if (((inst & 245483520) == 245483520 && (~inst & 553661440) == 553661440)) { // urecpe  Vd, Vn
            strcpy(output, "urecpe  Vd, Vn");
            return;
        }
        if (((inst & 773854208) == 773854208 && (~inst & 285272064) == 285272064)) { // urhadd  Vd, Vn, Vm
            strcpy(output, "urhadd  Vd, Vn, Vm");
            return;
        }
        if (((inst & 1042289664) == 1042289664 && (~inst & 16820224) == 16820224)) { // urshl   Sd, Sn, Sm
            strcpy(output, "urshl   Sd, Sn, Sm");
            return;
        }
        if (((inst & 773870592) == 773870592 && (~inst & 285255680) == 285255680)) { // urshl   Vd, Vn, Vm
            strcpy(output, "urshl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1056972800) == 1056972800 && (~inst & 53248) == 53248)) { // urshr   Sd, Sn, IMM_VLSR
            strcpy(output, "urshr   Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 782354432) == 782354432 && (~inst & 16790528) == 16790528)) { // ursqrte Vd, Vn
            strcpy(output, "ursqrte Vd, Vn");
            return;
        }
        if (((inst & 1056976896) == 1056976896 && (~inst & 49152) == 49152)) { // ursra   Sd, Sn, IMM_VLSR
            strcpy(output, "ursra   Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1042301952) == 1042301952 && (~inst & 16791552) == 16791552)) { // ushl    Sd, Sn, Sm
            strcpy(output, "ushl    Sd, Sn, Sm");
            return;
        }
        if (((inst & 773866496) == 773866496 && (~inst & 285259776) == 285259776)) { // ushl    Vd, Vn, Vm
            strcpy(output, "ushl    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1056964608) == 1056964608 && (~inst & 61440) == 61440)) { // ushr    Sd, Sn, IMM_VLSR
            strcpy(output, "ushr    Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1042298880) == 1042298880 && (~inst & 16827392) == 16827392)) { // usqadd  Sd, Sn
            strcpy(output, "usqadd  Sd, Sn");
            return;
        }
        if (((inst & 773863424) == 773863424 && (~inst & 286376960) == 286376960)) { // usqadd  Vd, Vn
            strcpy(output, "usqadd  Vd, Vn");
            return;
        }
        if (((inst & 1056968704) == 1056968704 && (~inst & 57344) == 57344)) { // usra    Sd, Sn, IMM_VLSR
            strcpy(output, "usra    Sd, Sn, IMM_VLSR");
            return;
        }
        if (((inst & 1847599104) == 1847599104 && (~inst & 285268992) == 285268992)) { // usubl2  Vd, Vn, Vm
            strcpy(output, "usubl2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 773857280) == 773857280 && (~inst & 1359010816) == 1359010816)) { // usubl   Vd, Vn, Vm
            strcpy(output, "usubl   Vd, Vn, Vm");
            return;
        }
        if (((inst & 1847603200) == 1847603200 && (~inst & 285264896) == 285264896)) { // usubw2  Vd, Vn, Vm
            strcpy(output, "usubw2  Vd, Vn, Vm");
            return;
        }
        if (((inst & 773861376) == 773861376 && (~inst & 1359006720) == 1359006720)) { // usubw   Vd, Vn, Vm
            strcpy(output, "usubw   Vd, Vn, Vm");
            return;
        }
        if (((inst & 234887168) == 234887168 && (~inst & 824206336) == 824206336)) { // uzp1    Vd, Vn, Vm
            strcpy(output, "uzp1    Vd, Vn, Vm");
            return;
        }
        if (((inst & 234903552) == 234903552 && (~inst & 824189952) == 824189952)) { // uzp2    Vd, Vn, Vm
            strcpy(output, "uzp2    Vd, Vn, Vm");
            return;
        }
        if (((inst & 1310795776) == 1310795776 && (~inst & 822137856) == 822137856)) { // xtn2    Vd, Vn
            strcpy(output, "xtn2    Vd, Vn");
            return;
        }
        if (((inst & 237053952) == 237053952 && (~inst & 1895879680) == 1895879680)) { // xtn     Vd, Vn
            strcpy(output, "xtn     Vd, Vn");
            return;
        }
        if (((inst & 234895360) == 234895360 && (~inst & 824198144) == 824198144)) { // zip1    Vd, Vn, Vm
            strcpy(output, "zip1    Vd, Vn, Vm");
            return;
        }
        if (((inst & 234911744) == 234911744 && (~inst & 824181760) == 824181760)) { // zip2    Vd, Vn, Vm
            strcpy(output, "zip2    Vd, Vn, Vm");
            return;
        }
        strcpy(output, "???"); // cannot decode
    }
} // ns
