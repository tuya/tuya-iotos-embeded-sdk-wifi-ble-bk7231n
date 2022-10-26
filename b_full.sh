#!/bin/bash
./build_app.sh apps/OpenBK7231N_App OpenBK7231N_App 1.0.7 clean
./build_app.sh apps/OpenBK7231N_App OpenBK7231N_App 1.0.7
cp -f apps/OpenBK7231N_App/output/1.0.7/OpenBK7231N_App_1.0.7.rbl /var/www/OpenBK7231N_App_1.0.7.rbl

