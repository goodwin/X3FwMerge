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

  RootFolder = malloc (strlen (InputFolderPath) + 1) ;
  strcpy (RootFolder, InputFolderPath) ;
  /*
    1. Travel directories and files
    2. After fount a entry type is file, add path to file list
  */
  
  strcat (CurrentPath, InputFolderPath) ;
  printf ("Root folder : %s\n", CurrentPath) ;
  Res = FolderTraveler (InputFolderPath) ;  /*Start from the root folder */
  printf ("-- Foder travel done. --\n") ;
  printf ("File counts : %d\n", FileCounter) ;
  printf ("File Size   : %d\n", FileTotalSize) ;



  /*
    1. Travel ihfs file list
    2. Create file image in memory
  */
  ListTraveler (OutputFileList) ;

  
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
  }

  /*
    Fill node data
  */
  CurrentListNode->Index = FileCounter ;

  /*
    Fill node FilePath
  */
  CurrentListNode->FilePath = malloc (strlen (FilePath) + 1 ) ;
  strcpy (CurrentListNode->FilePath, FilePath) ;

  /*
    Fill node file size
  */
  FileNode = fopen (FilePath, "rb") ;
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
  FILE *ReadBuffer = NULL;
  FileList *FileListPtr = IhfsFileList;
  
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
    while (FileListPtr != NULL) {

      /*
	Duplicate to raw image
      */
      UsedMemory += FileListPtr->FileSize;
      OutputFileImg = realloc (OutputFileImg, UsedMemory) ;
      printf ("%4d-> %30s : %08p : Used : %d\n", FileListPtr->Index,FileListPtr->IhfsFilePath, OutputFileImg, UsedMemory) ;

      /*
	Create sector table node
      */



      
      /*
	Move to next node
      */
      FileListPtr = (FileList *) FileListPtr->Next;
    }
  }
  return 0;
}

