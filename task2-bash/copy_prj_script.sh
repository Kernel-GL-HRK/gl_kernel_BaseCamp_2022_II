#! /bin/bash

Sourse="../task1-simple-program"
Folder="/tmp/guesanumber"

Folder_release="./release"
Archive=$Folder.tar.gz

check_command(){
if [ $? -eq 0 ]
then
    echo -e "\033[32m SUCCESS \033[0m"
else
    echo -e "\033[31m FAIL \033[0m"
fi

}

echo "Creating a folder $Folder . . ."
mkdir $Folder

check_command

echo "Copying the sourse to the folderp . . ."

cp -r $Sourse $Folder

check_command

echo "Creating a folder $Folder_release . . ."

mkdir $Folder_release

check_command

echo "Creating an archive . . ."

tar czfP $Archive $Folder

cp $Archive $Folder_release/

check_command