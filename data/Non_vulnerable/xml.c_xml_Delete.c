 *****************************************************************************/
void xml_Delete( xml_t *p_xml )
{
    module_unneed( p_xml, p_xml->p_module );
    vlc_object_release( p_xml );
}
