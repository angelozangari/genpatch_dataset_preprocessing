#undef sout_EncoderCreate
encoder_t *sout_EncoderCreate( vlc_object_t *p_this )
{
    return vlc_custom_create( p_this, sizeof( encoder_t ), "encoder" );
}
