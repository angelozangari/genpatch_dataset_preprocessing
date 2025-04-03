 *****************************************************************************/
static int MakeRingBuffer( float **pp_buffer, size_t *pi_buffer,
                           float **pp_write, float f_delay, unsigned i_rate )
{
    const size_t i_size = (2 * (size_t)(1 + f_delay * i_rate / 1000));
    if( unlikely(SIZE_MAX / sizeof(float) < i_size) )
        return VLC_EGENERIC;
    float *p_realloc = realloc( *pp_buffer, i_size * sizeof(float) );
    if( !p_realloc )
        return VLC_ENOMEM;
    memset( p_realloc, 0, i_size * sizeof(float) );
    *pp_write = *pp_buffer = p_realloc;
    *pi_buffer = i_size;
    return VLC_SUCCESS;
}
