#include <lib.h>

char buf[8192];

void cat(int f, char* s) {
    long n;
    int r;

    while ((n = read(f, buf, (long)sizeof buf)) > 0) {
        //debugf("buf:%s\n", buf);
        if ((r = write(1, buf, n)) != n) {
            user_panic("write error copying %s: %d", s, r);
        }
    }
    // debugf("n==%d\n", n);
    if (n < 0) {
        user_panic("error reading %s: %d", s, n);
    }
}

int main(int argc, char** argv) {
    int f, i;

    if (argc == 1) {
        cat(0, "<stdin>");
    } else {
        for (i = 1; i < argc; i++) {
            f = open(argv[i], O_RDONLY);
            //debugf("Open!!!\n");
            struct Fd* fd;
            // fd_lookup(f, &fd);
            // debugf("fd_offset: %d\n", fd->fd_offset);
            if (f < 0) {
                user_panic("can't open %s: %d", argv[i], f);
            } else {
                cat(f, argv[i]);
                close(f);
            }
        }
    }
    return 0;
}
