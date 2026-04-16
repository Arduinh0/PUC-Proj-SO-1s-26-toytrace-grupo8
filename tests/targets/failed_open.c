#include <fcntl.h>

int main(void)
{
    int fd = open("/arquivo/que/nao/deve/existir/toytrace", O_RDONLY);
    return fd < 0 ? 0 : 1;
}
