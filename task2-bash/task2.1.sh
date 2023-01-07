rm -r -f temp
mkdir temp

mkdir temp/guesanumber

cp ../task1-simple-program/*.c temp/guesanumber

cd temp/guesanumber

tar cfj ../../guesanumber.tar.gz *
cd ../..
rm -r -f temp
