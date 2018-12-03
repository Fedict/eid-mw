
/* ****************************************************************************
* eID Middleware Project.
* Copyright (C) 2008-2013 FedICT.
*
* This is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License version
* 3.0 as published by the Free Software Foundation.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this software; if not, see
* http://www.gnu.org/licenses/.
**************************************************************************** */

#pragma once

#define EIDMW_OK                   0

// Results of calling the function with incorrect parameters */

/** A function parameter has an unexpected value (general) */
#define EIDMW_ERR_PARAM_BAD            0xe1d00100L

/** A function parameter exceeded the allowed range */
#define EIDMW_ERR_PARAM_RANGE          0xe1d00101L

/** Bad file path (invalid characters, length no multiple of 4) */
#define EIDMW_ERR_BAD_PATH             0xe1d00102L

/** Unknown/unsupported algorithm */
#define EIDMW_ERR_ALGO_BAD             0xe1d00103L

/** Invalid/unsupported PIN operation */
#define EIDMW_ERR_PIN_OPERATION        0xe1d00104L

/** PIN not allowed for this card (invalid characters, too short/long) */
#define EIDMW_ERR_PIN_FORMAT           0xe1d00105L

/** Bad usage of a class or function */
#define EIDMW_ERR_BAD_USAGE            0xe1d00106L

/** wrong length */
#define EIDMW_ERR_LEN_BAD              0xe1d00107L

/** Condition of use not satisfied */
#define EIDMW_ERR_BAD_COND             0xe1d00108L

/** Command not available within current lifecycle */
#define EIDMW_ERR_CMD_NOT_AVAIL        0xe1d00109L

/** Wrong class byte */
#define EIDMW_ERR_CLASS_BAD            0xe1d0010AL


// Card errors

/** Generic card error */
#define EIDMW_ERR_CARD                 0xe1d00200L

/** Not Authenticated (no PIN specified) */
#define EIDMW_ERR_NOT_AUTHENTICATED    0xe1d00201L

/** This command is not supported by this card */
#define EIDMW_ERR_NOT_SUPPORTED        0xe1d00202L

/** Bad PIN */
#define EIDMW_ERR_PIN_BAD              0xe1d00203L

/** PIN blocked */
#define EIDMW_ERR_PIN_BLOCKED          0xe1d00204L

/** No card present or card has been removed */
#define EIDMW_ERR_NO_CARD              0xe1d00205L

/** Bad parameter P1 or P2 */
#define EIDMW_ERR_BAD_P1P2             0xe1d00206L

/** Command not allowed */
#define EIDMW_ERR_CMD_NOT_ALLOWED      0xe1d00207L

/** File not found */
#define EIDMW_ERR_FILE_NOT_FOUND       0xe1d00208L

/** Unable to read applet version from the card */
#define EIDMW_ERR_APPLET_VERSION_NOT_FOUND 0xe1d00209L

/** Card not activated */
#define EIDMW_ERR_NOT_ACTIVATED        0xe1d0020AL

/** EEPROM corrupted */
#define EIDMW_ERR_EEPROM               0xe1d0020BL

/** no precise diagnostic */
#define EIDMW_ERR_NOT_SPEC             0xe1d0020CL


// Reader errors

/** Error communicating with the card */
#define EIDMW_ERR_CARD_COMM            0xe1d00300L

/** No reader has been found */
#define EIDMW_ERR_NO_READER            0xe1d00301L

/** The pinpad reader returned an error */
#define EIDMW_ERR_PINPAD               0xe1d00302L

/** A card is present but we can't connect.
 * E.g. no card present, or a synchronous card is
 * inserted in the reader, but no 'reader specific card plugin'
 * is available for that reader */
#define EIDMW_ERR_CANT_CONNECT         0xe1d00303L

/** The card has been reset (e.g. by another application) */
#define EIDMW_ERR_CARD_RESET           0xe1d00304L

/** The card can't be accessed because of other connections */
#define EIDMW_ERR_CARD_SHARING         0xe1d00305L

/** Can't end a transaction that wasn't started (also returned
 *  on Mac OS X when multiple apps are access the card simultaneously) */
#define EIDMW_ERR_NOT_TRANSACTED       0xe1d00306L

// Internal errors (caused by the middleware)

/** An internal limit has been reached */
#define EIDMW_ERR_LIMIT                0xe1d00400L

/** An internal check failed */
#define EIDMW_ERR_CHECK	               0xe1d00401L

/** The PCSC library could not be located */
#define EIDMW_ERR_PCSC_LIB             0xe1d00402L

/** An attempt to resolve a Z-lib address failed */
#define EIDMW_ERR_ZLIB_RESOLVE         0xe1d00403L

/** And unknown error occurred */
#define EIDMW_ERR_UNKNOWN              0xe1d00404L

/** The pinpad reader received a wrong/unknown value */
#define EIDMW_PINPAD_ERR               0xe1d00405L

/** Dynamic library couldn't be loaded (found found at the specified location) */
#define EIDMW_CANT_LOAD_LIB            0xe1d00406L

/** Memory error */
#define EIDMW_ERR_MEMORY               0xe1d00407L

/** Couldn't delete cache file(s) */
#define EIDMW_ERR_DELETE_CACHE         0xe1d00408L

/** Error getting or writing config data */
#define EIDMW_CONF                     0xe1d00409L

/** The cached files have been tampered with */
#define EIDMW_CACHE_TAMPERED           0xe1d0040AL


// Socket related errors

/** Creation of a socket failed */
#define EIDMW_ERR_SOCKET_CREATE        0xe1d00500L

/** Socket 'send()' failed */
#define EIDMW_ERR_SOCKET_SEND          0xe1d00501L

/** Socket 'recv()' failed */
#define EIDMW_ERR_SOCKET_RECV          0xe1d00502L

/** Call to 'gethostbyname()' in SocketClient failed */
#define EIDMW_ERR_SOCKET_GETHOST       0xe1d00503L

/** Call to 'connect()' in SocketClient failed */
#define EIDMW_ERR_SOCKET_CONNECT       0xe1d00504L

/** Invalid in SocketServer, call to 'socket()' failed */
#define EIDMW_ERR_SOCKET_SOCKET        0xe1d00510L

/** Call to 'bind()' in SocketServer failed */
#define EIDMW_ERR_SOCKET_BIND          0xe1d00511L

/** Call to 'Accept()' in SocketServer failed */
#define EIDMW_ERR_SOCKET_ACCEPT        0xe1d00512L


// User errors/events

/** User pressed Cancel in PIN dialog */
#define EIDMW_ERR_PIN_CANCEL           0xe1d00600L

/** Pinpad timeout */
#define EIDMW_ERR_TIMEOUT              0xe1d00601L

/** The new PINs that were entered differ */
#define EIDMW_NEW_PINS_DIFFER          0xe1d00602L

/** A PIN with invalid length or format was entered */
#define EIDMW_WRONG_PIN_FORMAT         0xe1d00603L


// Parser errors

/** Could not find expected elements in parsed ASN.1 vector */
#define EIDMW_WRONG_ASN1_FORMAT        0xe1d00700L


// I/O errors
// errors modelled on the definitions in errno.h

/** File could not be opened */
#define EIDMW_FILE_NOT_OPENED          0xe1d00800L

/** Search permission is denied for a component of the path prefix of path. */
#define EIDMW_PERMISSION_DENIED        0xe1d00801L

/** A loop exists in symbolic links encountered during resolution of the path argument. */

/** A component of path does not name an existing file or path is an empty string.*/

/** A component of the path prefix is not a directory. */

/** The length of the path argument exceeds {PATH_MAX} or a pathname component is longer than {NAME_MAX}. */
#define EIDMW_INVALID_PATH             0xe1d00802L

/** {OPEN_MAX} file descriptors are currently open in the calling process. */

/** Too many files are currently open in the system.*/
#define EIDMW_TOO_MANY_OPENED_FILES    0xe1d00803L

/** The argument of closedir or readdir does not refer to an open directory stream. */
#define EIDMW_DIR_NOT_OPENED           0xe1d00804L

/** Interrupted by a signal */
#define EIDMW_INTERRUPTION             0xe1d00805L

/** One of the values in the structure to be returned cannot be represented correctly. */
#define EIDMW_OVERFLOW                 0xe1d00806L

/** An I/O error occurred while reading from the file system.*/
#define EIDMW_ERROR_IO                 0xe1d00807L

/** Call of the Logger after destruct time */
#define EIDMW_ERR_LOGGER_APPLEAVING    0xe1d00900L


// SDK error

/** The document type is unknown for this card */
#define EIDMW_ERR_DOCTYPE_UNKNOWN      0xe1d00b00L

/** The card type asked doesn't correspond with the real card type */
#define EIDMW_ERR_CARDTYPE_BAD         0xe1d00b01L

/** This card type is unknown */
#define EIDMW_ERR_CARDTYPE_UNKNOWN     0xe1d00b02L

/** This Certificate has no issuer (=root) */
#define EIDMW_ERR_CERT_NOISSUER        0xe1d00b03L

/** No release of SDK object has been done before closing the application */
#define EIDMW_ERR_RELEASE_NEEDED       0xe1d00b04L

/** Bad transaction commande (Unlock without Lock OR Lock without Unlock) */
#define EIDMW_ERR_BAD_TRANSACTION      0xe1d00b05L

/** The file type is unknown for this card */
#define EIDMW_ERR_FILETYPE_UNKNOWN     0xe1d00b06L

/** The card has been changed */
#define EIDMW_ERR_CARD_CHANGED         0xe1d00b07L

/** The reader set has been changed */
#define EIDMW_ERR_READERSET_CHANGED    0xe1d00b08L

/** User did not allow to read the card */
#define EIDMW_ERR_NOT_ALLOW_BY_USER    0xe1d00b09L

/** This Certificate has no CDP */
#define EIDMW_ERR_CERT_NOCRL           0xe1d00b0AL

/** This Certificate has no OCSP responder */
#define EIDMW_ERR_CERT_NOOCSP          0xe1d00b0BL

/** No root is found for the store or certificate */
#define EIDMW_ERR_CERT_NOROOT          0xe1d00b0CL

/** User did not yet allow or disallow to read the card */
#define EIDMW_ERR_USER_MUST_ANSWER     0xe1d00b0DL

/** User did not yet allow or disallow to read the card */
#define EIDMW_ERR_CANCELLED            0xe1d00b0EL
// Errors in system calls

/** a system call returned an error */
#define EIDMW_ERR_SYSTEM               0xe1d00c01L

/** a signal function returned an error */
#define EIDMW_ERR_SIGNAL               0xe1d00c02L
