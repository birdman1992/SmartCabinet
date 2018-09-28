#!/bin/bash
if [ ! -f "/home/config/network.ini" ];then
    exit
fi

source /home/config/network.ini

echo $ip
echo $netmask
echo $mac
echo $gateway

ETH0_STATE=`cat /sys/class/net/eth0/operstate`
ETH1_STATE=`cat /sys/class/net/eth1/operstate`

if [ -z "$device" ];then
    if [ "up" == $ETH0_STATE ];then
        device=eth0
    else
        device=eth1
    fi
fi
echo $device

ifconfig $device down
ifconfig $device hw ether $mac
ifconfig $device up

ifconfig $device $ip

ifconfig $device netmask $netmask

route add default gw $gateway

