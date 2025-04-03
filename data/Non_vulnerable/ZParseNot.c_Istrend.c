#undef next_field
static __inline__ char * Istrend (char *str) {
    /*
     * This should be faster on VAX models outside the 2 series.  Don't
     * use it if you are using MicroVAX 2 servers.  If you are using a
     * VS2 server, use something like
     *	#define next_field(ptr)		while(*ptr++)
     * instead of this code.
     *
     * This requires use of GCC to get the optimized code, but
     * everybody uses GCC, don't they? :-)
     */
    register char *str2 asm ("r1");
    /* Assumes that no field is longer than 64K.... */
    asm ("locc $0,$65535,(%1)" : "=r" (str2) : "r" (str) : "r0");
    return str2;
}
