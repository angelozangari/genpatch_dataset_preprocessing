}
static void VFlip(int *sx, int *sy, int w, int h, int dx, int dy)
{
    VLC_UNUSED( w );
    *sx = dx;
    *sy = h - 1 - dy;
}
