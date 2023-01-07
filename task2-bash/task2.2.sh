name=Vanga
C=clang
counter=1
reader=x
re='^[0-9]+$'

$C ../task1-simple-program/*.c -o $name


while [ $counter -gt 0 ] 
do
	let counter--
	
	./$name
	
	
	if [ $? -eq 0 ]
	then
		echo -e "\e[32mGood job\e[0m"
	else
		echo -e "\e[31mWish a good luck next time\e[0m"
	fi
	
	if [ $counter -gt 0 ]
	then
		continue
	fi
	
	
	while :
	do
		read -p "Press \"y\" for continue, \"n\" for exit, or number of repetitions without asking: " reader
	! [[ $reader =~ $re || $reader == Y || $reader == y || $reader == N || $reader == n ]] || break
	done
	
	if [[ $reader =~ $re ]]
	then
		let counter=$reader
	elif  [[ $reader == Y || $reader == y ]]
	then
		let counter=1
	fi

done


rm -f $name

