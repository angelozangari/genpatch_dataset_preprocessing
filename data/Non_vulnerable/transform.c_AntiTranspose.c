}
static void AntiTranspose(int *sx, int *sy, int w, int h, int dx, int dy)
{
    *sx = h - 1 - dy;
    *sy = w - 1 - dx;
}
