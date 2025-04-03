 */
void srtp_setrcc_rate (srtp_session_t *s, uint16_t rate)
{
    assert (rate != 0);
    s->rtp_rcc = rate;
}
