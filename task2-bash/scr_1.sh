#!/usr/bin/bash
TMP_DIR="/tmp/guesanumber"
RLS_DIR="../release"
SRC_DIR="../task1-simple-program"


#Check if TMP_DIR exist. If no - create it
if [[ ! -d ${TMP_DIR} ]]; then
	mkdir -p ${TMP_DIR}
fi

#Copy all .c an .h files  from SRC_DIR to TMP_DIR
cp "${SRC_DIR}/"*.c "${SRC_DIR}/"*.h -t ${TMP_DIR}

#Copy all .c an .h files  from SRC_DIR to TMP_DIR
cp "${SRC_DIR}/"*.c "${SRC_DIR}/"*.h -t ${TMP_DIR}

#Arcive copied files in TMP_DIR by tar
#cd to TMP_DIR is used to avoid adding the full path in .tar 
cd ${TMP_DIR}
tar -cvf "../$(basename ${TMP_DIR}).tar" "../$(basename ${TMP_DIR})" 
tar -czvf "../$(basename ${TMP_DIR}).tar.gz" "../$(basename ${TMP_DIR})"
cd $OLDPWD
