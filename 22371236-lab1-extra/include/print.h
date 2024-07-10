#ifndef _print_h_
#define _print_h_

#include <stdarg.h>
#include <types.h>

typedef void (*fmt_callback_t)(void *data, const char *buf, size_t len);

/* Lab 1 Key Code "vprintfmt-overview" */
/*
 * 'vprintfmt' is a formatting function that allows different backends (i.e., output sinks)
 * to be used for printing. It takes four arguments:
 *
 * - 'fmt_callback_t out': a function pointer to the output sink that receives the
 *   formatted output generated by 'vprintfmt'.
 * - 'void *data': a context pointer passed to the 'out' callback function. It can be used
 *   to store additional output sink-specific data.
 * - 'const char *fmt': the format string, similar to the format string in 'printf'.
 * - 'va_list ap': a variadic argument list that provides the arguments to be formatted.
 *
 * The format callback function 'out' receives the following arguments:
 *
 * - 'void *data': the same 'data' pointer passed to 'vprintfmt'.
 * - 'const char *buf': a pointer to a buffer containing the formatted output.
 * - 'size_t len': the number of bytes in the buffer.
 *
 * Note that the buffer may not be null-terminated and may contain embedded null bytes,
 * so the output sink should treat 'len' as the actual length of the buffer to print.
 */
void vprintfmt(fmt_callback_t out, void *data, const char *fmt, va_list ap);
/* End of Key Code "vprintfmt-overview" */

// include/print.h
typedef void (*scan_callback_t)(void *data, char *buf, size_t len);
int vscanfmt(scan_callback_t in, void *data, const char *fmt, va_list ap);

#endif
