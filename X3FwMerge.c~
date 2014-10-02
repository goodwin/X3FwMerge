#include <stdio.h>
#include <stdlib.h>

#include "X3FwMerge.h"

char CurrentPath[1024] = {0} ;
char *RootFolder = NULL;
unsigned long FileTotalSize = 0;
unsigned int FileCounter = 0;
unsigned long UsedMemory = 0;

/* IHFS File list */
FileList *OutputFileList = NULL;

/* Header */
FileHeader *OutputFileHeader = NULL;

/* Sector Table */
SectorHeader *OutputSectorHeader = NULL;

/* File image */
void *OutputFileImg = NULL;

/* Merge */
void *OutputFile = NULL;

int Folder2Img (char *InputFolderPath, char *OutputFilePath) 
{
  int Res = 0;
  FILE *fo = NULL;
  RootFolder = malloc (strlen (InputFolderPath) + 1) ;
  strcpy (RootFolder, InputFolderPath) ;
  /*
    1. Travel directories and files
    2. After fount a entry type is file, add path to file list
  */
  
  strcat (CurrentPath, InputFolderPath) ;
  printf ("Root folder : %s\n", CurrentPath) ;
  Res = FolderTraveler (InputFolderPath) ;  /*Start from the root folder */
  printf ("-- Folder travel done. --\n") ;
  printf ("File counts : %d\n", FileCounter) ;
  printf ("File Size   : %d\n", FileTotalSize) ;



  /*
    1. Travel ihfs file list
    2. Create file image in memory
  */
  ListTraveler (OutputFileList) ;

  //fo = fopen ("table", "wb") ;
  //fwrite (OutputSectorHeader, 1, FileCounter * 64, fo) ;
  //fclose (fo) ;

  /*
    Create file header info
  */
  CreateFileHeader () ;


  /*
    1. Merge 3 parts
    2. Write fw file
  */
  CreateFile () ;


  free (RootFolder) ;
  
  return 0;
}


int FolderTraveler (char *SourceFolderPath) 
{
  int Res = 0;
  int Index = 0;
  char ParentPath[1024] = {0} ;
  char FilePath[1024] = {0} ;
  DIR *SourceFolder = NULL;
  
  struct dirent *SubFolder = NULL;

  SourceFolder = opendir (SourceFolderPath) ;
  if (SourceFolder) {
    /*
      Open folder success
    */
    //printf ("CurrentPath=%s\n", CurrentPath) ;
    SubFolder = readdir (SourceFolder) ;
    while (SubFolder != NULL) {
      if (strcmp (SubFolder->d_name, ".") &&
	  strcmp (SubFolder->d_name, "..") ) {
	if (SubFolder->d_type == DT_DIR) {
	  /*
	    Open nest folder
	  */
	  strcpy (ParentPath, CurrentPath) ;
	  strcat (CurrentPath, "/") ;
	  strcat (CurrentPath, SubFolder->d_name) ;
	  FolderTraveler (CurrentPath) ;

	  /*
	    Resume parent folder path
	  */
	  for (Index = 0; Index < 1024; Index++)
	    *(CurrentPath + Index) = *(ParentPath + Index) ;
	  
	}
	if (SubFolder->d_type == DT_REG) {
	  /*
	    Write datas to list
	  */
	  strcat (FilePath, CurrentPath) ;
	  strcat (FilePath, "/") ;
	  strcat (FilePath, SubFolder->d_name) ;
	  AddFileList (FilePath) ;
	  memset (FilePath, 0, 1024) ;
	  FileCounter ++;
	}	
      }
      /*
	Next entry
      */
      SubFolder = readdir (SourceFolder) ;
    }
    closedir (SourceFolder) ;
  } else {
    perror ("*") ;
    return -1;
  } 
  return 0;
}

int AddFileList (char *FilePath) 
{
  int Res = 0;
  int Index = 0;
  static FileList *CurrentListNode = NULL;
  char *TempPath = NULL;
  FILE *FileNode = NULL;

  /*
    List node operation
  */
  if (OutputFileList == NULL) {
    /*
      Create List head
    */
    OutputFileList = malloc (sizeof (FileList) ) ;
    CurrentListNode = OutputFileList;
  } else {
    /*
      Create new list node
    */
    CurrentListNode->Next = malloc (sizeof (FileList) ) ;
    CurrentListNode = (FileList *) CurrentListNode->Next;
    CurrentListNode->Next = NULL;
  }

  /*
    Fill node data
  */
  CurrentListNode->Index = FileCounter ;

  /*
    Fill node FilePath
  */
  printf ("Add:%s to list\n", FilePath) ;
  CurrentListNode->FilePath = malloc (strlen (FilePath) + 1 ) ;
  strcpy (CurrentListNode->FilePath, FilePath) ;

  /*
    Fill node file size
  */
  FileNode = fopen (FilePath, "r") ;
  fseek (FileNode, 0, SEEK_END) ;
  CurrentListNode->FileSize = ftell (FileNode) ;
  FileTotalSize += CurrentListNode->FileSize ;
  fclose (FileNode) ;

  
  TempPath = (FilePath + strlen (RootFolder) + 1 ) ;
  for (Index = 0; Index < strlen (TempPath); Index++) {
    if (*(TempPath + Index) == '/') *(TempPath + Index) = '\\';
  }
  strcpy (CurrentListNode->IhfsFilePath, TempPath) ;
  
  return 0; /* success */
}

int ListTraveler (FileList *IhfsFileList) 
{
  int Res = 0;
  int Index = 0;
  unsigned int TempSectorAddress = 0;
  FILE *ReadBuffer = NULL;
  FileList *FileListPtr = IhfsFileList;
  SectorHeader *TempSectorHeader = NULL;
  
  /*
    Check Ihfs list valid
  */
  if (FileListPtr == NULL) {
    printf ("File list invalid.\n") ;
    return -1;
  } else {
    /*
      Start travel list
    */
    OutputSectorHeader = malloc (2048 * SECTOR_HEADER_SIZE) ;
    memset (OutputSectorHeader, 0, 2048 * SECTOR_HEADER_SIZE) ;
    while (FileListPtr != NULL) {
      printf ("#\n") ;

      /*
	Duplicate to raw image *bug
      */
      printf ("open:%p\n", FileListPtr) ;
      ReadBuffer = fopen (FileListPtr->FilePath, "rb") ;
      if (ReadBuffer != NULL) {
	printf ("Start @ %d \n", UsedMemory) ;
	OutputFileImg = realloc (OutputFileImg, UsedMemory + FileListPtr->FileSize) ;
	memset ((OutputFileImg + UsedMemory), 0, FileListPtr->FileSize) ;

	if ((UsedMemory % SECTOR_SIZE) == 0) {
	  TempSectorAddress = (UsedMemory / SECTOR_SIZE) + RAW_DATA_OFFSET; //
	} else {
	  printf ("Sector alignment fail (index:%d->mod:%d)\n", FileListPtr->Index, UsedMemory % SECTOR_SIZE) ;
	  return -1;
	}
;
	Res = fread ((OutputFileImg + UsedMemory),
		     1,
		     FileListPtr->FileSize,
		     ReadBuffer) ;
	printf ("Duplicate %d bytes,  ", Res) ;
	
	Res = fclose (ReadBuffer) ;
	UsedMemory += FileListPtr->FileSize;
	printf ("Next start %d \n", UsedMemory) ;

	/*
	  Sector size alignment
	*/
	if ( (UsedMemory % SECTOR_SIZE) != 0) {
	  printf ("%d(%d)->", UsedMemory, UsedMemory % 512) ;
	  UsedMemory = ((UsedMemory / SECTOR_SIZE) + 1) * SECTOR_SIZE ;
	  printf ("%d\n", UsedMemory) ;
	  OutputFileImg = realloc (OutputFileImg, UsedMemory) ;
	}
	FileTotalSize = UsedMemory;
      } else {
	printf ("List End!\n") ;
	return -1;
      }

      /*
	Create sector table node
      */
      //OutputSectorHeader = realloc (OutputSectorHeader,
      //				    (FileListPtr->Index + 1) * SECTOR_HEADER_SIZE) ;
      // OutputSectorHeader = malloc (2048 * SECTOR_HEADER_SIZE) ;
      //memset (OutputSectorHeader, 0, 2048 * SECTOR_HEADER_SIZE) ;
      printf ("Index:%d - Allocate:%d bytes\n", FileListPtr->Index, (FileListPtr->Index + 1)*64 ) ;
      TempSectorHeader = malloc (sizeof (SectorHeader)) ;
      memset (TempSectorHeader, 0, sizeof (SectorHeader) ) ;
      if (TempSectorHeader != NULL) {
	TempSectorHeader->SectorIndex = TempSectorAddress;
	TempSectorHeader->FileSize = FileListPtr->FileSize;
 
	strcpy (TempSectorHeader->FilePath, FileListPtr->IhfsFilePath) ;

	memcpy ((OutputSectorHeader + (FileListPtr->Index)), /* 1 offset = 64 bytes */
        	TempSectorHeader,
		SECTOR_HEADER_SIZE) ;
	printf ("%s \n", (OutputSectorHeader + FileListPtr->Index) ) ;	
	free (TempSectorHeader) ;
      } else {
	printf ("Create sector header fail.\n") ;
	return -1;
      }
      
      
      /*
	Move to next node
      */
      printf ("\n") ;

      FileListPtr = FileListPtr->Next;
    }
  }
  return 0;
}

int CreateFileHeader (void )
{
  const char *LocalTimeStamp = "201409161234";
  int Index = 0;

  
  OutputFileHeader = malloc (sizeof (FileHeader) ) ;
  if (OutputFileHeader != NULL) {
    OutputFileHeader->StartSignature = 0x49484653;
    OutputFileHeader->SectorAmount   = 0x07F00000; /*  */
    OutputFileHeader->Unknown1       = 0x00000004;
    OutputFileHeader->Unknown2       = 0x00000000;
    OutputFileHeader->FileAmount     = FileCounter;
    OutputFileHeader->FwVersion[0]   = 0x02;
    OutputFileHeader->FwModule[0]    = 'x';
    OutputFileHeader->FwModule[1]    = '3';
    OutputFileHeader->EndSignature   = 0x55AA55AA;


    for (Index = 1; Index < 0x10; Index ++) {
      OutputFileHeader->FwVersion[Index] = 0;
    }
    for (Index = 2; Index < 0x10; Index ++) {
      OutputFileHeader->FwModule[Index] = 0;
    }
    for (Index = 0; Index < 1536; Index++) {
      OutputFileHeader->Filter[Index] = 0;
    }
    for (Index = 0; Index < 444; Index++) {
      OutputFileHeader->Zeros[Index] = 0;
    }
    

    memcpy (OutputFileHeader->TimeStamp, LocalTimeStamp, 12) ;
  } else {
    printf ("Fail!\n") ;
    return -1;
  }
  return 0;
}

int CreateFile (void ) 
{
  int Res = 0;
  int Temp = 0;
  int Index = 0;
  unsigned char Zero = 0x00;
  FILE *fo = NULL;
  

  fo = fopen ("x3out.fw", "wb") ;
  if (fo != NULL) {
    printf ("Create output fw file success\n") ;

    /*
      Duplicate header info
    */
    printf ("Duplicate header info...") ;
    Res = fwrite (OutputFileHeader, sizeof (FileHeader), 1, fo) ;
    if (Res != -1) {
      printf ("ok! - %d\n", ftell (fo) ) ;
    }

    /*
      Duplicate sector table
    */
    printf ("Duplicate sector table...") ;
    Res = fwrite (OutputSectorHeader, sizeof (SectorHeader), FileCounter, fo) ;
    if (Res != -1) {
      printf ("ok! - %d\n", ftell (fo) ) ;
    }


    /*
      Duplicate file img
    */
    printf ("Duplicate file img...") ;
    Temp = (RAW_DATA_OFFSET * SECTOR_SIZE) - Res;
    for (Index = 0; Index < Temp; Index++) {
      
      Res = fwrite (&Zero, 1, 1, fo) ;
    }
    Res = fseek (fo, RAW_DATA_OFFSET * SECTOR_SIZE, SEEK_SET) ;
    printf ("- %d ", ftell (fo) ) ;

    Res = fwrite (OutputFileImg, 1, UsedMemory, fo) ;
    if (Res != -1) {
      printf ("ok! - %d\n", ftell (fo) ) ;
    }
    fclose (fo) ;
    
  } else {
    printf ("Create output fw file fail\n") ;
    perror ("x3") ;
    return -1;
  }
  
  

  
  return 0;
}
