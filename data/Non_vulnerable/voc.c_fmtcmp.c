}
static int fmtcmp( es_format_t *ofmt, es_format_t *nfmt )
{
    return (ofmt->audio.i_bitspersample != nfmt->audio.i_bitspersample)
        || (ofmt->audio.i_rate != nfmt->audio.i_rate)
        || (ofmt->audio.i_channels != nfmt->audio.i_channels);
}
