#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

struct NVMeUpdateLib NVMeUpdateLib; // I hate C++
void __fastcall NVMeUpdateLib::DumpIdentifyData(NVMeUpdateLib *this)
{
  NVMeUpdateLib *NVMEUpdateLibInstance; // x20
  unsigned __int16 *identifyData; // x19
  unsigned __int64 v3; // x8
  int v4; // w20
  signed __int64 v5; // x9
  unsigned int k2DLithography; // w10
  char **k2DLithographyStrings; // x11
  bool v8; // zf
  const char *error; // x10
  unsigned __int64 chipID; // x11
  const char *chipIDString; // x11
  signed __int64 dieType; // x22
  const char *vendor; // x12
  const char *v14; // x9
  unsigned int die; // w8
  const char *dieString; // x8
  __int64 i; // x21
  char v18; // [xsp+48h] [xbp-48h]
  __int64 v19; // [xsp+50h] [xbp-40h]
  unsigned __int64 identifyDataRef; // [xsp+60h] [xbp-30h]

  NVMEUpdateLibInstance = this;
  identifyData = (unsigned __int16 *)valloc(0x1000uLL);
  bzero(identifyData, 0x1000uLL);
  identifyDataRef = (unsigned __int64)identifyData;
  NVMeUpdateLib::SendNVMeCommand(NVMEUpdateLibInstance, &identifyDataRef, &v18);
  if ( v19 )
    return;
  *((_BYTE *)identifyData + 0xE15) = 0;
  v3 = identifyData[0x60F];
  v4 = v3 & 7;
  v5 = (v3 >> 9) & 7;
  k2DLithography = ((unsigned int)v3 >> 3) & 0xF;
  if ( (_DWORD)v5 == 1 )
  {
    if ( v4 == 4 )
      v8 = 1;
    else
      v8 = v4 == 1;
    if ( v8 )
    {
      if ( k2DLithography > 3 )
      {
        error = "<<Warning: Unknown value for k3DHynixLithographyStrings>>";
        goto LABEL_20;
      }
      k2DLithographyStrings = k3DHynixLithographyStrings;
    }
    else
    {
      if ( k2DLithography > 3 )
      {
        error = "<<Warning: Unknown value for k3DLithographyStrings>>";
        goto LABEL_20;
      }
      k2DLithographyStrings = k3DLithographyStrings;
    }
    goto LABEL_17;
  }
  if ( (_DWORD)v5 )
  {
    error = 0LL;
  }
  else
  {
    if ( k2DLithography <= 2 )
    {
      k2DLithographyStrings = ::k2DLithographyStrings;
LABEL_17:
      error = k2DLithographyStrings[k2DLithography];
      goto LABEL_20;
    }
    error = "<<Warning: Unknown value for k2DLithographyStrings>>";
  }
LABEL_20:
  chipID = *((unsigned __int8 *)identifyData + 3100);
  if ( chipID > 2 )
    chipIDString = "<<Warning: Unknown value for kChipIDStrings>>";
  else
    chipIDString = kChipIDStrings[chipID];
  dieType = (v3 >> 7) & 3;
  if ( v4 == 7 )
    vendor = "<<Warning: Unknown value for kVendorStrings>>";
  else
    vendor = kVendorStrings[v3 & 7];
  if ( (unsigned int)v5 > 1 )
    v14 = "<<Warning: Unknown value for kDimensionStrings>>";
  else
    v14 = (&kDimensionStrings)[v5];
  printf(
    "nvme: %s %X %s %s %s %s %d plane %dGB ",
    chipIDString,
    (unsigned int)*((unsigned __int8 *)identifyData + 3101) + 160,
    vendor,
    error,
    (&kDieTypeStrings)[dieType],
    v14,
    (unsigned int)(2 << ((((unsigned int)v3 >> 12) & 3) - 1)),
    identifyData[1795]);
  if ( v4 == 1 && (_DWORD)dieType == 1 )
  {
    die = *((unsigned __int8 *)identifyData + 3104);
    if ( die > 0x1F )
      dieString = "<<Warning: Unknown value for kChipDieStrings>>";
    else
      dieString = (&kChipDieStrings)[die >> 4];
    printf("%s Die ", dieString);
  }
  puts("NAND");
  *((_BYTE *)identifyData + 71) = 0;
  printf("ECCVersion   : %d\n", *((unsigned __int8 *)identifyData + 3104));
  printf("FTL Rev      : %d.%d\n", *((unsigned __int8 *)identifyData + 3105), *((unsigned __int8 *)identifyData + 3106));
  printf("DM_Version   : %d\n", *((unsigned __int8 *)identifyData + 3107));
  printf("PTS Version  : %s\n", identifyData + 1797);
  printf("FW Revision  : %s\n", identifyData + 32);
  puts("=======================");
  puts("Dumping raw identify data:");
  i = 0LL;
  do
  {
    if ( !(i & 0xF) )
      putchar('\n');
    printf("%02X ", *((unsigned __int8 *)identifyData + i++));
  }
  while ( i != 0x1000 );                        // hexdump
  free(identifyData);
}
