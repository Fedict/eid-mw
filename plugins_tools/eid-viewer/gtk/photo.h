#ifndef EID_VWR_GTK_PHOTO_H
#define EID_VWR_GTK_PHOTO_H

struct photo_info
{
	void *raw;
	int plen;
	void *hash;
	int hlen;
	GdkPixbuf *pixbuf;
};

void displayphoto(char *label, void *data, int length);
void photohash(char *label, void *data, int length);

const struct photo_info *photo_get_data();

#endif
