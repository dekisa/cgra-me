/******************************************************************************
* This work is part of master thesis
* Author: Dejan Lukic, July 2019
* This work provides functions and macros for generating bitstream for north,
* east, south, west type CGRA architecture as described in work by Capalija
******************************************************************************/

/*
*  Here are listed configuration values of multiplexers in format:
*  (destination mux)_to_(source selected by configuration)
*/

const char fu1_to_n = 0x0;
const char fu1_to_e = 0x1;
const char fu1_to_s = 0x2;
const char fu1_to_w = 0x3;
const char fu2_to_n = 0x0;
const char fu2_to_e = 0x1;
const char fu2_to_s = 0x2;
const char fu2_to_w = 0x3;
const char n_to_e   = 0x0;
const char n_to_s   = 0x1;
const char n_to_w   = 0x2;
const char n_to_fu  = 0x3;
const char e_to_n   = 0x0;
const char e_to_s   = 0x1;
const char e_to_w   = 0x2;
const char e_to_fu  = 0x3;
const char s_to_n   = 0x0;
const char s_to_e   = 0x1;
const char s_to_w   = 0x2;
const char s_to_fu  = 0x3;
const char w_to_n   = 0x0;
const char w_to_e   = 0x1;
const char w_to_s   = 0x2;
const char w_to_fu  = 0x3;

/*
* Values added to iterator to get the configuration of specific multiplexer
* This order is implied by naming of multiplexers as they are oredered
* alphabeticaly inside of NESWArchFU.cpp
*/
#define FU_CONFIG_IT    0
#define MUX_FU1_IT      1
#define MUX_FU2_IT      2
#define MUX_N_IT        4
#define MUX_E_IT        3
#define MUX_S_IT        5
#define MUX_W_IT        6
#define CONST_IT        7

/*
*  Used to mark invalid configuration cell values that should be ignored
*/
#define INVALID_VAL     0xffffffff

/*
* number of cells per block, used to iterate over blocks 
*/
#define CELLS_PER_BLOCK 8

/* 
*  Macro GET_FR is used for generating configuration for fork recieve
*  cells. Macro generates FR configuration for fr_target, where valid
*  target is: fu1, fu2.
*  Order of bits and associated multiplexer is defined by src[0,3]
*  There is a case for each configuration of associated multiplexer next to
*  fork recieve
*  FR configuration bits [0,3] are set to 1 if multiplexer src[0,3] is 
*  connected to the same point as the target multiplexer 
*/

#define GET_FR_FU(target, src0, src1, src2, src3)                   \
unsigned char fr_ ## target = 0x00;                                 \
switch (mux_ ## target){                                            \
case target ## _to_ ## src0:                                        \
    fr_ ## target = (0x0                                    << 3) | \
                    ((mux_ ## src1 == src1 ## _to_ ## src0) << 2) | \
                    ((mux_ ## src2 == src2 ## _to_ ## src0) << 1) | \
                    ((mux_ ## src3 == src3 ## _to_ ## src0) << 0);  \
    break;                                                          \
case target ## _to_ ## src1:                                        \
    fr_ ## target = ((mux_ ## src0 == src0 ## _to_ ## src1) << 3) | \
                    (0x0                                    << 2) | \
                    ((mux_ ## src2 == src2 ## _to_ ## src1) << 1) | \
                    ((mux_ ## src3 == src3 ## _to_ ## src1) << 0);  \
    break;                                                          \
case target ## _to_ ## src2:                                        \
    fr_ ## target = ((mux_ ## src0 == src0 ## _to_ ## src2) << 3) | \
                    ((mux_ ## src1 == src1 ## _to_ ## src2) << 2) | \
                    (0x0                                    << 1) | \
                    ((mux_ ## src3 == src3 ## _to_ ## src2) << 0);  \
    break;                                                          \
case target ## _to_ ## src3:                                        \
    fr_ ## target = ((mux_ ## src0 == src0 ## _to_ ## src3) << 3) | \
                    ((mux_ ## src1 == src1 ## _to_ ## src3) << 2) | \
                    ((mux_ ## src2 == src2 ## _to_ ## src3) << 1) | \
                    (0x0                                    << 0);  \
    break;                                                          \
default:                                                            \
    break;                                                          \
}                                                                   \

/*
*  Macro GET_FR is used for generating configuration for fork recieve cells
*  Macro generates FR configuration for fr_target, where valid target is:
*  n, e, s, w
*  Order of bits and associated multiplexer is defined by src[0,2]
*  There is a case for each configuration of associated multiplexer next to 
*  fork recieve.
*  FR configuration bits [0,2] are set to 1 if multiplexer src[0,2] is 
*  connected to the same point as the target multiplexer
*/

#define GET_FR_NESW(target, src0, src1, src2)                       \
unsigned char fr_ ## target = 0x00;                                 \
switch (mux_ ## target){                                            \
case target ## _to_ ## src0:                                        \
    fr_ ## target = ((mux_fu1      ==      fu1_to_ ## src0) << 4) | \
                    ((mux_fu2      ==      fu2_to_ ## src0) << 3) | \
                    (0x0                                    << 2) | \
                    ((mux_ ## src1 == src1 ## _to_ ## src0) << 1) | \
                    ((mux_ ## src2 == src2 ## _to_ ## src0) << 0);  \
    break;                                                          \
case target ## _to_ ## src1:                                        \
    fr_ ## target = ((mux_fu1      ==      fu1_to_ ## src1) << 4) | \
                    ((mux_fu2      ==      fu2_to_ ## src1) << 3) | \
                    ((mux_ ## src0 == src0 ## _to_ ## src1) << 2) | \
                    (0x0                                    << 1) | \
                    ((mux_ ## src2 == src2 ## _to_ ## src1) << 0);  \
    break;                                                          \
case target ## _to_ ## src2:                                        \
    fr_ ## target = ((mux_fu1      ==      fu1_to_ ## src2) << 4) | \
                    ((mux_fu2      ==      fu2_to_ ## src2) << 3) | \
                    ((mux_ ## src0 == src0 ## _to_ ## src2) << 2) | \
                    ((mux_ ## src1 == src1 ## _to_ ## src2) << 1) | \
                    (0x0                                    << 0);  \
    break;                                                          \
case target ## _to_fu:                                              \
    fr_ ## target = (0x0                                    << 4) | \
                    (0x0                                    << 3) | \
                    ((mux_ ## src0 == src0 ## _to_fu      ) << 2) | \
                    ((mux_ ## src1 == src1 ## _to_fu      ) << 1) | \
                    ((mux_ ## src2 == src2 ## _to_fu      ) << 0);  \
    break;                                                          \
default:                                                            \
    break;                                                          \
} 

/*
*  Macro GET_FS is used for generating configuration for fork send cells
*  Macro generates FS configuration for fs_target, where valid target is:
*  fu, n, e, s, w
*  Order of bits and associated multiplexer is defined by src[0,4]
*  FS configuration bits [0,4] are set to 1 if multiplexer src[0,4] is
*  connected to target
*/

#define GET_FS_NESW(target, src0, src1, src2, src3, src4)                \
unsigned char fs_ ## target = 0x0;                                  \
fs_ ## target = ((mux_ ## src0 == src0 ## _to_ ## target) << 4) |   \
                ((mux_ ## src1 == src1 ## _to_ ## target) << 3) |   \
                ((mux_ ## src2 == src2 ## _to_ ## target) << 2) |   \
                ((mux_ ## src3 == src3 ## _to_ ## target) << 1) |   \
                ((mux_ ## src4 == src4 ## _to_ ## target) << 0);    \

#define GET_FS_FU(target, src1, src2, src3, src4)                \
unsigned char fs_ ## target = 0x0;                                  \
fs_ ## target = ((mux_ ## src1 == src1 ## _to_ ## target) << 3) |   \
                ((mux_ ## src2 == src2 ## _to_ ## target) << 2) |   \
                ((mux_ ## src3 == src3 ## _to_ ## target) << 1) |   \
                ((mux_ ## src4 == src4 ## _to_ ## target) << 0);    \

/*
*  bitstream_word is used to fill and output a binary 32bit configuration word
*  class allows overfilling, if filled with more than 32 bits, oldest bits will
*  be lost
*/

class bitstream_word{
private:

    int current_position;
    unsigned int word;
    friend std::ostream& operator<<(std::ostream& os, const bitstream_word& bw);

public:

    bitstream_word(){
        current_position = 32;
        word = 0;
    }

    void fill(unsigned int val, int size);
};
