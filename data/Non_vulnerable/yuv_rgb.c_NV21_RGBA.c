}
static void NV21_RGBA (filter_t *filter, picture_t *src, picture_t *dst)
{
    struct yuv_pack out = { dst->p->p_pixels, dst->p->i_pitch };
    struct yuv_planes in = { src->Y_PIXELS, src->U_PIXELS, src->V_PIXELS, src->Y_PITCH };
    nv21_rgb_neon (&out, &in, filter->fmt_in.video.i_visible_width, filter->fmt_in.video.i_visible_height);
}
