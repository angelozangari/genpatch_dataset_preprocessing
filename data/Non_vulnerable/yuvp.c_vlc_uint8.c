/* FIXME copied from blend.c */
static inline uint8_t vlc_uint8( int v )
{
    if( v > 255 )
        return 255;
    else if( v < 0 )
        return 0;
    return v;
}
