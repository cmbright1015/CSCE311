/* user/hello.c */
#include <unistd.h>
int main() {
    const char *msg = "hello from user\n";
    write(1, msg, 18);
    return 0;
}
