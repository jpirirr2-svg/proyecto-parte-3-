#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/version.h>

#define PROC_NAME "procmon"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Grupo de Sistemas Operativos - Integrante 3");
MODULE_DESCRIPTION("Modulo de kernel para monitorear procesos activos y su PPID");
MODULE_VERSION("1.1");

static int procmon_show(struct seq_file *m, void *v) {
    struct task_struct *task;
    pid_t ppid;
    int total_procesos = 0;

    seq_printf(m, "==================================================================\n");
    seq_printf(m, "%-10s | %-10s | %-25s | %-10s\n", "PID", "PPID", "NOMBRE (COMM)", "ESTADO");
    seq_printf(m, "==================================================================\n");

    rcu_read_lock();

    for_each_process(task) {
        if (task->real_parent != NULL) {
            ppid = task->real_parent->pid;
        } else {
            ppid = 0;
        }

        seq_printf(m, "%-10d | %-10d | %-25s | %-10u\n",
                   task->pid,
                   ppid,
                   task->comm,
                   task->__state);

        total_procesos++;
    }

    rcu_read_unlock();

    seq_printf(m, "==================================================================\n");
    seq_printf(m, "Total de procesos listados: %d\n", total_procesos);

    return 0;
}

static int procmon_open(struct inode *inode, struct file *file) {
    return single_open(file, procmon_show, NULL);
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops procmon_fops = {
    .proc_open    = procmon_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};
#else
static const struct file_operations procmon_fops = {
    .owner   = THIS_MODULE,
    .open    = procmon_open,
    .read    = seq_read,
    .llseek  = seq_lseek,
    .release = single_release,
};
#endif

static int __init procmon_init(void) {
    struct proc_dir_entry *entry;

    entry = proc_create(PROC_NAME, 0444, NULL, &procmon_fops);
    if (!entry) {
        printk(KERN_ERR "procmon: Error al crear la entrada /proc/%s\n", PROC_NAME);
        return -ENOMEM;
    }

    printk(KERN_INFO "procmon: Modulo cargado exitosamente (Extension PPID activa).\n");
    return 0;
}

static void __exit procmon_exit(void) {
    remove_proc_entry(PROC_NAME, NULL);
    printk(KERN_INFO "procmon: Modulo descargado correctamente.\n");
}

module_init(procmon_init);
module_exit(procmon_exit);