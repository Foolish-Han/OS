#include <env.h>
#include <lib.h>
#include <mmu.h>

void exit(void) {
    // After fs is ready (lab5), all our open files should be closed before
    // dying.
#if !defined(LAB) || LAB >= 5
    close_all();
#endif
    // debugf("ret==%d\n", env->env_ret);
    syscall_env_destroy(0);
    user_panic("unreachable code");
}

const volatile struct Env* env;
extern int main(int, char**);

void libmain(int argc, char** argv) {
    env = &envs[ENVX(syscall_getenvid())];
    //! challenge-shell start
    int r = main(argc, argv);
    syscall_set_ret(r);
    //! challenge-shell end
    exit();
}
