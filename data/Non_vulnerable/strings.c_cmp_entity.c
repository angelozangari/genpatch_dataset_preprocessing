};
static int cmp_entity (const void *key, const void *elem)
{
    const struct xml_entity_s *ent = elem;
    const char *name = key;
    return strncmp (name, ent->psz_entity, strlen (ent->psz_entity));
}
