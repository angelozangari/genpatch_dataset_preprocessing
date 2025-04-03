}
void xiph_destroy (demux_t *demux, void *data)
{
    rtp_xiph_t *self = data;
    if (!data)
        return;
    if (self->block)
    {
        self->block->i_flags |= BLOCK_FLAG_CORRUPTED;
        codec_decode (demux, self->id, self->block);
    }
    codec_destroy (demux, self->id);
    free (self);
}
