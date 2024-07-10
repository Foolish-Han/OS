#include <lib.h>
int main(int argc, char** argv) {
    if (argc == 2) {
        char path[1024];
        strcpy(path, argv[1]);
        int r = open(path, O_RDONLY);
        if (r < 0) {
            printf("rm: cannot remove '%s': No such file or directory\n", path);
            return 1;
        } else {
            struct Fd* fd;
            fd = num2fd(r);
            struct Filefd* filefd = fd;
            if (filefd->f_file.f_type == FTYPE_DIR) {
                printf("rm: cannot remove '%s': Is a directory\n", path);
                return 1;
            } else {
                close(r);
                remove(path);
            }
        }
    } else if (argc == 3) {
        char path[1024];
        strcpy(path, argv[2]);
        if (strcmp(argv[1], "-r") == 0) {
            int r = open(path, O_RDONLY);
            if (r < 0) {
                printf("rm: cannot remove '%s': No such file or directory\n",
                       path);
                return 1;
            } else {
                close(r);
                remove(path);
            }
        } else if (strcmp(argv[1], "-rf") == 0) {
            int r = open(path, O_RDONLY);
            if (r < 0) {
                // user_panic("rm: cannot remove '%s': No such file or
                // directory",
                //            path);
            } else {
                close(r);
                remove(path);
            }
        }
    }

    return 0;
}