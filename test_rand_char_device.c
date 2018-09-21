#include <errno.h>
#include <fcntl.h> // for open(), close()
#include <stdio.h>
#include <string.h>
#include <unistd.h> // for read(), write()

int main(void)
{

    int status, file_device;
    int receive_num;

    printf("Test opening rand char device\n");
    file_device = open("/dev/rand_char", O_RDONLY); // read only access
    if (file_device < 0) {
        perror("Failed to open rand char device");
        return errno;
    }

    printf("Receiving random number from rand char device:\n");
    status = read(file_device, &receive_num, sizeof(receive_num));
    if (status < 0) {
        perror("Failed to read from rand char device");
        return errno;
    }
    printf("%d\n", receive_num);

    return 0;
}
