/*
 * scgui.h: GUI Support for Belgium EID card
 *
 * Copyright (C) 2003, Zetes Belgium
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SCGUI_H
#define SCGUI_H

/* if SCGUI_DLL not defined then used directly by opensc.dll */
#ifdef SCGUI_DLL

#ifdef SCGUI_EXPORTS
#define SCGUI_API __declspec(dllexport)
#else
#define SCGUI_API __declspec(dllimport)
#endif

#else
#define SCGUI_API
#endif

typedef int (*TimerFunc)(void);

typedef enum {
	SCGUI_OK,
	SCGUI_CANCEL,
	SCGUI_ERROR,
	SCGUI_NOT_INITED,
	SCGUI_BUFFER_TOO_SMALL,
	SCGUI_BAD_PARAM
} scgui_ret_t;

typedef enum {
	SCGUI_NO_ICON,
	SCGUI_SIGN_ICON
} scgui_param_t;

#define BELPIC_PIN_MIN_LENGTH 4
#define BELPIC_PIN_MAX_LENGTH 12

typedef struct 
{
	char pin[BELPIC_PIN_MAX_LENGTH+1];
	char *msg;
	char *btn_ok;
	char *btn_cancel;
	char *title;
	char *pinTooShort;
	scgui_param_t iconSign;
} VerifyPinData;

typedef struct 
{
	char oldpin[BELPIC_PIN_MAX_LENGTH+1];
	char newpin[BELPIC_PIN_MAX_LENGTH+1];
	char confirmpin[BELPIC_PIN_MAX_LENGTH+1];
	char *msg;
	char *btn_ok;
	char *btn_cancel;
	char *title;
	char *pinTooShort;
	char *confirmPinError;
} ChangePinData;

typedef struct 
{
	char *short_msg;
	char *long_msg;
	scgui_ret_t ret;
	char *btn_ok;
	char *btn_cancel;
	char *title;
	char *pinpad_reader;
} AskMessageData;

typedef struct 
{
	char *short_msg;
	char *long_msg;
	scgui_param_t iconSign;
	char *btn_cancel;
	char *title;
	char *pinpad_reader;
} DisplayMessageData;

typedef struct 
{
	char *msg;
	scgui_ret_t ret;
	char *btn_ok;
	char *btn_cancel;
	char *title;
    TimerFunc timer_func;
} InsertCardData;

#ifdef __cplusplus
extern "C"
{
#endif

typedef scgui_ret_t (*SCGUI_INIT)(void);
typedef scgui_ret_t (*SCGUI_ENTERPIN)(const char *, const char *, char *, int *, const char *, const char *, const char *, scgui_param_t);
typedef scgui_ret_t (*SCGUI_CHANGEPIN)(const char *, const char *, char *, int *, char *, int *, const char *, const char *, const char *, const char *);
typedef scgui_ret_t (*SCGUI_DISPLAY_MESSAGE)(const char *, const char *, const char *, const char *, void **, scgui_param_t, const char *);
typedef scgui_ret_t (*SCGUI_REMOVE_MESSAGE)(void *);
typedef scgui_ret_t (*SCGUI_INSERTCARD)(const char *, const char *, const char *, const char *, TimerFunc);
typedef scgui_ret_t (*SCGUI_ASK_MESSAGE)(const char *, const char *, const char *, const char *, const char *, const char *);

SCGUI_API scgui_ret_t scgui_init(void);

/* pin : memory allocated should be 13 bytes !! (12 + '\0') */
SCGUI_API scgui_ret_t scgui_enterpin(const char *title, const char *msg, char *pin, int *len, const char *btn_ok, const char *btn_cancel, const char *pinTooShort, scgui_param_t signIcon);

/* <old|new>pin : memory allocated should be 13 bytes !! (12 + '\0') */
SCGUI_API scgui_ret_t scgui_changepin(const char *title, const char *msg, char *oldpin, int *oldpinlen, char *newpin, int *newpinlen, const char *btn_ok, const char *btn_cancel, const char *pinTooShort, const char *confirmPinError);

/* if btn_cancel == NULL : only btn_ok shown */
SCGUI_API scgui_ret_t scgui_ask_message(const char *title, const char *short_msg, const char *long_msg, const char *btn_ok, const char *btn_cancel, const char *pinpad_reader);

SCGUI_API scgui_ret_t scgui_insertcard(const char *title, const char *msg, const char *btn_ok, const char *btn_cancel, TimerFunc pFunc);

/* modeless */
SCGUI_API scgui_ret_t scgui_display_message(const char *title, const char *short_msg, const char *long_msg, const char *btn_close, void **handle, scgui_param_t signIcon, const char *pinpad_reader);
SCGUI_API scgui_ret_t scgui_remove_message(void *handle);

#ifdef __cplusplus
}
#endif

#endif
