#!/bin/sh
rm -rf lost_decryption
mkdir lost_decryption
cd src
make clean
make
cd ..
cp src/cipher src/libencrypt.so src/libdecrypt.so lost_decryption
cd lost_decryption
cp ../../flag.txt .
dd if=/dev/urandom of=key.bin bs=1 count=16
LD_LIBRARY_PATH=. ./cipher encrypt key.bin flag.txt flag.enc
rm flag.txt
rm libdecrypt.so
rm -f lost_decryption.zip
cd ..
zip -r lost_decryption.zip lost_decryption
mkdir -p files
mv lost_decryption.zip files
