#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dr.h"
#include "drprint.h"


// 打印SubNode时候的缩进值，考虑多线程
DR_TLS static int padding = -3;
// 打印链表里所有的tag
void PrintTags(PElementsListNode List)
{
    padding += 3;
    PElementsListNode Node = List;
    while (Node != NULL)
    {
        char *temp = VRList[Node->Element.VR].PrintVR(Node->Element.ValueField, Node->Element.ValueLength);
        for (int i = 0; i < padding; i++)
            printf(" ");
        printf("(%04X,%04X), %s, %s, %1d, 0x%X, %s\n", 
                Node->Element.GroupNumber, Node->Element.ElementNumber, 
                Node->Element.pszTagName,
                VRList[Node->Element.VR].pszVR,
                Node->Element.VM,
                Node->Element.ValueLength,
                temp);
        free(temp);
        if (Node->SubNode != NULL)
            PrintTags(Node->SubNode);
        Node = Node->NextNode;
    };
    padding -= 3;
};

#define MAXBYTECOUNTTODISPLAY 100                                   // 对字节流VR，如果字节数大于100，仅打印(0xYYYYYYYY items)

static char * const pszMallocError = "(Error: malloc failed)";
static char * const pszULStringError = "(Error: string with undefined length)";

// define the print function for each VR
// 调用者必须手工free获取的字符串内存

// // error
// VR_ERROR = 0,
char * PrintERRORVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(6);     // "ERROR"
    if (pszOutput == NULL)
        return NULL;
    sprintf(pszOutput, "ERROR");
    return pszOutput;
};

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
char * PrintStringVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput;
    if (ValueLength == UNDEFINED_LENGTH)
        return pszULStringError;
    if (ValueLength > MAXBYTECOUNTTODISPLAY)
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X chars)", ValueLength);
        return pszOutput;
    };
    // + 0 at the end of the pszOutput to make a psz string
    pszOutput = (char *)malloc(ValueLength + 1);
    if (pszOutput == NULL)
        return pszMallocError;
    for (unsigned int i = 0; i < ValueLength; i++)
        *(pszOutput + i) = *(Value + i);
    *(pszOutput + ValueLength) = 0;
    return pszOutput;
};


// // number (8)

// 确定需要的字符串缓冲区的最大长度
// long long: 8 bytes / 64 bits = 2^64 = 18,446,744,073,709,551,616 (20 chars), 加1个负号 -> 21 chars max
// double: 8 bytes / 64 bits, 20 chars, 加1个小数点，加6位小数，加1个符号 -> 28 chars max
// 统一设定字符串缓冲区位60个字符
#define BUFFERSIZE 60

// VR_US, "US",     // unsigned short,          2 bytes
char * PrintUSVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%hu", *(unsigned short *)Value);
    return pszOutput;
};

// VR_SS, "SS",     // signed short,            2 bytes
char * PrintSSVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%hd", *(signed short *)Value);
    return pszOutput;
};

// VR_UL, "UL",     // unsigned long,           4 bytes
char * PrintULVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%u", *(unsigned int *)Value);
    return pszOutput;
};

// VR_SL, "SL",     // signed long,             4 bytes
char * PrintSLVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%u", *(signed int *)Value);
    return pszOutput;
};

// VR_UV, "UV",     // unsigned very long,      8 bytes
char * PrintUVVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%llu", *(unsigned long long *)Value);
    return pszOutput;
};

// VR_SV, "SV",     // signed very long,        8 bytes
char * PrintSVVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%lld", *(signed long long *)Value);
    return pszOutput;
};

// VR_FL, "FL",     // float,                   4 bytes
char * PrintFLVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%.6f", *(float *)Value);
    return pszOutput;
};

// VR_FD, "FD",     // double,                  8 bytes
char * PrintFDVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(BUFFERSIZE);
    if (pszOutput == NULL)
        return pszMallocError;
    if ((Value == NULL) || (ValueLength == 0))
        pszOutput[0] = 0;
    else
        sprintf(pszOutput, "0n%.6f", *(double *)Value);
    return pszOutput;
};



// // stream (7)
// VR_OB, "OB",     // octet-stream                                         VM=1
char * PrintOBVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength > MAXBYTECOUNTTODISPLAY)
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X items)", ValueLength);
    }
    else
    {
        pszOutput = (char *)malloc(ValueLength * 2 + (ValueLength - 1) + 1);    // each byte -> 2 chars, separated by '\' + 1 byte of 0
        if (pszOutput == NULL)
            return pszMallocError;
        char *temp = pszOutput;
        for (unsigned int i = 0; i < ValueLength; i++)
        {
            sprintf(temp, "%02X", Value[i]);
            temp += 2;
            if (i != (ValueLength - 1))
            {
                *temp = 0x5C;
                temp++;
            };
        };
    };
    return pszOutput;
};

// VR_OW, "OW",     // 16-bit stream                                        VM=1
char * PrintOWVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength % 2 != 0)                                              // 必须是2的倍数
    {
        pszOutput = (char *)malloc(27);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(error decoding OW stream)");
    }
    else if (ValueLength > MAXBYTECOUNTTODISPLAY)
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X items)", ValueLength / 2);
    }
    else
    {
        pszOutput = (char *)malloc(ValueLength * 4 + (ValueLength - 1) + 1);    // each word -> 4 chars, separated by '\' + 1 byte of 0
        if (pszOutput == NULL)
            return pszMallocError;
        char *temp = pszOutput;
        unsigned short tempword;
        for (unsigned int i = 0; i < ValueLength; i+=2)
        {
            tempword = *(unsigned short *)(Value + i);
            sprintf(temp, "%04X", tempword);
            temp += 4;
            if (i != (ValueLength - 1))
            {
                *temp = 0x5C;
                temp++;
            };
        };
    };
    return pszOutput;
};


// VR_OL, "OL",     // 32-bit stream                                        VM=1
char * PrintOLVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength % 4 != 0)                                              // 必须是4的倍数
    {
        pszOutput = (char *)malloc(27);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(error decoding OL stream)");
    }
    else
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X items)", ValueLength / 4);
    };
    return pszOutput;
};

// VR_OV, "OV",     // 64-bit stream
char * PrintOVVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength % 8 != 0)                                              // 必须是8的倍数
    {
        pszOutput = (char *)malloc(27);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(error decoding OV stream)");
    }
    else
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X items)", ValueLength / 8);
    };
    return pszOutput;
};

// VR_OF, "OF",     // 32-bit stream                                        VM=1
char * PrintOFVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength % 4 != 0)                                              // 必须是4的倍数
    {
        pszOutput = (char *)malloc(27);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(error decoding OF stream)");
    }
    else
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X items)", ValueLength / 4);
    };
    return pszOutput;
};

// VR_OD, "OD",     // 64-bit stream                                        VM=1
char * PrintODVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength % 8 != 0)                                              // 必须是8的倍数
    {
        pszOutput = (char *)malloc(27);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(error decoding OD stream)");
    }
    else
    {
        pszOutput = (char *)malloc(19);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(0x%08X items)", ValueLength / 8);
    };
    return pszOutput;
};

// VR_UN, "UN",     // octet-stream                                         VM=1
char * PrintUNVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(3);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "UN");
    return pszOutput;
};

// // others (2)
// VR_AT, "AT",     // tag,                     4 bytes
char * PrintATVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = NULL;
    if ((Value == NULL) || (ValueLength == 0))
    {
        pszOutput = (char *)malloc(1);
        if (pszOutput == NULL)
            return pszMallocError;
        pszOutput[0] = 0;
    }
    else if (ValueLength != 4)                                                  // fixed 4 bytes
    {
        pszOutput = (char *)malloc(20);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "(error - not 4 bytes)");
    }
    else
    {
        pszOutput = (char *)malloc(10);
        if (pszOutput == NULL)
            return pszMallocError;
        sprintf(pszOutput, "%04X,%04X", *(unsigned short *)Value, *(unsigned short *)(Value + 2));
    };
    return pszOutput;
};

// VR_SQ, "SQ"      // sequence
char * PrintSQVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(5);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(SQ)");
    return pszOutput;
};

// // for Data Set Elements only
// VR_Blank,
char * PrintBlankVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(47);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - Blank VR (0x%08X bytes))", ValueLength);
    return pszOutput;
};

// VR_NoVR,
char * PrintNoVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(44);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - No VR (0x%08X bytes))", ValueLength);
    return pszOutput;
};

// VR_OBorOW,
char * PrintOBorOWVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(45);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - OBorOW (0x%08X bytes))", ValueLength);
    return pszOutput;
};


// VR_USorOW,
char * PrintUSorOWVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(45);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - USorOW (0x%08X bytes))", ValueLength);
    return pszOutput;
};

// VR_USorSS,
char * PrintUSorSSVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(45);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - USorSS (0x%08X bytes))", ValueLength);
    return pszOutput;
};

// VR_USorSSorOW
char * PrintUSorSSorOWVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(49);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - USorSSorOW (0x%08X bytes))", ValueLength);
    return pszOutput;
};

// VR_PrivateUnknown
char * PrintPrivateUnknownVR(const unsigned char *Value, unsigned int ValueLength)
{
    char *pszOutput = (char *)malloc(53);
    if (pszOutput == NULL)
        return pszMallocError;
    sprintf(pszOutput, "(can not decode - PrivateUnknown (0x%08X bytes))", ValueLength);
    return pszOutput;
};

