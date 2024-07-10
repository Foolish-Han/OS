#include <lib.h>
int main(int argc, char** argv) {
    if (argc == 2) {
        char path[1024];
        strcpy(path, argv[1]);
        char* dir = path;
        char* name = path;
        int l = strlen(path);
        for (int i = l - 1; i >= 0; i--) {
            if (path[i] == '/') {
                path[i] = '\0';
                name = path + i + 1;
                break;
            }
        }
        if (name != dir && open(dir, O_RDONLY) < 0) {
            printf("touch: cannot touch '%s': No such file or directory\n",
                   argv[1]);
            return 1;
        } else {
            if (open(argv[1], O_CREAT | O_RDONLY) < 0) {
                printf("touch: fail to touch '%s'\n", argv[1]);
                return 1;
            }
        }
    }

    return 0;
}