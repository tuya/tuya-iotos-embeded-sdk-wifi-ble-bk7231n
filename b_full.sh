#!/bin/bash
./build_app.sh apps/OpenBK7231N_App OpenBK7231N_App 1.0.8 clean
./build_app.sh apps/OpenBK7231N_App OpenBK7231N_App 1.0.8
cp -f apps/OpenBK7231N_App/output/1.0.8/OpenBK7231N_App_1.0.8.rbl /var/www/OpenBK7231N_App_1.0.8.rbl

