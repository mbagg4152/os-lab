void
scheduler(void)
{
  struct proc *p;
  struct proc *p1;

  struct cpu *c = mycpu();
  c->proc = 0;

  for(;;){
    sti(); // Enable interrupts on this processor.

    
    //create a process pointer(highPri); This pointer will be used to point to the high priority process

    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->state != RUNNABLE)
            continue;

          // assign the current process as a high priority process
          // Compare the high priority process with the other processes. Remember larger value, lower priority
          // use process pointer p1 to loop over the process table for comparison


          p = highPri;// assigning the high priority process
          c->proc = p;
          switchuvm(p);
          p->state = RUNNING;
          //cprintf("Process chosen: %s, PID: %d\n", p->name, p->pid);
          swtch( &(c->scheduler), p->context);
          switchkvm();

          // Process is done running for now.
          // It should have changed its p->state before coming back.
          c->proc = 0;
    }
    release(&ptable.lock);

  }
}
