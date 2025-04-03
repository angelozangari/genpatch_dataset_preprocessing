#else /* stdarg */
void Z_debug (va_alist) va_dcl
{
    va_list pvar;
    char *format;
    if (!__Z_debug_print)
      return;
    va_start (pvar);
    format = va_arg (pvar, char *);
    (*__Z_debug_print) (format, pvar, __Z_debug_print_closure);
    va_end (pvar);
}
