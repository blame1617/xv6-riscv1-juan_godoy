#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "spinlock.h"
#include "proc.h"

struct spinlock tickslock;
uint ticks;

extern char trampoline[], uservec[];

// in kernelvec.S, calls kerneltrap().
void kernelvec();

extern int devintr();

void
trapinit(void)
{
  initlock(&tickslock, "time");
}

// set up to take exceptions and traps while in the kernel.
void
trapinithart(void)
{
  w_stvec((uint64)kernelvec);
}

//
// handle an interrupt, exception, or system call from user space.
// called from, and returns to, trampoline.S
// return value is user satp for trampoline.S to switch to.
//
uint64
usertrap(void)
{
  int which_dev = 0;

  if((r_sstatus() & SSTATUS_SPP) != 0)
    panic("usertrap: not from user mode");

  // send interrupts and exceptions to kerneltrap(),
  // since we're now in the kernel.
  w_stvec((uint64)kernelvec);

  struct proc *p = myproc();

  // save user program counter.
  p->trapframe->epc = r_sepc();

  if(r_scause() == 8){
    // system call

    if(killed(p))
      exit(-1);

    // sepc points to the ecall instruction,
    // but we want to return to the next instruction.
    p->trapframe->epc += 4;

    // an interrupt will change sepc, scause, and sstatus,
    // so enable only now that we're done with those registers.
    intr_on();

    syscall();
  } else if((which_dev = devintr()) != 0){
    // ok: device interrupt handled in devintr()
  } else if (r_scause() == 13 || r_scause() == 15) {
    // Page fault de lectura (13) o escritura (15)
    uint64 scause = r_scause();
    uint64 va = r_stval();
    uint64 va_page = PGROUNDDOWN(va);
    pte_t *pte = walk(p->pagetable, va_page, 0);

    // Caso 1: PTE inexistente o no válida → page fault real
    if (pte == 0 || !(*pte & PTE_V)) {
      printf("usertrap(): unexpected scause %lu pid=%d\n", scause, p->pid);
      printf("            sepc=%p stval=%p\n", (void*)r_sepc(), (void*)va);
      p->killed = 1;

    // Caso 2: acceso a memoria no de usuario
    } else if (!(*pte & PTE_U)) {
      printf("usertrap(): acceso ilegal a memoria del kernel pid=%d\n", p->pid);
      p->killed = 1;

    // Caso 3: página marcada como "no lectura" (tu bit especial PTE_RSW)
    } else if (*pte & PTE_RSW) {

      // Load fault → lectura prohibida: matar proceso (lo esperado en la tarea)
      if (scause == 13) {
        printf("usertrap(): lectura prohibida en va=%p pid=%d\n",
               (void*)va_page, p->pid);
        p->killed = 1;

      // Store fault → permitir escritura si la página es realmente escribible
      } else { // scause == 15
        if (*pte & PTE_W) {
          // Posible stale TLB: invalidar y continuar para reintentar la instrucción.
	 *pte |= PTE_R;
	 sfence_vma();
          // No matar; dejamos que prepare_return() ocurra más abajo y el usuario
          // reintente la instrucción.
          // Programar que se quite R al volver a user space
          // (esto es un hack: marcar que necesitamos limpiar R después)
          p->need_clear_pte_r = 1;
          p->clear_pte_addr = va_page;

        } else {
          printf("usertrap(): store fault en página no escribible va=%p pid=%d\n",
                 (void*)va_page, p->pid);
          p->killed = 1;
        }
      }

    // Caso 4: page fault normal en página no marcada
    } else {
      printf("usertrap(): unexpected scause %lu pid=%d\n", scause, p->pid);
      printf("            sepc=%p stval=%p\n", (void*)r_sepc(), (void*)va);
      p->killed = 1;
    }

    if (killed(p))
      exit(-1);
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2)
    yield();

  prepare_return();

  // the user page table to switch to, for trampoline.S
  uint64 satp = MAKE_SATP(p->pagetable);

  // return to trampoline.S; satp value in a0.
  return satp;
}

//
// set up trapframe and control registers for a return to user space
//
void
prepare_return(void)
{
  struct proc *p = myproc();
  // Limpiar PTE_R si se habilitó temporalmente
  if(p->need_clear_pte_r) {
    pte_t *pte = walk(p->pagetable, p->clear_pte_addr, 0);
    if(pte && (*pte & PTE_RSW)) {
      *pte &= ~PTE_R;
      sfence_vma();
    }
    p->need_clear_pte_r = 0;
  }

  // we're about to switch the destination of traps from
  // kerneltrap() to usertrap(). because a trap from kernel
  // code to usertrap would be a disaster, turn off interrupts.
  intr_off();

  // send syscalls, interrupts, and exceptions to uservec in trampoline.S
  uint64 trampoline_uservec = TRAMPOLINE + (uservec - trampoline);
  w_stvec(trampoline_uservec);

  // set up trapframe values that uservec will need when
  // the process next traps into the kernel.
  p->trapframe->kernel_satp = r_satp();         // kernel page table
  p->trapframe->kernel_sp = p->kstack + PGSIZE; // process's kernel stack
  p->trapframe->kernel_trap = (uint64)usertrap;
  p->trapframe->kernel_hartid = r_tp();         // hartid for cpuid()

  // set up the registers that trampoline.S's sret will use
  // to get to user space.

  // set S Previous Privilege mode to User.
  unsigned long x = r_sstatus();
  x &= ~SSTATUS_SPP; // clear SPP to 0 for user mode
  w_sstatus(x);

  // set S Exception Program Counter to the saved user pc.
  w_sepc(p->trapframe->epc);
}

// interrupts and exceptions from kernel code go here via kernelvec,
// on whatever the current kernel stack is.
void
kerneltrap()
{
  int which_dev = 0;
  uint64 sepc = r_sepc();
  uint64 sstatus = r_sstatus();
  uint64 scause = r_scause();

  if((sstatus & SSTATUS_SPP) == 0)
    panic("kerneltrap: not from supervisor mode");
  if(intr_get() != 0)
    panic("kerneltrap: interrupts enabled");

  if((which_dev = devintr()) == 0){
    // interrupt or trap from an unknown source
    printf("scause=0x%lx sepc=0x%lx stval=0x%lx\n", scause, r_sepc(), r_stval());
    panic("kerneltrap");
  }

  // give up the CPU if this is a timer interrupt.
  if(which_dev == 2 && myproc() != 0)
    yield();

  // the yield() may have caused some traps to occur,
  // so restore trap registers for use by kernelvec.S's sepc instruction.
  w_sepc(sepc);
  w_sstatus(sstatus);
}

void
clockintr()
{
  if(cpuid() == 0){
    acquire(&tickslock);
    ticks++;
    wakeup(&ticks);
    release(&tickslock);
  }

  // ask for the next timer interrupt. this also clears
  // the interrupt request. 1000000 is about a tenth
  // of a second.
  w_stimecmp(r_time() + 1000000);
}

// check if it's an external interrupt or software interrupt,
// and handle it.
// returns 2 if timer interrupt,
// 1 if other device,
// 0 if not recognized.
int
devintr()
{
  uint64 scause = r_scause();

  if(scause == 0x8000000000000009L){
    // this is a supervisor external interrupt, via PLIC.

    // irq indicates which device interrupted.
    int irq = plic_claim();

    if(irq == UART0_IRQ){
      uartintr();
    } else if(irq == VIRTIO0_IRQ){
      virtio_disk_intr();
    } else if(irq){
      printf("unexpected interrupt irq=%d\n", irq);
    }

    // the PLIC allows each device to raise at most one
    // interrupt at a time; tell the PLIC the device is
    // now allowed to interrupt again.
    if(irq)
      plic_complete(irq);

    return 1;
  } else if(scause == 0x8000000000000005L){
    // timer interrupt.
    clockintr();
    return 2;
  } else {
    return 0;
  }
}

