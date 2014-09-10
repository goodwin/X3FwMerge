#include <stdio.h>
#include <stdlib.h>

#include "X3FwMerge.h"

int Folder2Img (char *InputFolderPath, char *OutputFilePath) 
{
  int Res = 0;
  DIR *RootFolder = NULL;
  struct dirent *SubFolder = NULL;

  /*
    Open root folder given.
  */
  printf ("*Load folder %s...", InputFolderPath) ;
  RootFolder = opendir (InputFolderPath) ;
  if (RootFolder) {
    /*
      Open sucess
    */
    printf ("success.\n") ;

    /*
      Travel folders and build file list
    */
    SubFolder = readdir (RootFolder) ;
    while (SubFolder != NULL) {
      printf ("-> %s\n", SubFolder->d_name) ;

      /*
	Next entry
      */
      SubFolder = readdir (RootFolder) ;
    }
    



    
    Res = closedir (RootFolder) ;
  } else {
    perror (0) ;
    return -1;
  }
  return 0;
}

int FolderTraveler (char *SourceFolderName) 
{
  DIR *SourceFolder = NULL;
  struct dirent *SubFolder = NULL;


  
  return 0;
}
