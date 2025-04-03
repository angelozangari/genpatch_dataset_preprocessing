/* */
static picture_pool_t *Pool(vout_display_t *vd, unsigned count)
{
    vout_display_sys_t *sys = vd->sys;
    if (sys->pool)
        return sys->pool;
    if (count > sys->count)
        count = sys->count;
    picture_t *pictures[count];
    for (unsigned i = 0; i < count; i++) {
        picture_sys_t *picsys = malloc(sizeof (*picsys));
        if (unlikely(picsys == NULL))
        {
            count = i;
            break;
        }
        picsys->sys = sys;
        picsys->id = NULL;
        picture_resource_t rsc = { .p_sys = picsys };
        for (unsigned i = 0; i < PICTURE_PLANE_MAX; i++) {
            /* vmem-lock is responsible for the allocation */
            rsc.p[i].p_pixels = NULL;
            rsc.p[i].i_lines  = sys->lines[i];
            rsc.p[i].i_pitch  = sys->pitches[i];
        }
        pictures[i] = picture_NewFromResource(&vd->fmt, &rsc);
        if (!pictures[i]) {
            free(rsc.p_sys);
            count = i;
            break;
        }
    }
    /* */
    picture_pool_configuration_t pool;
    memset(&pool, 0, sizeof(pool));
    pool.picture_count = count;
    pool.picture       = pictures;
    pool.lock          = Lock;
    pool.unlock        = Unlock;
    sys->pool = picture_pool_NewExtended(&pool);
    if (!sys->pool) {
        for (unsigned i = 0; i < count; i++)
            picture_Release(pictures[i]);
    }
    return sys->pool;
}
