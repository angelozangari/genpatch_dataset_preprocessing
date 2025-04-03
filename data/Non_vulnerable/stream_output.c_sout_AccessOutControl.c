 */
int sout_AccessOutControl (sout_access_out_t *access, int query, ...)
{
    va_list ap;
    int ret;
    va_start (ap, query);
    if (access->pf_control)
        ret = access->pf_control (access, query, ap);
    else
        ret = VLC_EGENERIC;
    va_end (ap);
    return ret;
}
