}
static int LockSurface(picture_t *picture)
{
    picture_sys_t *picsys = picture->p_sys;
    vout_display_sys_t *p_sys = picsys->sys;
    OMX_BUFFERHEADERTYPE *p_buffer;
    OMX_FIFO_GET(&p_sys->port.fifo, p_buffer);
    for (int i = 0; i < 3; i++) {
        picture->p[i].p_pixels = p_buffer->pBuffer;
        picture->p[i].i_pitch = p_sys->port.definition.format.video.nStride;
        picture->p[i].i_lines = p_sys->port.definition.format.video.nSliceHeight;
        if (i > 0) {
            picture->p[i].p_pixels = picture->p[i-1].p_pixels + picture->p[i-1].i_pitch*picture->p[i-1].i_lines;
            picture->p[i].i_pitch /= 2;
            picture->p[i].i_lines /= 2;
        }
    }
    p_buffer->nOffset = 0;
    p_buffer->nFlags = 0;
    p_buffer->nTimeStamp = ToOmxTicks(0);
    p_buffer->nFilledLen = 0;
    picsys->buf = p_buffer;
    return VLC_SUCCESS;
}
