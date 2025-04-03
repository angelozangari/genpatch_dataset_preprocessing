  */
static void wma_window(WMADecodeContext *s, int32_t *in, int32_t *out)
{
    int block_len, bsize, n;
     /* left part */
     /*previous block was larger, so we'll use the size of the current block to set the window size*/
    if (s->block_len_bits <= s->prev_block_len_bits) {
         block_len = s->block_len;
         bsize = s->frame_len_bits - s->block_len_bits;
         vector_fmul_add_add(out, in, s->windows[bsize], block_len);
    } else {
         /*previous block was smaller or the same size, so use it's size to set the window length*/
         block_len = 1 << s->prev_block_len_bits;
         /*find the middle of the two overlapped blocks, this will be the first overlapped sample*/
         n = (s->block_len - block_len) >> 1;
         bsize = s->frame_len_bits - s->prev_block_len_bits;
         vector_fmul_add_add(out+n, in+n, s->windows[bsize],  block_len);
         memcpy(out+n+block_len, in+n+block_len, n*sizeof(int32_t));
    }
    /* Advance to the end of the current block and prepare to window it for the next block.
     * Since the window function needs to be reversed, we do it backwards starting with the
     * last sample and moving towards the first
     */
     out += s->block_len;
     in += s->block_len;
     /* right part */
     if (s->block_len_bits <= s->next_block_len_bits) {
         block_len = s->block_len;
         bsize = s->frame_len_bits - s->block_len_bits;
         vector_fmul_reverse(out, in, s->windows[bsize], block_len);
     } else {
         block_len = 1 << s->next_block_len_bits;
         n = (s->block_len - block_len) >> 1;
         bsize = s->frame_len_bits - s->next_block_len_bits;
         memcpy(out, in, n*sizeof(int32_t));
         vector_fmul_reverse(out+n, in+n, s->windows[bsize], block_len);
         memset(out+n+block_len, 0, n*sizeof(int32_t));
     }
}
