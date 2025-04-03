 *****************************************************************************/
void vout_EndWrapper(vout_thread_t *vout)
{
    vout_thread_sys_t *sys = vout->p;
    assert(!sys->display.filtered);
    if (sys->private_pool)
        picture_pool_Delete(sys->private_pool);
    if (sys->decoder_pool != sys->display_pool)
        picture_pool_Delete(sys->decoder_pool);
}
