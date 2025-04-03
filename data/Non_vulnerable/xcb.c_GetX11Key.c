};
static xcb_keysym_t GetX11Key( unsigned i_vlc )
{
    /* X11 and VLC use ASCII for printable ASCII characters */
    if( i_vlc >= 32 && i_vlc <= 127 )
        return i_vlc;
    for( int i = 0; x11keys_to_vlckeys[i].i_vlc != 0; i++ )
    {
        if( x11keys_to_vlckeys[i].i_vlc == i_vlc )
            return x11keys_to_vlckeys[i].i_x11;
    }
    return XK_VoidSymbol;
}
