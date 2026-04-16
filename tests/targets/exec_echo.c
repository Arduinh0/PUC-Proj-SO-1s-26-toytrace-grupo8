#include <unistd.h>

int main(void)
{
    char *const argv[] = {"/bin/echo", "eco", NULL};
    char *const envp[] = {NULL};

    execve("/bin/echo", argv, envp);
    return 1;
}
