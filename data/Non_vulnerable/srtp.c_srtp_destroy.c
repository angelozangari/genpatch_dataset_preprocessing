 */
void srtp_destroy (srtp_session_t *s)
{
    assert (s != NULL);
    proto_destroy (&s->rtcp);
    proto_destroy (&s->rtp);
    free (s);
}
