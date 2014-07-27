/*
    File: frame_pool.C

    Author: Raghavan

    Description: Management of the Free-Frame Pool.


*/

#include "frame_pool.H"
#include "console.H"

  unsigned long FramePool::Fp[240];
/* Initializes the data structures needed for the management of this
      frame pool. This function must be called before the paging system
      is initialized.
      _base_frame_no is the frame number at the start of the physical memory
      region that this frame pool manages.
      _nframes is the number of frames in the physical memory region that this
      frame pool manages.
      e.g. If _base_frame_no is 16 and _nframes is 4, this frame pool manages
      physical frames numbered 16, 17, 18 and 19
      _info_frame_no is the frame number (within the directly mapped region) of
      the frame that should be used to store the management information of the
      frame pool. However, if _info_frame_no is 0, the frame pool is free to
      choose any frame from the pool to store management information.
      */
FramePool::FramePool(unsigned long _base_frame_no, unsigned long _nframes, unsigned long _info_frame_no){
  unsigned int i,j;
  base_frame_no = _base_frame_no;
  if(base_frame_no<1024){
	for(i=0;i<(_nframes/32);i++){
		Fp[i] = 0;
  	}
	for(j=0;j<(_nframes%32);j++){
		Fp[i] = Fp[i] & (0<<j);
	}
  }
  else{
	for(i=16;i<(_nframes/32);i++){
		Fp[i] = 0;
	}
	for(j=0;j<(_nframes%32);j++){
		Fp[i] = Fp[i] & (0<<j);
	}
  }  
}

/* Allocates a frame from the frame pool. If successful, returns the frame
 number of the frame. If fails, returns 0. */

unsigned long FramePool::get_frame(){
  unsigned int i,j;
  if(this->base_frame_no <1024){
  for(i=0;i<16;i++){
	j=0;
		while(j<32){
			if((Fp[i] & (1<<j)) == 0){
				Fp[i] = Fp[i] | (1<<j);
				return (i*32+j+512); 
			}
			j++;
		}
  }	
  }	
  else{
  for(i=16;i<240;i++){
	j=0;
		while(j<32){
			if((Fp[i] & (1<<j)) == 0){
				Fp[i] = Fp[i] | (1<<j);
				return (i*32+j+512); 
			}
			j++;
		}
  }	
  }		
  return (unsigned long) (0);
}

 /* Mark the area of physical memory as inaccessible. The arguments have the
    * same semanticas as in the constructor.
    */
void FramePool::mark_inaccessible(unsigned long _base_frame_no, unsigned long _nframes){
  unsigned int i,j=31;
  _base_frame_no-=512;
  for(i=(_base_frame_no/32);i<((_base_frame_no+_nframes)/32);i++){
	if(((_base_frame_no%32)!=0) && (j==31)){
		while(j>=(_base_frame_no%32)){
			Fp[i] = Fp[i] | (1<<j);
			j--;
		}
	continue;
	}
	Fp[i]=0xFFFFFFFF;
  }
  j=0;
  while(j<((_base_frame_no+_nframes)%32)){
	Fp[i] = Fp[i] | (1<<j);
	j++;
  }
}  

/* Releases frame back to the given frame pool.
      The frame is identified by the frame number. 
      NOTE: This function is static because there may be more than one frame pool
      defined in the system, and it is unclear which one this frame belongs to.
      This function must first identify the correct frame pool and then call the frame
      pool's release_frame function. */
void FramePool:: release_frame(unsigned long _frame_no){
  unsigned int i,j=0;
  i = (_frame_no-512)/32;
  Fp[i] = Fp[i] & (!(1<<(_frame_no%32)));	
}
   

