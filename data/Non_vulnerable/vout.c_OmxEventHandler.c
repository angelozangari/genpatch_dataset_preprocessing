                                       OMX_BUFFERHEADERTYPE *);
static OMX_ERRORTYPE OmxEventHandler(OMX_HANDLETYPE omx_handle,
    OMX_PTR app_data, OMX_EVENTTYPE event, OMX_U32 data_1,
    OMX_U32 data_2, OMX_PTR event_data)
{
    vout_display_t *vd = (vout_display_t *)app_data;
    vout_display_sys_t *p_sys = vd->sys;
    (void)omx_handle;
    PrintOmxEvent((vlc_object_t *) vd, event, data_1, data_2, event_data);
    PostOmxEvent(&p_sys->event_queue, event, data_1, data_2, event_data);
    return OMX_ErrorNone;
}
