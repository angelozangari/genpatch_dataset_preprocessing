}
static int wait_for_hmack(ZNotice_t *notice, void *uid)
{
    return (notice->z_kind == HMACK && ZCompareUID(&notice->z_uid, (ZUnique_Id_t *)uid));
}
