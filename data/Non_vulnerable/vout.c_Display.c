}
static void Display(vout_display_t *vd, picture_t *picture, subpicture_t *subpicture)
{
    VLC_UNUSED(vd);
    VLC_UNUSED(subpicture);
    picture_sys_t *picsys = picture->p_sys;
    vout_display_sys_t *p_sys = picsys->sys;
    OMX_BUFFERHEADERTYPE *p_buffer = picsys->buf;
    p_buffer->nFilledLen = 3*p_sys->port.definition.format.video.nStride*p_sys->port.definition.format.video.nSliceHeight/2;
    p_buffer->nFlags = OMX_BUFFERFLAG_ENDOFFRAME;
    /* refcount lowers to 0, and pool_cfg.unlock is called */
    picture_Release(picture);
}
