}
static void Transpose(int *sx, int *sy, int w, int h, int dx, int dy)
{
    VLC_UNUSED( h ); VLC_UNUSED( w );
    *sx = dy;
    *sy = dx;
}
