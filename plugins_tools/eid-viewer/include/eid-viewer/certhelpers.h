#ifndef EID_VWR_CERT_HELPERS_H
#define EID_VWR_CERT_HELPERS_H

/** \addtogroup C_API_OSSL
  * @{
  */

/** \file certhelpers.h
  * \brief Certificate helper functions. Linux/OSX only.
  *
  * This file contains some helper functions to deal with certificates
  * and data integrity using OpenSSL. It is for Linux and OSX only.
  *
  * \note Some functions in this file share a common function signature,
  * so as to allow them to be used as callbacks from the same function.
  */

#include <openssl/x509.h>
#include <eid-viewer/macros.h>

enum cert_columns {
	CERT_COL_LABEL,
	CERT_COL_IMAGE,
	CERT_COL_VALIDFROM,
	CERT_COL_VALIDFROM_PAST,
	CERT_COL_VALIDTO,
	CERT_COL_VALIDTO_FUTURE,
	CERT_COL_USE,
	CERT_COL_VALIDITY,
	CERT_COL_DESC,
	CERT_COL_DATA,
	CERT_COL_NCOLS
};

/**
  * File type to be used when dumping data to a file
  */
enum dump_type {
	DUMP_DER, ///< Dump using ASN.1 Distinguished Encoding Rules (DER)
	DUMP_PEM, ///< Dump in PEM format
};

/**
  * \brief Get the X.509v3 Key Usage field of the given certificate
  *
  * \param label not used
  * \param cert the certificate to parse
  * \return a string representation of the X.509v3 Key Usage field, as
  * produced by OpenSSL. Should be free()d by the caller when no longer
  * needed.
  */
DllExport char* eid_vwr_get_use_flags(const char* label, X509* cert);
/**
  * \brief Get a printable version of the certificate's distinguished
  * name (DN).
  *
  * \param label not used
  * \param cert the certificate to parse
  * \return a multi-line version of the certificate's distinguished
  * name, as displayed on the eID Viewer's "certificates" pane. Should
  * be free()d by the caller when no longer needed.
  */
DllExport char* eid_vwr_detail_cert(const char* label, X509* cert);
/**
  * \brief Get a useful label for the certificate.
  *
  * \param label a fallback label, to be used if nothing better can be
  * found
  * \param cert the certificate to parse
  * \return the certificate's Common Name field, or a copy of the value
  * of the label parameter if the certificate has no common name. Should
  * be free()d by the caller when no longer needed.
  */
DllExport char* eid_vwr_describe_cert(const char* label, X509* cert);
/**
  * \brief Check that the data on the card is correctly signed.
  *
  * This method will:
  * - Take a hash of the photo, and compare it against the passed photo
  *   hash
  * - Verify the identity signature (which contains the photo hash)
  *   against the passed identity signature file
  * - Concatenate the passed identity file to the passed address file in
  *   the correct manner, and verify the passed address signature against the
  *   result
  *
  * \warning the validity of the RRN certificate is not checked by this
  * function.
  * \see eid_vwr_verify_rrncert() verifies the RRN certificate
  *
  * \param photo the contents of the binary data with label "PHOTO_FILE"
  * \param plen the length in bytes of the preceding parameter
  * \param photohash the contents of the binary data with label
  * "photo_hash"
  * \param hashlen the length in bytes of the preceding parameter
  * \param datafile the contents of the binary data with label
  * "DATA_FILE"
  * \param datfilelen the length in bytes of the preceding parameter
  * \param datasig the contents of the binary data with label
  * "SIGN_DATA_FILE"
  * \param datsiglen the length in bytes of the preceding parameter
  * \param addrfile the contents of the binary data with label
  * "ADDRESS_FILE"
  * \param addfilelen the length in bytes of the preceding parameter
  * \param addrsig the contents of the binary data with label
  * "SIGN_ADDRESS_FILE"
  * \param addsiglen the length in bytes of the preceding parameter
  * \param rrncert the contents of the binary data with label
  * CERT_RN_FILE (i.e., the RRN certificate)
  * \param certlen the length in bytes of the preceding parameter
  * \return 0 if one of the above-described tests indicates invalid
  * data, or 1 if all tests were successful.
  */
DllExport int eid_vwr_check_data_validity(const void* photo, int plen,
			const void* photohash, int hashlen,
			const void* datafile, int datfilelen,
			const void* datasig, int datsiglen,
			const void* addrfile, int addfilelen,
			const void* addrsig, int addsiglen,
			const void* rrncert, int certlen);
/**
  * \brief Write the given certificate to a file descriptor
  *
  * \param fd the filedescriptor to write to
  * \param derdata the certificate in DER format
  * \param len the length in bytes of the preceding parameter
  * \param how which format to use when writing
  */
DllExport void eid_vwr_dumpcert(int fd, const void* derdata, int len, enum dump_type how);

/**@}*/

void ensure_inited();

#endif
