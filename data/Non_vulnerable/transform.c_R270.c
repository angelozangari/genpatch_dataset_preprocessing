}
static void R270(int *sx, int *sy, int w, int h, int dx, int dy)
{
    VLC_UNUSED( w );
    *sx = h - 1 - dy;
    *sy = dx;
}
