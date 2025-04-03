static int wma_decode_block(WMADecodeContext *s)
{
    int n, v, a, ch, code, bsize;
    int coef_nb_bits, total_gain;
    int nb_coefs[MAX_CHANNELS];
    int32_t mdct_norm;
    /* compute current block length */
    if (s->use_variable_block_len)
    {
        n = av_log2(s->nb_block_sizes - 1) + 1;
        if (s->reset_block_lengths)
        {
            s->reset_block_lengths = 0;
            v = get_bits(&s->gb, n);
            if (v >= s->nb_block_sizes)
            {
                return -2;
            }
            s->prev_block_len_bits = s->frame_len_bits - v;
            v = get_bits(&s->gb, n);
            if (v >= s->nb_block_sizes)
            {
                return -3;
            }
            s->block_len_bits = s->frame_len_bits - v;
        }
        else
        {
            /* update block lengths */
            s->prev_block_len_bits = s->block_len_bits;
            s->block_len_bits = s->next_block_len_bits;
        }
        v = get_bits(&s->gb, n);
        if (v >= s->nb_block_sizes)
            return -4;
        s->next_block_len_bits = s->frame_len_bits - v;
    }
    else
    {
        /* fixed block len */
        s->next_block_len_bits = s->frame_len_bits;
        s->prev_block_len_bits = s->frame_len_bits;
        s->block_len_bits = s->frame_len_bits;
    }
    /* now check if the block length is coherent with the frame length */
    s->block_len = 1 << s->block_len_bits;
    if ((s->block_pos + s->block_len) > s->frame_len)
    {
        return -5; /* oddly 32k sample from tracker fails here */
    }
    if (s->nb_channels == 2)
    {
        s->ms_stereo = get_bits(&s->gb, 1);
    }
    v = 0;
    for (ch = 0; ch < s->nb_channels; ++ch)
    {
        a = get_bits(&s->gb, 1);
        s->channel_coded[ch] = a;
        v |= a;
    }
    /* if no channel coded, no need to go further */
    /* XXX: fix potential framing problems */
    if (!v)
    {
        goto next;
    }
    bsize = s->frame_len_bits - s->block_len_bits;
    /* read total gain and extract corresponding number of bits for
       coef escape coding */
    total_gain = 1;
    for(;;)
    {
        a = get_bits(&s->gb, 7);
        total_gain += a;
        if (a != 127)
        {
            break;
        }
    }
    if (total_gain < 15)
        coef_nb_bits = 13;
    else if (total_gain < 32)
        coef_nb_bits = 12;
    else if (total_gain < 40)
        coef_nb_bits = 11;
    else if (total_gain < 45)
        coef_nb_bits = 10;
    else
        coef_nb_bits = 9;
    /* compute number of coefficients */
    n = s->coefs_end[bsize] - s->coefs_start;
    for(ch = 0; ch < s->nb_channels; ++ch)
    {
        nb_coefs[ch] = n;
    }
    /* complex coding */
    if (s->use_noise_coding)
    {
        for(ch = 0; ch < s->nb_channels; ++ch)
        {
            if (s->channel_coded[ch])
            {
                int i, n, a;
                n = s->exponent_high_sizes[bsize];
                for(i=0;i<n;++i)
                {
                    a = get_bits(&s->gb, 1);
                    s->high_band_coded[ch][i] = a;
                    /* if noise coding, the coefficients are not transmitted */
                    if (a)
                        nb_coefs[ch] -= s->exponent_high_bands[bsize][i];
                }
            }
        }
        for(ch = 0; ch < s->nb_channels; ++ch)
        {
            if (s->channel_coded[ch])
            {
                int i, n, val, code;
                n = s->exponent_high_sizes[bsize];
                val = (int)0x80000000;
                for(i=0;i<n;++i)
                {
                    if (s->high_band_coded[ch][i])
                    {
                        if (val == (int)0x80000000)
                        {
                            val = get_bits(&s->gb, 7) - 19;
                        }
                        else
                        {
                            //code = get_vlc(&s->gb, &s->hgain_vlc);
                            code = get_vlc2(&s->gb, s->hgain_vlc.table, HGAINVLCBITS, HGAINMAX);
                            if (code < 0)
                            {
                                return -6;
                            }
                            val += code - 18;
                        }
                        s->high_band_values[ch][i] = val;
                    }
                }
            }
        }
    }
    /* exponents can be reused in short blocks. */
    if ((s->block_len_bits == s->frame_len_bits) || get_bits(&s->gb, 1))
    {
        for(ch = 0; ch < s->nb_channels; ++ch)
        {
            if (s->channel_coded[ch])
            {
                if (s->use_exp_vlc)
                {
                    if (decode_exp_vlc(s, ch) < 0)
                    {
                        return -7;
                    }
                }
                else
                {
                    decode_exp_lsp(s, ch);
                }
                s->exponents_bsize[ch] = bsize;
            }
        }
    }
    /* parse spectral coefficients : just RLE encoding */
    for(ch = 0; ch < s->nb_channels; ++ch)
    {
        if (s->channel_coded[ch])
        {
            VLC *coef_vlc;
            int level, run, sign, tindex;
            int16_t *ptr, *eptr;
            const uint16_t *level_table, *run_table;
            /* special VLC tables are used for ms stereo because
               there is potentially less energy there */
            tindex = (ch == 1 && s->ms_stereo);
            coef_vlc = &s->coef_vlc[tindex];
            run_table = s->run_table[tindex];
            level_table = s->level_table[tindex];
            /* XXX: optimize */
            ptr = &s->coefs1[ch][0];
            eptr = ptr + nb_coefs[ch];
            memset(ptr, 0, s->block_len * sizeof(int16_t));
            for(;;)
            {
                code = get_vlc2(&s->gb, coef_vlc->table, VLCBITS, VLCMAX);
                //code = get_vlc(&s->gb, coef_vlc);
                if (code < 0)
                {
                    return -8;
                }
                if (code == 1)
                {
                    /* EOB */
                    break;
                }
                else if (code == 0)
                {
                    /* escape */
                    level = get_bits(&s->gb, coef_nb_bits);
                    /* NOTE: this is rather suboptimal. reading
                       block_len_bits would be better */
                    run = get_bits(&s->gb, s->frame_len_bits);
                }
                else
                {
                    /* normal code */
                    run = run_table[code];
                    level = level_table[code];
                }
                sign = get_bits(&s->gb, 1);
                if (!sign)
                    level = -level;
                ptr += run;
                if (ptr >= eptr)
                {
                    break;
                }
                *ptr++ = level;
                /* NOTE: EOB can be omitted */
                if (ptr >= eptr)
                    break;
            }
        }
        if (s->version == 1 && s->nb_channels >= 2)
        {
            align_get_bits(&s->gb);
        }
    }
    {
        int n4 = s->block_len >> 1;
        /* theres no reason to do a divide by two in fixed precision ... */
        mdct_norm = 0x10000>>(s->block_len_bits-1);
        if (s->version == 1)
        {
            mdct_norm *= fixtoi32(fixsqrt32(itofix32(n4))); /* PJJ : exercise this path */
        }
    }
    /* finally compute the MDCT coefficients */
    for(ch = 0; ch < s->nb_channels; ++ch)
    {
        if (s->channel_coded[ch])
        {
            int16_t *coefs1;
            int32_t *exponents, *exp_ptr;
            int32_t *coefs, atemp;
            int64_t mult;
            int64_t mult1;
            int32_t noise, temp1, temp2, mult2;
            int i, j, n, n1, last_high_band, esize;
            int32_t exp_power[HIGH_BAND_MAX_SIZE];
            coefs1 = s->coefs1[ch];
            exponents = s->exponents[ch];
            esize = s->exponents_bsize[ch];
            coefs = (*(s->coefs))[ch];
            n=0;
            /*
             *  Previously the IMDCT was run in 17.15 precision to avoid overflow. However rare files could
             *  overflow here as well, so switch to 17.15 during coefs calculation.
             */
            if (s->use_noise_coding)
            {
                /*TODO:  mult should be converted to 32 bit to speed up noise coding*/
                mult = fixdiv64(pow_table[total_gain+20],Fixed32To64(s->max_exponent[ch]));
                mult = mult* mdct_norm; //what the hell?  This is actually int64_t*2^16!
                mult1 = mult;
                /* very low freqs : noise */
                for(i = 0;i < s->coefs_start; ++i)
                {
                    *coefs++ = fixmul32((fixmul32(s->noise_table[s->noise_index],
                                        (*exponents++))>>4),Fixed32From64(mult1)) >>1;
                    s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                }
                n1 = s->exponent_high_sizes[bsize];
                /* compute power of high bands */
                exp_ptr = exponents +
                          s->high_band_start[bsize] -
                          s->coefs_start;
                last_high_band = 0; /* avoid warning */
                for (j=0;j<n1;++j)
                {
                    n = s->exponent_high_bands[s->frame_len_bits -
                                               s->block_len_bits][j];
                    if (s->high_band_coded[ch][j])
                    {
                        int32_t e2, v;
                        e2 = 0;
                        for(i = 0;i < n; ++i)
                        {
                            /*v is noramlized later on so its fixed format is irrelevant*/
                            v = exp_ptr[i]>>4;
                            e2 += fixmul32(v, v)>>3;
                        }
                         exp_power[j] = e2/n; /*n is an int...*/
                        last_high_band = j;
                    }
                    exp_ptr += n;
                }
                /* main freqs and high freqs */
                for(j=-1;j<n1;++j)
                {
                    if (j < 0)
                    {
                        n = s->high_band_start[bsize] -
                            s->coefs_start;
                    }
                    else
                    {
                        n = s->exponent_high_bands[s->frame_len_bits -
                                                   s->block_len_bits][j];
                    }
                    if (j >= 0 && s->high_band_coded[ch][j])
                    {
                        /* use noise with specified power */
                        int32_t tmp = fixdiv32(exp_power[j],exp_power[last_high_band]);
                        mult1 = (int64_t)fixsqrt32(tmp);
                        /* XXX: use a table */
                        /*mult1 is 48.16, pow_table is 48.16*/
                        mult1 = mult1 * pow_table[s->high_band_values[ch][j]+20] >> PRECISION;
                        /*this step has a fairly high degree of error for some reason*/
                        mult1 = fixdiv64(mult1,fixmul32(s->max_exponent[ch],s->noise_mult));
                        mult1 = mult1*mdct_norm>>PRECISION;
                        for(i = 0;i < n; ++i)
                        {
                            noise = s->noise_table[s->noise_index];
                            s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                            *coefs++ = fixmul32((fixmul32(*exponents,noise)>>4),Fixed32From64(mult1)) >>1;
                            ++exponents;
                        }
                    }
                    else
                    {
                        /* coded values + small noise */
                        for(i = 0;i < n; ++i)
                        {
                            // PJJ: check code path
                            noise = s->noise_table[s->noise_index];
                            s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                           /*don't forget to renormalize the noise*/
                           temp1 = (((int32_t)*coefs1++)<<16) + (noise>>4);
                           temp2 = fixmul32(*exponents, mult>>17);
                           *coefs++ = fixmul32(temp1, temp2);
                           ++exponents;
                        }
                    }
                }
                /* very high freqs : noise */
                n = s->block_len - s->coefs_end[bsize];
                mult2 = fixmul32(mult>>16,exponents[-1]) ;  /*the work around for 32.32 vars are getting stupid*/
                for (i = 0; i < n; ++i)
                {
                    /*renormalize the noise product and then reduce to 17.15 precison*/
                    *coefs++ = fixmul32(s->noise_table[s->noise_index],mult2) >>5;
                    s->noise_index = (s->noise_index + 1) & (NOISE_TAB_SIZE - 1);
                }
            }
            else
            {
                /*Noise coding not used, simply convert from exp to fixed representation*/
                int32_t mult3 = (int32_t)(fixdiv64(pow_table[total_gain+20],Fixed32To64(s->max_exponent[ch])));
                mult3 = fixmul32(mult3, mdct_norm);
                n = nb_coefs[ch];
                /* XXX: optimize more, unrolling this loop in asm might be a good idea */
                for(i = 0;i < s->coefs_start; i++)
                    *coefs++ = 0;
                for(i = 0;i < n; ++i)
                {
                    atemp = (coefs1[i] * mult3)>>1;
                    *coefs++=fixmul32(atemp,exponents[i<<bsize>>esize]);
                }
                n = s->block_len - s->coefs_end[bsize];
                memset(coefs, 0, n*sizeof(int32_t));
            }
        }
    }
    if (s->ms_stereo && s->channel_coded[1])
    {
        int32_t a, b;
        int i;
        int32_t (*coefs)[MAX_CHANNELS][BLOCK_MAX_SIZE]  = (s->coefs);
        /* nominal case for ms stereo: we do it before mdct */
        /* no need to optimize this case because it should almost
           never happen */
        if (!s->channel_coded[0])
        {
            memset((*(s->coefs))[0], 0, sizeof(int32_t) * s->block_len);
            s->channel_coded[0] = 1;
        }
        for(i = 0; i < s->block_len; ++i)
        {
            a = (*coefs)[0][i];
            b = (*coefs)[1][i];
            (*coefs)[0][i] = a + b;
            (*coefs)[1][i] = a - b;
        }
    }
    for(ch = 0; ch < s->nb_channels; ++ch)
    {
        if (s->channel_coded[ch])
        {
            static int32_t  output[BLOCK_MAX_SIZE * 2] IBSS_ATTR;
            int n4, index, n;
            n = s->block_len;
            n4 = s->block_len >>1;
            ff_imdct_calc(&s->mdct_ctx[bsize],
                          output,
                          (*(s->coefs))[ch]);
            /* add in the frame */
            index = (s->frame_len / 2) + s->block_pos - n4;
            wma_window(s, output, &s->frame_out[ch][index]);
            /* specific fast case for ms-stereo : add to second
               channel if it is not coded */
            if (s->ms_stereo && !s->channel_coded[1])
            {
                wma_window(s, output, &s->frame_out[1][index]);
            }
        }
    }
next:
    /* update block number */
    ++s->block_num;
    s->block_pos += s->block_len;
    if (s->block_pos >= s->frame_len)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
