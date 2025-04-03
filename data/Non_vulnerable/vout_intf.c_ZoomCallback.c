}
static int ZoomCallback( vlc_object_t *obj, char const *name,
                         vlc_value_t prev, vlc_value_t cur, void *data )
{
    (void) name; (void) prev; (void) data;
    return var_SetFloat( obj, "scale", cur.f_float );
}
