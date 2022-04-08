#!/bin/sh
# Verifies the bridge_binding property of VLAN interfaces on top of a
# Linux bridge with vlan_filtering enabled.
#
# Bridge binding ensures link status of VLAN interfaces on top of of
# bridge track only the bridge ports in the same VLAN.
#
# Test in an unshare like this:
#
#        unshare -mrun
#        ./brbind.sh

cleanup()
{
	for iface in br0 h1 h2; do
		ip link del $iface 2>/dev/null
	done
}

check_iface()
{
	sleep 1
	state=$(ip -br link show dev $1 | awk '{print $2;}')
	if [ "$state" != "$2" ]; then
		echo "FAIL - $3"
		exit 1
	fi
}

cleanup

ip link add p1 type veth peer h1
ip link add p2 type veth peer h2

ip link add br0 type bridge vlan_filtering 1 vlan_default_pvid 0
ip link set p1 master br0
ip link set p2 master br0

# Move ports to VLANS
bridge vlan add dev p1 vid 1 pvid untagged
bridge vlan add dev p2 vid 2 pvid untagged

# Bridge tagged in all VLANs
bridge vlan add dev br0 vid 1 self
bridge vlan add dev br0 vid 2 self

# Use VLAN interfaces on top of bridge instead
# Note: must enable bridge_binding when creating interface!
ip link add link br0 name vlan1 type vlan id 1 bridge_binding on
ip link add link br0 name vlan2 type vlan id 2 bridge_binding on

for iface in p1 h1 p2 h2 br0 vlan1 vlan2; do
	ip link set $iface up
done

# Enabling bridge_binding after-the-fact does not work!
# The kernel reports <REORDER_HDR,BRIDGE_BINDING> when
# checking status with `ip -d link show`, but VLAN link
# propagation does not work and the interfaces remain
# UP regardless of external stimuli.
#ip link set vlan1 type vlan bridge_binding on
#ip link set vlan2 type vlan bridge_binding on

check_iface vlan1 UP "Not even initial bringup works."

ip link set h1 down
check_iface vlan1 LOWERLAYERDOWN "Bridge VLAN binding does not work."

echo "PASS"
exit 0
