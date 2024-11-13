```
     GPIO    g26    g27    g4    g5    g6      g7         g8        g9         g10         g11           g12         g13
      col     0      1      2     3     4       5          6         7          8           9             10          11

 led0/g20    A0     A1     A2    A3    A4      A5         A6        A7         A8          A9            A10         A11
 led1/g21    A12    A13    A14   A15   A16     A17        A18       A19        A20         A21
 led2/g22   ADD22  ADD18  ADD16 DATA  KERNEL  SUPER      USER     MASTER      PAUSE        RUN         ADDR_ERR    PAR_ERR
 led3/g23    D0     D1     D2    D3    D4      D5         D6        D7          D8         D9            D10         D11
 led4/g24    D12    D13    D14   D15 PAR_LOW PAR_HIGH R1_USER_D R1_SUPER_D R1_KERNEL_D  CONS_PHY    R2_DATA_PATHS R2_BUS_REG
 led5/g25                                             R1_USER_I R1_SUPER_I R1_KERNEL_I R1_PROG_PHI   MU_A_FPP/CPU R2_DISP_REG
 
 row0/g16   SR0    SR1    SR2    SR3   SR4     SR5        SR6       SR7        SR8         SR9           SR10        SR11 
 row1/g17   SR12   SR13   SR14   SR15  SR16    SR17       SR18      SR19       SR20        SR21         ADDR_SW     DATA_SW
 row2/g18   TEST LOAD_ADD EXAM   DEP   CONT  ENA_HALT  SING_INST   START    ADDR_ROT1    ADDR_ROT2     DATA_ROT1   DATA_ROT2

      col     0      1      2     3     4       5          6         7          8           9             10          11
      GPIO   g26    g27    g4    g5    g6      g7         g8        g9         g10         g11           g12         g13
```
1000:   005000 CLR R0
1002:   012701 MOV #177777,R1
1004:   177777
1006:   005301 DEC R1
1010:   001376 BNE 1006
1012:   005200 INC R0
1014:   001372 BNE 1002
