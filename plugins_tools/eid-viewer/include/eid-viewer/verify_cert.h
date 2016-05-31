#ifndef EID_VWR_VERIFY_H
#define EID_VWR_VERIFY_H

/**
  * \defgroup C_API_OSSL OpenSSL-based helper functions
  *
  * These functions are available on GNU/Linux and OS X *only*.
  * @{
  */

/**
  * \file verify_cert.h
  * \brief Helper functions to deal with certificate validation using OpenSSL. Linux/OSX only.
  */

#ifdef __cplusplus
extern "C" {
#endif

#include <eid-viewer/oslayer.h>

/**
  * \brief Verify the validity of a certificate.
  *
  * This function will verify the validity of a certificate by
  * checking signatures, ensuring they're signed by a valid root
  * certificate, and performing an OCSP request against the OCSP server.
  *
  * \param certificate the X.509 certificate to check, in DER format.
  * \param certlen the length (in bytes) of certificate.
  * \param ca the CA certificate by which the certificate is signed
  * \param calen the length (in bytes) of ca
  * \param perform_ocsp_request a pointer to a function which should
  * perform an HTTP POST request and return the reply. It should return
  * the data as returned by the OCSP server. Its parameters are:
  *   - the URL for the OCSP server
  *   - the data that should be sent (by way of an HTTP POST request) to
  *   the OCSP server
  *   - the length of the above data
  *   - a pointer to store the length of the returned body
  *   - a pointer to store a handle to the request (which may, but is
  *     not required to, be the same as as the return value).
  *  
  * \param free_ocsp_request: called when the return value of
  * perform_ocsp_request is no longer needed. Its parameter will be
  * the data which was returned in perform_ocsp_request's last
  * parameter.
  */
DllExport enum eid_vwr_result eid_vwr_verify_cert(const void* certificate, size_t certlen, const void* ca, size_t calen, const void*(*perform_ocsp_request)(char*, void*, long, long*, void**), void(*free_ocsp_request)(void*));
/**
  * \brief Verify the validity of the RRN certificate.
  *
  * This function will verify the validity of the RRN certificate by
  * checking that the certificate is signed by a valid root certificate.
  *
  * \note This function will *not* perform an OCSP request, since the
  * root CA does not have an OCSP server.
  *
  * \param certificate the X.509 certificate to check, in DER format.
  * \param certlen the length (in bytes) of certificate.
  */
DllExport enum eid_vwr_result eid_vwr_verify_rrncert(const void* certificate, size_t certlen);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
