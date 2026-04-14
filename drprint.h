#ifndef DR_PRINT_H
#define DR_PRINT_H

#include "dr.h"


// 打印链表的所有elements/tags
void PrintTags(PElementsListNode List);

// // error
// VR_ERROR = 0,
char * PrintERRORVR(const unsigned char *Value, unsigned int ValueLength);

// // string (17)
// VR_AE, "AE",     // string,                  <= 16 bytes
// VR_CS, "CS",     // string,                  <= 16 bytes
// VR_PN, "PN",     // name,                    <= 64 chars    
// VR_AS, "AS",     // age,                     4 bytes
// VR_DA, "DA",     // date,                    8 bytes or <= 18 bytes
// VR_TM, "TM",     // time,                    <= 14 bytes or <= 28 bytes
// VR_DT, "DT",     // datetime,                <= 26 bytes or <= 54 bytes
// VR_IS, "IS",     // int,                     <= 12 bytes
// VR_DS, "DS",     // decimal number,          <= 16 bytes
// VR_SH, "SH",     // string,                  <= 16 chars
// VR_LO, "LO",     // string,                  <= 64 chars
// VR_ST, "ST",     // string,                  <=1024 chars                VM=1
// VR_LT, "LT",     // string,                  <=10240 chars               VM=1
// VR_UC, "UC",     // unlimited
// VR_UT, "UT",     // unlimited                                            VM=1
// VR_UI, "UI",     // UID,                     64 bytes
// VR_UR, "UR",     // URI/URL                                              VM=1
// 调用方需要free(pszOutput)
// 如果输入的ValueLength = 0, 那会输出一个空字符串
char * PrintStringVR(const unsigned char *Value, unsigned int ValueLength);

// // number (8)
// VR_US, "US",     // unsigned short,          2 bytes
char * PrintUSVR(const unsigned char *Value, unsigned int ValueLength);

// VR_SS, "SS",     // signed short,            2 bytes
char * PrintSSVR(const unsigned char *Value, unsigned int ValueLength);

// VR_UL, "UL",     // unsigned long,           4 bytes
// 调用方需要free(pszOutput)
char * PrintULVR(const unsigned char *Value, unsigned int ValueLength);

// VR_SL, "SL",     // signed long,             4 bytes
char * PrintSLVR(const unsigned char *Value, unsigned int ValueLength);

// VR_UV, "UV",     // unsigned very long,      8 bytes
char * PrintUVVR(const unsigned char *Value, unsigned int ValueLength);

// VR_SV, "SV",     // signed very long,        8 bytes
char * PrintSVVR(const unsigned char *Value, unsigned int ValueLength);

// VR_FL, "FL",     // float,                   4 bytes
char * PrintFLVR(const unsigned char *Value, unsigned int ValueLength);

// VR_FD, "FD",     // double,                  8 bytes
char * PrintFDVR(const unsigned char *Value, unsigned int ValueLength);



// // stream (7)
// VR_OB, "OB",     // octet-stream                                         VM=1
char * PrintOBVR(const unsigned char *Value, unsigned int ValueLength);

// VR_OW, "OW",     // 16-bit stream                                        VM=1
char * PrintOWVR(const unsigned char *Value, unsigned int ValueLength);

// VR_OL, "OL",     // 32-bit stream                                        VM=1
char * PrintOLVR(const unsigned char *Value, unsigned int ValueLength);

// VR_OV, "OV",     // 64-bit stream
char * PrintOVVR(const unsigned char *Value, unsigned int ValueLength);

// VR_OF, "OF",     // 32-bit stream                                        VM=1
char * PrintOFVR(const unsigned char *Value, unsigned int ValueLength);

// VR_OD, "OD",     // 64-bit stream                                        VM=1
char * PrintODVR(const unsigned char *Value, unsigned int ValueLength);

// VR_UN, "UN",     // octet-stream                                         VM=1
char * PrintUNVR(const unsigned char *Value, unsigned int ValueLength);

// // others (2)
// VR_AT, "AT",     // tag,                     4 bytes
char * PrintATVR(const unsigned char *Value, unsigned int ValueLength);

// VR_SQ, "SQ"      // sequence
char * PrintSQVR(const unsigned char *Value, unsigned int ValueLength);

// // for Data Set Elements only
// VR_Blank,
char * PrintBlankVR(const unsigned char *Value, unsigned int ValueLength);

// VR_NoVR,
char * PrintNoVR(const unsigned char *Value, unsigned int ValueLength);

// VR_OBorOW,
char * PrintOBorOWVR(const unsigned char *Value, unsigned int ValueLength);

// VR_USorOW,
char * PrintUSorOWVR(const unsigned char *Value, unsigned int ValueLength);

// VR_USorSS,
char * PrintUSorSSVR(const unsigned char *Value, unsigned int ValueLength);

// VR_USorSSorOW
char * PrintUSorSSorOWVR(const unsigned char *Value, unsigned int ValueLength);

// VR_PrivateUnknown
char * PrintPrivateUnknownVR(const unsigned char *Value, unsigned int ValueLength);

#endif