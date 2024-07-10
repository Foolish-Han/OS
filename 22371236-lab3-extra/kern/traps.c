#include <env.h>
#include <pmap.h>
#include <printk.h>
#include <trap.h>

extern void handle_int(void);
extern void handle_tlb(void);
extern void handle_sys(void);
extern void handle_mod(void);
extern void handle_reserved(void);

extern void handle_ri(void);


void do_ri(struct Trapframe *tf) {
	unsigned long ins = *(unsigned long*)tf->cp0_epc;
	int index_rs = (ins >> 21) & 0x001f;
	int index_rt = (ins >> 16) & 0x001f;
	int index_rd = (ins >> 11) & 0x0000f800;


	// 你需要在此处实现问题描述的处理要求
	if ((tf->cp0_epc >>25 == 0) && (tf->cp0_epc & 0x003f == 0)) {
		unsigned long rd = 0;
		int i;
		for (i = 0; i < 32; i += 8) {
 			  u_int rs_i = tf->regs[index_rs] & (0xff << i);
			  u_int rt_i = tf->regs[index_rt] & (0xff << i);
			  if (rs_i < rt_i) {
			      rd = rd | rt_i;
			   } else {
			      rd = rd | rs_i;
			   }

		}
		tf->regs[index_rd] = rd;
	} else if ((tf->cp0_epc >>25 == 0) && (tf->cp0_epc & 0x003e == 0)) {
		unsigned long tmp = *(unsigned long*)tf->regs[index_rs];
		if (tmp == tf->regs[index_rt]) {
			*(unsigned long*)tf->regs[index_rs] = tf->regs[index_rd];
		}
		tf->regs[index_rd] = tmp;
	} else {

	}
	tf->cp0_epc+=4;
}

void (*exception_handlers[32])(void) = {
    [0 ... 31] = handle_reserved,
    [0] = handle_int,
    [2 ... 3] = handle_tlb,
    [10] = handle_ri,
#if !defined(LAB) || LAB >= 4
    [1] = handle_mod,
    [8] = handle_sys,
#endif
};

/* Overview:
 *   The fallback handler when an unknown exception code is encountered.
 *   'genex.S' wraps this function in 'handle_reserved'.
 */
void do_reserved(struct Trapframe *tf) {
	print_tf(tf);
	panic("Unknown ExcCode %2d", (tf->cp0_cause >> 2) & 0x1f);
}
