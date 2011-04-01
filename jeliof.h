#include "jelio.h"
#include <stdio.h>
#include <stdarg.h>

/* Extended output */

int vafprintv(FILE *f, char *fmt, va_list ap);

/* *printv returns number of characters written or EOF on error */
int printv(char *fmt, ...);
int fprintv(FILE *f, char *fmt, ...);

/* Extended input */
int fscanv(FILE *f, char *fmt, ...);
