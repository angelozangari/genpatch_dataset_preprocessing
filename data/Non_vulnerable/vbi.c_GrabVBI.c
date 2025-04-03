}
void GrabVBI (demux_t *p_demux, vlc_v4l2_vbi_t *vbi)
{
    vbi_capture_buffer *sliced_bytes;
    struct timeval timeout={0,0}; /* poll */
    int canc = vlc_savecancel ();
    int r = vbi_capture_pull_sliced (vbi->cap, &sliced_bytes, &timeout);
    switch (r) {
        case -1:
            msg_Err (p_demux, "error reading VBI: %s", vlc_strerror_c(errno));
        case  0: /* nothing avail */
            break;
        case  1: /* got data */
        {
            int n_lines = sliced_bytes->size / sizeof(vbi_sliced);
            if (!n_lines)
                break;
            int sliced_size = 2; /* Number of bytes per sliced line */
            int size = (sliced_size + 1) * n_lines;
            block_t *p_block = block_Alloc (size);
            if (unlikely(p_block == NULL))
                break;
            uint8_t* data = p_block->p_buffer;
            vbi_sliced *sliced_array = sliced_bytes->data;
            for (int field = 0; field < n_lines; field++)
            {
                *data = field;
                data++;
                memcpy(data, sliced_array[field].data, sliced_size);
                data += sliced_size;
            }
            p_block->i_pts = mdate();
            for (unsigned i = 0; i < VBI_NUM_CC_STREAMS; i++)
            {
                if (vbi->es[i] == NULL)
                    continue;
                block_t *dup = block_Duplicate(p_block);
                if (likely(dup != NULL))
                    es_out_Send(p_demux->out, vbi->es[i], dup);
            }
            block_Release(p_block);
        }
    }
    vlc_restorecancel (canc);
}
