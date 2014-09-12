#include <stdio.h>
#include <stdlib.h>

#include "X3FwMerge.h"

char CurrentPath[1024] = {0} ;
char *RootFolder = NULL;
int FileCounter = 0;

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
  printf ("Enter [%s]\n", CurrentPath) ;
  Res = FolderTraveler (InputFolderPath) ;  /*Start from the root folder */
  printf ("File counts : %d\n", FileCounter) ;



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

  
  CurrentListNode->FilePath = malloc (strlen (FilePath) + 1 ) ;
  strcpy (CurrentListNode->FilePath, FilePath) ;

  
  FileNode = fopen (FilePath, "rb") ;
  fseek (FileNode, 0, SEEK_END) ;
  CurrentListNode->FileSize = ftell (FileNode) ;
  fclose (FileNode) ;

  
  TempPath = (FilePath + strlen (RootFolder) + 1 ) ;
  strcpy (CurrentListNode->IhfsFilePath, TempPath) ;
  

  
  return 0; /* success */
}

int ListTraveler (FileList *IhfsFileList) 
{
  FileList *FileListPtr = IhfsFileList;
  
  if (IhfsFileList == NULL) {
    return -1;
  } else {
    while (FileListPtr != NULL) {
      printf ("%4d, path = %s\n", FileListPtr->Index, FileListPtr->IhfsFilePath) ;
      FileListPtr = (FileList *) FileListPtr->Next;
    }
  }
  return 0;
}

