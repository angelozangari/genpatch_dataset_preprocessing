 *****************************************************************************/
static bool ReadIndexRecord( FILE *p_file, bool b_ts, int64_t i_frame,
                            uint64_t *pi_offset, uint16_t *pi_file_num )
{
    uint8_t index_record[8];
    if( fseek( p_file, sizeof(index_record) * i_frame, SEEK_SET ) != 0 )
        return false;
    if( fread( &index_record, sizeof(index_record), 1, p_file ) <= 0 )
        return false;
    /* VDR usually (only?) runs on little endian machines, but VLC has a
     * broader audience. See recording.* in VDR source for data layout. */
    if( b_ts )
    {
        uint64_t i_index_entry = GetQWLE( &index_record );
        *pi_offset = i_index_entry & UINT64_C(0xFFFFFFFFFF);
        *pi_file_num = i_index_entry >> 48;
    }
    else
    {
        *pi_offset = GetDWLE( &index_record );
        *pi_file_num = index_record[5];
    }
    return true;
}
