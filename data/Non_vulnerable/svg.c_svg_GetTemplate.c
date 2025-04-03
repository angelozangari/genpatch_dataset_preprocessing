}
static char *svg_GetTemplate( vlc_object_t *p_this )
{
    filter_t *p_filter = ( filter_t * )p_this;
    char *psz_filename;
    char *psz_template;
    FILE *file;
    psz_filename = var_InheritString( p_filter, "svg-template-file" );
    if( !psz_filename || (psz_filename[0] == 0) )
    {
        /* No filename. Use a default value. */
        psz_template = NULL;
    }
    else
    {
        /* Read the template */
        file = vlc_fopen( psz_filename, "rt" );
        if( !file )
        {
            msg_Warn( p_this, "SVG template file %s does not exist.",
                                         psz_filename );
            psz_template = NULL;
        }
        else
        {
            struct stat s;
            if( fstat( fileno( file ), &s ) )
            {
                /* Problem accessing file information. Should not
                   happen as we could open it. */
                psz_template = NULL;
            }
            else
            if( ((signed)s.st_size) < 0 )
            {
                msg_Err( p_this, "SVG template too big" );
                psz_template = NULL;
            }
            else
            {
                msg_Dbg( p_this, "reading %ld bytes from template %s",
                         (unsigned long)s.st_size, psz_filename );
                psz_template = calloc( 1, s.st_size + 42 );
                if( !psz_template )
                {
                    fclose( file );
                    free( psz_filename );
                    return NULL;
                }
                if(! fread( psz_template, s.st_size, 1, file ) )
                {
                    msg_Dbg( p_this, "No data read from template." );
                }
            }
            fclose( file );
        }
    }
    free( psz_filename );
    if( !psz_template )
    {
        /* Either there was no file, or there was an error.
           Use the default value */
        psz_template = strdup( "<?xml version='1.0' encoding='UTF-8' standalone='no'?> \
<svg version='1' preserveAspectRatio='xMinYMin meet' viewBox='0 0 800 600'> \
  <text x='10' y='560' fill='white' font-size='32'  \
        font-family='sans-serif'>%s</text></svg>" );
    }
    return psz_template;
}
