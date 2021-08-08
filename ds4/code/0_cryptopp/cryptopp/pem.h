// pem.h - PEM read and write routines. Written and placed in the public domain by Jeffrey Walton
//         Copyright assigned to the Crypto++ project.
//
// Crypto++ Library is copyrighted as a compilation and (as of version 5.6.2) licensed
// under the Boost Software License 1.0, while the individual files in the compilation
// are all public domain.

//! \file pem.h
//! \brief Functions to read and write PEM encoded objects
//! \since This is a library add-on. You must download and compile it yourself. Also see
//!   <A HREF="http://www.cryptopp.com/wiki/PEM_Pack">PEM Pack</A> on the Crypto++ wiki.

///////////////////////////////////////////////////////////////////////////

#ifndef CRYPTOPP_PEM_H
#define CRYPTOPP_PEM_H

#include "pubkey.h"
#include "eccrypto.h"
#include "gfpcrypt.h"
#include "integer.h"
#include "dsa.h"
#include "rsa.h"

NAMESPACE_BEGIN(CryptoPP)

// Attempts to read a PEM encoded key or parameter. If there are multiple keys or parameters, then only
//   the first is read. If `trimTrailing` is true, then trailing whitespace is trimmed from the source
//   BufferedTransformation. The destination BufferedTransformation will have one line ending if it was
//   present in source.
// PEM_NextObject will parse an invalid object. For example, it will parse a key or parameter with
//   `-----BEGIN FOO-----` and `-----END BAR-----`. The parser only looks for BEGIN and END (and the
//   dashes). The malformed input will be caught later when a particular key or parameter is parsed.
// On failure, InvalidDataFormat is thrown.
void PEM_NextObject(BufferedTransformation& src, BufferedTransformation& dest, bool trimTrailing=true);

// PEM types we understand. We can read and write many of them, but not all of them.
//   http://stackoverflow.com/questions/5355046/where-is-the-pem-file-format-specified
enum PEM_Type { PEM_PUBLIC_KEY = 1, PEM_PRIVATE_KEY,
    PEM_RSA_PUBLIC_KEY, PEM_RSA_PRIVATE_KEY, PEM_RSA_ENC_PRIVATE_KEY,
    PEM_DSA_PUBLIC_KEY, PEM_DSA_PRIVATE_KEY, PEM_DSA_ENC_PRIVATE_KEY,
    PEM_EC_PUBLIC_KEY, PEM_ECDSA_PUBLIC_KEY, PEM_EC_PRIVATE_KEY, PEM_EC_ENC_PRIVATE_KEY,
    PEM_EC_PARAMETERS, PEM_DH_PARAMETERS, PEM_DSA_PARAMETERS,
    PEM_X509_CERTIFICATE, PEM_REQ_CERTIFICATE, PEM_CERTIFICATE,
    PEM_UNSUPPORTED = 0xFFFFFFFF };

// Attempts to determine the type of key or parameter
PEM_Type PEM_GetType(const BufferedTransformation& bt);

//////////////////////////////////////////////////////////////////////////////////////////

// Why Not Specialize Function Templates?
//   http://www.gotw.ca/publications/mill17.htm

//! \brief Load a PEM encoded RSA public key
//! \param bt the source BufferedTransformation
//! \param key the RSA public key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, RSA::PublicKey& key);

//! \brief Load a PEM encoded RSA private key
//! \param bt the source BufferedTransformation
//! \param key the RSA private key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, RSA::PrivateKey& key);

//! \brief Load a PEM encoded RSA private key
//! \param bt the source BufferedTransformation
//! \param key the RSA private key
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, RSA::PrivateKey& key,
              const char* password, size_t length);

//! \brief Load a PEM encoded DSA public key
//! \param bt the source BufferedTransformation
//! \param key the DSA public key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DSA::PublicKey& key);

//! \brief Load a PEM encoded DSA private key
//! \param bt the source BufferedTransformation
//! \param key the DSA private key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DSA::PrivateKey& key);

//! \brief Load a PEM encoded DSA private key
//! \param bt the source BufferedTransformation
//! \param key the DSA private key
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DSA::PrivateKey& key,
              const char* password, size_t length);

//! \brief Load a PEM encoded ECP public key
//! \param bt the source BufferedTransformation
//! \param key the ECP public key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_PublicKey_EC<ECP>& key);

//! \brief Load a PEM encoded ECP private key
//! \param bt the source BufferedTransformation
//! \param key the ECP private key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_PrivateKey_EC<ECP>& key);

//! \brief Load a PEM encoded ECP private key
//! \param bt the source BufferedTransformation
//! \param key the ECP private key
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_PrivateKey_EC<ECP>& key,
              const char* password, size_t length);

//! \brief Load a PEM encoded EC2N public key
//! \param bt the source BufferedTransformation
//! \param key the EC2N public key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_PublicKey_EC<EC2N>& key);

//! \brief Load a PEM encoded EC2N private key
//! \param bt the source BufferedTransformation
//! \param key the EC2N private key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_PrivateKey_EC<EC2N>& key);

//! \brief Load a PEM encoded EC2N private key
//! \param bt the source BufferedTransformation
//! \param key the EC2N private key
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_PrivateKey_EC<EC2N>& key,
              const char* password, size_t length);

//! \brief Load a PEM encoded ECDSA private key
//! \param bt the source BufferedTransformation
//! \param key the ECDSA private key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_Keys_ECDSA<ECP>::PrivateKey& key);

//! \brief Load a PEM encoded ECDSA private key
//! \param bt the source BufferedTransformation
//! \param key the ECDSA private key
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_Keys_ECDSA<ECP>::PrivateKey& key,
              const char* password, size_t length);

//! \brief Load a PEM encoded ECDSA public key
//! \param bt the source BufferedTransformation
//! \param key the ECDSA public key
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_Keys_ECDSA<EC2N>::PrivateKey& key);

//! \brief Load a PEM encoded ECDSA private key
//! \param bt the source BufferedTransformation
//! \param key the ECDSA private key
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_Keys_ECDSA<EC2N>::PrivateKey& key,
              const char* password, size_t length);

//! \brief Load a PEM encoded DSA group parameters
//! \param bt the source BufferedTransformation
//! \param params the DSA group parameters
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_GroupParameters_DSA& params);

//! \brief Load a PEM encoded ECP group parameters
//! \param bt the source BufferedTransformation
//! \param params the ECP group parameters
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_GroupParameters_EC<ECP>& params);

//! \brief Load a PEM encoded EC2N group parameters
//! \param bt the source BufferedTransformation
//! \param params the EC2N group parameters
//! \throws Exception on failure
void PEM_Load(BufferedTransformation& bt, DL_GroupParameters_EC<EC2N>& params);

//! \brief Load a PEM encoded Diffie-Hellman parameters
//! \param bt the source BufferedTransformation
//! \param p the prime modulus
//! \param g the group generator
//! \throws Exception on failure
void PEM_DH_Load(BufferedTransformation& bt, Integer& p, Integer& g);

//! \brief Load a PEM encoded Diffie-Hellman parameters
//! \param bt the source BufferedTransformation
//! \param p the prime modulus
//! \param q the subgroup order
//! \param g the group generator
//! \throws Exception on failure
void PEM_DH_Load(BufferedTransformation& bt, Integer& p, Integer& q, Integer& g);

//////////////////////////////////////////////////////////////////////////////////////////

// Begin the Write routines. The write routines always write the "named curve"
//   (i.e., the OID of secp256k1) rather than the domain parameters. This is because
//   RFC 5915 specifies the format. In addition, OpenSSL cannot load and utilize
//   an EC key with a non-named curve into a server.
// For encrpted private keys, the algorithm should be a value like
//   `AES-128-CBC`. See pem-rd.cpp and pem-wr.cpp for the values that are recognized.
// On failure, any number of Crypto++ exceptions are thrown. No custom exceptions
//   are thrown.

//! \brief Save a PEM encoded RSA public key
//! \param bt the destination BufferedTransformation
//! \param key the RSA public key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const RSA::PublicKey& key);

//! \brief Save a PEM encoded RSA private key
//! \param bt the destination BufferedTransformation
//! \param key the RSA private key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const RSA::PrivateKey& key);

//! \brief Save a PEM encoded RSA private key
//! \param bt the destination BufferedTransformation
//! \param rng a RandomNumberGenerator to produce an initialization vector
//! \param key the RSA private key
//! \param algorithm the encryption algorithm
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \details The algorithm should be a value like <tt>AES-128-CBC</tt>. See <tt>pem-rd.cpp</tt> and
//!   <tt>pem-wr.cpp</tt> for the values that are recognized.
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, RandomNumberGenerator& rng, const RSA::PrivateKey& key,
              const std::string& algorithm, const char* password, size_t length);

//! \brief Save a PEM encoded DSA public key
//! \param bt the destination BufferedTransformation
//! \param key the DSA public key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DSA::PublicKey& key);

//! \brief Save a PEM encoded DSA private key
//! \param bt the destination BufferedTransformation
//! \param key the DSA private key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DSA::PrivateKey& key);

//! \brief Save a PEM encoded DSA private key
//! \param bt the destination BufferedTransformation
//! \param rng a RandomNumberGenerator to produce an initialization vector
//! \param key the DSA private key
//! \param algorithm the encryption algorithm
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \details The algorithm should be a value like <tt>AES-128-CBC</tt>. See <tt>pem-rd.cpp</tt> and
//!   <tt>pem-wr.cpp</tt> for the values that are recognized.
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, RandomNumberGenerator& rng, const DSA::PrivateKey& key,
              const std::string& algorithm, const char* password, size_t length);

//! \brief Save a PEM encoded ECP public key
//! \param bt the destination BufferedTransformation
//! \param key the ECP public key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_PublicKey_EC<ECP>& key);

//! \brief Save a PEM encoded ECP private key
//! \param bt the destination BufferedTransformation
//! \param key the ECP private key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_PrivateKey_EC<ECP>& key);

//! \brief Save a PEM encoded ECP private key
//! \param bt the destination BufferedTransformation
//! \param rng a RandomNumberGenerator to produce an initialization vector
//! \param key the ECP private key
//! \param algorithm the encryption algorithm
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \details The algorithm should be a value like <tt>AES-128-CBC</tt>. See <tt>pem-rd.cpp</tt> and
//!   <tt>pem-wr.cpp</tt> for the values that are recognized.
//! \details The "named curve" (i.e., the OID of secp256k1) is used rather than the domain parameters.
//!   This is because RFC 5915 specifies the format. In addition, OpenSSL cannot load and utilize an
//!   EC key with a non-named curve into a server.
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, RandomNumberGenerator& rng, const DL_PrivateKey_EC<ECP>& key,
              const std::string& algorithm, const char* password, size_t length);

//! \brief Save a PEM encoded EC2N public key
//! \param bt the destination BufferedTransformation
//! \param key the EC2N public key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_PublicKey_EC<EC2N>& key);

//! \brief Save a PEM encoded EC2N private key
//! \param bt the destination BufferedTransformation
//! \param key the EC2N private key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_PrivateKey_EC<EC2N>& key);

//! \brief Save a PEM encoded EC2N private key
//! \param bt the destination BufferedTransformation
//! \param rng a RandomNumberGenerator to produce an initialization vector
//! \param key the EC2N private key
//! \param algorithm the encryption algorithm
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \details The algorithm should be a value like <tt>AES-128-CBC</tt>. See <tt>pem-rd.cpp</tt> and
//!   <tt>pem-wr.cpp</tt> for the values that are recognized.
//! \details The "named curve" (i.e., the OID of secp256k1) is used rather than the domain parameters.
//!   This is because RFC 5915 specifies the format. In addition, OpenSSL cannot load and utilize an
//!   EC key with a non-named curve into a server.
//! \throws Exception on failure
void PEM_Save(RandomNumberGenerator& rng,  BufferedTransformation& bt, const DL_PrivateKey_EC<EC2N>& key,
              const std::string& algorithm, const char* password, size_t length);

//! \brief Save a PEM encoded ECDSA private key
//! \param bt the destination BufferedTransformation
//! \param key the ECDSA private key
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_Keys_ECDSA<ECP>::PrivateKey& key);

//! \brief Save a PEM encoded ECDSA private key
//! \param bt the destination BufferedTransformation
//! \param rng a RandomNumberGenerator to produce an initialization vector
//! \param key the ECDSA private key
//! \param algorithm the encryption algorithm
//! \param password pointer to the password buffer
//! \param length the size of the password buffer
//! \details The algorithm should be a value like <tt>AES-128-CBC</tt>. See <tt>pem-rd.cpp</tt> and
//!   <tt>pem-wr.cpp</tt> for the values that are recognized.
//! \details The "named curve" (i.e., the OID of secp256k1) is used rather than the domain parameters.
//!   This is because RFC 5915 specifies the format. In addition, OpenSSL cannot load and utilize an
//!   EC key with a non-named curve into a server.
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, RandomNumberGenerator& rng, const DL_Keys_ECDSA<ECP>::PrivateKey& key,
              const std::string& algorithm, const char* password, size_t length);

//! \brief Save a PEM encoded DSA group parameters
//! \param bt the destination BufferedTransformation
//! \param params the DSA group parameters
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_GroupParameters_DSA& params);

//! \brief Save a PEM encoded ECP group parameters
//! \param bt the destination BufferedTransformation
//! \param params the ECP group parameters
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_GroupParameters_EC<ECP>& params);

//! \brief Save a PEM encoded EC2N group parameters
//! \param bt the destination BufferedTransformation
//! \param params the EC2N group parameters
//! \throws Exception on failure
void PEM_Save(BufferedTransformation& bt, const DL_GroupParameters_EC<EC2N>& params);

//! \brief Save a PEM encoded Diffie-Hellman parameters
//! \param bt the destination BufferedTransformation
//! \param p the prime modulus
//! \param g the group generator
//! \throws Exception on failure
void PEM_DH_Save(BufferedTransformation& bt, const Integer& p, const Integer& g);

//! \brief Save a PEM encoded Diffie-Hellman parameters
//! \param bt the destination BufferedTransformation
//! \param p the prime modulus
//! \param q the subgroup order
//! \param g the group generator
//! \throws Exception on failure
void PEM_DH_Save(BufferedTransformation& bt, const Integer& p, const Integer& q, const Integer& g);

NAMESPACE_END

#endif
