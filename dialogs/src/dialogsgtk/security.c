#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>	
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define COMMON_INTERPRETERS_COUNT 9
static char common_interpreters[COMMON_INTERPRETERS_COUNT][9]={"python","perl","php","lua","ruby","awk","groovy","d8","gdb"};

enum {PARENT_ERROR=-1,PARENT_UNTRUSTED=0,PARENT_TRUSTED,PARENT_INTERPRETER};
enum {FALSE=0,TRUE};


int is_file_trusted(char* path)
{
	struct stat statbuf;
	if(stat(path,&statbuf)<0)
		return PARENT_ERROR;
	if(statbuf.st_uid!=0 && (statbuf.st_mode & S_IWUSR))							// non-root owner has write permissions
		return PARENT_UNTRUSTED;
	if(statbuf.st_gid!=0 && (statbuf.st_mode & S_IWGRP))							// non-root group has write permissions
		return PARENT_UNTRUSTED;
	if(statbuf.st_mode & S_IWOTH)													// world has write permissions!
		return PARENT_UNTRUSTED;
	if(access(path,W_OK)==0)														// current user has write permissions!
		return PARENT_UNTRUSTED;
	return PARENT_TRUSTED;
}

char* get_env_for_other_process(char* label,pid_t pid)
{
	char 		proc_path[32];
	ssize_t		exec_path_len=-1;
	char*		value=NULL;

	snprintf(proc_path,sizeof(proc_path)-1,"/proc/%d/environ",pid);
	int environ=open(proc_path,O_RDONLY);
	if(environ>0)
	{
		char buffer[4096];
		int	label_pos=0,label_size=strlen(label);
		int parent_shlvl=0;

		int nread=read(environ,buffer,sizeof(buffer));
		while(nread>0 && value==NULL)
		{
			int i;
			for(i=0;i<nread;i++)
			{
				if(buffer[i]==label[label_pos])
				{
					label_pos++;
					if(label_pos==(label_size-1))
					{
						value=strdup(&(buffer[i]));
						break;
					}
				}
				else
					label_pos=0;
			}
					
			nread=read(environ,buffer,sizeof(buffer));
		}
		close(environ);
	}

	return value;
}

int parent_ends_in_sh(char* path)
{
	char* sh_found=strstr(path,"sh");					// executable name ends with "sh"
	return(sh_found!=NULL && (*(sh_found+2))=='\0');
}

int parent_is_system_shell(char* path)
{
    FILE* shells=fopen("/etc/shells","r");				// executable is listed in /etc/shells
    if(shells!=NULL)
	{
		int found=FALSE;
    	char line[BUFSIZ];

    	while((!found) && fgets(line,sizeof(line),shells))
		{
			line[strlen(line)-1]='\0';
			if(strcmp(path,line)==0)
				found=TRUE;	
		}
		fclose(shells);

		if(found)
			return TRUE;
    }
}

int parent_has_shlvl_minus_one(void)
{
	char* kid_shlvl_string=getenv("SHLVL");
	if(kid_shlvl_string!=NULL)
	{
		int kid_shlvl=atoi(kid_shlvl_string);
		if(kid_shlvl>0)
		{
			char* parent_shlvl_string=get_env_for_other_process("SHLVL",getppid());
			if(parent_shlvl_string!=NULL)
			{
				int parent_shlvl=atoi(parent_shlvl_string);
				if(parent_shlvl>0)
				{
					if(kid_shlvl==(parent_shlvl+1))
						return TRUE;
				}
			}
		}
	}

	return FALSE;
}


int parent_is_common_interpreter(char* path)
{
	char* filename=basename(path);
	int i;
	for(i=0;i<COMMON_INTERPRETERS_COUNT;i++)
		if(strcmp(common_interpreters[i],filename)==0)
			return TRUE;
	return FALSE;
}


int parent_looks_like_interpreter(char* path)
{
	if(parent_ends_in_sh(path))
		return TRUE;
	if(parent_is_system_shell(path))
		return TRUE;
	if(parent_is_common_interpreter(path))
		return TRUE;
	if(parent_has_shlvl_minus_one())
		return TRUE;
	return FALSE;
}

int is_regular_file_trusted(char* path)
{
	struct stat statbuf;
	stat(path,&statbuf);
	if(!S_ISREG(statbuf.st_mode))													// not a regular file
		return PARENT_UNTRUSTED;
	return is_file_trusted(path);
}

int is_directory_trusted(char* path)
{
	struct stat statbuf;
	stat(path,&statbuf);
	if(!S_ISDIR(statbuf.st_mode))													// not a directory
		return PARENT_UNTRUSTED;
	return is_file_trusted(path);
}

ssize_t get_parent_path(char* exec_path, size_t exec_path_size)
{
	char 		proc_path[32];
	ssize_t		exec_path_len=-1;

	snprintf(proc_path,sizeof(proc_path)-1,"/proc/%d/exe",getppid());
	if((exec_path_len=readlink(proc_path,exec_path,exec_path_size-1))!=-1)
		exec_path[exec_path_len]='\0';
	return exec_path_len;
}


int is_parent_trusted(char* exec_path)
{
	int 		trust;
	char*		dir_path;
	struct stat	statbuf;

	if(parent_looks_like_interpreter(exec_path))
		return PARENT_INTERPRETER;

	trust=is_regular_file_trusted(exec_path);
	if(trust==PARENT_ERROR || trust==PARENT_UNTRUSTED)
		return trust;

	dir_path=dirname(exec_path);
	while(strlen(dir_path)>1)
	{
		trust=is_directory_trusted(dir_path);
		if(trust==PARENT_ERROR || trust==PARENT_UNTRUSTED)
			return trust;
		dir_path=dirname(dir_path);
	}

	return is_directory_trusted(dir_path);
}

int error(void)
{
	fprintf(stderr,"\neID-Dialogs (c) 2011 FedICT.\n\n");
	fprintf(stderr,"This program is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n(at your option) any later version.\n\nThis program is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\nYou should have received a copy of the GNU General Public License\nalong with this program. If not, see <http://www.gnu.org/licenses/>.\n\n");
	fprintf(stderr,"eid-mw Dialogs are intended to be called by the eid pkcs#11 module.\nPlease do not call them directly.\n\n");
	printf("ERROR\n");
}

void abort_if_parent_not_considered_secure(void)
{
	char exec_path[2048];
	if(get_parent_path(exec_path,sizeof(exec_path))<0 || is_parent_trusted(exec_path)!=PARENT_TRUSTED)
	{
		error();
		exit(1);
	} 
}
