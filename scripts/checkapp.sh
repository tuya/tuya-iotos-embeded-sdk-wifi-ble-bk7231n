#!/bin/bash

APP_NAME=$1
if [ -z $APP_NAME ];then
    APP_NAME=`bash ./scripts/get_sub_dir.sh apps`
fi

if [ -z $APP_NAME ] || [ ! -d "./apps/$APP_NAME" ];then
    echo APP_NAME is error!
    exit 1
fi
# echo APP_NAME=$APP_NAME

APP_DIR=./apps/$APP_NAME
# echo APP_DIR=$APP_DIR

APP_SRC_DIR=`find $APP_DIR -type d -name "src"`
# echo APP_SRC_DIR=$APP_SRC_DIR

APP_INC_DIR=`find $APP_DIR -type d -name "include"`
# echo APP_INC_DIR=$APP_INC_DIR

COM_SRC_DIR=
COM_INC_DIR=
if [ -d components ];then
    COM_SRC_DIR=`find ./components -type d -name "src"`
    COM_INC_DIR=`find ./components -type d -name "include"`
fi
# echo COM_SRC_DIR=$COM_SRC_DIR
# echo COM_INC_DIR=$COM_INC_DIR

CHECK_SRC_DIR="$APP_SRC_DIR $COM_SRC_DIR"
# echo CHECK_SRC_DIR=$CHECK_SRC_DIR
CHECK_INC_DIR="$APP_INC_DIR $COM_INC_DIR"
# echo CHECK_INC_DIR=$CHECK_INC_DIR

CHECK_INC_I=""
for i in $CHECK_INC_DIR;do
CHECK_INC_I+="-I$i "
done
# echo CHECK_INC_I=$CHECK_INC_I

# cppcheck时需要定义的宏 -D开头
CPPCHECK_D=-Dbool_t=int
# cppcheck时不需要定义的宏 -U开头
CPPCHECK_U=-U__cplusplus

# 有error级别结果的会报错
echo "====== run cppcheck begin ======="
cppcheck --enable=warning,style,missingInclude \
$CPPCHECK_D $CPPCHECK_U  --error-exitcode=99 \
$CHECK_INC_I \
$CHECK_SRC_DIR
echo "------ run cppcheck end ---------"
