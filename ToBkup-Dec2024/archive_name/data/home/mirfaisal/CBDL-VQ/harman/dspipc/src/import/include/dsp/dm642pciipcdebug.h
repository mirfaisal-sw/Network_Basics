#ifndef DM642_DEBUG_H
#define DM642_DEBUG_H

#include <assert.h>

#define FILENUM(num)          \
        enum { F_NUM = num }; \
        void _dummy##num(void) {}

#define ASSERT(expr)                                                                              \
        if ((expr)) {                                                                             \
        } else {                                                                                  \
                driverSingleton.comm->dspAssertLine = __LINE__;                                   \
                driverSingleton.comm->dspAssertFile = F_NUM;                                      \
                CACHE_wbInvL1d(&driverSingleton.comm, sizeof(*driverSingleton.comm), CACHE_WAIT); \
                CACHE_wbL2(&driverSingleton.comm, sizeof(*driverSingleton.comm), CACHE_WAIT);     \
                pci_raiseInterrupt();                                                             \
                assert(expr);                                                                     \
        }

/** 
 * @brief Filenumber for the dm642emifipc file
 * 
 * Unique number to define to source file of an assert() 
 */
#define FNUM_DM642EMIFIPC_C 1

/**
 * @brief Filenumber entry
 * 
 * This structure defines a corresponding file nummer and the 
 * related filename string
 */
struct _DSPIPCDm642FileNames {
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
 * @brief DSPIPCDm642FileNames typedef
 */
typedef struct _DSPIPCDm642FileNames DSPIPCDm642FileNames;

/**
 * @brief Array with all filnumber/filename combinations
 *
 * This array is used to identiy the name of the file in which
 * an assert was taken
 */
DSPIPCDm642FileNames DSPIPCDm642FileNameArray[] = {
        { FNUM_DM642EMIFIPC_C, "DM642EMIFIPC.c" },
        { 0, NULL }
};

#endif /* DM642_DEBUG_H */
