 */
xml_reader_t *xml_ReaderReset(xml_reader_t *reader, stream_t *stream)
{
    if (reader->p_stream)
        module_stop(reader, reader->p_module);
    reader->p_stream = stream;
    if ((stream != NULL) && module_start(reader, reader->p_module))
    {
        vlc_object_release(reader);
        return NULL;
    }
    return reader;
}
