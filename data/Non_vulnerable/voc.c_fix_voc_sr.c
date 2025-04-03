 */
static unsigned int fix_voc_sr( unsigned int sr )
{
    switch( sr )
    {
        /*case 8000:
            return 8000;*/
        case 11111:
            return 11025;
        case 22222:
            return 22050;
        case 44444:
            return 44100;
    }
    return sr;
}
