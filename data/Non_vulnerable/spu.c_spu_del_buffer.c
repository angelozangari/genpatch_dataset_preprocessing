}
static void spu_del_buffer( decoder_t *p_dec, subpicture_t *p_subpic )
{
    VLC_UNUSED( p_dec );
    subpicture_Delete( p_subpic );
}
