#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

struct passwd *newpw(struct passwd *src) {
	struct passwd *dst;
	if((dst = malloc(sizeof(struct passwd))) == NULL) goto err1;
	if((dst -> pw_name = malloc((strlen(src->pw_name) + 1) * sizeof(char))) == NULL) goto err2;
	if((dst -> pw_passwd = malloc((strlen(src->pw_passwd) + 1) * sizeof(char))) == NULL) goto err3;
	if((dst -> pw_gecos = malloc((strlen(src->pw_gecos) + 1) * sizeof(char))) == NULL) goto err4;
	if((dst -> pw_dir = malloc((strlen(src->pw_dir) + 1) * sizeof(char))) == NULL) goto err5;
	if((dst -> pw_shell = malloc((strlen(src->pw_shell) + 1) * sizeof(char))) == NULL) goto err6;
	
	strcpy(dst -> pw_name, src -> pw_name);
	strcpy(dst -> pw_passwd, src -> pw_passwd);
	strcpy(dst -> pw_gecos, src -> pw_gecos);
	strcpy(dst -> pw_dir, src -> pw_dir);
	strcpy(dst -> pw_shell, src -> pw_shell);
	dst -> pw_uid = src -> pw_uid;
	dst -> pw_gid = src -> pw_gid;
	return dst;
	/* error path*/
	err6: free(dst -> pw_dir);
	err5: free(dst -> pw_gecos);
	err4: free(dst -> pw_passwd);
	err3: free(dst -> pw_name);
	err2: free(dst);
	err1:
	errno = ENOMEM;
	return NULL;
}

struct passwd* mygetpwnam(char* name)
{
	static struct passwd pwd;
	
	setpwent();
	struct passwd * p_pwd = NULL;
	struct passwd * pass = NULL;
	while((pass = getpwent()) != NULL)
	{
		if(strcmp(pass->pw_name, name) == 0)
		{
			p_pwd = newpw(pass);
			if(p_pwd == NULL)
			{
				int temp_err = errno;
				endpwent();
				errno = temp_err;
				return NULL;
			}
			
			free(pwd.pw_name);
			free(pwd.pw_passwd);
			free(pwd.pw_gecos);
			free(pwd.pw_class);
			free(pwd.pw_dir);
			free(pwd.pw_shell);
			
			pwd = *p_pwd;
			free(p_pwd);
			
			endpwent();
			return &pwd;
		}

		pass = getpwent();
	}
  
	endpwent();
	
	errno = ENOENT;
	return NULL;
}

int main(int argc, char * argv[])
{
  if(argc != 2)
  {
    fprintf(stderr, "Usage: %s username\n", argv[0]);
    return 1;
  }
  
  struct passwd * result = NULL;
  if((result = mygetpwnam(argv[1])) == NULL)
  {
	  perror(argv[1]);
	  return 1;
  }
  
  printf("gecos of %s: %s\n", argv[1], result->pw_gecos);
  
  return 0;
}

