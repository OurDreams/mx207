/**
 ***************************************************************************
 *
 * @file      syscalls.c
 * @brief     GLibc½Ó¿Úº¯Êý.
 *
 *
 *******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>

//struct _reent r = {0, (FILE *) 0, (FILE *) 1, (FILE *) 0};
//struct _reent *_impure_ptr = &r;

int
_close(int file)
{
    return -1;
}

int
_fstat(int file, struct stat *st)
{
    st->st_mode = S_IFCHR;
    return 0;
}


int
_isatty(int file)
{
    return 1;

}

int
_lseek(int file, int ptr, int dir)
{
    return 0;
}

int
_open(const char *name, int flags, int mode)
{
    return -1;
}

int
_read(int file, char *ptr, int len)
{
    return 0;
}


caddr_t
_sbrk(int incr)
{
#if 0

    extern char heap_low; /* Defined by the linker */
    extern char cstack_top;
    extern int  consoleFd;      /* fd of initial console device */


    static char *heap_end ;
    char *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = &heap_low;
    }
    prev_heap_end = heap_end;
    if ((heap_end + incr) > (&cstack_top - 0x200))
    {
        /* Heap and stack collision */
        char ch = 'X';
        ttyWrite(consoleFd, (char*) &ch, 1);
        return (caddr_t) 0;
    }
    heap_end += incr;
    return (caddr_t) prev_heap_end;
#else
return 0;
#endif
}

int
_write(int file, char *ptr, int len)
{
    return len;

}

