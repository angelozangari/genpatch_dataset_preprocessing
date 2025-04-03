/* Wait for a complete notice to become available */
Code_t Z_WaitForComplete(void)
{
    Code_t retval;
    if (__Q_CompleteLength)
	return (Z_ReadEnqueue());
    while (!__Q_CompleteLength)
	if ((retval = Z_ReadWait()) != ZERR_NONE)
	    return (retval);
    return (ZERR_NONE);
}
