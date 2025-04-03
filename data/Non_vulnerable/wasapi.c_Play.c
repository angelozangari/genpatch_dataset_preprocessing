}
static HRESULT Play(aout_stream_t *s, block_t *block)
{
    aout_stream_sys_t *sys = s->sys;
    void *pv;
    HRESULT hr;
    if (sys->chans_to_reorder)
        aout_ChannelReorder(block->p_buffer, block->i_buffer,
                          sys->chans_to_reorder, sys->chans_table, sys->format);
    hr = IAudioClient_GetService(sys->client, &IID_IAudioRenderClient, &pv);
    if (FAILED(hr))
    {
        msg_Err(s, "cannot get render client (error 0x%lx)", hr);
        goto out;
    }
    IAudioRenderClient *render = pv;
    for (;;)
    {
        UINT32 frames;
        hr = IAudioClient_GetCurrentPadding(sys->client, &frames);
        if (FAILED(hr))
        {
            msg_Err(s, "cannot get current padding (error 0x%lx)", hr);
            break;
        }
        assert(frames <= sys->frames);
        frames = sys->frames - frames;
        if (frames > block->i_nb_samples)
            frames = block->i_nb_samples;
        BYTE *dst;
        hr = IAudioRenderClient_GetBuffer(render, frames, &dst);
        if (FAILED(hr))
        {
            msg_Err(s, "cannot get buffer (error 0x%lx)", hr);
            break;
        }
        const size_t copy = frames * sys->bytes_per_frame;
        memcpy(dst, block->p_buffer, copy);
        hr = IAudioRenderClient_ReleaseBuffer(render, frames, 0);
        if (FAILED(hr))
        {
            msg_Err(s, "cannot release buffer (error 0x%lx)", hr);
            break;
        }
        IAudioClient_Start(sys->client);
        block->p_buffer += copy;
        block->i_buffer -= copy;
        block->i_nb_samples -= frames;
        sys->written += frames;
        if (block->i_nb_samples == 0)
            break; /* done */
        /* Out of buffer space, sleep */
        msleep(sys->frames * (CLOCK_FREQ / 2) / sys->rate);
    }
    IAudioRenderClient_Release(render);
out:
    block_Release(block);
    return hr;
}
