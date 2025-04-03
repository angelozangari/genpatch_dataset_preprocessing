#define FONT_FACE_UNDERLINE 0x4
static int ConvertFlags( int i_atomflags )
{
    int i_vlcstyles_flags = 0;
    if ( i_atomflags & FONT_FACE_BOLD )
        i_vlcstyles_flags |= STYLE_BOLD;
    if ( i_atomflags & FONT_FACE_ITALIC )
        i_vlcstyles_flags |= STYLE_ITALIC;
    if ( i_atomflags & FONT_FACE_UNDERLINE )
        i_vlcstyles_flags |= STYLE_UNDERLINE;
    return i_vlcstyles_flags;
}
