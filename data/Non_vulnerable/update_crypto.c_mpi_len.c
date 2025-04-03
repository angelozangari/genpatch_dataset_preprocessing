/* number of data bytes in a MPI */
static int mpi_len(const uint8_t *mpi)
{
    return (scalar_number(mpi, 2) + 7) / 8;
}
