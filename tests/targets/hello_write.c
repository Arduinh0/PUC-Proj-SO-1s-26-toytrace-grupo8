#include <string.h>
#include <unistd.h>

int main(void)
{
    const char msg[] = "ola toytrace\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    return 0;
}
