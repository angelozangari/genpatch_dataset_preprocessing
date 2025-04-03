}
void jabber_avatar_set(JabberStream *js, PurpleStoredImage *img)
{
	xmlnode *publish, *metadata, *item;
	if (!js->pep)
		return;
	/* Hmmm, not sure if this is worth the traffic, but meh */
	remove_avatar_0_12_nodes(js);
	if (!img) {
		publish = xmlnode_new("publish");
		xmlnode_set_attrib(publish, "node", NS_AVATAR_1_1_METADATA);
		item = xmlnode_new_child(publish, "item");
		metadata = xmlnode_new_child(item, "metadata");
		xmlnode_set_namespace(metadata, NS_AVATAR_1_1_METADATA);
		/* publish */
		jabber_pep_publish(js, publish);
	} else {
		/*
		 * TODO: This is pretty gross.  The Jabber PRPL really shouldn't
		 *       do voodoo to try to determine the image type, height
		 *       and width.
		 */
		/* A PNG header, including the IHDR, but nothing else */
		/* ATTN: this is in network byte order! */
		const struct {
			guchar signature[8]; /* must be hex 89 50 4E 47 0D 0A 1A 0A */
			struct {
				guint32 length; /* must be 0x0d */
				guchar type[4]; /* must be 'I' 'H' 'D' 'R' */
				guint32 width;
				guint32 height;
				guchar bitdepth;
				guchar colortype;
				guchar compression;
				guchar filter;
				guchar interlace;
			} ihdr;
		} *png = NULL;
		if (purple_imgstore_get_size(img) > sizeof(*png))
			png = purple_imgstore_get_data(img);
		/* check if the data is a valid png file (well, at least to some extent) */
		if(png && png->signature[0] == 0x89 &&
		   png->signature[1] == 0x50 &&
		   png->signature[2] == 0x4e &&
		   png->signature[3] == 0x47 &&
		   png->signature[4] == 0x0d &&
		   png->signature[5] == 0x0a &&
		   png->signature[6] == 0x1a &&
		   png->signature[7] == 0x0a &&
		   ntohl(png->ihdr.length) == 0x0d &&
		   png->ihdr.type[0] == 'I' &&
		   png->ihdr.type[1] == 'H' &&
		   png->ihdr.type[2] == 'D' &&
		   png->ihdr.type[3] == 'R') {
			/* parse PNG header to get the size of the image (yes, this is required) */
			guint32 width = ntohl(png->ihdr.width);
			guint32 height = ntohl(png->ihdr.height);
			xmlnode *data, *info;
			char *lengthstring, *widthstring, *heightstring;
			/* compute the sha1 hash */
			char *hash = jabber_calculate_data_hash(purple_imgstore_get_data(img),
			                                        purple_imgstore_get_size(img),
			    									"sha1");
			char *base64avatar = purple_base64_encode(purple_imgstore_get_data(img),
			                                          purple_imgstore_get_size(img));
			publish = xmlnode_new("publish");
			xmlnode_set_attrib(publish, "node", NS_AVATAR_1_1_DATA);
			item = xmlnode_new_child(publish, "item");
			xmlnode_set_attrib(item, "id", hash);
			data = xmlnode_new_child(item, "data");
			xmlnode_set_namespace(data, NS_AVATAR_1_1_DATA);
			xmlnode_insert_data(data, base64avatar, -1);
			/* publish the avatar itself */
			jabber_pep_publish(js, publish);
			g_free(base64avatar);
			lengthstring = g_strdup_printf("%" G_GSIZE_FORMAT,
			                               purple_imgstore_get_size(img));
			widthstring = g_strdup_printf("%u", width);
			heightstring = g_strdup_printf("%u", height);
			/* publish the metadata */
			publish = xmlnode_new("publish");
			xmlnode_set_attrib(publish, "node", NS_AVATAR_1_1_METADATA);
			item = xmlnode_new_child(publish, "item");
			xmlnode_set_attrib(item, "id", hash);
			metadata = xmlnode_new_child(item, "metadata");
			xmlnode_set_namespace(metadata, NS_AVATAR_1_1_METADATA);
			info = xmlnode_new_child(metadata, "info");
			xmlnode_set_attrib(info, "id", hash);
			xmlnode_set_attrib(info, "type", "image/png");
			xmlnode_set_attrib(info, "bytes", lengthstring);
			xmlnode_set_attrib(info, "width", widthstring);
			xmlnode_set_attrib(info, "height", heightstring);
			jabber_pep_publish(js, publish);
			g_free(lengthstring);
			g_free(widthstring);
			g_free(heightstring);
			g_free(hash);
		} else {
			purple_debug_error("jabber", "Cannot set PEP avatar to non-PNG data\n");
		}
	}
}
