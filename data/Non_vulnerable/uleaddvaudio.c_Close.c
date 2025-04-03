}
static void Close(vlc_object_t *object)
{
    decoder_t *dec = (decoder_t *)object;
    free(dec->p_sys);
}
