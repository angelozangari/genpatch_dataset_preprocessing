}
static void SwapUV( picture_t *p_dst, const picture_t *p_src )
{
    picture_t tmp = *p_src;
    tmp.p[1] = p_src->p[2];
    tmp.p[2] = p_src->p[1];
    picture_CopyPixels( p_dst, &tmp );
}
