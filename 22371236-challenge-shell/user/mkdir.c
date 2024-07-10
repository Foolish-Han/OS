#include <lib.h>
int main(int argc, char** argv) {
    char path[1024];
    if (argc == 2) {
        strcpy(path, argv[1]);
        if (open(path, O_RDONLY) >= 0) {
            printf("mkdir: cannot create directory '%s': File exists\n",
                   argv[1]);
            return 1;
        } else {
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
                printf(
                    "mkdir: cannot create directory '%s': No such file or "
                    "directory\n",
                    argv[1]);
                return 1;
            } else {
                if (open(argv[1], O_CREAT | O_MKDIR | O_RDONLY) < 0) {
                    printf("mkdir: fail to mkdir '%s'\n", argv[1]);
                    return 1;
                }
            }
        }
    } else if (argc == 3) {
        if (strcmp("-p", argv[1]) == 0) {
            // printf("-p\n");
            strcpy(path, argv[2]);
        }
        if (open(path, O_RDONLY) < 0) {
            char* dir = path;
            char* name = path;
            int l = strlen(path);
            for (int i = l - 1; i >= 0; i--) {
                if (path[i] == '/') {
                    path[i] = '\0';
                }
            }
            while (l) {
                // printf("%s\n", dir);
                if (open(dir, O_CREAT | O_MKDIR | O_RDONLY) < 0) {
                    printf("mkdir: fail to mkdir '%s'\n", dir);
                    return 1;
                }
                if (strlen(dir) >= l) {
                    break;
                }
                dir[strlen(dir)] = '/';
            }
        }
    }

    return 0;
}