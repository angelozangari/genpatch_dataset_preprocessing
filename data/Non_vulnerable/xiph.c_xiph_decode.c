}
void xiph_decode (demux_t *demux, void *data, block_t *block)
{
    rtp_xiph_t *self = data;
    if (!data || block->i_buffer < 4)
        goto drop;
    /* 32-bits RTP header (§2.2) */
    uint32_t ident = GetDWBE (block->p_buffer);
    block->i_buffer -= 4;
    block->p_buffer += 4;
    unsigned fragtype = (ident >> 6) & 3;
    unsigned datatype = (ident >> 4) & 3;
    unsigned pkts = (ident) & 15;
    ident >>= 8;
    /* RTP defragmentation */
    if (self->block && (block->i_flags & BLOCK_FLAG_DISCONTINUITY))
    {   /* Screwed! discontinuity within a fragmented packet */
        msg_Warn (demux, self->vorbis ?
                  "discontinuity in fragmented Vorbis packet" :
                  "discontinuity in fragmented Theora packet");
        block_Release (self->block);
        self->block = NULL;
    }
    if (fragtype <= 1)
    {
        if (self->block) /* Invalid first fragment */
        {
            block_Release (self->block);
            self->block = NULL;
        }
    }
    else
    {
        if (!self->block)
            goto drop; /* Invalid non-first fragment */
    }
    if (fragtype > 0)
    {   /* Fragment */
        if (pkts > 0 || block->i_buffer < 2)
            goto drop;
        size_t fraglen = GetWBE (block->p_buffer);
        if (block->i_buffer < (fraglen + 2))
            goto drop; /* Invalid payload length */
        block->i_buffer = fraglen;
        if (fragtype == 1)/* Keep first fragment */
        {
            block->i_buffer += 2;
            self->block = block;
        }
        else
        {   /* Append non-first fragment */
            size_t len = self->block->i_buffer;
            self->block = block_Realloc (self->block, 0, len + fraglen);
            if (!self->block)
            {
                block_Release (block);
                return;
            }
            memcpy (self->block->p_buffer + len, block->p_buffer + 2,
                    fraglen);
            block_Release (block);
        }
        if (fragtype < 3)
            return; /* Non-last fragment */
        /* Last fragment reached, process it */
        block = self->block;
        self->block = NULL;
        SetWBE (block->p_buffer, block->i_buffer - 2);
        pkts = 1;
    }
    /* RTP payload packets processing */
    while (pkts > 0)
    {
        if (block->i_buffer < 2)
            goto drop;
        size_t len = GetWBE (block->p_buffer);
        block->i_buffer -= 2;
        block->p_buffer += 2;
        if (block->i_buffer < len)
            goto drop;
        switch (datatype)
        {
            case 0: /* Raw payload */
            {
                if (self->ident != ident)
                {
                    msg_Warn (demux, self->vorbis ?
                        "ignoring raw Vorbis payload without configuration" :
                        "ignoring raw Theora payload without configuration");
                    break;
                }
                block_t *raw = block_Alloc (len);
                memcpy (raw->p_buffer, block->p_buffer, len);
                raw->i_pts = block->i_pts; /* FIXME: what about pkts > 1 */
                codec_decode (demux, self->id, raw);
                break;
            }
            case 1: /* Packed configuration frame (§3.1.1) */
            {
                if (self->ident == ident)
                    break; /* Ignore config retransmission */
                void *extv;
                ssize_t extc = xiph_header (&extv, block->p_buffer, len);
                if (extc < 0)
                    break;
                es_format_t fmt;
                es_format_Init (&fmt, self->vorbis ? AUDIO_ES : VIDEO_ES,
                                self->vorbis ? VLC_CODEC_VORBIS
                                             : VLC_CODEC_THEORA);
                fmt.p_extra = extv;
                fmt.i_extra = extc;
                codec_destroy (demux, self->id);
                msg_Dbg (demux, self->vorbis ?
                         "Vorbis packed configuration received (%06"PRIx32")" :
                         "Theora packed configuration received (%06"PRIx32")",
                         ident);
                self->ident = ident;
                self->id = codec_init (demux, &fmt);
                break;
            }
        }
        block->i_buffer -= len;
        block->p_buffer += len;
        pkts--;
    }
drop:
    block_Release (block);
}
