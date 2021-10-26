
THIS_DIR=`dirname $0`


grep -B 4 '^\s*locater:\s*apps\s*$' $1  \
    | sed -n 's/^\s*name\s*\s*:\s*\(.*\)\s*$/\1/p'  \
    | bash $THIS_DIR/listmenu.sh "Product" 20 60 13

