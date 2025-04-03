}
int vout_InitWrapper(vout_thread_t *vout)
{
    vout_thread_sys_t *sys = vout->p;
    vout_display_t *vd = sys->display.vd;
    video_format_t source = vd->source;
    sys->display.use_dr = !vout_IsDisplayFiltered(vd);
    const bool allow_dr = !vd->info.has_pictures_invalid && !vd->info.is_slow && sys->display.use_dr;
    const unsigned private_picture  = 4; /* XXX 3 for filter, 1 for SPU */
    const unsigned decoder_picture  = 1 + sys->dpb_size;
    const unsigned kept_picture     = 1; /* last displayed picture */
    const unsigned reserved_picture = DISPLAY_PICTURE_COUNT +
                                      private_picture +
                                      kept_picture;
    picture_pool_t *display_pool =
        vout_display_Pool(vd, allow_dr ? __MAX(VOUT_MAX_PICTURES,
                                               reserved_picture + decoder_picture) : 3);
    if (allow_dr &&
        picture_pool_GetSize(display_pool) >= reserved_picture + decoder_picture) {
        sys->dpb_size     = picture_pool_GetSize(display_pool) - reserved_picture;
        sys->decoder_pool = display_pool;
        sys->display_pool = display_pool;
    } else if (!sys->decoder_pool) {
        sys->decoder_pool =
            picture_pool_NewFromFormat(&source,
                                       __MAX(VOUT_MAX_PICTURES,
                                             reserved_picture + decoder_picture - DISPLAY_PICTURE_COUNT));
        if (!sys->decoder_pool)
            return VLC_EGENERIC;
        if (allow_dr) {
            msg_Warn(vout, "Not enough direct buffers, using system memory");
            sys->dpb_size = 0;
        } else {
            sys->dpb_size = picture_pool_GetSize(sys->decoder_pool) - reserved_picture;
        }
        NoDrInit(vout);
    }
    sys->private_pool = picture_pool_Reserve(sys->decoder_pool, private_picture);
    sys->display.filtered = NULL;
    return VLC_SUCCESS;
}
