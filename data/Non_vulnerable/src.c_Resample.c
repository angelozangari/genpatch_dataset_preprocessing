}
static block_t *Resample (filter_t *filter, block_t *in)
{
    block_t *out = NULL;
    const size_t framesize = filter->fmt_out.audio.i_bytes_per_frame;
    SRC_STATE *s = (SRC_STATE *)filter->p_sys;
    SRC_DATA src;
    src.src_ratio = (double)filter->fmt_out.audio.i_rate
                  / (double)filter->fmt_in.audio.i_rate;
    int err = src_set_ratio (s, src.src_ratio);
    if (err != 0)
    {
        msg_Err (filter, "cannot update resampling ratio: %s",
                 src_strerror (err));
        goto error;
    }
    src.input_frames = in->i_nb_samples;
    src.output_frames = ceil (src.src_ratio * src.input_frames);
    src.end_of_input = 0;
    out = block_Alloc (src.output_frames * framesize);
    if (unlikely(out == NULL))
        goto error;
    src.data_in = (float *)in->p_buffer;
    src.data_out = (float *)out->p_buffer;
    err = src_process (s, &src);
    if (err != 0)
    {
        msg_Err (filter, "cannot resample: %s", src_strerror (err));
        block_Release (out);
        out = NULL;
        goto error;
    }
    if (src.input_frames_used < src.input_frames)
        msg_Err (filter, "lost %ld of %ld input frames",
                 src.input_frames - src.input_frames_used, src.input_frames);
    out->i_buffer = src.output_frames_gen * framesize;
    out->i_nb_samples = src.output_frames_gen;
    out->i_pts = in->i_pts;
    out->i_length = src.output_frames_gen * CLOCK_FREQ
                  / filter->fmt_out.audio.i_rate;
error:
    block_Release (in);
    return out;
}
