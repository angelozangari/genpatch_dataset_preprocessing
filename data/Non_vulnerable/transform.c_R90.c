}
static void R90(int *sx, int *sy, int w, int h, int dx, int dy)
{
    VLC_UNUSED( h );
    *sx = dy;
    *sy = w - 1 - dx;
}
