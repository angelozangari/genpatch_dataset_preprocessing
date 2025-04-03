 */
void xml_ReaderDelete(xml_reader_t *reader)
{
    if (reader->p_stream)
        module_stop(reader, reader->p_module);
    vlc_object_release(reader);
}
