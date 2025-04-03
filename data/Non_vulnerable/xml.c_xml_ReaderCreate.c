 */
xml_reader_t *xml_ReaderCreate(vlc_object_t *obj, stream_t *stream)
{
    xml_reader_t *reader;
    reader = vlc_custom_create(obj, sizeof(*reader), "xml reader");
    reader->p_stream = stream;
    reader->p_module = module_need(reader, "xml reader", NULL, false);
    if (unlikely(reader->p_module == NULL))
    {
        msg_Err(reader, "XML reader not found");
        vlc_object_release(reader);
        return NULL;
    }
    return reader;
}
