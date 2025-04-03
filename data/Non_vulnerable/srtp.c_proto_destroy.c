}
static void proto_destroy (srtp_proto_t *p)
{
    gcry_md_close (p->mac);
    gcry_cipher_close (p->cipher);
}
