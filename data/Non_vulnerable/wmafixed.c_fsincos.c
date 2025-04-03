 */
long fsincos(unsigned long phase, int32_t *cos)
{
    int32_t x, x1, y, y1;
    unsigned long z, z1;
    int i;
    /* Setup initial vector */
    x = cordic_circular_gain;
    y = 0;
    z = phase;
    /* The phase has to be somewhere between 0..pi for this to work right */
    if (z < 0xffffffff >> 2) {
        /* z in first quadrant, z += pi/2 to correct */
        x = -x;
        z += 0xffffffff >> 2;
    } else if (z < 3 * (0xffffffff >> 2)) {
        /* z in third quadrant, z -= pi/2 to correct */
        z -= 0xffffffff >> 2;
    } else {
        /* z in fourth quadrant, z -= 3pi/2 to correct */
        x = -x;
        z -= 3 * (0xffffffff >> 2);
    }
    /* Each iteration adds roughly 1-bit of extra precision */
    for (i = 0; i < 31; i++) {
        x1 = x >> i;
        y1 = y >> i;
        z1 = atan_table[i];
        /* Decided which direction to rotate vector. Pivot point is pi/2 */
        if (z >= 0xffffffff >> 2) {
            x -= y1;
            y += x1;
            z -= z1;
        } else {
            x += y1;
            y -= x1;
            z += z1;
        }
    }
    if (cos)
        *cos = x;
    return y;
}
