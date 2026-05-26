#include "photo_export.h"
#include "backend.h"
#include "cache.h"
#include "state.h"

#include <stdio.h>

static int export_photo_to_file(const EID_CHAR* target_file) {
	const struct eid_vwr_cache_item* item;
	FILE* file;
	size_t written;
	int close_result;

	if(target_file == NULL || !cache_have_label(TEXT("PHOTO_FILE"))) {
		return 1;
	}

	item = cache_get_data(TEXT("PHOTO_FILE"));
	if(item == NULL || item->data == NULL || item->len <= 0) {
		return 1;
	}

	file = EID_FOPEN(target_file, TEXT("wb"));
	if(file == NULL) {
		return 1;
	}

	written = fwrite(item->data, 1, item->len, file);
	close_result = fclose(file);

	return written == (size_t)item->len && close_result == 0 ? 0 : 1;
}

int eid_vwr_export_photo(void* data) {
	const EID_CHAR* target_file = (const EID_CHAR*)data;
	int result = export_photo_to_file(target_file);

	if(result != 0) {
		be_log(EID_VWR_LOG_ERROR, TEXT("failed exporting photo"));
		return result;
	}

	sm_handle_event(EVENT_EXPORT_PHOTO_READY, NULL, NULL, NULL);
	return 0;
}
