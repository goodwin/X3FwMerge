
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define LINUX

#define SECTOR_SIZE 512 /* 1 Sector = n bytes */

#define FILE_HEADER_SIZE 512 
#define SECTOR_HEADER_SIZE 64

#define SECTOR_TABLE_OFFSET 4 /*Sector Table starts at sector 4 */
#define RAW_DATA_OFFSET 260 /* Raw data starts at sector 260 */

#define INPUT_ROOT_FOLDER "InputX3"
#define FILE_PERMISSION (S_IRWXU | S_IRWXG | S_IRWXO))

typedef struct {
  unsigned int Index ;
  unsigned int FileSize;
  unsigned int SectorIndex;
  char *FilePath;
  char IhfsFilePath[56];
  void *Next;
} FileList ;

typedef struct {
  char StartSignature[4] ;
  unsigned int SectorAmount;
  unsigned int Unknown1;
  unsigned int Unknown2;
  char TimeStamp[12];
  unsigned int FileAmount;
  char FwVersion[0x10];
  char FwModule[0x10];
  char Zeros[444];
  unsigned int EndSignature;
} FileHeader ;

typedef struct {
  char FilePath[56];
  unsigned int SectorIndex;
  unsigned int FileSize;
} SectorHeader ;

int Folder2Img (char *InputFolderPath, char *OutputFilePath) ;

int FolderTraveler (char *SourceFolderPath) ;
int ListTraveler (FileList *IhfsFileList) ;

