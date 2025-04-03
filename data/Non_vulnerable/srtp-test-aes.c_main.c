}
int main (void)
{
    gcry_control (GCRYCTL_DISABLE_SECMEM, NULL);
    srtp_test ();
    return 0;
}
