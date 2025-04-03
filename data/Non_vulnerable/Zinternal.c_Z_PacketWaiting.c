/* Return 1 if there is a packet waiting, 0 otherwise */
static int Z_PacketWaiting(void)
{
    struct timeval tv;
    fd_set read;
    tv.tv_sec = tv.tv_usec = 0;
    FD_ZERO(&read);
    FD_SET(ZGetFD(), &read);
    return (select(ZGetFD() + 1, &read, NULL, NULL, &tv));
}
