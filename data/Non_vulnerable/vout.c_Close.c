}
static void Close(vlc_object_t *p_this)
{
    vout_display_t *vd = (vout_display_t *)p_this;
    vout_display_sys_t *p_sys = vd->sys;
    if (p_sys->omx_handle) {
        OMX_STATETYPE state;
        OMX_GetState(p_sys->omx_handle, &state);
        if (state == OMX_StateExecuting) {
            OMX_SendCommand(p_sys->omx_handle, OMX_CommandStateSet, OMX_StateIdle, 0);
            while (1) {
                OMX_U32 cmd, state;
                WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, &cmd, &state, 0);
                if (cmd == OMX_CommandStateSet && state == OMX_StateIdle)
                    break;
            }
        }
        OMX_GetState(p_sys->omx_handle, &state);
        if (state == OMX_StateIdle) {
            OMX_SendCommand(p_sys->omx_handle, OMX_CommandStateSet, OMX_StateLoaded, 0);
            for (unsigned int i = 0; i < p_sys->port.i_buffers; i++) {
                OMX_BUFFERHEADERTYPE *p_buffer;
                OMX_FIFO_GET(&p_sys->port.fifo, p_buffer);
                OMX_FreeBuffer(p_sys->omx_handle, p_sys->port.i_port_index, p_buffer);
            }
            WaitForSpecificOmxEvent(&p_sys->event_queue, OMX_EventCmdComplete, 0, 0, 0);
        }
        free(p_sys->port.pp_buffers);
        pf_free_handle(p_sys->omx_handle);
        DeinitOmxEventQueue(&p_sys->event_queue);
        vlc_mutex_destroy(&p_sys->port.fifo.lock);
        vlc_cond_destroy(&p_sys->port.fifo.wait);
    }
    if (p_sys->pool)
        picture_pool_Delete(p_sys->pool);
    free(p_sys);
    DeinitOmxCore();
}
