#include <stdio.h>
#include <stdlib.h>

#include "X3FwMerge.h"

int Folder2Img (char *InputFolderPath, char *OutputFilePath) 
{
  int Res = 0;
  char *RootFolderPath ;

  /*
    1. Travel directories and files
    2. After fount a entry type is file, add path to file list
  */
  Res = FolderTraveler (InputFolderPath) ; /*Start from the root folder */

  
  return 0;
}


int FolderTraveler (char *SourceFolderPath) 
{
  int Res = 0;
  DIR *SourceFolder = NULL;
  
  struct dirent *SubFolder = NULL;

  SourceFolder = opendir (SourceFolderPath) ;
  if (SourceFolder) {
    /*
      Open folder success
    */
    printf ("Open %s ok!\n", SourceFolderPath) ;
    SubFolder = readdir (SourceFolder) ;
    while (SubFolder != NULL) {
      if (strcmp (SubFolder->d_name, ".") &&
	  strcmp (SubFolder->d_name, "..") ) {
	printf ("%s\n", SubFolder->d_name) ;
	if (SubFolder->d_type == DT_DIR) {
	  /*
	    Open nest folder
	  */
	  Res = chdir (SubFolder->d_name) ;
	  printf ("Change dir to %s = %d\n", SubFolder->d_name, Res) ;
	  //FolderTraveler (NextFolder) ;
	}
	if (SubFolder->d_type == DT_REG) {
	  /*
	    Write datas to list
	  */
	  //printf ("%s\n", FilePath) ;
	}
	
      }
      /*
	Next entry
      */
      SubFolder = readdir (SourceFolder) ;
    }
    closedir (SourceFolder) ;
  } else {
    perror (0) ;
    return -1;
  } 
  return 0;
}
