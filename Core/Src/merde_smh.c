/* USER CODE BEGIN 0 */
#include <time.h>

//#pragma import(__use_no_semihosting_swi)  
//#pragma import(_main_redirection)

const char __stdin_name[150];  
const char __stdout_name[150];  
const char __stderr_name[150];  
typedef int FILEHANDLE;  
 
void _sys_exit(int status)  
{  
    while(1);  
}  

FILEHANDLE _sys_open(const char *name, int openmode)  
{  
    return 0;  
}  

int _sys_close(FILEHANDLE fh)  
{  
    return 0;  
}  

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)  
{  
    return 0;  
}  

int _sys_read(FILEHANDLE fh, unsigned char*buf, unsigned len, int mode)  
{  
    return 0;  
}  

int _sys_istty(FILEHANDLE fh)  
{  
    return 0;  
}  

int _sys_seek(FILEHANDLE fh, long pos)  
{  
    return 0;  
}  

int _sys_ensure(FILEHANDLE fh)  
{  
    return 0;  
}  

long _sys_flen(FILEHANDLE fh)  
{  
    return 0;  
}  

int _sys_tmpnam(char *name, int fileno, unsigned maxlength)  
{  
    return 0;  
}  

void _ttywrch(int ch)  
{  
}

time_t time(time_t *t)  
{  
    return 0;  
}  
int remove(const char *filename)  
{  
    return 0;  
}  

char *_sys_command_string(char *cmd, int len)  
{  
    return 0;  
}  

clock_t clock(void)  
{  
    return 0;  
}

/* USER CODE END 0 */
