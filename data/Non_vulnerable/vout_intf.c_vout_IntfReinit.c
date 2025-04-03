}
void vout_IntfReinit( vout_thread_t *p_vout )
{
    var_TriggerCallback( p_vout, "zoom" );
    var_TriggerCallback( p_vout, "crop" );
    var_TriggerCallback( p_vout, "aspect-ratio" );
    var_TriggerCallback( p_vout, "video-on-top" );
    var_TriggerCallback( p_vout, "video-wallpaper" );
    var_TriggerCallback( p_vout, "video-filter" );
    var_TriggerCallback( p_vout, "sub-source" );
    var_TriggerCallback( p_vout, "sub-filter" );
    var_TriggerCallback( p_vout, "sub-margin" );
}
