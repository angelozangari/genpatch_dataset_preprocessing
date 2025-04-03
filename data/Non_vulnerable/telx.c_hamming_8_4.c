}
static int hamming_8_4( int a )
{
    switch (a) {
    case 0xA8:
        return 0;
    case 0x0B:
        return 1;
    case 0x26:
        return 2;
    case 0x85:
        return 3;
    case 0x92:
        return 4;
    case 0x31:
        return 5;
    case 0x1C:
        return 6;
    case 0xBF:
        return 7;
    case 0x40:
        return 8;
    case 0xE3:
        return 9;
    case 0xCE:
        return 10;
    case 0x6D:
        return 11;
    case 0x7A:
        return 12;
    case 0xD9:
        return 13;
    case 0xF4:
        return 14;
    case 0x57:
        return 15;
    default:
        return -1;     // decoding error , not yet corrected
    }
}
