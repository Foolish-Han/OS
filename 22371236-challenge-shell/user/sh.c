#include <args.h>
#include <lib.h>

#define WHITESPACE " \t\r\n"
#define SYMBOLS "<|>&;()"
#define NORMAL 256
#define AND 257
#define OR 258
#define APPEND 259
#define UP "\e[A"
#define DOWN "\e[B"
#define MAXHISTORYNUM 20
#define MAXLINELENGTH 1024
#define MAXJOBNUM 32
#define WHITELINE                                                              \
    "                                                                        " \
    "                                                       "

struct Job {
    /* data */
    char cmd[128];
    int job_id;
    int env_id;
} jobs[MAXJOBNUM];
int jobIds[MAXJOBNUM + 1];

void cmd_jobs() {
    for (int i = 1; i < MAXJOBNUM; i++) {
        if (jobIds[i]) {
            char status[32];
            if (syscall_env_status(jobs[i].env_id) == 0) {
                strcpy(status, "Done");
            } else {
                strcpy(status, "Running");
            }
            printf("[%d] %-10s 0x%08x %s\n", jobs[i].job_id, status,
                   jobs[i].env_id, jobs[i].cmd);
        }
    }
}

void add_job(int env_id, char* cmd) {
    for (int i = 1; i < MAXJOBNUM; i++) {
        if (jobIds[i] == 0) {
            jobIds[i] = 1;
            jobs[i].job_id = i;
            jobs[i].env_id = env_id;
            strcpy(jobs[i].cmd, cmd);
            break;
        }
    }
}

void fg_job(int job_id) {
    if (jobIds[job_id] == 0) {
        printf("fg: job (%d) do not exist\n", job_id);
    } else if (syscall_env_status(jobs[job_id].env_id) == 0) {
        printf("fg: (0x%08x) not running\n", jobs[job_id].env_id);
    } else {
        printf("\n$ %s\n", jobs[job_id].cmd);
        wait(jobs[job_id].env_id);
    }
}

void kill_job(int job_id) {
    if (jobIds[job_id] == 0) {
        printf("fg: job (%d) do not exist\n", job_id);
    } else if (syscall_env_status(jobs[job_id].env_id) == 0) {
        printf("fg: (0x%08x) not running\n", jobs[job_id].env_id);
        jobIds[job_id] = 0;
    } else {
        syscall_env_destroy(jobs[job_id].env_id);
    }
}

void runcmd(char* s, int* append);
/* Overview:
 *   Parse the next token from the string at s.
 *
 * Post-Condition:
 *   Set '*p1' to the beginning of the token and '*p2' to just past the token.
 *   Return:
 *     - 0 if the end of string is reached.
 *     - '<' for < (stdin redirection).
 *     - '>' for > (stdout redirection).
 *     - '|' for | (pipe).
 *     - 'w' for a word (command, argument, or file name).
 *
 *   The buffer is modified to turn the spaces after words into zero bytes
 * ('\0'), so that the returned token is a null-terminated string.
 */
int _gettoken(char* s, char** p1, char** p2) {
    *p1 = 0;
    *p2 = 0;
    if (s == 0) {
        return 0;
    }

    while (strchr(WHITESPACE, *s)) {
        *s++ = 0;
    }
    if (*s == 0) {
        return 0;
    }

    if (strchr(SYMBOLS, *s)) {
        int t = *s;
        *p1 = s;
        *s++ = 0;
        *p2 = s;
        return t;
    }

    //! challenge-shell start
    if (*s == '`') {
        s++;
        *p1 = s;
        while (*s && *s != '`') {
            s++;
        }
        *s = '\0';
        *p2 = s + 1;
        return '`';
    }
    //! challenge-shell end

    //! challenge-shell start
    if (*s == '#') {
        return '#';
    }
    //! challenge-shell end

    //! challenge-shell start
    if (*s == '\"') {
        s++;
        *p1 = s;
        while (*s && *s != '\"') {
            s++;
        }
        *s = '\0';
        *p2 = s + 1;
        return 'w';
    }
    //! challenge-shell end

    *p1 = s;
    while (*s && !strchr(WHITESPACE SYMBOLS, *s)) {
        s++;
    }
    *p2 = s;
    return 'w';
}

int gettoken(char* s, char** p1) {
    static int c, nc;
    static char *np1, *np2;

    if (s) {
        nc = _gettoken(s, &np1, &np2);
        return 0;
    }
    c = nc;
    *p1 = np1;
    nc = _gettoken(np2, &np1, &np2);

    //! challenge-shell start
    /*
     * find token "&&", "||" and ">>"
     */
    if (c == '&' && nc == '&') {
        *p1 = np1;
        nc = _gettoken(np2, &np1, &np2);
        return AND;
    } else if (c == '|' && nc == '|') {
        *p1 = np1;
        nc = _gettoken(np2, &np1, &np2);
        return OR;
    } else if (c == '>' && nc == '>') {
        *p1 = np1;
        nc = _gettoken(np2, &np1, &np2);
        return APPEND;
    }
    //! challenge-shell start

    return c;
}

#define MAXARGS 128
//! challenge-shell start
char temp[10][4096];
int num;
//! challenge-shell end
int parsecmd(char** argv,
             int* rightpipe,
             int* waitType,
             int* nextCmd,
             int* append) {
    int argc = 0;
    while (1) {
        char* t;
        int fd, r;
        int c = gettoken(0, &t);
        switch (c) {
            case 0:
                // debugf("%s\n", argv[0]);

                return argc;
            case 'w':
                if (argc >= MAXARGS) {
                    debugf("too many arguments\n");
                    exit();
                }
                argv[argc++] = t;
                // debugf("%s\n", t);
                break;
            case '<':
                if (gettoken(0, &t) != 'w') {
                    debugf("syntax error: < not followed by word\n");
                    exit();
                }
                // Open 't' for reading, dup it onto fd 0, and then close the
                // original fd. If the 'open' function encounters an error,
                // utilize 'debugf' to print relevant messages,
                // and subsequently terminate the process using 'exit'.
                /* Exercise 6.5: Your code here. (1/3) */
                fd = open(t, O_RDONLY);
                if (fd < 0) {
                    debugf("open for reading %s error\n", t);
                    exit();
                }
                dup(fd, 0);
                close(fd);
                // user_panic("< redirection not implemented");

                break;
            case '>':
                if (gettoken(0, &t) != 'w') {
                    debugf("syntax error: > not followed by word\n");
                    exit();
                }
                // Open 't' for writing, create it if not exist and trunc it if
                // exist, dup it onto fd 1, and then close the original fd. If
                // the 'open' function encounters an error, utilize 'debugf' to
                // print relevant messages, and subsequently terminate the
                // process using 'exit'.
                /* Exercise 6.5: Your code here. (2/3) */
                fd = open(t, O_WRONLY | O_CREAT | O_TRUNC);
                if (fd < 0) {
                    debugf("open for writing %s error\n", t);
                    exit();
                }
                dup(fd, 1);
                close(fd);
                // user_panic("> redirection not implemented");

                break;
            case '|':;
                /*
                 * First, allocate a pipe.
                 * Then fork, set '*rightpipe' to the returned child envid or
                 * zero. The child runs the right side of the pipe:
                 * - dup the read end of the pipe onto 0
                 * - close the read end of the pipe
                 * - close the write end of the pipe
                 * - and 'return parsecmd(argv, rightpipe)' again, to parse the
                 * rest of the command line. The parent runs the left side of
                 * the pipe:
                 * - dup the write end of the pipe onto 1
                 * - close the write end of the pipe
                 * - close the read end of the pipe
                 * - and 'return argc', to execute the left of the pipeline.
                 */
                int p[2];
                /* Exercise 6.5: Your code here. (3/3) */
                if ((r = pipe(p)) < 0) {
                    return r;
                }
                r = fork();
                *rightpipe = r;
                if (r == 0) {
                    *waitType = NORMAL;
                    *nextCmd = 0;
                    dup(p[0], 0);
                    close(p[0]);
                    close(p[1]);
                    return parsecmd(argv, rightpipe, waitType, nextCmd, append);
                } else {
                    // debugf("##pipe fork: ENV_%x create ENV_%x\n",
                    // env->env_id,r);
                    dup(p[1], 1);
                    close(p[1]);
                    close(p[0]);
                    return argc;
                }
                // user_panic("| not implemented");

                break;
            case AND:
                r = fork();
                if (r == 0) {
                    r = opencons();
                    dup(r, 0);
                    dup(0, 1);
                    *waitType = AND;
                    *nextCmd = 0;
                    return parsecmd(argv, rightpipe, waitType, nextCmd, append);
                }
                *nextCmd = r;
                return argc;
                break;
            case OR:
                r = fork();
                if (r == 0) {
                    r = opencons();
                    dup(r, 0);
                    dup(0, 1);
                    *waitType = OR;
                    *nextCmd = 0;
                    return parsecmd(argv, rightpipe, waitType, nextCmd, append);
                }
                *nextCmd = r;
                return argc;
                break;
            case '`':;
                r = fork();
                if (r == 0) {
                    *append = 1;
                    strcpy(temp[num], t);
                    fd = open(".TEMP", O_WRONLY | O_CREAT | O_TRUNC);
                    if (fd < 0) {
                        debugf("open for writing %s error\n", ".TEMP");
                        exit();
                    }
                    dup(fd, 1);
                    close(fd);
                    runcmd(temp[num], append);
                    exit();
                } else {
                    wait(r);
                    r = open(".TEMP", O_RDONLY);
                    read(r, temp[num], 4096);
                    struct Filefd* filefd = num2fd(r);
                    int l = filefd->f_file.f_size;
                    argv[argc++] = temp[num];
                    for (int i = 0; i < l; i++) {
                        if (strchr(WHITESPACE, temp[num][i])) {
                            while (i < l && strchr(WHITESPACE, temp[num][i])) {
                                temp[num][i] = 0;
                                i++;
                            }
                            if (i < l) {
                                argv[argc++] = temp[num] + i;
                            }
                            // debugf("test:argv[%d]= %s\n", argc - 1,
                            //        argv[argc - 1]);
                        }
                    }
                    close(r);
                    remove(".TEMP");
                    num++;
                }
                break;
            case '#':
                return argc;
                break;
            case ';':
                r = fork();
                if (r == 0) {
                    // debugf("%s\n", argv[0]);
                    return argc;
                } else {
                    wait(r);
                    if (*append == 1) {
                        fd = open(".TEMP", O_WRONLY | O_CREAT);
                        if (fd < 0) {
                            debugf("open for writing %s error\n", ".TEMP");
                            exit();
                        }
                        struct Filefd* filefd = num2fd(fd);
                        int size = filefd->f_file.f_size;
                        dup(fd, 1);
                        struct Fd* fd1 = num2fd(1);
                        fd1->fd_offset = size;
                        close(fd);
                    } else {
                        r = opencons();
                        dup(r, 0);
                        // stdout
                        dup(0, 1);
                    }
                    argc = 0;
                }
                break;
            case APPEND:
                if (gettoken(0, &t) != 'w') {
                    debugf("syntax error: > not followed by word\n");
                    exit();
                }
                fd = open(t, O_WRONLY | O_CREAT);
                if (fd < 0) {
                    debugf("open for writing %s error\n", t);
                    exit();
                }
                struct Filefd* filefd = num2fd(fd);
                int size = filefd->f_file.f_size;
                dup(fd, 1);
                struct Fd* fd1 = num2fd(1);
                fd1->fd_offset = size;
                close(fd);
                break;
        }
    }

    return argc;
}

void runcmd(char* s, int* append) {
    gettoken(s, 0);

    char* argv[MAXARGS];
    int rightpipe = 0;
    int waitType = NORMAL;
    int nextCmd = 0;
    int ret;
    int tmp;

    int argc = parsecmd(argv, &rightpipe, &waitType, &nextCmd, append);
    if (argc == 0) {
        return;
    }
    argv[argc] = 0;

    //! challenge-shell start
    if (waitType == AND || waitType == OR) {
        tmp = ipc_recv(0, 0, 0);
    }

    if (waitType == AND && tmp == 1) {
        ret = 1;
    } else if (waitType == OR && tmp == 0) {
        ret = 0;
    } else {
        int child;
        if (strcmp(argv[0], "history") == 0) {
            child = fork();
            if (child > 0) {
                close_all();
                wait(child);
                ret = 0;
            } else {
                historyOut();
                exit();
                //?
            }
        } else {
            child = spawn(argv[0], argv);
            close_all();
            if (child >= 0) {
                wait(child);
                ret = envs[ENVX(child)].env_ret;
            } else {
                debugf("spawn %s: %d\n", argv[0], child);
                ret = 1;
            }
        }
    }
    //! challenge-shell end

    if (rightpipe) {
        wait(rightpipe);
    }

    //! challenge-shell start
    if (nextCmd) {
        ipc_send(nextCmd, ret, 0, 0);
        wait(nextCmd);
    }
    //! challenge-shell end
    exit();
}

int history;
int hisStart = 1;
int hisEnd = 1;
int hisNow = 1;
void historyOut() {
    struct Fd* fd = num2fd(history);
    struct Filefd* filefd = fd;
    int n = filefd->f_file.f_size / MAXLINELENGTH;
    for (int i = hisStart; i < hisEnd; i++) {
        char buf[MAXLINELENGTH];
        fd->fd_offset = ((i - 1) % MAXHISTORYNUM) * MAXLINELENGTH;
        read(history, buf, MAXLINELENGTH);
        debugf("%s\n", buf);
    }
}
int readhis(char* buf, int lineNum) {
    struct Fd* fd = num2fd(history);
    fd->fd_offset = ((lineNum - 1) % MAXHISTORYNUM) * MAXLINELENGTH;
    return read(history, buf, MAXLINELENGTH);
}

int writehis(char* buf, int lineNum) {
    struct Fd* fd = num2fd(history);
    fd->fd_offset = ((lineNum - 1) % MAXHISTORYNUM) * MAXLINELENGTH;
    return write(history, buf, MAXLINELENGTH);
}

void readline(char* buf, u_int n) {
    int r;
    for (int i = 0; i < n; i++) {
        if ((r = read(0, buf + i, 1)) != 1) {
            if (r < 0) {
                debugf("read error: %d\n", r);
            }
            exit();
        }

        if (buf[i] == '\e') {
            i++;
            read(0, buf + i, 1);
            if (buf[i] == '[') {
                i++;
                read(0, buf + i, 1);
                if (buf[i] == 'A') {
                    i -= 2;
                    buf[i] = '\0';
                    if (hisNow == hisEnd) {
                        writehis(buf, hisEnd);
                    }
                    debugf("%s", DOWN);
                    debugf("\r%s", WHITELINE);
                    hisNow--;
                    if (hisNow < hisStart) {
                        hisNow = hisStart;
                    }
                    readhis(buf, hisNow);
                    debugf("\r$ %s", buf);
                    i = strlen(buf) - 1;
                    continue;

                } else if (buf[i] == 'B') {
                    i -= 2;
                    buf[i] = '\0';
                    if (hisNow == hisEnd) {
                        writehis(buf, hisEnd);
                    }
                    debugf("\r%s", WHITELINE);
                    hisNow++;
                    if (hisNow > hisEnd) {
                        hisNow = hisEnd;
                    }
                    readhis(buf, hisNow);
                    debugf("\r$ %s", buf);
                    i = strlen(buf) - 1;
                    continue;
                }
            }
        }

        if (buf[i] == '\b' || buf[i] == 0x7f) {
            if (i == 0) {
                i -= 1;
                printf(" \b");
            } else {
                i -= 2;
                printf("\b \b");
            }
        }
        if (buf[i] == '\r' || buf[i] == '\n') {
            buf[i] = 0;
            writehis(buf, hisEnd);
            if (hisEnd - hisStart >= MAXHISTORYNUM) {
                hisStart++;
            }
            hisEnd++;
            hisNow = hisEnd;
            return;
        }
    }
    debugf("line too long\n");
    while ((r = read(0, buf, 1)) == 1 && buf[0] != '\r' && buf[0] != '\n') {
        ;
    }
    buf[0] = 0;
}

char buf[1024];

void usage(void) {
    printf("usage: sh [-ix] [script-file]\n");
    exit();
}

int main(int argc, char** argv) {
    int r;
    int interactive = iscons(0);
    int echocmds = 0;
    printf("\n:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    printf("::                                                         ::\n");
    printf("::                     MOS Shell 2024                      ::\n");
    printf("::                                                         ::\n");
    printf(":::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::\n");
    ARGBEGIN {
        case 'i':
            interactive = 1;
            break;
        case 'x':
            echocmds = 1;
            break;
        default:
            usage();
    }
    ARGEND

    if (argc > 1) {
        usage();
    }
    if (argc == 1) {
        close(0);
        if ((r = open(argv[0], O_RDONLY)) < 0) {
            user_panic("open %s: %d", argv[0], r);
        }
        user_assert(r == 0);
    }

    //! challenge-shell start
    history = open(".mosh_history", O_CREAT | O_RDWR | O_TRUNC);
    //! challenge-shell end

    for (;;) {
        if (interactive) {
            printf("\n$ ");
        }
        readline(buf, sizeof buf);

        if (buf[0] == '#') {
            continue;
        }
        if (echocmds) {
            printf("# %s\n", buf);
        }

        // // debugf("cmd:%s\n", buf);
        if (strncmp(buf, "fg", 2) == 0) {
            // debugf("fg\n");
            int id = 0;
            for (int j = 3; j < strlen(buf); j++) {
                if (isdigit(buf[j])) {
                    id = id * 10 + buf[j] - '0';
                }
            }
            fg_job(id);
            continue;
        } else if (strncmp(buf, "kill", 4) == 0) {
            // debugf("kill\n");
            int id = 0;
            for (int j = 5; j < strlen(buf); j++) {
                if (isdigit(buf[j])) {
                    id = id * 10 + buf[j] - '0';
                }
            }
            kill_job(id);
            continue;
        } else if (strcmp(buf, "jobs") == 0) {
            // debugf("jobs\n");
            cmd_jobs();
            continue;
        }

        int l = strlen(buf);
        int flag = 0;
        if (buf[l - 1] == '&') {
            flag = 1;
            for (int i = 0; i < l; i++) {
                if (buf[i] == '#') {
                    flag = 0;
                }
            }
        }

        if ((r = fork()) < 0) {
            user_panic("fork: %d", r);
        }
        if (r == 0) {
            if (flag) {
                buf[l - 1] = 0;
            }
            int append = 0;
            // debugf("run job:%s\n", buf);
            runcmd(buf, &append);
            debugf("runcmd has exited! can't reach here!\n");
            exit();
        } else {
            if (flag) {
                // debugf("add job\n");
                add_job(r, buf);
            } else {
                wait(r);
            }
            // debugf("##shell fork: ENV_%x create ENV_%x\n", env->env_id, r);
        }
    }
    return 0;
}
