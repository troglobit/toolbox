/* VIF cleaner for Linux 5.4 */
#include <err.h>
#include <sys/socket.h>
//#include <linux/mroute.h>
#include <netinet/in.h>
#include "mroute.h"



/*
 * Delete a virtual interface in the kernel.
 */
void k_del_vif(socket, vifi)
    int socket;
    vifi_t vifi;
{
	struct vifctl vc = { 0 };
	int rc;

	/*
	 * Unfortunately Linux MRT_DEL_VIF API differs a bit from the *BSD one.  It
	 * expects to receive a pointer to struct vifctl that corresponds to the VIF
	 * we're going to delete.  *BSD systems on the other hand exepect only the
	 * index of that VIF.
	 */
	vc.vifc_vifi = vifi;
	rc = setsockopt(socket, IPPROTO_IP, MRT_DEL_VIF, &vc, sizeof(vc));
	if (rc < 0)
		warn("setsockopt MRT_DEL_VIF on vif %d", vifi);
}

int main(void)
{
	int sd;

	sd = socket(AF_INET, SOCK_RAW, IPPROTO_IGMP);
	if (sd == -1)
		err(1, "Failed creating SOCK_RAW IGMP proto socket");


	for (int i = 0; i < 32; i++)
		k_del_vif(sd, i);

	return 0;
}
