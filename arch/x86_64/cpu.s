.global cpu_irq_save
.global cpu_irq_restore

cpu_irq_save:
    pushfq
    pop %rax
    cli
    ret

cpu_irq_restore:
    push %rdi
    popfq
    ret