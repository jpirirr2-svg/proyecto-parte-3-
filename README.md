# Extensión del Módulo de Kernel: `procmon` (Integrante 3)

## Descripción de la Modificación
Se implementó la **Opción 1: Mostrar el PPID (Parent Process ID)** de cada proceso activo en el sistema. El módulo crea el archivo virtual `/proc/procmon` y despliega una tabla formateada con el PID, PPID, nombre del ejecutable (`comm`) y el estado actual del proceso.

## Cambios Realizados en `procmon.c`

1. **Inclusión de cabeceras del planificador (`<linux/sched/signal.h>`):**
   Se añadió esta librería para garantizar el acceso a la macro `for_each_process` y a la estructura `task_struct` en versiones modernas del kernel de Linux (4.11+).

2. **Extracción del PPID mediante `real_parent`:**
   Dentro del bucle `for_each_process(task)`, se accedió al puntero `task->real_parent`, el cual apunta al `task_struct` del proceso que creó al proceso actual. A partir de esta estructura padre, se extrajo el identificador con `task->real_parent->pid`.

3. **Validación de Punteros y Concurrencia (RCU Lock):**
    * Se implementó una validación `if (task->real_parent != NULL)` antes de obtener el PPID para evitar fallos de segmentación en el kernel en caso de evaluar tareas base del sistema.
    * Se envolvió el recorrido de procesos entre las funciones `rcu_read_lock()` y `rcu_read_unlock()`. Esto bloquea de forma segura la lista de procesos durante la lectura, evitando que un proceso sea destruido en memoria justo mientras el módulo lee su información.

4. **Compatibilidad de Versiones (`proc_ops` vs `file_operations`):**
   Se incluyó una directiva condicional con `LINUX_VERSION_CODE` para que el módulo compile sin errores tanto en kernels recientes (5.6+, que usan `struct proc_ops`) como en versiones anteriores.

## Instrucciones Rápidas (Para el Integrante 4)
Para compilar y probar este repositorio en Ubuntu:
1. Clonar el repositorio y entrar a la carpeta.
2. Compilar ejecutando: `make`
3. Cargar el módulo: `sudo insmod procmon.ko`
4. Ver la tabla de procesos y PPID: `cat /proc/procmon`
5. Descargar el módulo: `sudo rmmod procmon`