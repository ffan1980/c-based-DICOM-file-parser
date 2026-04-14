#ifndef DR_H
#define DR_H

#include <stddef.h>
#include <stdlib.h>


// define this when use cl.exe & link.exe at Windows platform
// #define DR_WINDOWS

// define this when use clang at MacOS platform
#define DR_MACOS

// Platform specific
#ifdef DR_WINDOWS
    // MS SAL
    #define DR_INOUT_ _Inout_
    #define DR_OUT_ _Out_
    // TLS
    #define DR_TLS __declspec(thread)
#elif defined DR_MACOS
    //
    #define DR_INOUT_
    #define DR_OUT_
    // TLS
    #define DR_TLS __thread
#endif


// StorageSOPClass

// Do not change this list
typedef enum
{
    MissingStorageSOPClass = -10,
    PrivateStorageSOPClass = -1,
    ComputedRadiographyImageStorage = 0,
    DigitalXRayImageStorageForPresentation,
    DigitalXRayImageStorageForProcessing,
    DigitalMammographyXRayImageStorageForPresentation,
    DigitalMammographyXRayImageStorageForProcessing,
    DigitalIntraOralXRayImageStorageForPresentation,
    DigitalIntraOralXRayImageStorageForProcessing,
    CTImageStorage,
    EnhancedCTImageStorage,
    LegacyConvertedEnhancedCTImageStorage,
    UltrasoundMultiframeImageStorage,
    MRImageStorage,
    EnhancedMRImageStorage,
    MRSpectroscopyStorage,
    EnhancedMRColorImageStorage,
    LegacyConvertedEnhancedMRImageStorage,
    UltrasoundImageStorage,
    EnhancedUSVolumeStorage,
    SecondaryCaptureImageStorage,
    MultiframeSingleBitSecondaryCaptureImageStorage,
    MultiframeGrayscaleByteSecondaryCaptureImageStorage,
    MultiframeGrayscaleWordSecondaryCaptureImageStorage,
    MultiframeTrueColorSecondaryCaptureImageStorage,
    TwelveLeadECGWaveformStorage,
    GeneralECGWaveformStorage,
    AmbulatoryECGWaveformStorage,
    HemodynamicWaveformStorage,
    CardiacElectrophysiologyWaveformStorage,
    BasicVoiceAudioWaveformStorage,
    GeneralAudioWaveformStorage,
    ArterialPulseWaveformStorage,
    RespiratoryWaveformStorage,
    MultichannelRespiratoryWaveformStorage,
    RoutineScalpElectroencephalogramWaveformStorage,
    ElectromyogramWaveformStorage,
    ElectrooculogramWaveformStorage,
    SleepElectroencephalogramWaveformStorage,
    BodyPositionWaveformStorage,
    GrayscaleSoftcopyPresentationStateStorage,
    ColorSoftcopyPresentationStateStorage,
    PseudoColorSoftcopyPresentationStateStorage,
    BlendingSoftcopyPresentationStateStorage,
    XAorXRFGrayscaleSoftcopyPresentationStateStorage,
    GrayscalePlanarMPRVolumetricPresentationStateStorage,
    CompositingPlanarMPRVolumetricPresentationStateStorage,
    AdvancedBlendingPresentationStateStorage,
    VolumeRenderingVolumetricPresentationStateStorage,
    SegmentedVolumeRenderingVolumetricPresentationStateStorage,
    MultipleVolumeRenderingVolumetricPresentationStateStorage,
    XRayAngiographicImageStorage,
    EnhancedXAImageStorage,
    XRayRadiofluoroscopicImageStorage,
    EnhancedXRFImageStorage,
    XRay3DAngiographicImageStorage,
    XRay3DCraniofacialImageStorage,
    BreastTomosynthesisImageStorage,
    BreastProjectionXRayImageStorageForPresentation,
    BreastProjectionXRayImageStorageForProcessing,
    IntravascularOpticalCoherenceTomographyImageStorageForPresentation,
    IntravascularOpticalCoherenceTomographyImageStorageForProcessing,
    NuclearMedicineImageStorage,
    ParametricMapStorage,
    RawDataStorage,
    SpatialRegistrationStorage,
    SpatialFiducialsStorage,
    DeformableSpatialRegistrationStorage,
    SegmentationStorage,
    SurfaceSegmentationStorage,
    TractographyResultsStorage,
    RealWorldValueMappingStorage,
    SurfaceScanMeshStorage,
    SurfaceScanPointCloudStorage,
    VLEndoscopicImageStorage,
    VideoEndoscopicImageStorage,
    VLMicroscopicImageStorage,
    VideoMicroscopicImageStorage,
    VLSlideCoordinatesMicroscopicImageStorage,
    VLPhotographicImageStorage,
    VideoPhotographicImageStorage,
    OphthalmicPhotography8BitImageStorage,
    OphthalmicPhotography16BitImageStorage,
    StereometricRelationshipStorage,
    OphthalmicTomographyImageStorage,
    WideFieldOphthalmicPhotographyStereographicProjectionImageStorage,
    WideFieldOphthalmicPhotography3DCoordinatesImageStorage,
    OphthalmicOpticalCoherenceTomographyEnFaceImageStorage,
    OphthalmicOpticalCoherenceTomographyBscanVolumeAnalysisStorage,
    VLWholeSlideMicroscopyImageStorage,
    DermoscopicPhotographyImageStorage,
    LensometryMeasurementsStorage,
    AutorefractionMeasurementsStorage,
    KeratometryMeasurementsStorage,
    SubjectiveRefractionMeasurementsStorage,
    VisualAcuityMeasurementsStorage,
    SpectaclePrescriptionReportStorage,
    OphthalmicAxialMeasurementsStorage,
    IntraocularLensCalculationsStorage,
    MacularGridThicknessandVolumeReport,
    OphthalmicVisualFieldStaticPerimetryMeasurementsStorage,
    OphthalmicThicknessMapStorage,
    CornealTopographyMapStorage,
    BasicTextSRStorage,
    EnhancedSRStorage,
    ComprehensiveSRStorage,
    Comprehensive3DSRStorage,
    ExtensibleSRStorage,
    ProcedureLogStorage,
    MammographyCADSRStorage,
    KeyObjectSelectionDocumentStorage,
    ChestCADSRStorage,
    XRayRadiationDoseSRStorage,
    RadiopharmaceuticalRadiationDoseSRStorage,
    ColonCADSRStorage,
    ImplantationPlanSRDocumentStorage,
    AcquisitionContextSRStorage,
    SimplifiedAdultEchoSRStorage,
    PatientRadiationDoseSRStorage,
    PlannedImagingAgentAdministrationSRStorage,
    PerformedImagingAgentAdministrationSRStorage,
    EnhancedXRayRadiationDoseSRStorage,
    ContentAssessmentResultsStorage,
    MicroscopyBulkSimpleAnnotationsStorage,
    EncapsulatedPDFStorage,
    EncapsulatedCDAStorage,
    EncapsulatedSTLStorage,
    EncapsulatedOBJStorage,
    EncapsulatedMTLStorage,
    PositronEmissionTomographyImageStorage,
    EnhancedPETImageStorage,
    LegacyConvertedEnhancedPETImageStorage,
    BasicStructuredDisplayStorage,
    CTPerformedProcedureProtocolStorage,
    XAPerformedProcedureProtocolStorage,
    RTImageStorage,
    RTDoseStorage,
    RTStructureSetStorage,
    RTBeamsTreatmentRecordStorage,
    RTPlanStorage,
    RTBrachyTreatmentRecordStorage,
    RTTreatmentSummaryRecordStorage,
    RTIonPlanStorage,
    RTIonBeamsTreatmentRecordStorage,
    RTPhysicianIntentStorage,
    RTSegmentAnnotationStorage,
    RTRadiationSetStorage,
    CArmPhotonElectronRadiationStorage,
    TomotherapeuticRadiationStorage,
    RoboticArmRadiationStorage,
    RTRadiationRecordSetStorage,
    RTRadiationSalvageRecordStorage,
    TomotherapeuticRadiationRecordStorage,
    CArmPhotonElectronRadiationRecordStorage,
    RoboticRadiationRecordStorage,
    RTRadiationSetDeliveryInstructionStorage,
    RTTreatmentPreparationStorage,
    RTBeamsDeliveryInstructionStorage,
    RTBrachyApplicationSetupDeliveryInstructionStorage
} StorageSOPClassName;

typedef struct
{
    int IsSupported;                                                                        // 1: supported; 0: not supported
    StorageSOPClassName ClassName;
    char *pszClassName;
    char *pszClassUID;
} StorageSOPClass;

#define STORAGESOPCLASSLISTCOUNT 157                                                        // don't change this number
extern const StorageSOPClass StorageSOPClassList[STORAGESOPCLASSLISTCOUNT];                 // defined in drinit.c


// Transfer Syntax

// Transfer Syntax list, from PS3.6 (except UnknownTransferSyntax)
typedef enum
{
    UnknownTransferSyntax = -1,
    ImplicitVRLittleEndian = 0,
    ExplicitVRLittleEndian,
    EncapsulatedUncompressedExplicitVRLittleEndian,
    DeflatedExplicitVRLittleEndian,
    ExplicitVRBigEndian,
    JPEGBaseline8Bit,
    JPEGExtended12Bit,
    JPEGExtended35,
    JPEGSpectralSelectionNonHierarchical68,
    JPEGSpectralSelectionNonHierarchical79,
    JPEGFullProgressionNonHierarchical1012,
    JPEGFullProgressionNonHierarchical1113,
    JPEGLossless,
    JPEGLosslessNonHierarchical15,
    JPEGExtendedHierarchical1618,
    JPEGExtendedHierarchical1719,
    JPEGSpectralSelectionHierarchical2022,
    JPEGSpectralSelectionHierarchical2123,
    JPEGFullProgressionHierarchical2426,
    JPEGFullProgressionHierarchical2527,
    JPEGLosslessHierarchical28,
    JPEGLosslessHierarchical29,
    JPEGLosslessSV1,
    JPEGLSLossless,
    JPEGLSNearLossless,
    JPEG2000Lossless,
    JPEG2000,
    JPEG2000MCLossless,
    JPEG2000MC,
    JPIPReferenced,
    JPIPReferencedDeflate,
    MPEG2MPML,
    MPEG2MPHL,
    MPEG4HP41,
    MPEG4HP41BD,
    MPEG4HP422D,
    MPEG4HP423D,
    MPEG4HP42STEREO,
    HEVCMP51,
    HEVCM10P51,
    RLELossless,
    RFC2557MIMEEncapsulation,
    XMLEncoding,
    SMPTEST211020UncompressedProgressiveActiveVideo,
    SMPTEST211020UncompressedInterlacedActiveVideo,
    SMPTEST211030PCMDigitalAudio,
    Papyrus3ImplicitVRLittleEndian
} TransferSyntaxName;

typedef enum
{
    UnknownImplicitOrExplicitVR = 0,
    ImplicitVR,
    ExplicitVR
} ImplicitOrExplicitVR;

typedef struct
{
    int IsSupported;                                                                        // 1: supported; 0: not supported
    ImplicitOrExplicitVR ImorExVR;
    TransferSyntaxName SyntaxName;
    char *pszSyntaxName;
    char *pszSyntaxUID;
} TransferSyntax;

#define TRANSFERSYNTAXLISTCOUNT 47                                                          // don't change this number
extern const TransferSyntax TransferSyntaxList[TRANSFERSYNTAXLISTCOUNT];                    // defined in drinit.c


// VR

// VR list, from PS3.5 Table 6.2-1 (except VR_ERROR, VR_Blank, VR_NoVR, VR_OBorOW, VR_USorOW, VR_USorSS, VR_USorSSorOW, VR_PrivateUnknown)
typedef enum
{
    // error (1) - not in PS3.5 standard
    VR_ERROR = 0,
    // string (17) - in PS3.5 standard
    VR_AE,      // string,                  <= 16 bytes
    VR_CS,      // string,                  <= 16 bytes
    VR_PN,      // name,                    <= 64 chars    
    VR_AS,      // age,                     4 bytes
    VR_DA,      // date,                    8 bytes or <= 18 bytes
    VR_TM,      // time,                    <= 14 bytes or <= 28 bytes
    VR_DT,      // datetime,                <= 26 bytes or <= 54 bytes
    VR_IS,      // int,                     <= 12 bytes
    VR_DS,      // decimal number,          <= 16 bytes
    VR_SH,      // string,                  <= 16 chars
    VR_LO,      // string,                  <= 64 chars
    VR_ST,      // string,                  <=1024 chars                VM=1
    VR_LT,      // string,                  <=10240 chars               VM=1
    VR_UC,      // unlimited                2^32-2 bytes
    VR_UT,      // unlimited                2^32-2 bytes                VM=1
    VR_UI,      // UID,                     64 bytes
    VR_UR,      // URI/URL                  2^32-2 bytes                VM=1
    // number (8) - in PS3.5 standard
    VR_US,      // unsigned short,          2 bytes
    VR_SS,      // signed short,            2 bytes
    VR_UL,      // unsigned long,           4 bytes
    VR_SL,      // signed long,             4 bytes
    VR_UV,      // unsigned very long,      8 bytes
    VR_SV,      // signed very long,        8 bytes
    VR_FL,      // float,                   4 bytes
    VR_FD,      // double,                  8 bytes
    // stream (7) - in PS3.5 standard
    VR_OB,      // octet-stream             per TransferSyntax          VM=1
    VR_OW,      // 16-bit stream            per TransferSyntax          VM=1
    VR_OL,      // 32-bit stream            per TransferSyntax          VM=1
    VR_OV,      // 64-bit stream            per TransferSyntax
    VR_OF,      // 32-bit stream            2^32-4 bytes                VM=1
    VR_OD,      // 64-bit stream            2^32-8 bytes                VM=1
    VR_UN,      // octet-stream             任何其他VR的长度             VM=1
    // others (2) - in PS3.5 standard
    VR_AT,      // tag,                     4 bytes
    VR_SQ,      // sequence                 N/A                         VM=1
    // for Data Set Elements only (7) - not in PS3.5 standard
    VR_Blank,
    VR_NoVR,
    VR_OBorOW,
    VR_USorOW,
    VR_USorSS,
    VR_USorSSorOW,
    VR_PrivateUnknown
} ValueRepresentation;

// VR and the function to print such VR data
typedef struct
{
    ValueRepresentation VR;
    char *pszVR;
    char * (* PrintVR)(const unsigned char *, unsigned int);                                // defined in drprint.c
} VRString;

#define VRSTRINGLISTCOUNT 42                                                                // don't change this number
extern const VRString VRList[VRSTRINGLISTCOUNT];                                            // defined in drinit.c

// VM
typedef enum
{
    VM_Single,              // VM = 1
    VM_Multiple,            // VM >=1
    VM_PrivateUnknown,      // VM unknown - private tag
    VM_Unknown              // VM unknown
} ValueMultiplicity;


// Registry

// GN, EN是数字的Registry
typedef struct
{
    unsigned short GroupNumber;
    unsigned short ElementNumber;
    char *pszName;
    char *pszVR;
    ValueRepresentation VR;
    ValueMultiplicity VM;
} RegistryElement;

// GN, EN是字符串的Registry (包含字符'x')
typedef struct
{
    char *pszGroupNumber;
    char *pszElementNumber;
    char *pszName;
    char *pszVR;
    ValueRepresentation VR;
    ValueMultiplicity VM;
} RegistryElementChar;

// Registry declaration
#define METAREGISTRYELEMENTSCOUNT 22                                                        // Meta Elements 22 - PS3.6 standard
#define DATAREGISTRYELEMENTSNUMBERCOUNT 4905 - 88                                           // Data Elements 4905 - PS3.6 standard
#define DATAREGISTRYELEMENTSCHARCOUNT 88 + 1                                                // tag with "x" - PS3.6 standard
extern const RegistryElement MetaRegistry[METAREGISTRYELEMENTSCOUNT];                       // defined in drinit.c
extern const RegistryElement DataSetRegistryNumber[DATAREGISTRYELEMENTSNUMBERCOUNT];        // defined in drinit.c
extern const RegistryElementChar DataSetRegistryChar[DATAREGISTRYELEMENTSCHARCOUNT];        // defined in drinit.c

// 单独定义这2个tag name，为了在GetULPixelDataItems()和GetSQItems()不用再查registry
extern char * const TagNameFFFEE000;                                // Item, defined in drinit.c
extern char * const TagNameFFFEE0DD;                                // Sequence Delimitation Item, defined in drinit.c

// Private Data Element tag name
extern char * const PrivateCreatorTag;                                                      // defined in drinit.c
extern char * const PrivateUnknownTag;                                                      // defined in drinit.c


// 读取的Element结构，链表节点，链表

// Undefined Length for the Value Length of an Element
#define UNDEFINED_LENGTH 0xFFFFFFFF

// 注意：ValueLength仅支持到2^32(4GB), 在选择支持的StorageSOPClass和TransferSyntax时必须满足这个条件
typedef struct
{
    unsigned short GroupNumber;
    unsigned short ElementNumber;
    ValueRepresentation VR;
    ValueMultiplicity VM;
    unsigned int ValueLength;                                                               // ValueLength可以=0，这时ValueField=NULL
    unsigned char *ValueField;                                                              // point -> raw value in the buffer (no copy)
    char *pszTagName;
} Element, *PElement;

// tag链表节点
struct ElementsListNode
{
    Element Element;
    struct ElementsListNode *SubNode;                                                       // a sub node (in a sequence only)
    struct ElementsListNode *NextNode;                                                      // 链表里并列的下一个node
};
typedef struct ElementsListNode *PElementsListNode;

// DICOM文件信息
typedef struct
{
    StorageSOPClassName SOPClass;
    TransferSyntaxName TransferSyntax;
    unsigned char *DcmFileBuffer;                                                           // -> 读取的DICOM文件的字节流数组
} DcmFileInfo, *PDcmFileInfo;

// each raw pixel data item
struct PixelDataItem
{
    unsigned int PixelLenght;                   // count of pixel bytes
    unsigned char *PixelData;                   // pointer to the starting byte of the pixel bytes
    struct PixelDataItem *NextPixelDataItem;    // pointer to next item; NULL if the last one
};
typedef struct PixelDataItem *PPixelDataItem;

// raw pixel data
typedef struct
{
    int PixelDateItemCount;         // 这个dcm文件总共有几个pixel data items: 
                                    // -1: 还没有遇到(7FE0,0010) 
                                    // 0: 遇到了遇到(7FE0,0010)，但还没有length>0的pixel data item
                                    // >0: 这个(7FE0,0010)下有多少个length>0的pixel data item
                                    // 程序逻辑假定一个dcm文件只能有一个(7FE0,0010)，否则报错
    PPixelDataItem PixelItem;       // pixel data item链表的第一个
} PixelData, *PPixelData;

// 读取的DICOM文件数据 - 由ReadDcmFile()返回
typedef struct
{
    PElementsListNode List;
    PDcmFileInfo FileInfo;
    PPixelData PixelDataInfo;
} DcmFile, *PDcmFile;


// function declaration - defined in dr.c

// 读取给定的DICOM file，创建DcmFile结构体并返回其地址
// 不需要该文件时，需要调用CloseDcmFile()来释放内存
// return
// 0: success
// error code: failed
PDcmFile ReadDcmFile(const char *szDcmFileName, int MetaOnly);

// 释放ReadDcmFile分配的内存资源
// 调用方需要把ReadDcmFile()返回的DcmFile结构体的地址作为drFile参数传递过来
void CloseDcmFile(PDcmFile drFile);


#endif