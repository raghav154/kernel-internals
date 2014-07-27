/*
    File: vm_pool.H

    Author: raghavan

    Description: Management of the Virtual Memory Pool


*/

#include "vm_pool.H"
#include "console.H"
#include "page_table.H"

//const unsigned int PageTable::PAGE_SIZE ; /* in bytes */
//const unsigned int PageTable::ENTRIES_PER_PAGE ; /* in entries, duh! */

/* Initializes the data structures needed for the management of this
    * virtual-memory pool.
    * _base_address is the logical start address of the pool.
    * _size is the size of the pool in bytes.
    * _frame_pool points to the frame pool that provides the virtual
    * memory pool with physical memory frames.
    * _page_table points to the page table that maps the logical memory
    * references to physical addresses. */
VMPool::VMPool(unsigned long _base_address, unsigned long _size, FramePool *_frame_pool,        PageTable *_page_table){ 
  frame_pool = _frame_pool;
  page_table = _page_table;
  base_address = _base_address;
  size = _size;
  count=0;
  VM_addrs = NULL;
  VM_sizes = NULL;
  page_table->register_vmpool(this);
}   

/* Allocates a region of _size bytes of memory from the virtual
    * memory pool. If successful, returns the virtual address of the
    * start of the allocated region of memory. If fails, returns 0. */
unsigned long VMPool::allocate(unsigned long _size){
  page_table->register_vmpool(this);
  if(VM_addrs==NULL && VM_sizes==NULL){
	VM_addrs = (unsigned long *) ((frame_pool->get_frame())*(page_table->PAGE_SIZE));
  	VM_sizes = (unsigned long *) ((frame_pool->get_frame())*(page_table->PAGE_SIZE));
  }
  if (_size > size)
	return 0;
  VM_sizes[count] = _size;
  VM_addrs[count] = base_address;
  for(unsigned int i=0;i<count;i++){
	VM_addrs[count] += VM_sizes[i];
  }
  size -= _size;
  count++;
  return VM_addrs[count-1];
}
  
/* Releases a region of previously allocated memory. The region
    * is identified by its start address, which was returned when the
    * region was allocated. */   
void VMPool::release(unsigned long _start_address){
  unsigned int i,j;
  for(i=0;i<count;i++){
	if(VM_addrs[i] == _start_address){
		size+= VM_sizes[i];
		j = VM_sizes[i]/(page_table->PAGE_SIZE);
		VM_addrs[i] = NULL;
		while(j<0){
			page_table->free_page(((unsigned long)(&VM_addrs[i]))+page_table->ENTRIES_PER_PAGE+j);		
			j--;
		}
		break;
	}
  }
}

/* Returns FALSE if the address is not valid. An address is not valid
    * if it is not part of a region that is currently allocated. */  
BOOLEAN VMPool::is_legitimate(unsigned long _address){
  if((_address == ((unsigned long)VM_addrs)) || (((unsigned long)VM_sizes) == _address))
	return true;
  
  if((_address >= base_address) && (_address < base_address+size)){	
	return true;
  }
  return false;
}





 
