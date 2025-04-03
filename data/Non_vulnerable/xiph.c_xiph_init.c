} rtp_xiph_t;
static void *xiph_init (bool vorbis)
{
    rtp_xiph_t *self = malloc (sizeof (*self));
    if (self == NULL)
        return NULL;
    self->id = NULL;
    self->block = NULL;
    self->ident = 0xffffffff; /* impossible value on the wire */
    self->vorbis = vorbis;
    return self;
}
