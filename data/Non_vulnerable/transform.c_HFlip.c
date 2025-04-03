 *****************************************************************************/
static void HFlip(int *sx, int *sy, int w, int h, int dx, int dy)
{
    VLC_UNUSED( h );
    *sx = w - 1 - dx;
    *sy = dy;
}
