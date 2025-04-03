/* */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    VLC_UNUSED(count);
    return vd->sys->pool;
}
