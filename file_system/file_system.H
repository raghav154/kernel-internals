/* 
    File: file_system.H

    Author: Raghavan
    Date  : 04/28/14

    Description: File System.
    

*/

#ifndef _FILE_SYSTEM_H_                   // include file only once
#define _FILE_SYSTEM_H_

#define BLOCK_SIZE 		512
#define NO_OF_FILES		2000
#define BLOCKS_PER_FILE		10
#define BLOCKS_MASK		((NO_OF_FILES * BLOCKS_PER_FILE)/32 + 2)
/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "utils.H"
#include "blocking_disk.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARD DECLARATIONS */ 
/*--------------------------------------------------------------------------*/

class FileSystem;

/*--------------------------------------------------------------------------*/
/* F i l e */
/*--------------------------------------------------------------------------*/

class File {
friend class FileSystem;
private: 
     /* -- DEFINE YOUR FILE DATA STRUCTURES HERE. */
     
     FileSystem   * file_system;

     int   file_id;

     unsigned int start_pos;
     unsigned int curr_pos;

public:

    File();
    /* Constructor for the file handle. Set the 'current
       position' to be at the beginning of the file. */

    unsigned int Read(unsigned int _n, unsigned char * _buf);
    /* Read _n characters from the file starting at the 
       current location and copy them in _buf.
       Return the number of characters read. */

    unsigned int Write(unsigned int _n, unsigned char * _buf);
    /* Write _n characters to the file starting at the current 
       location, if we run past the end of file, we increase 
       the size of the file as needed. 
     */

    void Reset();
    /* Set the 'current position' at the beginning of the file. */

    void Rewrite();
    /* Erase the content of the file. Return any freed blocks. 
       Note: This function does not delete the file! It just erases its
       content. */

    bool EoF();
    /* Is the current location for the file at the end of the file? */


};

/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/

class FileSystem {

friend class File;

private:
     /* -- DEFINE YOUR FILE SYSTEM DATA STRUCTURES HERE. */
     
     BlockingDisk * disk; /* Pointer to disk used to mount file system */
     unsigned int size; /* Size of FileSystem */
     File *files[NO_OF_FILES]; /* Directory of Files */
     unsigned long block_mask[BLOCKS_MASK]; /* Keeping Free list of blocks (Bit-MAP) */
     unsigned int NO_OF_BLOCKS; /* Total number of blocks on file system */
     unsigned int Dir_files; /* Total number of files that could be stored with the given size */

public:

   FileSystem();   
   /* Just initializes local data structures. Does not connect to disk yet. */

   bool Mount(BlockingDisk * _disk);
   /* Associates the file system with a disk. We limit ourselves to at most one
      file system per disk. Returns TRUE if 'Mount' operation successful (i.e. there
      is indeed a file system on the disk. */

   bool Format(BlockingDisk * _disk, unsigned int _size);
   /* Wipes any file system from the given disk and installs a new, empty, file
      system that supports up to _size Byte. */

   bool LookupFile(int _file_id, File * _file);
   /* Find file with given id in file system. If found, initialize the file 
      object and return TRUE. Otherwise, return FALSE. */

   bool CreateFile(int _file_id);
   /* Create file with given id in the file system. If file exists already,
      abort and return FALSE. Otherwise, return TRUE. */

   bool DeleteFile(int _file_id);
   /* Delete file with given id in the file system and free any disk block
      occupied by the file. */

   
};
#endif
