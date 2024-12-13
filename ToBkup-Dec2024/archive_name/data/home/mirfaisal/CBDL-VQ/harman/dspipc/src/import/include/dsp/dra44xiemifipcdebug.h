#ifndef MMIPC_DEBUG_H
#define MMIPC_DEBUG_H

#include <assert.h>

#define FILENUM(num)          \
        enum { F_NUM = num }; \
        void _dummy##num(void) {}

#define ASSERT(expr)                                              \
        if ((expr)) {                                             \
        } else {                                                  \
                uint16_t errCode;                                 \
                errCode = 0;                                      \
                errCode = __LINE__;                               \
                errCode |= ((F_NUM) << 13);                       \
                WRITE_EMIF_REG(EMIF_CONTROLMAILBOX_REG, errCode); \
                assert(expr);                                     \
        }

/** 
 * @brief Filenumber for the dra300emifipc file
 * 
 * Unique number to define to source file of an assert() 
 */
#define FNUM_DRA44XIEMIFIPC_C 1

/**
 * @brief Filenumber entry
 * 
 * This structure defines a corresponding file nummer and the 
 * related filename string
 */
struct _DSPIPCDra44xiFileNames {
        /**
    * @brief File number
    *
    * This field contains the number of the file
    */
        mUInt32_t num;
        /**
    * @brief File name
    *
    * This field contains the name of the file
    */
        char* name;
};

/**
 * @brief DSPIPCDra44xeFileNames typedef
 */
typedef struct _DSPIPCDra44xiFileNames DSPIPCDra44xiFileNames;

/**
 * @brief Array with all filnumber/filename combinations
 *
 * This array is used to identiy the name of the file in which
 * an assert was taken
 */
DSPIPCDra44xiFileNames DSPIPCdra44xiFileNameArray[] = {
        { FNUM_DRA44XIEMIFIPC_C, "DRA44XIEMIFIPC.c" },
        { 0, NULL }
};

#endif /* MMIPC_DEBUG_H */
