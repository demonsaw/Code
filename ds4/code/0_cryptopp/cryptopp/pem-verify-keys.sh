#! /bin/sh

# Script to verify the test keys written by pem-test.cpp

#################
# RSA keys

# The RSA command returns 0 on success

echo "read RSA key"
openssl rsa -in rsa-pub-xxx.pem -pubin -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read RSA public key"
fi

echo "read RSA key"
openssl rsa -in rsa-priv-xxx.pem -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read RSA private key"
fi

echo "read RSA key"
openssl rsa -in rsa-enc-priv-xxx.pem -passin pass:test -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read encrypted RSA private key"
fi

#################
# DSA keys

# The DSA command is broken. It returns 1 when using '-noout' option instead of 0.

echo "read DSA param"
openssl dsaparam -in dsa-params-xxx.pem -text -noout >/dev/null

openssl dsa -in dsa-pub-xxx.pem -pubin -text -noout >/dev/null

openssl dsa -in dsa-priv-xxx.pem -text -noout >/dev/null

openssl dsa -in dsa-enc-priv-xxx.pem -passin pass:test -text -noout >/dev/null

#################
# EC keys

# The EC command returns 0 on success

echo "read EC param"
openssl ecparam -in ec-params-xxx.pem -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read EC params"
fi

openssl ec -in ec-pub-xxx.pem -pubin -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read EC public key"
fi

openssl ec -in ec-priv-xxx.pem -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read EC private key"
fi

openssl ec -in ec-enc-priv-xxx.pem -passin pass:test -text -noout >/dev/null
RET=$?
if [ $RET -ne 0 ];then
echo "Failed to read encrypted EC private key"
fi

echo "Finished testing keys written by Crypto++"
