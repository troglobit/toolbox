#!/bin/sh
# Read out SNMP entries

IP_ADR=$1
while [ 1 ]
do
    #echo
    #echo "==================================================================="
    #date
    #echo "==================================================================="
    #break_nr=$((break_nr+1))
    #echo
    #echo $break_nr "Read out SNMP Status from $IP_ADR"

    # sysUpTime - "1.3.6.1.2.1.1.3"
    snmpwalk  -c public -v2c $IP_ADR sysUpTime &
#    snmpget -c public -v2c $IP_ADR sysUpTime
    # sysName  -  "1.3.6.1.2.1.1.5"
    snmpwalk  -c public -v2c $IP_ADR sysName &

    # sysServices - "1.3.6.1.2.1.1.7"  - sysHealth, sysAlert
    snmpwalk  -c public -v2c $IP_ADR sysServices &

    # Read - SHDSL_SnrPort1, 2
    snmpwalk  -c public -v2c $IP_ADR 1.3.6.1.2.1.10.48.1.5 &

    # Read Port status from IF-MIB
    # snmpbulkwalk -v2c -Os -c public 10.10.22.11 ifType
    snmpwalk  -c public -v2c $IP_ADR ifType &
    snmpwalk  -c public -v2c $IP_ADR ifSpeed &
    snmpwalk  -c public -v2c $IP_ADR ifAdminStatus &
    snmpwalk  -c public -v2c $IP_ADR ifOperStatus &
    snmpwalk  -c public -v2c $IP_ADR ifindex &
    snmpwalk  -c public -v2c $IP_ADR ifDescr &


    for job in `jobs -p`; do
	wait $job
    done
    sleep 1
done
