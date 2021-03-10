./etapa3 < $1 > $1_raw_out.txt;
python3 txt2dot.py $1_raw_out.txt $1_out.dot
dot -Tpng $1_out.dot -o $1_out.png