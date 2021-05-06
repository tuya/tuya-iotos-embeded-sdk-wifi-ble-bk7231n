#!/bin/sh
set -v
find . -name *.h | xargs sed -i 's/\<ke_/kernel_/g'
find . -name *.c | xargs sed -i 's/\<ke_/kernel_/g'
find . -name *.h | xargs sed -i 's/_ke_/_kernel_/g'
find . -name *.c | xargs sed -i 's/_ke_/_kernel_/g'
find . -name *.h | xargs sed -i 's/\<KE_/KERNEL_/g'
find . -name *.c | xargs sed -i 's/\<KE_/KERNEL_/g'
find . -name *.h | xargs sed -i 's/_KE_/_KERNEL_/g'
find . -name *.c | xargs sed -i 's/_KE_/_KERNEL_/g'
find . -name *.h | xargs sed -i 's/\<co_/common_/g'
find . -name *.c | xargs sed -i 's/\<co_/common_/g'
find . -name *.h | xargs sed -i 's/_co_/_common_/g'
find . -name *.c | xargs sed -i 's/_co_/_common_/g'
find . -name *.h | xargs sed -i 's/\<CO_/COMMON_/g'
find . -name *.c | xargs sed -i 's/\<CO_/COMMON_/g'
find . -name *.h | xargs sed -i 's/_CO_/_COMMON_/g'
find . -name *.c | xargs sed -i 's/_CO_/_COMMON_/g'
find . -name *.h | xargs sed -i 's/GLOBAL_INT_DISABLE/GLOBAL_INT_DIS/g'
find . -name *.c | xargs sed -i 's/GLOBAL_INT_DISABLE/GLOBAL_INT_DIS/g'
find . -name *.h | xargs sed -i 's/GLOBAL_INT_RESTORE/GLOBAL_INT_RES/g'
find . -name *.c | xargs sed -i 's/GLOBAL_INT_RESTORE/GLOBAL_INT_RES/g'
find . -name *.h | xargs sed -i 's/\<llc\.h/llctrl\.h/g'
find . -name *.c | xargs sed -i 's/\<llc\.h/llctrl\.h/g'
find . -name *.h | xargs sed -i 's/\<arch\.h/architect\.h/g'
find . -name *.c | xargs sed -i 's/\<arch\.h/architect\.h/g'
find . -name *.h | xargs sed -i 's/\<ASSERT_/BLE_ASSERT_/g'
find . -name *.c | xargs sed -i 's/\<ASSERT_/BLE_ASSERT_/g'
find . -name *.h | xargs sed -i 's/\<ll\.h/lowlevel\.h/g'
find . -name *.c | xargs sed -i 's/\<ll\.h/lowlevel\.h/g'
find . -name *.h | xargs sed -i 's/\(\<reg_.*\.h\>\)/ble_\1/g'
find . -name *.c | xargs sed -i 's/\(\<reg_.*\.h\>\)/ble_\1/g'
find . -name *.h | xargs sed -i 's/ke\.h/kernel\.h/g'
find . -name *.c | xargs sed -i 's/ke\.h/kernel\.h/g'
find . -name *.h | xargs sed -i 's/\<aes_/ble_aes_/g'
find . -name *.c | xargs sed -i 's/\<aes_/ble_aes_/g'
find . -name *.h | xargs sed -i 's/\<TASK_/TASK_BLE_/g'
find . -name *.c | xargs sed -i 's/\<TASK_/TASK_BLE_/g'

for f in `find . -name ke_*`
do
t=`echo $f | sed 's/ke_/kernel_/g'`
mv $f $t
done

for f in `find . -name co_*`
do
t=`echo $f | sed 's/co_/common_/g'`
mv $f $t
done

for f in `find . -name reg_*`
do
t=`echo $f | sed 's/reg_/ble_reg_/g'`
mv $f $t
done

for f in `find . -name aes_*`
do
t=`echo $f | sed 's/aes_/ble_aes_/g'`
mv $f $t
done
