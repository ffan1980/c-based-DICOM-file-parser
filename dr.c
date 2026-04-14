#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dr.h"


// 前置声明

int TagMatched(const char *pszTagAGN, const char *pszTagAEN, const char *pszTagBGN, const char *pszTagBEN);

int GetDataElements(unsigned char **Pos, unsigned char *EOFPos, unsigned int DataSetLength, DR_INOUT_ PElementsListNode *List,
                    StorageSOPClassName FileStorageSOPClass, TransferSyntaxName FileTransferSyntax, ImplicitOrExplicitVR FileIEVR);


// read # bytes from *Pos; if sucessful, *Pos += # bytes; otherwise, *Pos is unchanged
// return
// != 0: failed
// 0: sucessful
int BufferRead(void *Dest, size_t BytesToRead, unsigned char **Pos, unsigned char *EOFPos)
{
    if ((Pos == NULL) || (*Pos == NULL) || (EOFPos == NULL))
        return -1;
    size_t AvailableBytesToRead = EOFPos - *Pos + 1;
    if (AvailableBytesToRead < BytesToRead)
        return -1;
    void *ret = memcpy(Dest, *Pos, BytesToRead);
    if (ret != NULL)
    {
        *Pos += BytesToRead;
        return 0;
    }
    else
        return -1;
};

// move *Pos forward by # bytes
// return
// -1: failed
// 0: successful
int BufferSkip(size_t BytesToSkip, unsigned char **Pos, unsigned char *EOFPos)
{
    if ((Pos == NULL) || (*Pos == NULL) || (EOFPos == NULL))
        return -1;
    size_t AvailableBytesToSkip = EOFPos - *Pos;
    if (AvailableBytesToSkip < BytesToSkip)
        return -1;
    *Pos += BytesToSkip;
    return 0;
};

// return VR
ValueRepresentation GetVR(unsigned char VR[])
{
    // string (17)
    if ((VR[0] == 'A') && (VR[1] == 'E'))
        return VR_AE;      // string,                  <= 16 bytes
    else if ((VR[0] == 'C') && (VR[1] == 'S'))
        return VR_CS;      // string,                  <= 16 bytes
    else if ((VR[0] == 'P') && (VR[1] == 'N'))
        return VR_PN;      // name,                    <= 64 chars
    else if ((VR[0] == 'A') && (VR[1] == 'S'))
        return VR_AS;      // age,                     4 bytes
    else if ((VR[0] == 'D') && (VR[1] == 'A'))
        return VR_DA;      // date,                    8 bytes or <= 18 bytes
    else if ((VR[0] == 'T') && (VR[1] == 'M'))
        return VR_TM;      // time,                    <= 14 bytes or <= 28 bytes
    else if ((VR[0] == 'D') && (VR[1] == 'T'))
        return VR_DT;      // datetime,                <= 26 bytes or <= 54 bytes
    else if ((VR[0] == 'I') && (VR[1] == 'S'))
        return VR_IS;      // int,                     <= 12 bytes
    else if ((VR[0] == 'D') && (VR[1] == 'S'))
        return VR_DS;      // decimal number,          <= 16 bytes
    else if ((VR[0] == 'S') && (VR[1] == 'H'))
        return VR_SH;      // string,                  <= 16 chars
    else if ((VR[0] == 'L') && (VR[1] == 'O'))
        return VR_LO;      // string,                  <= 64 chars
    else if ((VR[0] == 'S') && (VR[1] == 'T'))
        return VR_ST;      // string,                  <=1024 chars                VM=1
    else if ((VR[0] == 'L') && (VR[1] == 'T'))
        return VR_LT;      // string,                  <=10240 chars               VM=1
    else if ((VR[0] == 'U') && (VR[1] == 'C'))
        return VR_UC;      // unlimited
    else if ((VR[0] == 'U') && (VR[1] == 'T'))
        return VR_UT;      // unlimited                                            VM=1
    else if ((VR[0] == 'U') && (VR[1] == 'I'))
        return VR_UI;      // UID,                     64 bytes
    else if ((VR[0] == 'U') && (VR[1] == 'R'))
        return VR_UR;      // URI/URL                                              VM=1
    // number (8)
    else if ((VR[0] == 'U') && (VR[1] == 'S'))
        return VR_US;      // unsigned short,          2 bytes
    else if ((VR[0] == 'S') && (VR[1] == 'S'))
        return VR_SS;      // signed short,            2 bytes
    else if ((VR[0] == 'U') && (VR[1] == 'L'))
        return VR_UL;      // unsigned long,           4 bytes
    else if ((VR[0] == 'S') && (VR[1] == 'L'))
        return VR_SL;      // signed long,             4 bytes
    else if ((VR[0] == 'U') && (VR[1] == 'V'))
        return VR_UV;      // unsigned very long,      8 bytes
    else if ((VR[0] == 'S') && (VR[1] == 'V'))
        return VR_SV;      // signed very long,        8 bytes
    else if ((VR[0] == 'F') && (VR[1] == 'L'))
        return VR_FL;      // float,                   4 bytes
    else if ((VR[0] == 'F') && (VR[1] == 'D'))
        return VR_FD;      // double,                  8 bytes
    // stream (7)
    else if ((VR[0] == 'O') && (VR[1] == 'B'))
        return VR_OB;      // octet-stream                                         VM=1
    else if ((VR[0] == 'O') && (VR[1] == 'W'))
        return VR_OW;      // 16-bit stream                                        VM=1
    else if ((VR[0] == 'O') && (VR[1] == 'L'))
        return VR_OL;      // 32-bit stream                                        VM=1
    else if ((VR[0] == 'O') && (VR[1] == 'V'))
        return VR_OV;      // 64-bit stream
    else if ((VR[0] == 'O') && (VR[1] == 'F'))
        return VR_OF;      // 32-bit stream                                        VM=1
    else if ((VR[0] == 'O') && (VR[1] == 'D'))
        return VR_OD;      // 64-bit stream                                        VM=1
    else if ((VR[0] == 'U') && (VR[1] == 'N'))
        return VR_UN;      // octet-stream                                         VM=1
    // others (2)
    else if ((VR[0] == 'A') && (VR[1] == 'T'))
        return VR_AT;      // tag,                     4 bytes
    else if ((VR[0] == 'S') && (VR[1] == 'Q'))
        return VR_SQ;      // sequence                                             VM=1
    // for Data Set Elements only
    else if (((VR[0] == 0) && (VR[1] == 0)) || ((VR[0] == ' ') && (VR[1] == ' ')))
        return VR_Blank;
    else
        return VR_ERROR;
};

// 判断一个VR是否满足如下条件: Explicit (1)
// return
// 1: a Special VR, i.e. Explicit (1) as below
// 0: not a Special VR 
// 说明：
// 1. Explict: 
//  (1) VR = SV, UC, UR, UV, UT, OB, OD, OF, OL, OV, OW, SQ, UN:
//      组号(2b), 元素号(2b), VR(2b), 预留(2b: 0x00, 0x00), 值长度(4b), 数据(值长度个bytes)
//          其中：VR = OB, OD, OF, OL, OV, OW, SQ, UN:
//                   Value Length 可以= Undefined Length (FFFFFFFFH): a Sequence​ Delimitation Item marks the end of the Value Field
//  (2) VR为其他普通类型：
//      组号(2b), 元素号(2b), VR(2b), 值长度(2b), 数据(值长度个bytes)
// 2. Implicit:
//      组号(2b), 元素号(2b), 值长度(4b), 数据(值长度个bytes/Undefined Length)
// 3. 数据(值长度个bytes)：如果以fffe,e000开头，则是一个数据段，包括：段的值长度(4b), 数据(值长度个bytes)；数据段的数据，则可能包含以上(1)、(2)、(3)
int IsExplicitSpecialVR(ValueRepresentation VR)
{
    if ((VR == VR_SV) ||
        (VR == VR_UC) ||
        (VR == VR_UR) ||
        (VR == VR_UV) ||
        (VR == VR_UT) ||
        (VR == VR_OB) ||
        (VR == VR_OD) ||
        (VR == VR_OF) ||
        (VR == VR_OL) ||
        (VR == VR_OV) ||
        (VR == VR_OW) ||
        (VR == VR_SQ) ||
        (VR == VR_UN))
        return 1;
    else
        return 0;
};

// 
// Explicit & VR for which Undefined Length is possible: OB, OD, OF, OL, OV, OW, SQ, UN
int IsExplicitSpecialUDLVR(ValueRepresentation VR)
{
    if ((VR == VR_OB) ||
        (VR == VR_OD) ||
        (VR == VR_OF) ||
        (VR == VR_OL) ||
        (VR == VR_OV) ||
        (VR == VR_OW) ||
        (VR == VR_SQ) ||
        (VR == VR_UN))
        return 1;
    else
        return 0;    
};

// 判断从文件中读取的VR与该tag在registry里的VR是否相符
// return
// 1: matched
// 0: not matched
int VRMatched(ValueRepresentation TagVR, ValueRepresentation RegistryVR)
{
    // 从文件读取的tag VR可能对应如下registry里的VR：
    // (1) 标准的2字节VR
    // (2) VR_ERROR             return 0
    // (3) VR_Blank
    // (4) VR_NoVR              return 0
    // (2) VR_OBorOW
    // (3) VR_USorOW
    // (4) VR_USorSS
    // (5) VR_USorSSorOW
    // (6) VR_PrivateUnknown
    if ((TagVR == VR_ERROR) || (TagVR == VR_NoVR) ||
        (RegistryVR == VR_ERROR) || (RegistryVR == VR_NoVR))
        return 0;
    else if (RegistryVR == TagVR)
        return 1;
    else if ((RegistryVR == VR_OBorOW) && ((TagVR == VR_OB) || (TagVR == VR_OW)))
        return 1;
    else if ((RegistryVR == VR_USorOW) && ((TagVR == VR_US) || (TagVR == VR_OW)))
        return 1;
    else if ((RegistryVR == VR_USorSS) && ((TagVR == VR_US) || (TagVR == VR_SS)))
        return 1;
    else if ((RegistryVR == VR_USorSSorOW) && ((TagVR == VR_US) || (TagVR == VR_SS) || (TagVR == VR_OW)))
        return 1;
    else if (RegistryVR == VR_PrivateUnknown)
        return 1;
    else
        return 0;
};

// 确认Tag是否为private
// return
// 1: yes private
// 0: not private
int IsPrivateTag(unsigned short GroupNumber, unsigned short ElementNumber,
                    DR_OUT_ ValueRepresentation *VR, DR_OUT_ char **TagName, DR_OUT_ ValueMultiplicity *VM)
{
    // P3.5 page 51
    if ((GroupNumber % 2 == 0) ||                                                           // GroupNumber必须为奇数
        ((GroupNumber == 0x0001) && (ElementNumber == 0xEEEE)) ||                           // 5个例外
        ((GroupNumber == 0x0003) && (ElementNumber == 0xEEEE)) ||
        ((GroupNumber == 0x0005) && (ElementNumber == 0xEEEE)) ||
        ((GroupNumber == 0x0007) && (ElementNumber == 0xEEEE)) ||
        ((GroupNumber == 0xFFFF) && (ElementNumber == 0xEEEE)))
    {
        *TagName = NULL;
        *VR = VR_ERROR;
        *VM = VM_Unknown;        
        return 0;
    };
    if ((ElementNumber >= 0x0010) && (ElementNumber <= 0x00FF))
    {
        // Private Creator tag
        *TagName = PrivateCreatorTag;
        *VR = VR_LO;
        *VM = VM_Single;
    }
    else
    {
        // Private Unknown
        *TagName = PrivateUnknownTag;
        *VR = VR_PrivateUnknown;
        *VM = VM_PrivateUnknown;
    };
    return 1;
};

// 根据PS3.5 Page 97 Implicit VR定义, 把Registry里根据GN+EN查询到的待明确的RegistryVR明确为正确的VR值，用于后续decoding
// 仅适用于Data Set & Implicit transfer syntax
// 但是，这里只能明确一部分VR，其他VR还需要结合StorageSOPClass来明确
// 参数：
// OriginalVR：从Registry里根据GN+EN查询到的RegistryVR
// return:
//  需要特殊处理的VR(e.g. XXorYY)，返回正确的VR值
//  否则，返回传入的OriginalVR
ValueRepresentation GetImplicitVR(const char *pszGroupNumber, const char *pszElementNumber, ValueRepresentation OriginalVR)
{
    if (TagMatched(pszGroupNumber, pszElementNumber, "7FE0", "0010") == 1)                      // pixel data (07FE,0010)
        return VR_OW;
    if (TagMatched(pszGroupNumber, pszElementNumber, "60xx", "3000") == 1)                      // overlay data (60xx,3000)
        return VR_OW;
    if (TagMatched(pszGroupNumber, pszElementNumber, "5400", "1010") == 1)                      // Waveform Data
        return VR_OW;
    if (TagMatched(pszGroupNumber, pszElementNumber, "5400", "100A") == 1)                      // Waveform Padding Value
        return VR_OW;
    if (TagMatched(pszGroupNumber, pszElementNumber, "5400", "0112") == 1)                      // Channel Maximum Value
        return VR_OW;
    if (TagMatched(pszGroupNumber, pszElementNumber, "5400", "0110") == 1)                      // Channel Minimum Value
        return VR_OW;
    
    // below conditions are skipped, because the VR in registry is already correct:
    // + R/G/B/A Palette Color Lookup Table Data (0028,1201),(0028,1202),(0028,1203),(0028,1204) = OW
    // + R/G/B Palette Color Lookup Table Descriptor (0028,1101),(0028,1102),(0028,1103) = SS or US
    // + Segmented R/G/B Palette Color Lookup Table Data (0028,1221),(0028,1222),(0028,1223) = OW
    // + LUT Data (0028,3006) has the Value Representation = US or OW
    // + LUT Descriptor (0028,3002) has the Value Representation = SS or US (depending on rules specified in the IOD in PS3.3)
    // + Blending Lookup Table Data (0028,1408) = OW
    // + Track Point Index List (0066,0129) = OL

    return OriginalVR;
};

// for tags in DataSetRegistryChar (tags with 'x')
// return
// 1: matched
// 0: not matched
int Matched(const char c1, const char c2)
{
    if ((c1 == c2) || (c1 == 'x') || (c2 == 'x'))
        return 1;
    else
        return 0;
};

// 判断两个tag是否一致，考虑tag带'x'的情况, for tags in DataSetRegistryChar (tags with 'x')
// return
// 1: matched
// 0: not matched
int TagMatched(const char *pszTagAGN, const char *pszTagAEN, const char *pszTagBGN, const char *pszTagBEN)
{
    if ((Matched(pszTagAGN[0], pszTagBGN[0]) == 1) &&
        (Matched(pszTagAGN[1], pszTagBGN[1]) == 1) &&
        (Matched(pszTagAGN[2], pszTagBGN[2]) == 1) &&
        (Matched(pszTagAGN[3], pszTagBGN[3]) == 1) &&
        (Matched(pszTagAEN[0], pszTagBEN[0]) == 1) &&
        (Matched(pszTagAEN[1], pszTagBEN[1]) == 1) &&
        (Matched(pszTagAEN[2], pszTagBEN[2]) == 1) &&
        (Matched(pszTagAEN[3], pszTagBEN[3]) == 1))
        return 1;
    else
        return 0;
};

// 考虑多线程的情况, used only by IsInMetaRegistry
DR_TLS static int MERPos = 0;

// 判断一个tag是否在MetaRegistry，并返回相应的VR, TagName和VM
// return
// 0: not found
// 1: found
int IsInMetaRegistry(unsigned short GroupNumber, unsigned short ElementNumber,
                    DR_OUT_ ValueRepresentation *VR, DR_OUT_ char **TagName, DR_OUT_ ValueMultiplicity *VM)
{
    int OriginalMERPos = MERPos;
    for (; MERPos < METAREGISTRYELEMENTSCOUNT; MERPos++)
    {
        if ((GroupNumber == MetaRegistry[MERPos].GroupNumber) &&
            (ElementNumber == MetaRegistry[MERPos].ElementNumber))
        {
            *VR = MetaRegistry[MERPos].VR;
            *TagName = MetaRegistry[MERPos].pszName;
            *VM = MetaRegistry[MERPos].VM;
            return 1;
        };
    };
    for (MERPos = 0; MERPos < OriginalMERPos; MERPos++)
    {
        if ((GroupNumber == MetaRegistry[MERPos].GroupNumber) &&
            (ElementNumber == MetaRegistry[MERPos].ElementNumber))
        {
            *VR = MetaRegistry[MERPos].VR;
            *TagName = MetaRegistry[MERPos].pszName;
            *VM = MetaRegistry[MERPos].VM;
            return 1;
        };
    };
    *VR = VR_ERROR;
    *TagName = NULL;
    *VM = VM_Unknown;
    return 0;
};

// 考虑多线程的情况, used only by IsInDataRegistry
DR_TLS static int DERPos = 0;
DR_TLS static int DERCharPos = 0;

// 判断一个tag是否在DataSetRegistry，并返回相应的VR, TagName和VM
// return
// 0: not found
// 1: found
// parameter: IsExplicit - 1 yes, 0 no
int IsInDataRegistry(unsigned short GroupNumber, unsigned short ElementNumber,              // for DataSetRegistryNumber
                    char *pszGroupNumber, char *pszElementNumber,                           // for DataSetRegistryChar
                    DR_OUT_ ValueRepresentation *VR, DR_OUT_ char **TagName, DR_OUT_ ValueMultiplicity *VM)
{
    // first check in the DataSetRegistryNumber
    int OriginalDERPos = DERPos;
    for (; DERPos < DATAREGISTRYELEMENTSNUMBERCOUNT; DERPos++)
    {
        if ((GroupNumber == DataSetRegistryNumber[DERPos].GroupNumber) &&
            (ElementNumber == DataSetRegistryNumber[DERPos].ElementNumber))
        {
            *VR = DataSetRegistryNumber[DERPos].VR;
            *TagName = DataSetRegistryNumber[DERPos].pszName;
            *VM = DataSetRegistryNumber[DERPos].VM;
            return 1;
        };
    };
    for (DERPos = 0; DERPos < OriginalDERPos; DERPos++)
    {
        if ((GroupNumber == DataSetRegistryNumber[DERPos].GroupNumber) &&
            (ElementNumber == DataSetRegistryNumber[DERPos].ElementNumber))
        {
            *VR = DataSetRegistryNumber[DERPos].VR;
            *TagName = DataSetRegistryNumber[DERPos].pszName;
            *VM = DataSetRegistryNumber[DERPos].VM;
            return 1;
        };
    };

    // not in the DataSetRegistryNumber, then check if private tag
    // printf("checking if private.\n");
    if (IsPrivateTag(GroupNumber, ElementNumber, VR, TagName, VM) == 1)
        return 1;

    // not private tag, then check in the DataSetRegistryChar
    // printf("checking into DataSetRegistryChar: %s, %s.\n", pszGroupNumber, pszElementNumber);
    int OriginalDERCharPos = DERCharPos;
    for (; DERCharPos < DATAREGISTRYELEMENTSCHARCOUNT; DERCharPos++)
    {
        if (TagMatched(pszGroupNumber, pszElementNumber, 
                        DataSetRegistryChar[DERCharPos].pszGroupNumber, 
                        DataSetRegistryChar[DERCharPos].pszElementNumber) == 1)
        {
            *VR = DataSetRegistryChar[DERCharPos].VR;
            *TagName = DataSetRegistryChar[DERCharPos].pszName;
            *VM = DataSetRegistryChar[DERCharPos].VM;
            return 1;
        };
    };
    for (DERCharPos = 0; DERCharPos < OriginalDERCharPos; DERCharPos++)
    {
        if (TagMatched(pszGroupNumber, pszElementNumber, 
                        DataSetRegistryChar[DERCharPos].pszGroupNumber, 
                        DataSetRegistryChar[DERCharPos].pszElementNumber) == 1)
        {
            *VR = DataSetRegistryChar[DERCharPos].VR;
            *TagName = DataSetRegistryChar[DERCharPos].pszName;
            *VM = DataSetRegistryChar[DERCharPos].VM;
            return 1;
        };
    };

    // not found
    *VR = VR_ERROR;
    *TagName = NULL;
    *VM = VM_Unknown;
    return 0;
};

// 找Sequence Delimitation Item (FFFE,E0DD)，并更新tag value length (TagLengthLong); 找到后，复位buffer指针到初始点
// 用在tag with Undefined Length (不适用SQ with Undefined Length)
// return:
// -19: error - failed to find Sequence Delimitation Item (FFFE, E0DD)
// 0: success
int GetUndefinedLengthSQ(unsigned char **Pos, unsigned char *EOFPos, DR_OUT_ unsigned int *TagLengthLong)
{
    *TagLengthLong = 0;
    unsigned short tempGN, tempEN;
    while (1)                                                                               // try find Sequence Delimitation Item (FFFE,E0DD)
    {
        if ((BufferRead(&tempGN, 2, Pos, EOFPos) == 0) &&                                   // 读4个字节
            (BufferRead(&tempEN, 2, Pos, EOFPos) == 0))
        {
            if ((tempGN == 0xFFFE) && (tempEN == 0xE0DD))
            {                                                                               // found (FFFE,E0DD)
                *Pos -= (*TagLengthLong + 4);                                               // 回退文件指针到Value起始处
                return 0;
            }
            else                                                                            // not (FFFE,E0DD)
            {
                *Pos -= 3;                                                                  // 回退3个字节
                (*TagLengthLong)++;                                                         // 读了1个字节了，计数
            };
        }
        else
            return -19;                                                                     // BufferRead失败, stop
    };
};

// 读取Pixel Data (07FE,0010) with Undefined Length, 并更新链表
// 从*DcmFile文件指针开始读，文件指针在Value Field起始处(First Item), 生成的链表第1个node的地址存在*List里
// 说明：
// (07FE,0010)的Value Length可以是:
//   (1) Explicit for native pixel data: value是字节流
//   (2) Undefined Length for encapsulated pixel data: value由Items组成
//      => 每个Item是(FFFE,E000) & Explicit Length
//      => 结尾是Sequence Delimitation Item (FFFE,E0DD): 8 bytes
// return
// < 0: error
// 0: success
int GetULPixelDataItems(unsigned char **Pos, unsigned char *EOFPos, DR_OUT_ PElementsListNode *List)
{
    *List = NULL;
    // while使用的临时变量
    PElementsListNode Node = NULL, PreviosNode = NULL;
    unsigned short TagGroupNumber, TagElementNumber;
    unsigned int TagLengthLong;
    // start to read
    while (1)
    {
        // 判断是否需要返回，1种可能:
        // Undefined Length: 读到Sequence Delimitation Item (FFFE,E0DD)
        if ((BufferRead(&TagGroupNumber, 2, Pos, EOFPos) != 0) ||                               // get Group Number: 2 bytes
            (BufferRead(&TagElementNumber, 2, Pos, EOFPos) != 0))                               // get Element Number: 2 bytes
            return -24;                                                                         // 读取失败, stop
        // go on
        if ((TagGroupNumber != 0xFFFE) ||                                                       // Item tag只能是(FFFE,E000) or (FFFE,E0DD)
            ((TagElementNumber != 0xE000) && (TagElementNumber != 0xE0DD)))                     // 否则，error, stop
            return -25;
        // 读ValueLength，把文件指针推进4 bytes
        if (BufferRead(&TagLengthLong, 4, Pos, EOFPos) != 0)                                    // get Value Length
            return -14;                                                                         // 读取Value Length失败, stop
        // here, TagLengthLong = Value Length
        
        // create a new node
        PreviosNode = Node;
        Node = (PElementsListNode)malloc(sizeof(struct ElementsListNode));
        if (Node == NULL)                                                                       // malloc failed, stop
            return -15;
        Node->Element.GroupNumber = TagGroupNumber;
        Node->Element.ElementNumber = TagElementNumber;
        Node->Element.VR = VR_NoVR;
        Node->Element.VM = VM_Single;
        Node->Element.ValueLength = TagLengthLong;
        Node->NextNode = NULL;
        Node->SubNode = NULL;
        Node->Element.ValueField = NULL;
        if (TagElementNumber == 0xE000)
            Node->Element.pszTagName = TagNameFFFEE000;
        else
            // TagElementNumber must = 0xE0DD
            Node->Element.pszTagName = TagNameFFFEE0DD;

        // each Item Value is octet stream
        if (TagLengthLong > 0)
        {
            Node->Element.ValueField = *Pos;
            if (BufferSkip(TagLengthLong, Pos, EOFPos) != 0)                                    // 跳到下一个item
            {                                                                                   // if error -> stop
                free(Node);
                Node = NULL;
                return -16;
            };
        };
        // 更新链表
        if (*List == NULL)
            *List = Node;
        else
            PreviosNode->NextNode = Node;

        // 如果是(FFFE,E0DD)就返回, 注意这时buffer指针已经越过了(FFFE,E0DD)的4字节value field，指向下一个tag（或buffer末尾）
        if ((TagGroupNumber == 0xFFFE) && (TagElementNumber == 0xE0DD))
            return 0;
        
        // here, 文件指针指向下一个Item (FFFE,E000)
    }; // while
};

// 读取SQ并更新链表
// buffer指针在SQ的Value Field起始处(First Item), 生成的链表第1个node的地址存在*List里
// 参数：
// SQLength: 可以是Explicit Length or Undefined Length (0xFFFFFFFF)
// return
// -25: error - item Tag不是 (FFFE,E000), (FFFE,E0DD)
// -24: error - failed to read tag
// 0: success
int GetSQItems(unsigned char **Pos, unsigned char *EOFPos, unsigned int SQLength, DR_INOUT_ PElementsListNode *List,
                    StorageSOPClassName FileStorageSOPClass, TransferSyntaxName FileTransferSyntax, ImplicitOrExplicitVR FileIEVR)
{
    // 记录指针起点
    unsigned char *StartPos = *Pos;
    
    int ret = 0;
    *List = NULL;
    // while使用的临时变量
    PElementsListNode Node = NULL, PreviosNode = NULL;
    unsigned short TagGroupNumber, TagElementNumber;
    unsigned int TagLengthLong;
    // start to read
    while (1)
    {
        // 判断是否需要返回，2种可能:
        // (1) Explicit Length: 读完
        // (2) Undefined Length: 读到Sequence Delimitation Item (FFFE,E0DD)
        
        // Explicit Length
        if (SQLength != UNDEFINED_LENGTH)
        {
            if (*Pos - StartPos == SQLength)                                                    // 读完DataSetLength，返回
                return 0;
            if (*Pos - StartPos > SQLength)                                                     // if error, stop
                return -27;
        };
        // go on
        if ((BufferRead(&TagGroupNumber, 2, Pos, EOFPos) != 0) ||                               // get Group Number: 2 bytes
            (BufferRead(&TagElementNumber, 2, Pos, EOFPos) != 0))                               // get Element Number: 2 bytes
            return -24;                                                                         // 读取失败, stop
        // go on
        if ((TagGroupNumber != 0xFFFE) ||                                                       // Item tag只能是(FFFE,E000) or (FFFE,E0DD)
            ((TagElementNumber != 0xE000) && (TagElementNumber != 0xE0DD)))                     // stop
            return -25;
        // 读ValueLength，把文件指针推进4 bytes
        if (BufferRead(&TagLengthLong, 4, Pos, EOFPos) != 0)                                    // get Value Length
            return -14;                                                                         // 读取Value Length失败, stop
        // here, TagLengthLong = Value Length
        
        // create a new node
        PreviosNode = Node;
        Node = (PElementsListNode)malloc(sizeof(struct ElementsListNode));
        if (Node == NULL)                                                                       // malloc failed, stop
            return -15;
        Node->Element.GroupNumber = TagGroupNumber;
        Node->Element.ElementNumber = TagElementNumber;
        Node->Element.VR = VR_NoVR;
        Node->Element.VM = VM_Single;
        Node->Element.ValueLength = TagLengthLong;
        Node->NextNode = NULL;
        Node->SubNode = NULL;
        Node->Element.ValueField = NULL;                                                        // 不需要赋值: data都在SubNode里
        if (TagElementNumber == 0xE000)
            Node->Element.pszTagName = TagNameFFFEE000;
        else
            // TagElementNumber must = 0xE0DD
            Node->Element.pszTagName = TagNameFFFEE0DD;

        // 更新链表
        if (*List == NULL)
            *List = Node;
        else
            PreviosNode->NextNode = Node;

        // each Item Value is a Data Set，从Data Set读取的tag作为SubNode
        ret = GetDataElements(Pos, EOFPos, TagLengthLong, &(Node->SubNode), FileStorageSOPClass, FileTransferSyntax, FileIEVR);
        if (ret < 0)
            return ret;

        // 如果是Sequence Delimitation Item (FFFE,E0DD)就返回, 注意这时buffer指针已经越过了(FFFE,E0DD)的4字节value field，指向下一个tag
        if ((TagGroupNumber == 0xFFFE) && (TagElementNumber == 0xE0DD))
            return 0;

        // here, 文件指针指向下一个Item (FFFE,E000)
    }; // while
};

// 从*DcmFile文件指针开始读Data Set，生成的链表第1个node的地址存在*List里
// 参数：
// DataSetLength: 可以是Explicit Length or Undefined Length (0xFFFFFFFF)
// return
// -28: error - fseek failed to skip (FFFE,E00D)
// -27: error - FilePos - FileStartPos > DataSetLength
// -23: error - fseek failed to skip (FFFC,FFFC)
// -22: error - ftell failed
// 0: success
int GetDataElements(unsigned char **Pos, unsigned char *EOFPos, unsigned int DataSetLength, DR_INOUT_ PElementsListNode *List,
                    StorageSOPClassName FileStorageSOPClass, TransferSyntaxName FileTransferSyntax, ImplicitOrExplicitVR FileIEVR)
{
    // 记录文件指针起点
    unsigned char *StartPos = *Pos;
    
    int ret = 0;
    *List = NULL;
    // while使用的临时变量
    PElementsListNode Node = NULL, PreviosNode = NULL;
    unsigned short TagGroupNumber, TagElementNumber;
    char pszTagGroupNumber[5], pszTagElementNumber[5], *pszTagName = NULL;
    unsigned char buffer[2];
    ValueRepresentation TagVR, RegistryVR;
    ValueMultiplicity RegistryVM;
    unsigned int TagLengthLong;
    unsigned short TagLengthShort;
    // start to read
    while (1)
    {
        // 判断是否需要返回，4种可能:
        // (1) Explicit Length: 读完
        // (2) Undefined Length: 读到文件末尾
        // (3) Undefined Length: 读到(FFFC,FFFC) -> a normal tag VR=OB
        // (4) Undefined Length: 读到Item Delimitation Item (FFFE,E00D) - Data Set in a SQ Item -> 8 bytes FFFE E00D 0000 0000
        
        // 返回可能(1) Explicit Length
        if (DataSetLength != UNDEFINED_LENGTH)
        {
            if (*Pos - StartPos == DataSetLength)                                        // 读完DataSetLength，返回
                return 0;
            if (*Pos - StartPos > DataSetLength)                                         // error, stop
                return -27;
        };

        // 读tag; 返回可能(2) Undefined Length: 读到文件末尾
        if ((BufferRead(&TagGroupNumber, 2, Pos, EOFPos) != 0) ||                               // get Group Number: 2 bytes
            (BufferRead(&TagElementNumber, 2, Pos, EOFPos) != 0))                               // get Element Number: 2 bytes
            return 0;                                                                           // 读到文件末尾，返回
        
        sprintf(pszTagGroupNumber, "%04X", TagGroupNumber);
        sprintf(pszTagElementNumber, "%04X", TagElementNumber);
        if (IsInDataRegistry(TagGroupNumber, TagElementNumber,
                            pszTagGroupNumber, pszTagElementNumber, &RegistryVR, &pszTagName, &RegistryVM) == 0)
        {                                                                                       // GN+EN组合不在Data Registry里
            return -11;                                                                         // stop
        };
        // 读取了正确的Element tag，并且在registry查到了对应的RegistryVR，或确认为Private tag
        
        // 给TagVR赋值
        // RegistryVR is VR_Blank
        if (RegistryVR == VR_Blank)                                                             // RegistryVR is VR_Blank, 无法处理, stop
            return -18;
        // RegistryVR is VR_NoVR: 读到(FFFE,E00D) Item Delimitation Item; 不可能读到(FFFE,E000), (FFFE,E0DD)
        else if (RegistryVR == VR_NoVR)
        {
            TagVR = VR_NoVR;
        }
        // RegistryVR is other valid VR
        // if Explicit, then 验证VR
        else if (FileIEVR == ExplicitVR)
        {
            if (BufferRead(buffer, 2, Pos, EOFPos) != 0)                                        // get VR: 2 bytes
                return -12;                                                                     // 读取VR失败, stop
            TagVR = GetVR(buffer);
            if ((TagVR == VR_ERROR) ||                                                          // 读出的VR不在VR Registry
                (VRMatched(TagVR, RegistryVR) == 0))                                            // 读出的VR 和 Registry里该GN+EN的VR不符合
                ret = -10;                                                                      // stop
        }
        else                                                                                    // Implicit VR
        {
            // 根据PS3.5 Page 97 Implicit VR定义, 把Registry里根据GN+EN查询到的待明确的RegistryVR明确为正确的VR值，用于后续decoding
            TagVR = GetImplicitVR(pszTagGroupNumber, pszTagElementNumber, RegistryVR);
        };
        // here, TagGroupNumber+TagElementNumber正确(Implicit)
        // or TagGroupNumber+TagElementNumber+TagVR正确(Explicit)

        // 确定ValueLength
        // RegistryVR is VR_NoVR: 读到(FFFE,E00D) Item Delimitation Item
        // 不可能是(FFFE,E000): 在SQ tag里
        // 不可能是(FFFE,E0DD): 跟在Undefined Length tag value后面，在下面单独读取处理了
        if (RegistryVR == VR_NoVR)
        {
            if (BufferSkip(4, Pos, EOFPos) != 0)
                return -14;
            TagLengthLong = 0;                                                                  // FFFE E00D 0000 0000
        }
        else if (FileIEVR == ExplicitVR)
        {                        
            if (IsExplicitSpecialVR(TagVR))
            {
                if ((BufferSkip(2, Pos, EOFPos) != 0) ||                                        // skip 2 bytes
                    (BufferRead(&TagLengthLong, 4, Pos, EOFPos) != 0))                          // get Value Length
                    return -14;                                                                 // 读取Value Length失败, stop
                // now TagLengthLong = Value Length
            }
            else
            {
                if (BufferRead(&TagLengthShort, 2, Pos, EOFPos) != 0)                           // get Value Length
                    return -14;                                                                 // 读取Value Length失败, stop
                // now TagLengthShort = Value Length
                // pass it to TagLengthLong - alwasy use TagLengthLong
                TagLengthLong = TagLengthShort;
            };
            // Tag value length 使用 TagLengthLong的值
        }
        else                                                                                    // Implicit VR
        {
            if (BufferRead(&TagLengthLong, 4, Pos, EOFPos) != 0)                                // get Value Length
                return -14;                                                                     // 读取Value Length失败, stop
        }
        // here, TagLengthLong = Value Length
        // if TagLengthLong == Undefined Length, 可能有3种情况，仅对如下情况(3), 计算Value Length正确的值：
        //  (1) SQ tag:                     不做处理，由GetSQItems()处理
        //  (2) pixel data (7FE0,0010):     不做处理, 由GetULPixelDataItems()处理
        //  (3) 其他tag:                    计算Value Length正确的值，并通过标记skip8bytes=1在后面处理后面紧跟的(FFFE,E0DD)
        // part3.5 page 52:
        // for VRs of OB, OD, OF, OL, OV, OW, SQ and UN, if the Value Field has an Explicit Length, then the Value Length Field shall​
        // contain a value equal to the length (in bytes) of the Value Field, otherwise, the Value Field has an Undefined Length and a Sequence​
        // Delimitation Item marks the end of the Value Field
        // 是否在读取Value Field后要把buffer指针往后额外移动8个字节(FFFE E0DD 0000 0000)
        int skip8bytes = 0;
        if ((TagLengthLong == UNDEFINED_LENGTH) &&                                              // Undefined Length
            (TagVR != VR_SQ) &&                                                                 // 不是SQ
            ((TagGroupNumber != 0x7FE0) && (TagElementNumber != 0x0010)))                       // 不是Pixel Data
        {
            ret = GetUndefinedLengthSQ(Pos, EOFPos, &TagLengthLong);                            // 找Sequence Delimitation Item，确定value length
            if (ret < 0)                                                                        // if error, stop
                return ret;
            skip8bytes = 1;                                                                     // 找到了
        };
        // here, ret must = 0，buffer指针位于Value Field起点
        // if SQ tag, TagLengthLong = Explicit Length or Undefined Length (未修改的原始数值)
        // if pixel data, TagLengthLong = Explicit Length or Undefined Length (未修改的原始数值)
        // if others，TagLengthLong为真实的长度

        // create a new node
        PreviosNode = Node;
        Node = (PElementsListNode)malloc(sizeof(struct ElementsListNode));
        if (Node == NULL)                                                                       // malloc failed, stop
            return -15;
        Node->Element.GroupNumber = TagGroupNumber;
        Node->Element.ElementNumber = TagElementNumber;
        Node->Element.VR = TagVR;
        Node->Element.VM = RegistryVM;
        Node->Element.ValueLength = TagLengthLong;
        Node->Element.pszTagName = pszTagName;
        Node->NextNode = NULL;
        Node->SubNode = NULL;
        if ((TagLengthLong == 0) ||                                                             // Value Length=0，则ValueField=NULL
            (TagVR == VR_SQ) ||                                                                 // SQ: data在Item里，下面处理
            ((TagGroupNumber == 0x7FE0) && (TagElementNumber == 0x0010) &&                      // Undefined Length Pixel Data: data在Item里
             (TagLengthLong == UNDEFINED_LENGTH)))                                              // 下面处理
        {
            Node->Element.ValueField = NULL;
        }
        else                                                                                    // TagLengthLong > 0, 非SQ，非ULPD，读Value Field
        {
            Node->Element.ValueField = *Pos;
            if (BufferSkip(TagLengthLong, Pos, EOFPos) != 0)                                    // 跳过value field
            {                                                                                   // if error, stop
                free(Node);
                Node = NULL;
                return -16;
            };
        };

        // 先更新链表，再创建SubNode
        if (*List == NULL)
            *List = Node;
        else
            PreviosNode->NextNode = Node;

        // 创建SubNode

        // 当前tag is SQ: 创建SubNode(每个Item，包括(FFFE,E0DD)), 并移动文件指针到SQ之后的tag
        if (TagVR == VR_SQ)
        {
            // here, 文件指针在SQ的Value Field起始处(First Item)
            ret = GetSQItems(Pos, EOFPos, TagLengthLong, &(Node->SubNode), FileStorageSOPClass, FileTransferSyntax, FileIEVR);
            if (ret < 0)
                return ret;
        }
        // 当前tag is Pixel Data & Undefined Length: 创建SubNode(每个Item，包括(FFFE,E0DD)), 并移动文件指针到SQ之后的tag
        // 也是SQ形式，但每个Item的Value是字节流 & Explicit Length
        else if ((TagGroupNumber == 0x7FE0) && (TagElementNumber == 0x0010) && (TagLengthLong == UNDEFINED_LENGTH))
        {
            ret = GetULPixelDataItems(Pos, EOFPos, &(Node->SubNode));
            if (ret < 0)
                return ret;
        }
        // 是否上面设定了要skip8bytes
        // 即, 当前tag is Undefined Length tag (not SQ, not pixel data)
        // 上面已计算了实际的value length，并已据此赋值 Node->Element.ValueLength & Node->Element.ValueField
        // 该tag value field后紧跟Sequence Delimitation Item (FFFE,E0DD), 将其设置为1个SubNode
        else if (skip8bytes == 1)
        {
            Node->SubNode = (PElementsListNode)malloc(sizeof(struct ElementsListNode));
            if (Node->SubNode == NULL)                                                          // malloc failed, stop
                return -15;
            Node->SubNode->Element.GroupNumber = 0xFFFE;
            Node->SubNode->Element.ElementNumber = 0xE0DD;
            Node->SubNode->Element.VR = VR_NoVR;
            Node->SubNode->Element.VM = VM_Single;
            Node->SubNode->Element.ValueLength = 0;
            Node->SubNode->Element.pszTagName = TagNameFFFEE0DD;
            Node->SubNode->Element.ValueField = NULL;
            Node->SubNode->NextNode = NULL;
            Node->SubNode->SubNode = NULL;
        };
        
        // 返回可能(3) Undefined Length: 读到(FFFC,FFFC) -> a normal tag VR=OB
        // 当前tag is (FFFC,FFFC)
        if ((TagGroupNumber == 0xFFFC) && (TagElementNumber == 0xFFFC))
            return 0;                                                                           // 读到(FFFC,FFFC)，返回

        // 返回可能(4) Undefined Length: 读到Item Delimitation Item (FFFE,E00D) - Data Set in a SQ Item -> 8 bytes FFFE E00D 0000 0000
        // 当前tag is (FFFE, E00D) Item Delimitation Item
        if ((TagGroupNumber == 0xFFFE) && (TagElementNumber == 0xE00D))
            return 0;                                                                           // 读到(FFFE,E00D)，返回

        // 返回读取下一个tag
    }; // while
};

// 读取dcm文件，创建并返回链表(PElementsListNode)和文件信息(FileInfo)
// 是否只读meta - MetaOnly: 1 yes, 0 no
// 调用方必须手工调用FreeDcmFileElements和FreeDcmFileInfo来释放内存
// return
// -30: error - failed to read the dcm file to buffer
// -29: error - failed to read tag
// -21: error - invalid TransferSyntaxUID
// -20: error - invalid StorageSOPClassUID
// -19: error - failed to find Sequence Delimitation Item (FFFE, E0DD)
// -18: error - blank VR (0008,0202), (0018,9445), (0028,0020)
// -17: error - fseek(-4): failed back to Data Set starting-point
// -16: error - failed to read Value Field
// -15: error - malloc failed
// -14: error - failed to read Value Length
// -13: error - no StorageSOPClass or TransferSyntax or VRIE when entering Data Set section
// -12: error - failed to read VR
// -11: error - can not find Group Number + Element Number in registry
// -10: error - invalid VR
// -3: not a valid DICOM file
// -2: error reading Meta section - fseek(128 bytes) failed
// -1: failed to open the file
// 0: no error
int GetDcmFileElements(const char *szDcmFileName, int MetaOnly, DR_OUT_ PElementsListNode *List, DR_OUT_ PDcmFileInfo *FileInfo)
{
    FILE *DcmFile = NULL;
    unsigned char *DcmFileBuffer = NULL, *Pos = NULL, *EOFPos = NULL;
    size_t DcmFileSize = 0;
    int ret = 0;

    //
    MERPos = 0;
    DERPos = 0;
    DERCharPos = 0;
    *List = NULL;
    *FileInfo = NULL;

    // open file    
    DcmFile = fopen(szDcmFileName, "rb");                                                       // svc #0x80 398
    if (DcmFile == NULL)                                                                        // error - fopen failed, stop
        return -1;

    // read to buffer
    if (fseek(DcmFile, 0, SEEK_END) == 0)
    {
        long long temp = ftell(DcmFile);
        if (temp == -1)
            ret = -30;                                                                          // error - failed to read to buffer
        else
        {
            DcmFileSize = (size_t)temp;
            if (DcmFileSize < 128 + 4)
                ret = -3;                                                                       // error - not a valid DICOM file
            else
            {                                                                                   // file size > 128 + 4 bytes
                if (fseek(DcmFile, 0, SEEK_SET) == 0)                                           // back to file begining
                {
                    DcmFileBuffer = malloc(DcmFileSize);
                    if (DcmFileBuffer == NULL)
                        ret = -15;                                                              // error - malloc failed
                    else
                    {
                        Pos = DcmFileBuffer + 128;                                              // move Pos to "DICM"
                        EOFPos = DcmFileBuffer + DcmFileSize;
                        if (DcmFileSize != fread(DcmFileBuffer, 1, DcmFileSize, DcmFile))
                            ret = -30;                                                          // error - failed to read to buffer
                    };
                }
                else
                    ret = -30;                                                                  // error - failed to read to buffer
            };
        };
    }
    else
        ret = -30;                                                                              // error - failed to read to buffer

    fclose(DcmFile);

    // check if error or not
    if (ret < 0)
    {
        if (DcmFileBuffer != NULL)
            free(DcmFileBuffer);
        return ret;                                                                             // if error, stop
    };

    // here ret = 0; valid: DcmFileBuffer, Pos(->"DICM"), EOFPos, DcmFileSize

    unsigned char buffer[4];
    if ((BufferRead(buffer, 4, &Pos, EOFPos) == 0) && 
        (buffer[0] == 'D') && (buffer[1] == 'I') && (buffer[2] == 'C') && (buffer[3] == 'M'))  // if not, return -3 error
    {
        // valid DICOM file, then read Elements
        // 文件属性
        StorageSOPClassName FileStorageSOPClass = MissingStorageSOPClass;
        TransferSyntaxName FileTransferSyntax = UnknownTransferSyntax;
        ImplicitOrExplicitVR FileIEVR = UnknownImplicitOrExplicitVR;
        // while使用的临时变量
        PElementsListNode Node = NULL, PreviosNode = NULL;
        unsigned short TagGroupNumber, TagElementNumber;
        char szTagGroupNumber[5], szTagElementNumber[5];
        char *pszTagName = NULL;
        ValueRepresentation TagVR, RegistryVR;
        ValueMultiplicity RegistryVM;
        unsigned int TagLengthLong;
        unsigned short TagLengthShort;
        // start to read
        while (1)
        {
            if ((BufferRead(&TagGroupNumber, 2, &Pos, EOFPos) != 0) ||                  // get Group Number: 2 bytes
                (BufferRead(&TagElementNumber, 2, &Pos, EOFPos) != 0))                  // get Element Number: 2 bytes
            {
                ret = -29;
                break;
            };
            sprintf(szTagGroupNumber, "%04X", TagGroupNumber);
            sprintf(szTagElementNumber, "%04X", TagElementNumber);
            // 验证读取的tag是否在Meta registry
            if ((TagGroupNumber != 0x0002) ||
                (IsInMetaRegistry(TagGroupNumber, TagElementNumber, &RegistryVR, &pszTagName, &RegistryVM) == 0))
            {                                                                           // GN+EN组合不在Meta Registry里
                if (IsInDataRegistry(TagGroupNumber, TagElementNumber,
                                    szTagGroupNumber, szTagElementNumber, &RegistryVR, &pszTagName, &RegistryVM) == 0)
                {
                    ret = -11;                                                          // GN+EN组合也不在Data Registry里，出错了
                    break;                                                              // stop
                }
                else
                {                                                                       // 没有出错的读进了Data Set section
                    if ((FileStorageSOPClass == MissingStorageSOPClass) ||
                        (FileTransferSyntax == UnknownTransferSyntax) ||
                        (FileIEVR == UnknownImplicitOrExplicitVR))                      // 读进Data Set section的时候，必须已赋值：
                    {                                                                   // StorageSOPClass & TransferSyntax & VRIE
                        ret = -13;                                                      // 否则 stop
                        break;
                    };
                    Pos -= 4;                                                           // 回退4个字节到Data Set起点
                    // here, 无error的完成读取Meta section，退出while
                    // ret must = 0, 链表List的最后一个node地址=Node
                    break;
                };
            };
            // here, 读取了正确的Meta Element tag，并且在Meta registry查到了对应的RegistryVR
            // 读VR - part10 page33 Meta区所有tag都是Explicit VR Little Endian
            if (BufferRead(buffer, 2, &Pos, EOFPos) != 0)                               // get VR: 2 bytes
            {                                                                           // 读取VR失败
                ret = -12;                                                              // stop
                break;
            };
            TagVR = GetVR(buffer);
            if ((TagVR == VR_ERROR) ||                                                  // 读出的VR不在VR Registry
                (TagVR != RegistryVR))                                                  // 读出的VR和RegistryVR不同
            {                                                                           // stop
                ret = -10;
                break;
            };
            // here, TagGroupNumber+TagElementNumber+TagVR正确(Explicit)
            // 读ValueLength
            if (IsExplicitSpecialVR(TagVR))
            {
                if ((BufferSkip(2, &Pos, EOFPos) != 0) ||                               // skip 2 bytes
                    (BufferRead(&TagLengthLong, 4, &Pos, EOFPos) != 0))                 // get Value Length
                {                                                                       // 读取Value Length失败
                    ret = -14;                                                          // stop
                    break;
                };
                // now TagLengthLong = Value Length
            }
            else
            {
                if (BufferRead(&TagLengthShort, 2, &Pos, EOFPos) != 0)                  // get Value Length
                {                                                                       // 读取Value Length失败
                    ret = -14;                                                          // stop
                    break;
                };
                // now TagLengthShort = Value Length
                // pass it to TagLengthLong - alwasy use TagLengthLong
                TagLengthLong = TagLengthShort;
            };
            // here, TagLengthLong = Value Length

            // 处理一些tag可能有Undefined Length的情况（这里，tag不可能是SQ/Pixel Data - Meta section没有SQ/Pixel Data）
            // 如果是Undefined Length，需要获取value length(TagLengthLong)正确的长度 - 寻找Sequence Delimitation Item (FFFE,E0DD)
            // part3.5 page 52:
            // for VRs of OB, OD, OF, OL, OV, OW, SQ and UN, if the Value Field has an Explicit Length, then the Value Length Field shall​
            // contain a value equal to the length (in bytes) of the Value Field, otherwise, the Value Field has an Undefined Length and a Sequence​
            // Delimitation Item marks the end of the Value Field
            // skip8bytes: 是否在后续读取Value Field后要把buffer指针往后额外移动8个字节(FF FE E0 DD 00 00 00 00)
            int skip8bytes = 0;
            if (TagLengthLong == UNDEFINED_LENGTH)
            {
                ret = GetUndefinedLengthSQ(&Pos, EOFPos, &TagLengthLong);               // 寻找Sequence Delimitation Item (FFFE,E0DD)
                if (ret < 0)                                                            // 并获取真实的value length
                    break;                                                              // if error, stop
                skip8bytes = 1;                                                         // 找到了
            };
            // here, ret must = 0，而且TagLengthLong已修改为真实的value length长度，buffer指针位于Value Field起点
            
            // 读Value Field（需要考虑skip8bytes的情况）
            // create a new node并加入链表List
            PreviosNode = Node;
            Node = (PElementsListNode)malloc(sizeof(struct ElementsListNode));
            if (Node == NULL)                                                           // malloc failed
            {                                                                           // stop
                ret = -15;
                break;
            };
            // fill the Value
            if (TagLengthLong == 0)                                                     // Value Length=0则置ValueField=NULL
                Node->Element.ValueField = NULL;
            else
            {
                Node->Element.ValueField = Pos;                                         // Value Length != 0，记录地址
                if (BufferSkip(TagLengthLong, &Pos, EOFPos) != 0)                       // 跳过value到下一个tag
                {                                                                       // if error, stop
                    ret = -16;
                    free(Node);
                    break;
                };
            };                            
            if (skip8bytes == 1)                                                        // 是否需要再跳过Sequence Delimitation Item (FFFE,E0DD)
            {
                if (BufferSkip(8, &Pos, EOFPos) != 0)                                   // skip 8 bytes
                {                                                                       // if error, stop
                    ret = -16;
                    free(Node);
                    break;                                
                };
            };
            // fill the node
            Node->Element.GroupNumber = TagGroupNumber;
            Node->Element.ElementNumber = TagElementNumber;
            Node->Element.VR = TagVR;
            Node->Element.VM = RegistryVM;
            Node->Element.ValueLength = TagLengthLong;
            Node->Element.pszTagName = pszTagName;
            Node->NextNode = NULL;
            Node->SubNode = NULL;
            if (*List == NULL)
                *List = Node;
            else
                PreviosNode->NextNode = Node;
            // node creation complete

            // 记录文件信息
            // record the StorageSOPClassUID
            if ((FileStorageSOPClass == MissingStorageSOPClass) &&                      // 如果还没有记录FileStorageSOPClass
                ((TagGroupNumber == 0x0002) && (TagElementNumber == 0x0002)))           // 如果这个tag刚好是StorageSOPClassUID
            {
                FileStorageSOPClass = PrivateStorageSOPClass;
                for (int i = 0; i < STORAGESOPCLASSLISTCOUNT; i++)                      // 在StorageSOPClassList里找读取的UID
                {
                    // 对VR=UI: 若UID的长度为奇数，会最后加1个byte 0；若为偶数，不补0
                    // 因此，比对的时候要考虑这个情况
                    int found = 1;
                    unsigned int j;
                    for (j = 0; (j < TagLengthLong) && (Node->Element.ValueField[j] != 0); j++)
                    {
                        if ((StorageSOPClassList[i].pszClassUID[j] == 0) ||             // 读到ClassList该UID的末尾了还没有match
                            (StorageSOPClassList[i].pszClassUID[j] != Node->Element.ValueField[j])) // 字符不匹配
                        {                                                               // 试下一个
                            found = 0;
                            break;
                        };
                    };
                    if ((found == 1) &&                                                 // node UID每个字符都能匹配到list UID
                        (StorageSOPClassList[i].pszClassUID[j] == 0))                   // list UID也到了最后一个字符
                    {
                        FileStorageSOPClass = StorageSOPClassList[i].ClassName;         // 在list找到了相同的UID，记下来
                        break;
                    };
                };
            };
            // record the TransferSyntaxUID, Implicit/Explict
            if ((FileTransferSyntax == UnknownTransferSyntax) &&                        // 如果还没有记录FileTransferSyntax
                ((TagGroupNumber == 0x0002) && (TagElementNumber == 0x0010)))           // 如果这个tag刚好是TransferSyntaxUID
            {
                for (int i = 0; i < TRANSFERSYNTAXLISTCOUNT; i++)
                {
                    // 逻辑同上
                    int found = 1;
                    unsigned int j;
                    for (j = 0; (j < TagLengthLong) && (Node->Element.ValueField[j] != 0); j++)
                    {
                        if ((TransferSyntaxList[i].pszSyntaxUID[j] == 0) ||
                            (TransferSyntaxList[i].pszSyntaxUID[j] != Node->Element.ValueField[j]))
                        {                                                               // 试下一个
                            found = 0;
                            break;
                        };
                    };
                    if ((found == 1) &&
                        (TransferSyntaxList[i].pszSyntaxUID[j] == 0))
                    {
                        FileTransferSyntax = TransferSyntaxList[i].SyntaxName;
                        FileIEVR = TransferSyntaxList[i].ImorExVR;
                        break;
                    };
                };
                if (FileTransferSyntax == UnknownTransferSyntax)                        // 在list没找到相同的UID
                {                                                                       // error, stop
                    ret = -21;
                    break;
                };
            };
            // back to loop, to check the next tag
        };  // while loop

        if (FileStorageSOPClass == MissingStorageSOPClass)
        {
            ret = -20;
        };

        // here, ret could be:
        // < 0: error to read Meta section -> quit
        // = 0: success to read Meta section -> go on

        // read Data Set
        if ((ret == 0) && (MetaOnly == 0))
        {
            // read Data Set
            // Node是链表List里最后一个node
            ret = GetDataElements(&Pos, EOFPos, UNDEFINED_LENGTH, &(Node->NextNode), FileStorageSOPClass, FileTransferSyntax, FileIEVR);
        };

        // here, ret could be:
        // < 0: error to read File and stopped at the error
        // = 0: success to read the entire File

        // 填写File Information
        if (ret == 0)
        {
            *FileInfo = (PDcmFileInfo)malloc(sizeof(DcmFileInfo));
            (*FileInfo)->SOPClass = FileStorageSOPClass;
            (*FileInfo)->TransferSyntax = FileTransferSyntax;
            (*FileInfo)->DcmFileBuffer = DcmFileBuffer;
        };
    }  // if: read "DICM"
    else
        ret = -3;                                                                       // 读"DICM"出错，停止读取文件，返回

    return ret;    
};

// 链表
void FreeDcmFileElements(PElementsListNode List)
{
    PElementsListNode CurrentListNode = List, NextListNode = NULL;
    while (CurrentListNode != NULL)
    {
        NextListNode = CurrentListNode->NextNode;
        if (CurrentListNode->SubNode != NULL)
            FreeDcmFileElements(CurrentListNode->SubNode);
        free(CurrentListNode);
        CurrentListNode = NextListNode;
    };
};

// 文件信息
void FreeDcmFileInfo(PDcmFileInfo FileInfo)
{
    if (FileInfo != NULL)
    {
        if (FileInfo->DcmFileBuffer != NULL)
            free(FileInfo->DcmFileBuffer);
        free(FileInfo);
    };
};

// raw pixel data信息
void FreeDcmPixelDataInfo(PPixelData PixelDataInfo)
{
    if (PixelDataInfo->PixelDateItemCount > 0)
    {
        PPixelDataItem CurrentItem = PixelDataInfo->PixelItem, NextItem = NULL;
        while (CurrentItem != NULL)
        {
            NextItem = CurrentItem->NextPixelDataItem;
            free(CurrentItem);
            CurrentItem = NextItem;
        };
    };
    free(PixelDataInfo);
};

// 读取给定的DICOM file，创建DcmFile结构体并返回其地址
// return
// success: 创建的DcmFile结构体的地址
// fail: NULL
PDcmFile ReadDcmFile(const char *szDcmFileName, int MetaOnly)
{
    PDcmFile drFile = (PDcmFile)malloc(sizeof(DcmFile));
    if (drFile == NULL)
        return NULL;
    int ret = GetDcmFileElements(szDcmFileName, MetaOnly, &((*drFile).List), &((*drFile).FileInfo));
    printf("ret=%d\n", ret);
    if (ret == 0)
    {
        if (drFile->List != NULL)
        {
            // get pixel data info
            // 只在node链表的第一级寻找: 假定(7FE0,0010)只在第一级
            drFile->PixelDataInfo = malloc(sizeof(PixelData));
            drFile->PixelDataInfo->PixelDateItemCount = -1;  // 假定没有pixel data
            drFile->PixelDataInfo->PixelItem = NULL;
            PElementsListNode Node = drFile->List;
            while (Node != NULL)
            {
                if ((Node->Element.GroupNumber == 0x7FE0) && (Node->Element.ElementNumber == 0x0010))
                {
                    if (Node->Element.ValueLength == UNDEFINED_LENGTH)
                    {
                        drFile->PixelDataInfo->PixelDateItemCount = 0;
                        drFile->PixelDataInfo->PixelItem = NULL;
                        PPixelDataItem *Item = &(drFile->PixelDataInfo->PixelItem);
                        if (Node->SubNode == NULL)
                            break;
                        Node = Node->SubNode;
                        // Item tag只能是(FFFE,E000):读pixel or (FFFE,E0DD):返回
                        while (Node != NULL)
                        {
                            if ((Node->Element.GroupNumber != 0xFFFE) && (Node->Element.ElementNumber != 0xE0DD))
                                break;
                            if ((Node->Element.GroupNumber == 0xFFFE ) && (Node->Element.ElementNumber == 0xE000))
                            {
                                if (Node->Element.ValueLength > 0)
                                {
                                    (drFile->PixelDataInfo->PixelDateItemCount)++;
                                    *Item = malloc(sizeof(struct PixelDataItem));
                                    (*Item)->PixelLenght = Node->Element.ValueLength;
                                    (*Item)->PixelData = Node->Element.ValueField;
                                    (*Item)->NextPixelDataItem = NULL;
                                    Item = &((*Item)->NextPixelDataItem);
                                };
                            };
                            Node = Node->NextNode;
                        };
                    }
                    else
                    {
                        drFile->PixelDataInfo->PixelDateItemCount = 1;
                        drFile->PixelDataInfo->PixelItem = malloc(sizeof(struct PixelDataItem));
                        drFile->PixelDataInfo->PixelItem->PixelLenght = Node->Element.ValueLength;
                        drFile->PixelDataInfo->PixelItem->PixelData = Node->Element.ValueField;
                        drFile->PixelDataInfo->PixelItem->NextPixelDataItem = NULL;
                    };
                    break;
                }
                else
                    Node = Node->NextNode;
            };
        };

        return drFile;
    }
    else
        return NULL;
};

// 释放ReadDcmFile分配的内存资源
// 调用方需要把DcmFile结构体的地址作为drFile参数传递过来
void CloseDcmFile(PDcmFile drFile)
{
    if (drFile != NULL)
    {
        FreeDcmFileElements((*drFile).List);
        FreeDcmFileInfo((*drFile).FileInfo);
        FreeDcmPixelDataInfo((*drFile).PixelDataInfo);
        free(drFile);
    };
};