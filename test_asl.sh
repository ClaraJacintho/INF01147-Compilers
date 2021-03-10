for i in $(seq 1 67) 
do  
    echo "$i\n";
    sh make_tree.sh "E2/asl_($i)";
done