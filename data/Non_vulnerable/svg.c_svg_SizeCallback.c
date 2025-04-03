}
static void svg_SizeCallback( int *width, int *height, gpointer data )
{
    filter_t *p_filter = data;
    *width = p_filter->p_sys->i_width;
    *height = p_filter->p_sys->i_height;
    return;
}
