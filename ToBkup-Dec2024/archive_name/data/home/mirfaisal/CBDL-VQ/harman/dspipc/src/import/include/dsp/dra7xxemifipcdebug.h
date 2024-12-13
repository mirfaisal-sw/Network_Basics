#ifndef MMIPC_DEBUG_H
#define MMIPC_DEBUG_H

#define FILENUM(num)          \
        enum { F_NUM = num }; \
        void _dummy##num(void) {}

#define ASSERT(expr)                                                                                  \
        if ((expr)) {                                                                                 \
        } else {                                                                                      \
                uint16_t errCode;                                                                     \
                errCode = 0;                                                                          \
                errCode = __LINE__;                                                                   \
                errCode |= ((F_NUM) << 13);                                                           \
                WRITE_EMIF_REG(EMIF_CONTROLMAILBOX_REG, errCode);                                     \
                printk("[dspipc] [%s]: line %d <%s> ASSERT FAILED!", __FUNCTION__, __LINE__, ##expr); \
        }

/** 
 * @brief Filenumber for the dra300emifipc file
 * 
 * Unique number to define to source file of an assert() 
 */
#define FNUM_DRA7XXEMIFIPC_C 1

/**
 * @brief Filenumber entry
 * 
 * This structure defines a corresponding file nummer and the 
 * related filename string
 */
struct _DSPIPCDra7xxFileNames {
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
typedef struct _DSPIPCDra7xxFileNames DSPIPCDra7xxFileNames;

#endif /* MMIPC_DEBUG_H */
