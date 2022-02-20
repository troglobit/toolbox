#!/bin/sh

#      cap_net_raw+ep $(which hping3) \
#     cap_net_raw+ep $(which trafgen) \

sudo setcap \
     cap_net_admin,cap_net_raw,cap_net_bind_service+ep $(which gdb-multiarch) \
     cap_net_admin,cap_net_raw,cap_net_bind_service+ep $(which dnsmasq) \
     \
     cap_net_admin,cap_net_raw+ep /usr/lib/qemu/qemu-bridge-helper \
     cap_net_admin+ep $(which qemu-system-aarch64) \
     cap_net_admin+ep $(which qemu-system-arm) \
     cap_net_admin+ep $(which qemu-system-ppc) \
     cap_net_admin+ep $(which qemu-system-ppc64) \
     cap_net_admin+ep $(which qemu-system-x86_64) \
     cap_net_admin+ep $(which bridge) \
     cap_net_admin+ep $(which brctl) \
     cap_net_admin+ep $(which ifconfig) \
     \
     cap_net_admin,cap_net_raw+ep /bin/ip \
     \
     cap_net_raw+ep $(which tcpdump) \
     cap_net_raw+ep $(which nemesis)
