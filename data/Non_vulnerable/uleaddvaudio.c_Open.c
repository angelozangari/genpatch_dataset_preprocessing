}
static int Open(vlc_object_t *object)
{
    decoder_t *dec = (decoder_t*)object;
    if (dec->fmt_in.i_codec != VLC_CODEC_ULEAD_DV_AUDIO_NTSC &&
        dec->fmt_in.i_codec != VLC_CODEC_ULEAD_DV_AUDIO_PAL)
        return VLC_EGENERIC;
    if (dec->fmt_in.audio.i_bitspersample != 12 && dec->fmt_in.audio.i_bitspersample != 16)
        return VLC_EGENERIC;
    if (dec->fmt_in.audio.i_channels != 2)
        return VLC_EGENERIC;
    if (dec->fmt_in.audio.i_rate <= 0)
        return VLC_EGENERIC;
    decoder_sys_t *sys = dec->p_sys = malloc(sizeof(*sys));
    if (!sys)
        return VLC_ENOMEM;
    sys->is_pal = dec->fmt_in.i_codec == VLC_CODEC_ULEAD_DV_AUDIO_PAL;
    sys->is_12bit = dec->fmt_in.audio.i_bitspersample == 12;
    date_Init(&sys->end_date, dec->fmt_in.audio.i_rate, 1);
    date_Set(&sys->end_date, 0);
    for (unsigned i = 0; i < sizeof(sys->shuffle) / sizeof(*sys->shuffle); i++) {
        const unsigned a = sys->is_pal ? 18 : 15;
        const unsigned b = 3 * a;
        sys->shuffle[i] = 80 * ((21 * (i % 3) + 9 * (i / 3) + ((i / a) % 3)) % b) +
                          (2 + sys->is_12bit) * (i / b) + 8;
    }
    es_format_Init(&dec->fmt_out, AUDIO_ES, VLC_CODEC_S16N);
    dec->fmt_out.audio.i_rate = dec->fmt_in.audio.i_rate;
    dec->fmt_out.audio.i_channels = 2;
    dec->fmt_out.audio.i_physical_channels =
    dec->fmt_out.audio.i_original_channels = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT;
    dec->pf_decode_audio = Decode;
    return VLC_SUCCESS;
}
