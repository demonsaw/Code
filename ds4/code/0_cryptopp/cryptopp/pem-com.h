// pem-com.h - commom PEM routines. Written and placed in the public domain by Jeffrey Walton
//             Copyright assigned to the Crypto++ project.
//
// Crypto++ Library is copyrighted as a compilation and (as of version 5.6.2) licensed
// under the Boost Software License 1.0, while the individual files in the compilation
// are all public domain.

///////////////////////////////////////////////////////////////////////////
// For documentation on the PEM read and write routines, see
//   http://www.cryptopp.com/wiki/PEM_Pack
///////////////////////////////////////////////////////////////////////////

#ifndef CRYPTOPP_PEM_COM_H
#define CRYPTOPP_PEM_COM_H

#include "cryptlib.h"
#include "secblock.h"
#include "osrng.h"
#include "pem.h"

#include <string>

NAMESPACE_BEGIN(CryptoPP)

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

// By default, keys and parameters are validated after reading in Debug builds.
//   You will have to call key.Validate() yourself if desired. If you want automatic
//   validation, then uncomment the line below or set it on the command line.
// #define PEM_KEY_OR_PARAMETER_VALIDATION 1

// Perform key or parameter validation in Debug builds.
#if !defined(NDEBUG) && !defined(PEM_KEY_OR_PARAMETER_VALIDATION)
# define PEM_KEY_OR_PARAMETER_VALIDATION 1
#endif

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

static inline SecByteBlock StringToSecByteBlock(const std::string& str)
{
    return SecByteBlock(reinterpret_cast<const byte*>(str.data()), str.size());
}

static inline SecByteBlock StringToSecByteBlock(const char* str)
{
    return SecByteBlock(reinterpret_cast<const byte*>(str), strlen(str));
}

static inline const byte* BYTE_PTR(const char* cstr)
{
    return reinterpret_cast<const byte*>(cstr);
}

static inline byte* BYTE_PTR(char* cstr)
{
    return reinterpret_cast<byte*>(cstr);
}

// Attempts to locate a control field in a line
SecByteBlock GetControlField(const SecByteBlock& line);

// Attempts to fetch the data from a control line
SecByteBlock GetControlFieldData(const SecByteBlock& line);

// Returns 0 if a match, non-0 otherwise
int CompareNoCase(const SecByteBlock& first, const SecByteBlock& second);

// Base64 Encode
void PEM_Base64Encode(BufferedTransformation& source, BufferedTransformation& dest);

// Base64 Decode
void PEM_Base64Decode(BufferedTransformation& source, BufferedTransformation& dest);

// Write to a BufferedTransformation
void PEM_WriteLine(BufferedTransformation& bt, const std::string& line);
void PEM_WriteLine(BufferedTransformation& bt, const SecByteBlock& line);

// Signature changed a bit to match Crypto++. Salt must be PKCS5_SALT_LEN in length.
//  Salt, Data and Count are IN; Key and IV are OUT.
int OPENSSL_EVP_BytesToKey(HashTransformation& hash,
                           const unsigned char *salt, const unsigned char* data, int dlen,
                           unsigned int count, unsigned char *key, unsigned int ksize,
                           unsigned char *iv, unsigned int vsize);

// From OpenSSL, crypto/evp/evp.h.
static const unsigned int OPENSSL_PKCS5_SALT_LEN = 8;

// 64-character line length is required by RFC 1421.
static const unsigned int RFC1421_LINE_BREAK = 64;
// static const unsigned int OPENSSL_B64_LINE_BREAK = 76;


// Signals failure
static const size_t PEM_INVALID = static_cast<size_t>(-1);

static const std::string LBL_PUBLIC_BEGIN("-----BEGIN PUBLIC KEY-----");
static const std::string LBL_PUBLIC_END("-----END PUBLIC KEY-----");

static const std::string LBL_PRIVATE_BEGIN("-----BEGIN PRIVATE KEY-----");
static const std::string LBL_PRIVATE_END("-----END PRIVATE KEY-----");

static const std::string LBL_RSA_PUBLIC_BEGIN("-----BEGIN RSA PUBLIC KEY-----");
static const std::string LBL_RSA_PUBLIC_END("-----END RSA PUBLIC KEY-----");

static const std::string LBL_RSA_PRIVATE_BEGIN("-----BEGIN RSA PRIVATE KEY-----");
static const std::string LBL_RSA_PRIVATE_END("-----END RSA PRIVATE KEY-----");

static const std::string LBL_DSA_PUBLIC_BEGIN("-----BEGIN DSA PUBLIC KEY-----");
static const std::string LBL_DSA_PUBLIC_END("-----END DSA PUBLIC KEY-----");

static const std::string LBL_DSA_PRIVATE_BEGIN("-----BEGIN DSA PRIVATE KEY-----");
static const std::string LBL_DSA_PRIVATE_END("-----END DSA PRIVATE KEY-----");

static const std::string LBL_EC_PUBLIC_BEGIN("-----BEGIN EC PUBLIC KEY-----");
static const std::string LBL_EC_PUBLIC_END("-----END EC PUBLIC KEY-----");

static const std::string LBL_ECDSA_PUBLIC_BEGIN("-----BEGIN ECDSA PUBLIC KEY-----");
static const std::string LBL_ECDSA_PUBLIC_END("-----END ECDSA PUBLIC KEY-----");

static const std::string LBL_EC_PRIVATE_BEGIN("-----BEGIN EC PRIVATE KEY-----");
static const std::string LBL_EC_PRIVATE_END("-----END EC PRIVATE KEY-----");

static const std::string LBL_EC_PARAMETERS_BEGIN("-----BEGIN EC PARAMETERS-----");
static const std::string LBL_EC_PARAMETERS_END("-----END EC PARAMETERS-----");

static const std::string LBL_DH_PARAMETERS_BEGIN("-----BEGIN DH PARAMETERS-----");
static const std::string LBL_DH_PARAMETERS_END("-----END DH PARAMETERS-----");

static const std::string LBL_DSA_PARAMETERS_BEGIN("-----BEGIN DSA PARAMETERS-----");
static const std::string LBL_DSA_PARAMETERS_END("-----END DSA PARAMETERS-----");

static const std::string LBL_CERTIFICATE_BEGIN("-----BEGIN CERTIFICATE-----");
static const std::string LBL_CERTIFICATE_END("-----END CERTIFICATE-----");

static const std::string LBL_X509_CERTIFICATE_BEGIN("-----BEGIN X509 CERTIFICATE-----");
static const std::string LBL_X509_CERTIFICATE_END("-----END X509 CERTIFICATE-----");

static const std::string LBL_REQ_CERTIFICATE_BEGIN("-----BEGIN CERTIFICATE REQUEST-----");
static const std::string LBL_REQ_CERTIFICATE_END("-----END CERTIFICATE REQUEST-----");

static const std::string LBL_PROC_TYPE("Proc-Type");
static const std::string LBL_PROC_TYPE_ENC("Proc-Type: 4,ENCRYPTED");
static const std::string LBL_ENCRYPTED("ENCRYPTED");
static const std::string LBL_DEK_INFO("DEK-Info");
static const std::string LBL_CONTENT_DOMAIN("Content-Domain");
static const std::string LBL_COMMA(",");
static const std::string LBL_SPACE(" ");
static const std::string LBL_COLON(":");


static const SecByteBlock CR(BYTE_PTR("\r"), 1);
static const SecByteBlock LF(BYTE_PTR("\n"), 1);
static const SecByteBlock CRLF(BYTE_PTR("\r\n"), 2);

static const SecByteBlock SBB_PEM_BEGIN(BYTE_PTR("-----BEGIN"), 10);
static const SecByteBlock SBB_PEM_TAIL(BYTE_PTR("-----"), 5);
static const SecByteBlock SBB_PEM_END(BYTE_PTR("-----END"), 8);

static const SecByteBlock SBB_PUBLIC_BEGIN(StringToSecByteBlock(LBL_PUBLIC_BEGIN));
static const SecByteBlock SBB_PUBLIC_END(StringToSecByteBlock(LBL_PUBLIC_END));

static const SecByteBlock SBB_PRIVATE_BEGIN(StringToSecByteBlock(LBL_PRIVATE_BEGIN));
static const SecByteBlock SBB_PRIVATE_END(StringToSecByteBlock(LBL_PRIVATE_END));

static const SecByteBlock SBB_RSA_PUBLIC_BEGIN(StringToSecByteBlock(LBL_RSA_PUBLIC_BEGIN));
static const SecByteBlock SBB_RSA_PUBLIC_END(StringToSecByteBlock(LBL_RSA_PUBLIC_END));

static const SecByteBlock SBB_RSA_PRIVATE_BEGIN(StringToSecByteBlock(LBL_RSA_PRIVATE_BEGIN));
static const SecByteBlock SBB_RSA_PRIVATE_END(StringToSecByteBlock(LBL_RSA_PRIVATE_END));

static const SecByteBlock SBB_DSA_PUBLIC_BEGIN(StringToSecByteBlock(LBL_DSA_PUBLIC_BEGIN));
static const SecByteBlock SBB_DSA_PUBLIC_END(StringToSecByteBlock(LBL_DSA_PUBLIC_END));

static const SecByteBlock SBB_DSA_PRIVATE_BEGIN(StringToSecByteBlock(LBL_DSA_PRIVATE_BEGIN));
static const SecByteBlock SBB_DSA_PRIVATE_END(StringToSecByteBlock(LBL_DSA_PRIVATE_END));

static const SecByteBlock SBB_EC_PUBLIC_BEGIN(StringToSecByteBlock(LBL_EC_PUBLIC_BEGIN));
static const SecByteBlock SBB_EC_PUBLIC_END(StringToSecByteBlock(LBL_EC_PUBLIC_END));

static const SecByteBlock SBB_ECDSA_PUBLIC_BEGIN(StringToSecByteBlock(LBL_ECDSA_PUBLIC_BEGIN));
static const SecByteBlock SBB_ECDSA_PUBLIC_END(StringToSecByteBlock(LBL_ECDSA_PUBLIC_END));

static const SecByteBlock SBB_EC_PRIVATE_BEGIN(StringToSecByteBlock(LBL_EC_PRIVATE_BEGIN));
static const SecByteBlock SBB_EC_PRIVATE_END(StringToSecByteBlock(LBL_EC_PRIVATE_END));

static const SecByteBlock SBB_EC_PARAMETERS_BEGIN(StringToSecByteBlock(LBL_EC_PARAMETERS_BEGIN));
static const SecByteBlock SBB_EC_PARAMETERS_END(StringToSecByteBlock(LBL_EC_PARAMETERS_END));

static const SecByteBlock SBB_DH_PARAMETERS_BEGIN(StringToSecByteBlock(LBL_DH_PARAMETERS_BEGIN));
static const SecByteBlock SBB_DH_PARAMETERS_END(StringToSecByteBlock(LBL_DH_PARAMETERS_END));

static const SecByteBlock SBB_DSA_PARAMETERS_BEGIN(StringToSecByteBlock(LBL_DSA_PARAMETERS_BEGIN));
static const SecByteBlock SBB_DSA_PARAMETERS_END(StringToSecByteBlock(LBL_DSA_PARAMETERS_END));

static const SecByteBlock SBB_CERTIFICATE_BEGIN(StringToSecByteBlock(LBL_CERTIFICATE_BEGIN));
static const SecByteBlock SBB_CERTIFICATE_END(StringToSecByteBlock(LBL_CERTIFICATE_END));

static const SecByteBlock SBB_X509_CERTIFICATE_BEGIN(StringToSecByteBlock(LBL_X509_CERTIFICATE_BEGIN));
static const SecByteBlock SBB_X509_CERTIFICATE_END(StringToSecByteBlock(LBL_X509_CERTIFICATE_END));

static const SecByteBlock SBB_REQ_CERTIFICATE_BEGIN(StringToSecByteBlock(LBL_REQ_CERTIFICATE_BEGIN));
static const SecByteBlock SBB_REQ_CERTIFICATE_END(StringToSecByteBlock(LBL_REQ_CERTIFICATE_END));

static const SecByteBlock SBB_PROC_TYPE(StringToSecByteBlock(LBL_PROC_TYPE));
static const SecByteBlock SBB_PROC_TYPE_ENC(StringToSecByteBlock(LBL_PROC_TYPE_ENC));
static const SecByteBlock SBB_DEK_INFO(StringToSecByteBlock(LBL_DEK_INFO));
static const SecByteBlock SBB_CONTENT_DOMAIN(StringToSecByteBlock(LBL_CONTENT_DOMAIN));
static const SecByteBlock SBB_ENCRYPTED(StringToSecByteBlock(LBL_ENCRYPTED));
static const SecByteBlock SBB_COMMA(StringToSecByteBlock(LBL_COMMA));
static const SecByteBlock SBB_SPACE(StringToSecByteBlock(LBL_SPACE));
static const SecByteBlock SBB_COLON(StringToSecByteBlock(LBL_COLON));

NAMESPACE_END

#endif // CRYPTOPP_PEM_COM_H
