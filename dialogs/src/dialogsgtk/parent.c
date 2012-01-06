#include <stdlib.h>
#include "config.h"
#include "parent.h"

// get the path of the parent process' executable
///////////////////////////////////////////////////////////////
ssize_t get_parent_path(char* exec_path, size_t exec_path_size)
{
        char            proc_path[32];
        ssize_t         exec_path_len=-1;

        snprintf(proc_path,sizeof(proc_path)-1,"/proc/%d/exe",getppid());
        if((exec_path_len=readlink(proc_path,exec_path,exec_path_size-1))!=-1)
                exec_path[exec_path_len]='\0';
        return exec_path_len;
}

