}
int wma_decode_init(WMADecodeContext* s, asf_waveformatex_t *wfx)
{
    int i, flags1, flags2;
    int32_t *window;
    uint8_t *extradata;
    int64_t bps1;
    int32_t high_freq;
    int64_t bps;
    int sample_rate1;
    int coef_vlc_table;
    #ifdef CPU_COLDFIRE
    coldfire_set_macsr(EMAC_FRACTIONAL | EMAC_SATURATE);
    #endif
    s->sample_rate = wfx->rate;
    s->nb_channels = wfx->channels;
    s->bit_rate = wfx->bitrate;
    s->block_align = wfx->blockalign;
    s->coefs = &coefsarray;
    if (wfx->codec_id == ASF_CODEC_ID_WMAV1) {
        s->version = 1;
    } else if (wfx->codec_id == ASF_CODEC_ID_WMAV2 ) {
        s->version = 2;
    } else {
        /*one of those other wma flavors that don't have GPLed decoders */
        return -1;
    }
    /* extract flag infos */
    flags1 = 0;
    flags2 = 0;
    extradata = wfx->data;
    if (s->version == 1 && wfx->datalen >= 4) {
        flags1 = extradata[0] | (extradata[1] << 8);
        flags2 = extradata[2] | (extradata[3] << 8);
    }else if (s->version == 2 && wfx->datalen >= 6){
        flags1 = extradata[0] | (extradata[1] << 8) |
                 (extradata[2] << 16) | (extradata[3] << 24);
        flags2 = extradata[4] | (extradata[5] << 8);
    }
    s->use_exp_vlc = flags2 & 0x0001;
    s->use_bit_reservoir = flags2 & 0x0002;
    s->use_variable_block_len = flags2 & 0x0004;
    /* compute MDCT block size */
    if (s->sample_rate <= 16000){
        s->frame_len_bits = 9;
    }else if (s->sample_rate <= 22050 ||
             (s->sample_rate <= 32000 && s->version == 1)){
        s->frame_len_bits = 10;
    }else{
        s->frame_len_bits = 11;
    }
    s->frame_len = 1 << s->frame_len_bits;
    if (s-> use_variable_block_len)
    {
        int nb_max, nb;
        nb = ((flags2 >> 3) & 3) + 1;
        if ((s->bit_rate / s->nb_channels) >= 32000)
        {
            nb += 2;
        }
        nb_max = s->frame_len_bits - BLOCK_MIN_BITS;  /* max is 11-7 */
        if (nb > nb_max)
            nb = nb_max;
        s->nb_block_sizes = nb + 1;
    }
    else
    {
        s->nb_block_sizes = 1;
    }
    /* init rate dependant parameters */
    s->use_noise_coding = 1;
    high_freq = itofix64(s->sample_rate) >> 1;
    /* if version 2, then the rates are normalized */
    sample_rate1 = s->sample_rate;
    if (s->version == 2)
    {
        if (sample_rate1 >= 44100)
            sample_rate1 = 44100;
        else if (sample_rate1 >= 22050)
            sample_rate1 = 22050;
        else if (sample_rate1 >= 16000)
            sample_rate1 = 16000;
        else if (sample_rate1 >= 11025)
            sample_rate1 = 11025;
        else if (sample_rate1 >= 8000)
            sample_rate1 = 8000;
    }
    int64_t tmp = itofix64(s->bit_rate);
    int64_t tmp2 = itofix64(s->nb_channels * s->sample_rate);
    bps = fixdiv64(tmp, tmp2);
    int64_t tim = bps * s->frame_len;
    int64_t tmpi = fixdiv64(tim,itofix64(8));
    s->byte_offset_bits = av_log2(fixtoi64(tmpi+0x8000)) + 2;
    /* compute high frequency value and choose if noise coding should
       be activated */
    bps1 = bps;
    if (s->nb_channels == 2)
        bps1 = fixmul32(bps,0x1999a);
    if (sample_rate1 == 44100)
    {
        if (bps1 >= 0x9c29)
            s->use_noise_coding = 0;
        else
            high_freq = fixmul32(high_freq,0x6666);
    }
    else if (sample_rate1 == 22050)
    {
        if (bps1 >= 0x128f6)
            s->use_noise_coding = 0;
        else if (bps1 >= 0xb852)
            high_freq = fixmul32(high_freq,0xb333);
        else
            high_freq = fixmul32(high_freq,0x999a);
    }
    else if (sample_rate1 == 16000)
    {
        if (bps > 0x8000)
            high_freq = fixmul32(high_freq,0x8000);
        else
            high_freq = fixmul32(high_freq,0x4ccd);
    }
    else if (sample_rate1 == 11025)
    {
        high_freq = fixmul32(high_freq,0xb333);
    }
    else if (sample_rate1 == 8000)
    {
        if (bps <= 0xa000)
        {
           high_freq = fixmul32(high_freq,0x8000);
        }
        else if (bps > 0xc000)
        {
            s->use_noise_coding = 0;
        }
        else
        {
            high_freq = fixmul32(high_freq,0xa666);
        }
    }
    else
    {
        if (bps >= 0xcccd)
        {
            high_freq = fixmul32(high_freq,0xc000);
        }
        else if (bps >= 0x999a)
        {
            high_freq = fixmul32(high_freq,0x999a);
        }
        else
        {
            high_freq = fixmul32(high_freq,0x8000);
        }
    }
    /* compute the scale factor band sizes for each MDCT block size */
    {
        int a, b, pos, lpos, k, block_len, i, j, n;
        const uint8_t *table;
        if (s->version == 1)
        {
            s->coefs_start = 3;
        }
        else
        {
            s->coefs_start = 0;
        }
        for(k = 0; k < s->nb_block_sizes; ++k)
        {
            block_len = s->frame_len >> k;
            if (s->version == 1)
            {
                lpos = 0;
                for(i=0;i<25;++i)
                {
                    a = wma_critical_freqs[i];
                    b = s->sample_rate;
                    pos = ((block_len * 2 * a)  + (b >> 1)) / b;
                    if (pos > block_len)
                        pos = block_len;
                    s->exponent_bands[0][i] = pos - lpos;
                    if (pos >= block_len)
                    {
                        ++i;
                        break;
                    }
                    lpos = pos;
                }
                s->exponent_sizes[0] = i;
            }
            else
            {
                /* hardcoded tables */
                table = NULL;
                a = s->frame_len_bits - BLOCK_MIN_BITS - k;
                if (a < 3)
                {
                    if (s->sample_rate >= 44100)
                        table = exponent_band_44100[a];
                    else if (s->sample_rate >= 32000)
                        table = exponent_band_32000[a];
                    else if (s->sample_rate >= 22050)
                        table = exponent_band_22050[a];
                }
                if (table)
                {
                    n = *table++;
                    for(i=0;i<n;++i)
                        s->exponent_bands[k][i] = table[i];
                    s->exponent_sizes[k] = n;
                }
                else
                {
                    j = 0;
                    lpos = 0;
                    for(i=0;i<25;++i)
                    {
                        a = wma_critical_freqs[i];
                        b = s->sample_rate;
                        pos = ((block_len * 2 * a)  + (b << 1)) / (4 * b);
                        pos <<= 2;
                        if (pos > block_len)
                            pos = block_len;
                        if (pos > lpos)
                            s->exponent_bands[k][j++] = pos - lpos;
                        if (pos >= block_len)
                            break;
                        lpos = pos;
                    }
                    s->exponent_sizes[k] = j;
                }
            }
            /* max number of coefs */
            s->coefs_end[k] = (s->frame_len - ((s->frame_len * 9) / 100)) >> k;
            /* high freq computation */
            int32_t tmp1 = high_freq*2;            /* high_freq is a int32_t!*/
            int32_t tmp2=itofix32(s->sample_rate>>1);
            s->high_band_start[k] = fixtoi32( fixdiv32(tmp1, tmp2) * (block_len>>1) +0x8000);
            /*
            s->high_band_start[k] = (int)((block_len * 2 * high_freq) /
                                          s->sample_rate + 0.5);*/
            n = s->exponent_sizes[k];
            j = 0;
            pos = 0;
            for(i=0;i<n;++i)
            {
                int start, end;
                start = pos;
                pos += s->exponent_bands[k][i];
                end = pos;
                if (start < s->high_band_start[k])
                    start = s->high_band_start[k];
                if (end > s->coefs_end[k])
                    end = s->coefs_end[k];
                if (end > start)
                    s->exponent_high_bands[k][j++] = end - start;
            }
            s->exponent_high_sizes[k] = j;
        }
    }
    mdct_init_global();
    for(i = 0; i < s->nb_block_sizes; ++i)
    {
        ff_mdct_init(&s->mdct_ctx[i], s->frame_len_bits - i + 1, 1);
    }
    /*ffmpeg uses malloc to only allocate as many window sizes as needed.
     * However, we're really only interested in the worst case memory usage.
     * In the worst case you can have 5 window sizes, 128 doubling up 2048
     * Smaller windows are handled differently.
     * Since we don't have malloc, just statically allocate this
     */
    int32_t *temp[5];
    temp[0] = stat0;
    temp[1] = stat1;
    temp[2] = stat2;
    temp[3] = stat3;
    temp[4] = stat4;
    /* init MDCT windows : simple sinus window */
    for(i = 0; i < s->nb_block_sizes; i++)
    {
        int n, j;
        int32_t alpha;
        n = 1 << (s->frame_len_bits - i);
        window = temp[i];
        alpha = (1<<15)>>(s->frame_len_bits - i+1);   /* this calculates 0.5/(2*n) */
        for(j=0;j<n;++j)
        {
            int32_t j2 = itofix32(j) + 0x8000;
            window[j] = fsincos(fixmul32(j2,alpha)<<16, 0); /* alpha between 0 and pi/2 */
        }
        s->windows[i] = window;
    }
    s->reset_block_lengths = 1;
    if (s->use_noise_coding)
    {
        /* init the noise generator */
        if (s->use_exp_vlc)
        {
            s->noise_mult = 0x51f;
            s->noise_table = noisetable_exp;
        }
        else
        {
            s->noise_mult = 0xa3d;
            /* LSP values are simply 2x the EXP values */
            for (i=0;i<NOISE_TAB_SIZE;++i)
                noisetable_exp[i] = noisetable_exp[i]<< 1;
            s->noise_table = noisetable_exp;
        }
#if 0
        {
            unsigned int seed;
            int32_t norm;
            seed = 1;
            norm = 0;   // PJJ: near as makes any diff to 0!
            for (i=0;i<NOISE_TAB_SIZE;++i)
            {
                seed = seed * 314159 + 1;
                s->noise_table[i] = itofix32((int)seed) * norm;
            }
        }
#endif
         s->hgain_vlc.table = vlcbuf4;
         s->hgain_vlc.table_allocated = VLCBUF4SIZE;
         init_vlc(&s->hgain_vlc, HGAINVLCBITS, sizeof(hgain_huffbits),
                  hgain_huffbits, 1, 1,
                  hgain_huffcodes, 2, 2, 0);
    }
    if (s->use_exp_vlc)
    {
        s->exp_vlc.table = vlcbuf3;
        s->exp_vlc.table_allocated = VLCBUF3SIZE;
         init_vlc(&s->exp_vlc, EXPVLCBITS, sizeof(scale_huffbits),
                  scale_huffbits, 1, 1,
                  scale_huffcodes, 4, 4, 0);
    }
    else
    {
        wma_lsp_to_curve_init(s, s->frame_len);
    }
    /* choose the VLC tables for the coefficients */
    coef_vlc_table = 2;
    if (s->sample_rate >= 32000)
    {
        if (bps1 < 0xb852)
            coef_vlc_table = 0;
        else if (bps1 < 0x128f6)
            coef_vlc_table = 1;
    }
    runtabarray[0] = runtab0; runtabarray[1] = runtab1;
    levtabarray[0] = levtab0; levtabarray[1] = levtab1;
    s->coef_vlc[0].table = vlcbuf1;
    s->coef_vlc[0].table_allocated = VLCBUF1SIZE;
    s->coef_vlc[1].table = vlcbuf2;
    s->coef_vlc[1].table_allocated = VLCBUF2SIZE;
    init_coef_vlc(&s->coef_vlc[0], &s->run_table[0], &s->level_table[0],
                  &coef_vlcs[coef_vlc_table * 2], 0);
    init_coef_vlc(&s->coef_vlc[1], &s->run_table[1], &s->level_table[1],
                  &coef_vlcs[coef_vlc_table * 2 + 1], 1);
    s->last_superframe_len = 0;
    s->last_bitoffset = 0;
    return 0;
}
