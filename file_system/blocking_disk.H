

#include "simple_disk.H"

/* Global count for interating through queue */
static int i1=0;

class BlockingDisk : public SimpleDisk{

	DISK_ID      disk_id;            /* This disk is either MASTER or SLAVE */

	unsigned int disk_size;          /* In Byte */

	void issue_operation(DISK_OPERATION _op, unsigned long _block_no);
     /* Send a sequence of commands to the controller to initialize the READ/WRITE 
        operation. This operation is called by read() and write(). */ 

        struct blocked_queue {
		DISK_OPERATION _op;
		unsigned long _block_no;
		unsigned char * _buf;
	};
	blocked_queue *bq[5];

public:

	bool is_ready();
     	/* Return TRUE if disk is ready to transfer data from/to disk, FALSE otherwise. */

	BlockingDisk(DISK_ID _disk_id, unsigned int _size) : SimpleDisk(_disk_id,_size) {
		disk_id   = _disk_id;
   		disk_size = _size;
	}
	/* Creates a BlcokingDisk device derived from SimpleDiskwith the given size connected to the 		MASTER or SLAVE slot of the primary ATA controller. */

	void read(unsigned long _block_no, unsigned char * _buf);
   	/* Reads 512 Bytes from the given block of the disk and copies them 
        to the given buffer. No error check! */

	void write(unsigned long _block_no, unsigned char * _buf);
   	/* Writes 512 Bytes from the buffer to the given block on the disk. */

	void operate();
	/* Function to perform the read/write operation, after the disk is ready */
};

