// This main entry file is for Mac only
#import <Foundation/Foundation.h>

#include <stdio.h>
#include <mach/clock.h>
#include <mach/mach.h>

#include "dr.h"
#include "drprint.h"

int main(int argc, char *argv[])
{
    PDcmFile drFile = NULL;

    clock_serv_t cclock;
    mach_timespec_t mts;

    @try
    {
        if (argc == 2)
        {
            host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
            clock_get_time(cclock, &mts);
            long long starttime = (long long)mts.tv_sec * 1000000000LL + mts.tv_nsec;               // ns
            
            drFile = ReadDcmFile(argv[1], 0);

            if (drFile != NULL)
            {
                host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
                clock_get_time(cclock, &mts);
                long long timeused = (long long)mts.tv_sec * 1000000000LL + mts.tv_nsec - starttime;    // ns

                printf("%s\n", argv[1]);
                printf("read complete with %lld us.\n", timeused / 1000);                               // us
                printf("drFile=%p, ", drFile);
                printf("List=%p\n", drFile->List);
                if (drFile->FileInfo->SOPClass != PrivateStorageSOPClass)
                    printf("StorageSOPClass=%s(%s), supported=%d\n", 
                            StorageSOPClassList[drFile->FileInfo->SOPClass].pszClassName, 
                            StorageSOPClassList[drFile->FileInfo->SOPClass].pszClassUID,
                            StorageSOPClassList[drFile->FileInfo->SOPClass].IsSupported);
                else
                    printf("StorageSOPClass=%s, supported=%d\n", 
                            "Private StorageSOPClass", 0);
                printf("TransferSyntax=%s(%s), supported=%d\n",
                        TransferSyntaxList[drFile->FileInfo->TransferSyntax].pszSyntaxName,
                        TransferSyntaxList[drFile->FileInfo->TransferSyntax].pszSyntaxUID,
                        TransferSyntaxList[drFile->FileInfo->TransferSyntax].IsSupported);            
                PrintTags(drFile->List);
                printf("Count of pixel data items: %d.\n", drFile->PixelDataInfo->PixelDateItemCount);
                int i = 0;
                PPixelDataItem Item = drFile->PixelDataInfo->PixelItem;
                while (Item != NULL)
                {
                    printf("Pixel item #%d: 0x%X bytes.\n", ++i, Item->PixelLenght);
                    Item = Item->NextPixelDataItem;
                };
            }
            else
                printf("ReadDcmFile failed.\n");
                
            CloseDcmFile(drFile);            
        }
        else
            printf("invalid input file.\n");
    }
    @catch(NSException *e)
    {
        NSLog(@"%@", e);
    };

    return 0;
};