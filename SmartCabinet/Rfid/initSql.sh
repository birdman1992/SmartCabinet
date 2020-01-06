#!/bin/bash

while read line
do
	cmd=$(echo $line | sed -e "s/\([0-9 A-Z]*\) \([0-9 A-Z]*\)/INSERT INTO EpcInfo(epc_code,goods_code, goods_id) VALUES('\1','\2','TEST-MARK');/")
	echo $cmd >> init.sql
	sqlite3 /home/config/Cabinet.sql "$cmd"
done < data.txt
