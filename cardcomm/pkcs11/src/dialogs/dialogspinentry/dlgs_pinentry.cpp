#include "dialogs.h"
#include <assuan.h>
#include <sstream>
#include "../../common/log.h"
#include <config.h>
#include "../dialogsgtk/gettext.h"

static assuan_context_t ctx;
static bool inited = false;

#if ENABLE_NLS
#define _(s) dgettext("dialogs-beid", (s))
#define n_(s, p, i) dngettext("dialogs-beid", (s), (p), (i))
#else
#define _(s) s
#define n_(s, p, i) (i == 1 ? s : p)
#endif

using namespace eIDMW;

struct pindata {
	wchar_t *pin;
	unsigned long pinlen;
};

static gpg_error_t return_pin(void* where, const void* data, size_t datalen) {
	struct pindata *pinvals = (struct pindata*)where;
	if(datalen >= pinvals->pinlen) {
		return gpg_error(GPG_ERR_ASS_TOO_MUCH_DATA);
	}
	mbstowcs(pinvals->pin, (char*)data, (size_t)pinvals->pinlen);
	return 0;
}

static bool setup() {
	if(!inited) {
		gpg_error_t rc;
		if ((rc = assuan_new(&ctx)) != 0) {
			MWLOG(LEV_ERROR, MOD_DLG, L"Could not initialize libassuan for pinentry dialog: %s", gpg_strerror(rc));
			return true;
		}
		const char* argv[] = { "pinentry", NULL };
		assuan_fd_t fds[] = { 2, ASSUAN_INVALID_FD };
		if ((rc = assuan_pipe_connect(ctx, "/usr/bin/pinentry", argv, fds, NULL, NULL, ASSUAN_PIPE_CONNECT_DETACHED)) != 0) {
			MWLOG(LEV_ERROR, MOD_DLG, L"Could not start pinentry: %s", gpg_strerror(rc));
			return true;
		}
		for(int i=0; i<3; i++) {
			if(isatty(i)) {
				char path[64], cmd[64];
				int e;
				if((e = ttyname_r(i, path, sizeof path))) {
					MWLOG(LEV_WARN, MOD_DLG, L"Could not get TTY name: %s. Ignoring.", strerror(e));
					continue;
				}
				gpg_error_t r;
				snprintf(cmd, sizeof cmd, "OPTION ttyname=%s", path);
				if((r = assuan_transact(ctx, cmd, NULL, NULL, NULL, NULL, NULL, NULL))) {
					MWLOG(LEV_WARN, MOD_DLG, L"Could not set TTY: %s. Ignoring.", gpg_strerror(r));
					break;
				}
				char *termname = getenv("TERM");
				termname = strchr(termname, '=');
				if(!termname) {
					MWLOG(LEV_WARN, MOD_DLG, L"Could not set TTY type: TERM variable not found or invalid. Ignoring.");
					break;
				}
				snprintf(cmd, sizeof cmd, "OPTION ttytype=%s", termname);
				if((r = assuan_transact(ctx, cmd, NULL, NULL, NULL, NULL, NULL, NULL))) {
					MWLOG(LEV_WARN, MOD_DLG, L"Could not set TTY type: %s. Ignoring.", gpg_strerror(r));
					break;
				}
				// only need to do this once...
				break;
			}
		}
#if ENABLE_NLS
		bindtextdomain("dialogs-beid", DATAROOTDIR "/locale");
#endif
	}
	inited = true;
	return false;
}

static ssize_t get_exec_path(char *exec_path, size_t exec_path_size) {
	char proc_path[32];
	ssize_t exec_path_len = -1;
	pid_t pid = getpid();

	snprintf(proc_path, sizeof(proc_path) - 1, "/proc/%d/exe", pid);
	if ((exec_path_len = readlink(proc_path, exec_path, exec_path_size - 1)) != -1) {
		exec_path[exec_path_len] = '\0';
	} else {
		snprintf(exec_path, exec_path_size - 1, "A process with PID %d", pid);
		exec_path_len = strlen(exec_path);
	}
	return exec_path_len;
}

static DlgRet setup_dialog(char* title, char* desc, const wchar_t *csPinName, bool do_exec_path, bool is_error) {
	if(setup()) return DLG_ERR;

	char s[1024], t[1024];
	gpg_error_t r;

	snprintf(s, sizeof s, "SETTITLE %s", title);
	if((r = assuan_transact(ctx, s, NULL, NULL, NULL, NULL, NULL, NULL))) {
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not set title: %s", gpg_strerror(r));
		return DLG_ERR;
	}

	const char* cmd;
	if(is_error) {
		cmd = "SETERROR %s";
	} else {
		cmd = "SETDESC %s";
	}
	if(do_exec_path) {
		get_exec_path(s, sizeof s);
		snprintf(t, sizeof t, desc, s, csPinName);
		snprintf(s, sizeof s, cmd, t);
	} else {
		snprintf(t, sizeof t, desc, csPinName);
		snprintf(s, sizeof s, cmd, t);
	}
	char *loc;
	while((loc = strchr(s, '\n'))) {
		*loc = '\0';
		snprintf(t, sizeof t, "%s%%0A%s", s, loc+1);
		strncpy(s, t, (sizeof s) - 1);
		s[(sizeof s) - 1] = '\0';
	}
	if((r = assuan_transact(ctx, s, NULL, NULL, NULL, NULL, NULL, NULL))) {
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not set description: %s", gpg_strerror(r));
		return DLG_ERR;
	}

	if((r = assuan_transact(ctx, "SETOK OK", NULL, NULL, NULL, NULL, NULL, NULL))) {
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not set ok button text: %s", gpg_strerror(r));
		return DLG_ERR;
	}

	snprintf(s, sizeof s, "SETCANCEL %s", _("Cancel"));
	if((r = assuan_transact(ctx, s, NULL, NULL, NULL, NULL, NULL, NULL))) {
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not set cancel button text: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	return DLG_OK;
}

DlgRet eIDMW::DlgAskPin(DlgPinOperation operation,
			DlgPinUsage usage,
			const wchar_t * csPinName,
			DlgPinInfo pinInfo, wchar_t * csPin,
			unsigned long ulPinBufferLen) {
	DlgRet rv = setup_dialog(_("beID: PIN Code Required"), _("The application [%s] requests your eID %ls code."), csPinName, true, false);
	if(rv != DLG_OK) return rv;

	gpg_error_t r;
	struct pindata rval = { csPin, ulPinBufferLen };
	if((r = assuan_transact(ctx, "GETPIN", return_pin, &rval, NULL, NULL, NULL, NULL))) {
		if(gpg_err_code(r) == GPG_ERR_CANCELED) {
			return DLG_CANCEL;
		}
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not get pin: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	return DLG_OK;
}

DlgRet eIDMW::DlgAskPins(DlgPinOperation operation, DlgPinUsage usage, const wchar_t * csPinName, DlgPinInfo pin1Info, wchar_t * csPin1, unsigned long ulPin1BufferLen, DlgPinInfo pin2Info, wchar_t * csPin2, unsigned long ulPin2BufferLen) {
	DlgRet rv = setup_dialog(_("beID: PIN Code Required"), _("Request from Application [%s]:\nPlease enter your current eID %ls"), csPinName, true, false);
	if(rv != DLG_OK) return rv;

	gpg_error_t r;
	struct pindata pin = { csPin1, ulPin1BufferLen };
	if ((r = assuan_transact(ctx, "GETPIN", return_pin, &pin, NULL, NULL, NULL, NULL))) {
		if(gpg_err_code(r) == GPG_ERR_CANCELED) {
			return DLG_CANCEL;
		}
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not get old pin: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	pin.pin = csPin2;
	pin.pinlen = ulPin2BufferLen;
	if( (rv = setup_dialog(_("beID: PIN Code Required"), _("Please enter your new eID %ls (twice)"), csPinName, false, false)) != DLG_OK) {
		return rv;
	}
	if ((r = assuan_transact(ctx, "SETREPEAT", NULL, NULL, NULL, NULL, NULL, NULL))) {
		MWLOG(LEV_ERROR, MOD_DLG, L"Could not set repeat: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	if ((r = assuan_transact(ctx, "GETPIN", return_pin, &pin, NULL, NULL, NULL, NULL))) {
		if(gpg_err_code(r) == GPG_ERR_CANCELED) {
			return DLG_CANCEL;
		}
		MWLOG(LEV_ERROR, MOD_DLG, L"Coul not get new pin: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	return DLG_OK;
}

DlgRet eIDMW::DlgBadPin(DlgPinUsage usage, const wchar_t *csPinName, unsigned long ulRemainingTries) {
	gpg_error_t r;
	char *prompt;
	asprintf(&prompt, n_("You have entered an incorrect eID %%ls code.\nPlease note that at the next incorrect entry your PIN code will be blocked.","You have entered an incorrect eID %%ls code.\nPlease note that you have only %d attempts left before your PIN is blocked.", ulRemainingTries), (int)ulRemainingTries);
	DlgRet rv = setup_dialog(_("beID: Incorrect PIN Code"), prompt, csPinName, false, true);
	if(rv != DLG_OK) return rv;

	if((r = assuan_transact(ctx, "MESSAGE", NULL, NULL, NULL, NULL, NULL, NULL))) {
		if(gpg_err_code(r) == GPG_ERR_CANCELED) {
			return DLG_CANCEL;
		}
		MWLOG(LEV_ERROR, MOD_DLG, L"Showing message failed: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	return DLG_OK;
}

DlgRet eIDMW::DlgDisplayPinpadInfo(DlgPinOperation operation, const wchar_t * csReader, DlgPinUsage usage, const wchar_t * csPinName, const wchar_t * csMessage, unsigned long *pulHandle) {
	char msg[1024];
	char *title;

	switch(operation) {
		case DLG_PIN_OP_VERIFY:
		case DLG_PIN_OP_UNBLOCK_NO_CHANGE:
			snprintf(msg, sizeof msg, _("The application [%%s] requests your eID %%ls code on the secure pinpad reader:[%ls]..."), csReader);
			title = _("beID: PIN Code Required");
			break;
		case DLG_PIN_OP_CHANGE:
		case DLG_PIN_OP_UNBLOCK_CHANGE:
			snprintf(msg, sizeof msg, _("Request from application [%%s]:\nPlease change your eID %%ls code on the secure pinpad reader:\n[%ls]..."), csReader);
			title = _("beID Change PIN Code");
			break;
		default:
			MWLOG(LEV_ERROR, MOD_DLG, L"Unknown pinop received: %d", operation);
			return DLG_ERR;
	}
	DlgRet rv = setup_dialog(title, msg, csPinName, true, false);
	if(rv != DLG_OK) {
		return rv;
	}

	gpg_error_t r;
	if((r = assuan_transact(ctx, "MESSAGE", NULL, NULL, NULL, NULL, NULL, NULL))) {
		if(gpg_err_code(r) == GPG_ERR_CANCELED) {
			return DLG_CANCEL;
		}
		MWLOG(LEV_ERROR, MOD_DLG, L"Showing message failed: %s", gpg_strerror(r));
		return DLG_ERR;
	}
	return DLG_OK;
}

// We can't close PINENTRY message boxes.
void eIDMW::DlgClosePinpadInfo(unsigned long ulHandle) {
}
void eIDMW::DlgCloseAllPinpadInfo() {
}
