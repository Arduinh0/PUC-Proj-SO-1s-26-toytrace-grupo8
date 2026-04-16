#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    char buf[32];
    int fd = open("/etc/hosts", O_RDONLY);
    if (fd < 0) {
        return 1;
    }
    read(fd, buf, sizeof(buf));
    close(fd);
    return 0;
}
