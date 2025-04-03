#include <assert.h>
static subpicture_t *spu_new_buffer( decoder_t *p_dec,
                                     const subpicture_updater_t *p_upd )
{
    VLC_UNUSED( p_dec );
    subpicture_t *p_subpicture = subpicture_New( p_upd );
    if( likely(p_subpicture != NULL) )
        p_subpicture->b_subtitle = true;
    return p_subpicture;
}
