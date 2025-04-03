 */
int verify_signature( signature_packet_t *sign, public_key_packet_t *p_key,
                      uint8_t *p_hash )
{
    /* the data to be verified (a hash) */
    const char *hash_sexp_s = "(data(flags raw)(value %m))";
    /* the public key */
    const char *key_sexp_s = "(public-key(dsa(p %m)(q %m)(g %m)(y %m)))";
    /* the signature */
    const char *sig_sexp_s = "(sig-val(dsa(r %m )(s %m )))";
    size_t erroff;
    gcry_mpi_t p, q, g, y, r, s, hash;
    p = q = g = y = r = s = hash = NULL;
    gcry_sexp_t key_sexp, hash_sexp, sig_sexp;
    key_sexp = hash_sexp = sig_sexp = NULL;
    int i_p_len = mpi_len( p_key->p );
    int i_q_len = mpi_len( p_key->q );
    int i_g_len = mpi_len( p_key->g );
    int i_y_len = mpi_len( p_key->y );
    if( gcry_mpi_scan( &p, GCRYMPI_FMT_USG, p_key->p + 2, i_p_len, NULL ) ||
        gcry_mpi_scan( &q, GCRYMPI_FMT_USG, p_key->q + 2, i_q_len, NULL ) ||
        gcry_mpi_scan( &g, GCRYMPI_FMT_USG, p_key->g + 2, i_g_len, NULL ) ||
        gcry_mpi_scan( &y, GCRYMPI_FMT_USG, p_key->y + 2, i_y_len, NULL ) ||
        gcry_sexp_build( &key_sexp, &erroff, key_sexp_s, p, q, g, y ) )
        goto problem;
    uint8_t *p_r = sign->r;
    uint8_t *p_s = sign->s;
    int i_r_len = mpi_len( p_r );
    int i_s_len = mpi_len( p_s );
    if( gcry_mpi_scan( &r, GCRYMPI_FMT_USG, p_r + 2, i_r_len, NULL ) ||
        gcry_mpi_scan( &s, GCRYMPI_FMT_USG, p_s + 2, i_s_len, NULL ) ||
        gcry_sexp_build( &sig_sexp, &erroff, sig_sexp_s, r, s ) )
        goto problem;
    int i_hash_len = gcry_md_get_algo_dlen (sign->digest_algo);
    if (sign->public_key_algo == GCRY_PK_DSA) {
        if (i_hash_len > i_q_len)
            i_hash_len = i_q_len;
    }
    if( gcry_mpi_scan( &hash, GCRYMPI_FMT_USG, p_hash, i_hash_len, NULL ) ||
        gcry_sexp_build( &hash_sexp, &erroff, hash_sexp_s, hash ) )
        goto problem;
    if( gcry_pk_verify( sig_sexp, hash_sexp, key_sexp ) )
        goto problem;
    return VLC_SUCCESS;
problem:
    if( p ) gcry_mpi_release( p );
    if( q ) gcry_mpi_release( q );
    if( g ) gcry_mpi_release( g );
    if( y ) gcry_mpi_release( y );
    if( r ) gcry_mpi_release( r );
    if( s ) gcry_mpi_release( s );
    if( hash ) gcry_mpi_release( hash );
    if( key_sexp ) gcry_sexp_release( key_sexp );
    if( sig_sexp ) gcry_sexp_release( sig_sexp );
    if( hash_sexp ) gcry_sexp_release( hash_sexp );
    return VLC_EGENERIC;
}
