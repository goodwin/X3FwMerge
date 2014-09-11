#include <stdio.h>
#include <stdlib.h>

#include "X3FwMerge.h"

char CurrentPath[1024] = {0} ;
int FileCounter = 0;
FileList *InputFileList = NULL;

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

  
  /*
    1. Travel directories and files
    2. After fount a entry type is file, add path to file list
  */
  
  strcat (CurrentPath, InputFolderPath) ;
  printf ("Enter [%s]\n", CurrentPath) ;
  Res = FolderTraveler (InputFolderPath) ;  /*Start from the root folder */
  printf ("File : %d\n", FileCounter) ;

  
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
	  //printf ("-- %s --\n", SubFolder->d_name) ;

	  strcpy (ParentPath, CurrentPath) ;
	  strcat (CurrentPath, "/") ;
	  strcat (CurrentPath, SubFolder->d_name) ;
	  //Res = chdir (CurrentPath) ;
	  FolderTraveler (CurrentPath) ;
	  //Res = chdir ("..") ;
	  for (Index = 0; Index < 1024; Index++)
	    *(CurrentPath + Index) = *(ParentPath + Index) ;
	  
	}
	if (SubFolder->d_type == DT_REG) {
	  /*
	    Write datas to list
	  */
	  strcat (FilePath, CurrentPath) ;
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
  FILE *F = NULL;
  printf ("File : %s\n", FilePath) ;
  F = fopen (FilePath, "wb") ;
  if (InputFileList == NULL) {
    /*
      Create List head
    */
  } else {
    /*
      Add new file node to list
    */
  }
  return 0; /* success */
}
