}
void vout_ChangeAspectRatio( vout_thread_t *p_vout,
                             unsigned int i_num, unsigned int i_den )
{
    vout_ControlChangeSampleAspectRatio( p_vout, i_num, i_den );
}
