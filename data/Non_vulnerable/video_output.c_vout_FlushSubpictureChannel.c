}
void vout_FlushSubpictureChannel( vout_thread_t *vout, int channel )
{
    vout_control_PushInteger(&vout->p->control, VOUT_CONTROL_FLUSH_SUBPICTURE,
                             channel);
}
