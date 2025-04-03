 */
static picture_pool_t *Pool (vout_display_t *vd, unsigned requested_count)
{
    vout_display_sys_t *p_sys = vd->sys;
    if (!p_sys->pool)
        PoolAlloc (vd, requested_count);
    return p_sys->pool;
}
