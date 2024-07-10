#include <lib.h>

int main(int argc, char** argv) {
    int i, nflag;

    nflag = 0;
    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        nflag = 1;
        argc--;
        argv++;
    }
    for (i = 1; i < argc; i++) {
        if (i > 1) {
            printf(" ");
        }
        printf("%s", argv[i]);
    }
    if (!nflag) {
        printf("\n");
    }
    struct Filefd* fd = num2fd(1);
    //printf("name:%s\n", fd->f_file.f_name);
    return 0;
}
