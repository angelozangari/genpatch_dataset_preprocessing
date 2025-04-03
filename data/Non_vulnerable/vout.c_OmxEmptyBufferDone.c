}
static OMX_ERRORTYPE OmxEmptyBufferDone(OMX_HANDLETYPE omx_handle,
    OMX_PTR app_data, OMX_BUFFERHEADERTYPE *omx_header)
{
    vout_display_t *vd = (vout_display_t *)app_data;
    vout_display_sys_t *p_sys = vd->sys;
    (void)omx_handle;
#ifndef NDEBUG
    msg_Dbg(vd, "OmxEmptyBufferDone %p, %p", omx_header, omx_header->pBuffer);
#endif
    OMX_FIFO_PUT(&p_sys->port.fifo, omx_header);
    return OMX_ErrorNone;
}
