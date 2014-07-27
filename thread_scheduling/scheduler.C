/* 
    Author: Raghavan
    A thread scheduler.

*/

#include "scheduler.H"
#include "console.H"
#include "machine.H"

Thread * Scheduler::curr_thread;
Thread * Scheduler::nxt_thread;

/* Setup the scheduler. This sets up the ready queue, for example.
      If the scheduler implements some sort of round-robin scheme, then the 
      end_of_quantum handler is installed here as well. */
Scheduler::Scheduler()
{
  /* Assign all pointers to zero initially */
  curr_thread = 0;
  nxt_thread = 0;
}

/* Called by the currently running thread in order to give up the CPU. 
      The scheduler selects the next thread from the ready queue to load onto 
      the CPU, and calls the dispatcher function defined in 'threads.h' to
      do the context switch. */
void Scheduler::yield()
{
  /* Dispatch call to the next thread in the queue */
  Thread::dispatch_to(nxt_thread);
}

/* Add the given thread to the ready queue of the scheduler. This is called
      for threads that were waiting for an event to happen, or that have 
      to give up the CPU in response to a preemption. */
void Scheduler::resume(Thread * _thread)
{
  /* If the first thread assign it and define the next thread in the queue appropriately */
  if(curr_thread == 0){
  	curr_thread = _thread;
	curr_thread->next_thread = nxt_thread;
  }
  /* If the last thread, then loop back to point to the first thread in the queue */
  else if(_thread->next_thread == 0){
	_thread->next_thread = curr_thread;
	nxt_thread = curr_thread;
  }
  else
	nxt_thread = _thread->next_thread;
}
   
/* Make the given thread runnable by the scheduler. This function is called
	  typically after thread creationl. Depending on the
      implementation, this may not entail more than simply adding the 
      thread to the ready queue (see scheduler_resume). */
void Scheduler::add(Thread * _thread)
{
  /* Temp thread pointer to iterate through the queue */
  Thread * temp;
  /* If first thread, then the first add will be next thread */
  if(nxt_thread == 0)
	nxt_thread = _thread;

  else{
	temp = nxt_thread;
	while(temp!= 0){
  		/* If the next thread not defined, assign it and then break */
		if(temp->next_thread == 0){
			temp->next_thread = _thread;
			break;
		}
	temp = temp->next_thread;
	}
  }  
}

/* Remove the given thread from the scheduler in preparation for destruction
      of the thread. */
void Scheduler::terminate(Thread * _thread)

{
  /* Temp thread pointer to iterate through the queue */
  Thread * temp = curr_thread;
  while(temp!=0){
	/* Check if the next thread pointer is the one to be terminated */
	if(temp->next_thread == _thread){
		/* Disable interrupts for mutual exclusion while terminating thread */
  		Machine::disable_interrupts();
		  /* Point Next thread pointer of current thread to the next->next thread pointer*/
		temp->next_thread = _thread->next_thread;
		  /* If the first thread is to be terminated make additional changes */
 		if(_thread == curr_thread){
			curr_thread = _thread->next_thread;
			nxt_thread = curr_thread->next_thread;
		}
		else
			nxt_thread = _thread->next_thread;
 		_thread = 0;
		/* Re-enable interrupts after terminating thread */
		Machine::enable_interrupts();
		/* Call yield() in order to dispatch the next thread */
		Scheduler::yield();
	}
	temp = temp->next_thread;
  }
}
   
