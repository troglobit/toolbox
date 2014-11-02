#!/bin/bash
#
# Copyright (c) 2014  Joachim Nilsson <troglobit@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#
# Setup very simple testbed for mrouted/pimd/smcroute using:
#  - network namespaces
#  - veth pairs
#  - standard Linux bridge to connect several namespaces
#
# Example:
#          sudo ./setup-net-testbed.sh /path/to/mrouted --some --args
#
BRIDGE=br-test
routed=$*

function clean_up()
{
        ip netns exec ns1 ip link set tap1 netns 1
        ip netns exec ns2 ip link set tap2 netns 1

        ip link delete tap1 type veth
        ip link delete tap2 type veth

        ip netns delete ns1
        ip netns delete ns2
        
        ip link set dev $BRIDGE down
        brctl delbr $BRIDGE
	exit
}

trap clean_up SIGHUP SIGINT SIGTERM

# add the namespaces
ip netns add ns1
ip netns add ns2
# create the switch
brctl addbr $BRIDGE
brctl stp   $BRIDGE off
ip link set dev $BRIDGE up

#### PORT 1
# create a port pair
ip link add tap1 type veth peer name br-tap1
# attach one side to linuxbridge
brctl addif br-test br-tap1 
# attach the other side to namespace
ip link set tap1 netns ns1
# set the ports to up
ip netns exec ns1 ip link set dev tap1 up
ip link set dev br-tap1 up
ip netns exec ns1 ip addr add 127.0.0.1/8 dev lo
ip netns exec ns1 ip link set dev lo up
ip netns exec ns1 ip addr add 172.16.1.1/24 dev tap1

#### PORT 2
# create a port pair
ip link add tap2 type veth peer name br-tap2
# attach one side to linuxbridge
brctl addif br-test br-tap2
# attach the other side to namespace
ip link set tap2 netns ns2
# set the ports to up
ip netns exec ns2 ip link set dev tap2 up
ip link set dev br-tap2 up
ip netns exec ns2 ip addr add 127.0.0.1/8 dev lo
ip netns exec ns2 ip link set dev lo up
#ip netns exec ns2 ip addr add 172.16.1.2/24 dev tap2
ip netns exec ns2 ifconfig tap2 172.16.1.2/24

### Start test
sleep 2
ip netns exec ns1 $routed &
ip netns exec ns2 $routed &

# Wait for input
read
clean_up
