#!/bin/sh -x
# Sets up a complete network world inside a network namespace

ns=world

ip netns add $ns
for i in 1 2 3 4; do
    ip netns exec $ns ip link add eth$i type veth peer p$i
done

ip netns exec $ns ip link add br0 type bridge vlan_filtering 1
for i in 1 2 3 4; do
    ip netns exec $ns ip link set p$i master br0
done

# move half the ports to VLAN 2
for i in 3 4; do
    ip netns exec $ns bridge vlan add vid 2 dev p$i pvid untagged
    ip netns exec $ns bridge vlan del vid 1 dev p$i
done

# set bridge itself as tagged member in both VLANs
ip netns exec $ns bridge vlan add vid 1 dev br0 self
ip netns exec $ns bridge vlan add vid 2 dev br0 self

# add VLAN interfaces on top of bridge
ip netns exec $ns ip link add name vlan1 link br0 type vlan id 1
ip netns exec $ns ip link add name vlan2 link br0 type vlan id 2

ip netns exec $ns ip addr add 192.168.1.1/24 dev vlan1
ip netns exec $ns ip addr add 192.168.2.1/24 dev vlan2

# bring everything up
for i in 1 2 3 4; do
    ip netns exec $ns ip link set eth$i up
    ip netns exec $ns ip link set p$i up
done

for iface in br0 vlan1 vlan2; do
    ip netns exec $ns ip link set $iface up
done

# enter namespace
ip netns exec $ns $SHELL

ip netns del $ns
