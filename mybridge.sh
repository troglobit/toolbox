#!/bin/sh -x
# Sets up a test bench with a bridge and some veth-pairs as virtual
# ports.  Useful for testing bridge functions like VLAN filtering,
# IGMP snooping, blocking per port and VLAN, etc.
#
# Setup:
#       192.168.1.1  vlan1    ¦  vlan2  192.168.2.1
#                         \   ¦  /
#                          bench0
#                        _____¦_____     
#                       |     ¦     | 
#                       |_#_#_¦#__#_|
#                         / / ¦ \ \
#                       p0 p1 ¦ p2 p3
#            port0 -----'  |  ¦ |   '------ port3
#            port1 --------'  ¦ '---------- port2
#                             ¦
#       VLAN 1]_______________¦________________[VLAN 2
#                             ¦
# NOTE: to avoid name clashes with existing interfaces, either run
#       this script inside an empty network namespace, or change
#       the names of the interfaces this script creates.
#

br=bench0

for i in 1 2 3 4; do
    ip link add port$i type veth peer p$i
done

# Create a VLAN-aware bridge and add all our veth ports as members By
# default all ports, including the bridge itself, are untagged members
# in VLAN 1 (not to be confused with the vlan_filtering argument)
ip link add $br type bridge vlan_filtering 1
for i in 1 2 3 4; do
    ip link set p$i master $br
done

# move half the ports to VLAN 2
for i in 3 4; do
    bridge vlan add vid 2 dev p$i pvid untagged
    bridge vlan del vid 1 dev p$i
done

# set bridge itself as tagged member in both VLANs
# required to be able to create the VLAN interfaces
bridge vlan add vid 1 dev $br self
bridge vlan add vid 2 dev $br self

# add VLAN interfaces on top of bridge
ip link add name vlan1 link $br type vlan id 1
ip link add name vlan2 link $br type vlan id 2

# set up basic networking, no default route
ip addr add 192.168.1.1/24 dev vlan1
ip addr add 192.168.2.1/24 dev vlan2

# bring everything up
for i in 1 2 3 4; do
    ip link set port$i up
    ip link set p$i up
done

for iface in $br vlan1 vlan2; do
    ip link set $iface up
done
