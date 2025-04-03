}
static picture_t *Decode (decoder_t *dec, block_t **pp)
{
    picture_t *pic = NULL;
    if (pp == NULL)
        return NULL;
    block_t *block = *pp;
    if (block == NULL)
        return NULL;
    *pp = NULL;
    if (block->i_pts <= VLC_TS_INVALID)
        goto drop; /* undated block, should never happen */
    if (block->i_buffer < sz_XWDheader)
        goto drop;
    /* Skip XWD header */
    const XWDFileHeader *hdr = (const void *)block->p_buffer;
    uint32_t hdrlen = ntohl(hdr->header_size);
    if (hdrlen < sz_XWDheader
     || ntohl(hdr->file_version) < XWD_FILE_VERSION
     || ntohl(hdr->pixmap_format) != 2 /* ZPixmap */)
        goto drop;
    hdrlen += ntohl(hdr->ncolors) * sz_XWDColor;
    if (hdrlen > block->i_buffer)
        goto drop;
    block->p_buffer += hdrlen;
    block->i_buffer -= hdrlen;
    /* Parse XWD header */
    vlc_fourcc_t chroma = 0;
    switch (ntohl(hdr->pixmap_depth))
    {
        case 8:
            if (ntohl(hdr->bits_per_pixel) == 8)
                chroma = VLC_CODEC_RGB8;
            break;
        case 15:
            if (ntohl(hdr->bits_per_pixel) == 16)
                chroma = VLC_CODEC_RGB15;
            break;
        case 16:
            if (ntohl(hdr->bits_per_pixel) == 16)
                chroma = VLC_CODEC_RGB16;
            break;
        case 24:
            switch (ntohl(hdr->bits_per_pixel))
            {
                case 32: chroma = VLC_CODEC_RGB32; break;
                case 24: chroma = VLC_CODEC_RGB24; break;
            }
            break;
        case 32:
            if (ntohl(hdr->bits_per_pixel) == 32)
                chroma = VLC_CODEC_ARGB;
            break;
    }
    /* TODO: check image endianess, set RGB mask */
    if (!chroma)
        goto drop;
    video_format_Setup(&dec->fmt_out.video, chroma,
                       ntohl(hdr->pixmap_width), ntohl(hdr->pixmap_height),
                       ntohl(hdr->pixmap_width), ntohl(hdr->pixmap_height),
                       dec->fmt_in.video.i_sar_num,
                       dec->fmt_in.video.i_sar_den);
    const size_t copy = dec->fmt_out.video.i_width
                        * (dec->fmt_out.video.i_bits_per_pixel / 8);
    const uint32_t pitch = ntohl(hdr->bytes_per_line);
    /* Build picture */
    if (pitch < copy
     || (block->i_buffer / pitch) < dec->fmt_out.video.i_height)
        goto drop;
    pic = decoder_NewPicture(dec);
    if (pic == NULL)
        goto drop;
    const uint8_t *in = block->p_buffer;
    uint8_t *out = pic->p->p_pixels;
    for (unsigned i = 0; i < dec->fmt_out.video.i_height; i++)
    {
        memcpy(out, in, copy);
        in += pitch;
        out += pic->p->i_pitch;
    }
    pic->date = block->i_pts;
    pic->b_progressive = true;
drop:
    block_Release(block);
    return pic;
}
