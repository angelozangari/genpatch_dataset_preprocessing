/* Read any available packets and enqueue them */
Code_t Z_ReadEnqueue()
{
    Code_t retval;
    if (ZGetFD() < 0)
	return (ZERR_NOPORT);
    while (Z_PacketWaiting())
	if ((retval = Z_ReadWait()) != ZERR_NONE)
	    return (retval);
    return (ZERR_NONE);
}
