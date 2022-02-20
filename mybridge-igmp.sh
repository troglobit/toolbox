#!/bin/sh -x
# Sets up a test bench with a bridge and some veth-pairs as virtual
# ports.  Useful for testing bridge functions like VLAN filtering,
# IGMP snooping, blocking per port and VLAN, etc.
#
# Here we create a bridge (igmp0) with five ports, the fifth is also
# moved to a separate network namespace to simulate an external end
# device.  What we do inside it is not covered here, typically we
# however create VLAN interfaces on top of port5 to be able to send
# untagged traffic with ping, or listen with tcpdump.
#
# 192.168.1.1  vlan1       vlan2  192.168.2.1
#                   \     /                            _____________
#                    igmp0  1T,2T                     |       netns |
#                 _____|_____                         |             |
#                |_#_#_#_#_#_|                        |    ?????    |
#                  / / |  \ \                         |             |
#                p0 p1 |  p2 p3                       | port5 1T,2T |
# 1U  port0 -----'  |  |  |   '------ port3  2U       |__/__________|
# 1U  port1 --------'  |  '---------- port2  2U         /
#                      |                               /
#                      '------------------------------'
#

br=igmp0
ifa1=192.168.1.1/24
ifa2=192.168.2.1/24
ns=netns

for i in 1 2 3 4 5; do
    ip link add port$i type veth peer p$i
done

# Create a VLAN-aware bridge and add all our veth ports as members By
# default all ports, including the bridge itself, are untagged members
# in VLAN 1 (not to be confused with the vlan_filtering argument)
ip link add $br type bridge vlan_filtering 1
for i in 1 2 3 4 5; do
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

# set p5 as external trunk port, member of all VLANs
bridge vlan add vid 1 dev p5
bridge vlan add vid 2 dev p5

# move the p5 counterpart, port5, to a separate netns
ip netns add $ns
ip link set port5 netns $ns

# add VLAN interfaces on top of bridge
ip link add name vlan1 link $br type vlan id 1
ip link add name vlan2 link $br type vlan id 2

# set up basic networking, no default route
ip addr add $ifa1 dev vlan1
ip addr add $ifa2 dev vlan2

# bring everything up
for i in 1 2 3 4; do
    ip link set port$i up
    ip link set p$i up
done

for iface in $br vlan1 vlan2; do
    ip link set $iface up
done

# EOF
echo "Your bridge $br has been set up.  Enter netns $ns with: ip netns exec $ns $SHELL"
