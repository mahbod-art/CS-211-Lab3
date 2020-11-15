declare -a method_names=("0" "1" "2" "3")
method_nums=${#method_names[@]}

declare -a node_nums=("32" "64" "128" "256")
n_node=${#node_nums[@]}

output_name="final.txt"

cd ../result

echo "" &> ${output_name}

for (( i=1; i<${method_nums}+1; i++ ));
do
	method_name="sieve${method_names[$i-1]}";
	echo "${method_name}: " >> ${output_name} 
#	echo "${method_name}:"
	for (( j=1; j<${n_node}+1; j++ ));
	do
		file_name="${method_name}_np_${node_nums[$j-1]}_output.txt"
#		echo ${file_name}
		cat ${file_name} >> ${output_name}
	done
done

mv sieve*.txt tmp/

