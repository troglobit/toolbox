#!/bin/sh

ip link add p1 type veth peer h1
ip link add p2 type veth peer h2

ip link add br0 type bridge vlan_filtering 1
ip link set p1 master br0
ip link set p2 master br0

# Move port p2 to VLAN 2
bridge vlan add dev p2 vid 2 pvid untagged
bridge vlan del dev p2 vid 1

# Bridge tagged in all VLANs
bridge vlan add dev br0 vid 1 self
bridge vlan add dev br0 vid 2 self

# Use VLAN interfaces on top of bridge instead
ip link add link br0 name vlan1 type vlan id 1
ip link add link br0 name vlan2 type vlan id 2

for iface in p1 h1 p2 h2 br0 vlan1 vlan2; do
	ip link set $iface up
done

ip link set vlan1 type vlan bridge_binding on
ip link set vlan2 type vlan bridge_binding on

