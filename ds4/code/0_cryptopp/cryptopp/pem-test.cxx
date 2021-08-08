#include <string>
#include <iostream>

#include "integer.h"
#include "rsa.h"
#include "pem.h"
using namespace CryptoPP;

int main(int argc, char* argv[])
{
    std::string publickey_str =
	"-----BEGIN PUBLIC KEY-----\n"
	"gm6mZA1NTZQJVUk9AGDb6NRngzRlRAgXBTWAispwlqsuHFoCrv02xPm1uxkLyfUq\n"
	"LoA4/EQJ25okjmGkrjgak+XmQIPKmAg94gWAtvRIrLZNmCj/aPeuikmCPXkKtg2b\n"
	"pdB6xzHY0ftGu0l6Vb8zttg7Wfo1kJowjoqCRwo9ex/IKwPXxE3UsugshcZOGdqT\n"
	"6E3B/Vw+JoerL/LfeOU2OYcSFEXsWqjzkrGzEVuKzRnve5RlXyY0gShP33f+hDnC\n"
	"F+Uu2tFfFgxRkdQPk7AKm4MCAwEAAQ==\n"
	"-----END PUBLIC KEY-----\n";

    RSA::PublicKey publicKey;
    
    try
    {
	StringSource source(publickey_str, true);
	PEM_Load(source, publicKey);
    }
    catch(const Exception& ex)
    {
	std::cerr << ex.what() << std::endl;
	return 1;
    }

    const Integer& e = publicKey.GetPublicExponent();
    std:: cout << e << std::endl;

    const Integer& n = publicKey.GetModulus();
    std:: cout << n << std::endl;

    return 0;
}
