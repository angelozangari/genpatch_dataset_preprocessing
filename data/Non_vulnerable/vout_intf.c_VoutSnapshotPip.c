 */
static int VoutSnapshotPip( vout_thread_t *p_vout, picture_t *p_pic )
{
    subpicture_t *p_subpic = subpicture_NewFromPicture( VLC_OBJECT(p_vout),
                                                        p_pic, VLC_CODEC_YUVA );
    if( !p_subpic )
        return VLC_EGENERIC;
    /* FIXME SPU_DEFAULT_CHANNEL is not good (used by the text) but
     * hardcoded 0 doesn't seem right */
    p_subpic->i_channel = 0;
    p_subpic->i_start = mdate();
    p_subpic->i_stop  = p_subpic->i_start + 4000000;
    p_subpic->b_ephemer = true;
    p_subpic->b_fade = true;
    /* Reduce the picture to 1/4^2 of the screen */
    p_subpic->i_original_picture_width  *= 4;
    p_subpic->i_original_picture_height *= 4;
    vout_PutSubpicture( p_vout, p_subpic );
    return VLC_SUCCESS;
}
