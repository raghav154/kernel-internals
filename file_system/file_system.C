/* 
    File: file_system.C
    Author: Raghavan
    Date  : 04/26/14

    Description: File System.
    

*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
//#include <stdlib.h>
#include "file_system.H"
#include "machine.H"
#include "console.H"



extern BlockingDisk *SYSTEM_DISK;
/* Constructor for tne file handle. Set the 'current
       position' to be at the beginning of the file.*/
File::File() {
  start_pos = 0;
  curr_pos = start_pos;
}    

/* Read _n characters from the file starting at the 
       current location and copy them in _buf.
       Return the number of characters read. */
unsigned int File::Read(unsigned int _n, unsigned char * _buf) {
  /* -- Read */
  unsigned int temp = curr_pos;
  Console::puts("Reading from file...\n");
  while((!EoF()) && (((curr_pos-temp)*BLOCK_SIZE) < _n)) {
  	(SYSTEM_DISK)->read(curr_pos, _buf);
	  Console::putui(curr_pos);
	/* -- Display */
	int i;
	for (i = 0; i < BLOCK_SIZE; i++) {
		Console::putch(_buf[i]);
	}
	curr_pos++;
  }
  return ((curr_pos-temp)*BLOCK_SIZE);
}
    
/* Write _n characters to the file starting at the current 
       location, if we run past the end of file, we increase 
       the size of the file as needed. 
     */
unsigned int File::Write(unsigned int _n, unsigned char * _buf) {
  /* Write */
  unsigned int temp = curr_pos;
  Console::puts("Writing to file...\n");
  while((!EoF()) && (((curr_pos-temp)*BLOCK_SIZE) < _n)) {
	Console::putui(curr_pos);
  	(SYSTEM_DISK)->write(curr_pos, _buf);
	curr_pos++;
  }
  return ((curr_pos-temp)*BLOCK_SIZE);
}    

/* Set the 'current position' at the beginning of the file. */
void File::Reset() {
  curr_pos = start_pos;
}

/* Erase the content of the file. Return any freed blocks. 
Note: This function does not delete the file! It just erases its content. */
void File::Rewrite() {
  Reset();
  /* Erasing contents by writing NULL */
  unsigned char buff[BLOCK_SIZE*BLOCKS_PER_FILE];
  for(int i=0;i<BLOCK_SIZE*BLOCKS_PER_FILE;i++)
	buff[i]=NULL;
  Write(BLOCK_SIZE*BLOCKS_PER_FILE,buff);
}

/* Is the current location for the file at the end of the file? */
bool File::EoF() {
  return (curr_pos == (start_pos + BLOCKS_PER_FILE));
}   


/*--------------------------------------------------------------------------*/
/* F i l e S y s t e m  */
/*--------------------------------------------------------------------------*/

FileSystem::FileSystem() {
   /* Just initializes local data structures. Does not connect to disk yet. */
  size = 0 ; /* Size of the filesystem in bytes */
  disk = NULL;
  Dir_files = 0;
  NO_OF_BLOCKS = 0;
}

bool FileSystem::Mount(BlockingDisk * _disk) {
   /* Associates the file system with a disk. We limit ourselves to at most one
      file system per disk. Returns TRUE if 'Mount' operation successful (i.e. there
      is indeed a file system on the disk. */
  /* Check if disk is empty and also the size if not 0 */
  Console::puts("In mount \n");
  if((disk == NULL) && (Dir_files!=0)) {
	disk = _disk;
	block_mask[0] = 0xFFFFFFFF; /* Marking the first 32 blocks as used, to store FILE SYSTEM management information */
	return true; /*The disk is associated with this file system and can be mounted */
  }
  else {
	Console::puts("Mount not possible. Another Filesystem on disk \n");
	return false; /* No file system associated with this disk */
  }
}

bool FileSystem::Format(BlockingDisk * _disk, unsigned int _size) {
   /* Wipes any file system from the given disk and installs a new, empty, file
      system that supports up to _size Byte. */
  Console::puts("In format \n");
  if((_size < (BLOCK_SIZE * BLOCKS_PER_FILE)) || (_size > (BLOCK_SIZE * NO_OF_FILES * BLOCKS_PER_FILE))){
	Console::puts("Size of file system is either too small (<5120 bytes) or too big (>10240000 bytes)\n");
	return false;
  }
  disk = NULL;
  size = _size;
  NO_OF_BLOCKS = size/BLOCK_SIZE; /* Total number of blocks in the file system */
  /* Supporting a maximum of 2000 files with 10 blocks each */
  Dir_files = (NO_OF_BLOCKS - 32)/BLOCKS_PER_FILE; /* Total files possible with given size */
						   /* 32 blocks of file system used to store management 						   information */ 
  /* Formatting file system */
  for(int i=0; i<NO_OF_FILES; i++) {
	files[i] = *(&files[0]+sizeof(File)*i);//new File;
	files[i]->file_id = -1;
  }
  /* Marking all blocks as unused - Bitmap clear*/
  for(int i=0; i<BLOCKS_MASK; i++) {
	block_mask[i] = 0;
  }  
  return true;
}

bool FileSystem::LookupFile(int _file_id, File * _file) {
   /* Find file with given id in file system. If found, initialize the file 
      object and return TRUE. Otherwise, return FALSE. */
  for(int i=0; i<NO_OF_FILES; i++) {
	if((files[i]->file_id == _file_id) /*&& (files[i]!=NULL)*/) {
		_file = files[i];
		return true;			
	}
  }
  return false;
}

bool FileSystem::CreateFile(int _file_id) {
   /* Create file with given id in the file system. If file exists already,
      abort and return FALSE. Otherwise, return TRUE. */
  /* Checking if file with the file id already exists, the abort */
  Console::puts("In Createfile \n");
  for(int i=0; i<NO_OF_FILES; i++) {
	if(files[i]->file_id == _file_id) {
		return false;			
	}
  }
  Console::puts("In Createfile pass \n");
  for(int i=0; i<NO_OF_FILES; i++) {

	if(files[i]->file_id == -1/*NULL*/) {
		files[i]->file_system = this;
		files[i]->file_id = _file_id;
 		int count_blocks = 0;
		for(int j=0; j<BLOCKS_MASK; j++ ) {
			int k = 0;
			while((k<32) && (count_blocks < BLOCKS_PER_FILE)){
				if((block_mask[j] & (1<<k)) == 0){
					if(count_blocks == 0) {
						/* Store starting point of block into file */
						files[i]->start_pos = 32*j + k;
					}				
					block_mask[j] = block_mask[j] | (1<<k);
					count_blocks++;
					if(count_blocks == BLOCKS_PER_FILE) {
						return true;
					}
				}
				k++;
			}
		}				
			
	}
  }
  return false;
}

bool FileSystem::DeleteFile(int _file_id) {
   /* Delete file with given id in the file system and free any disk block
      occupied by the file. */
  unsigned int start_blk,temp,t;
  for(int i=0; i<NO_OF_FILES; i++) {
	if(files[i]->file_id == _file_id) {
		start_blk = files[i]->start_pos;
  		temp = (start_blk)/32;
		t = start_blk%32;
		if(t < (32-BLOCKS_PER_FILE))
			block_mask[temp] = block_mask[temp] & (!(0x3FF<<t));
		else {
			block_mask[temp] = block_mask[temp] & (!(((1<<(32-t))-1)<<t));
			block_mask[temp+1] = block_mask[temp+1] & (!((1<<(BLOCKS_PER_FILE-32-t))-1));			
		}
		files[i]->file_id = -1; //NULL;
		return true;
	}
  }
  return false;
}

   

