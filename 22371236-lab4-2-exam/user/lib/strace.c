#include <lib.h>

void strace_barrier(u_int env_id) {
	int straced_bak = straced;
	straced = 0;
	while (envs[ENVX(env_id)].env_status == ENV_RUNNABLE) {
		syscall_yield();
	}
	straced = straced_bak;
}

void strace_send(int sysno) {
	if (!((SYS_putchar <= sysno && sysno <= SYS_set_tlb_mod_entry) ||
	      (SYS_exofork <= sysno && sysno <= SYS_panic)) ||
	    sysno == SYS_set_trapframe) {
		return;
	}
	
	// Lab4-2-exam
	if (straced) {
		int tmp = straced;
		straced = 0;
		u_int parentId = env->env_parent_id;
		ipc_send(parentId, sysno, 0, 0);
		syscall_ipc_try_send(parentId, sysno, 0, 0);
		syscall_set_env_status(0, ENV_NOT_RUNNABLE);
		straced = tmp;
	}

	// Your code here. (1/2)
}

void strace_recv() {

	// Lab4-2-exam
	while (1) {
		u_int sonId; 
		u_int sysno;
		sysno = ipc_recv(&sonId, 0, NULL);
		strace_barrier(sonId);
		recv_sysno(sonId, sysno);
		syscall_set_env_status(sonId, ENV_RUNNABLE);
		if (sysno == SYS_env_destroy) {
			break;
		}
	}

	// Your code here. (2/2)
}
