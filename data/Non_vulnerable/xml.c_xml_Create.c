 *****************************************************************************/
xml_t *xml_Create( vlc_object_t *p_this )
{
    xml_t *p_xml;
    p_xml = vlc_custom_create( p_this, sizeof( *p_xml ), "xml" );
    p_xml->p_module = module_need( p_xml, "xml", NULL, false );
    if( !p_xml->p_module )
    {
        vlc_object_release( p_xml );
        msg_Err( p_this, "XML provider not found" );
        return NULL;
    }
    return p_xml;
}
