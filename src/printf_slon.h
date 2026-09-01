// #define _DEFAULT_SOURCE
#ifndef PRINTF_SLON_H
#define PRINTF_SLON_H

/*              default printf with delay='ms'               */
void printf_slon(unsigned delay_ms, const char *fmt, ...);
void print_version2(unsigned);

void slon_dir_analytics(const char *path, unsigned ms);
#endif // PRINTF_SLON_H
