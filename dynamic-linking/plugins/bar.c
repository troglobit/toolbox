/* Test of plugin to dynamic-linking daemon.
 *
 * Copyright (c) 2012  Joachim Nilsson <troglobit@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdio.h>
#include "plugin.h"

static void service(void *arg __unused)
{
	printf("Service %s\n", __FILE__);
}

static void hook(void *arg)
{
	char *msg = arg;

	printf("Hook %s: %s\n", __FILE__, msg);
}

static plugin_t plugin = {
        .svc = {
		.cb = service
        },
	.hook[HOOK_POST_LOAD] = {
		.cb  = hook,
		.arg = "hooked"
	},
	.hook[HOOK_PRE_RUNLOOP] = {
		.cb  = hook,
		.arg = "Before runloop"
	}
};

static void init_plugin(void)
{
	plugin_register(&plugin);
}

static void exit_plugin(void)
{
	plugin_unregister(&plugin);
}

PLUGIN_INIT(init_plugin)
PLUGIN_EXIT(exit_plugin)

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
