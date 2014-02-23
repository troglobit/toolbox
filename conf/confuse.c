/* Example parsing an inadyn configuration file with libConfuse
 *
 * Copyright (c) 2014  Joachim Nilsson <troglobit@gmail.com>
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

#include <errno.h>
#include <string.h>
#include <confuse.h>

static void print_bool(cfg_t *cfg, char *key)
{
	if (cfg_getbool(cfg, key))
		printf("%s\n", key);
}

static void print_integer(cfg_t *cfg, char *key)
{
	printf("%s %ld\n", key, cfg_getint(cfg, key));
}

static void print_string(cfg_t *cfg, char *key, int indent)
{
	printf("%s%s %s\n", indent ? "  ": "", key, cfg_getstr(cfg, key));
}

static void print_system(cfg_t *provider)
{
	size_t i;

	printf("\n");
	printf("system %s\n", cfg_title(provider));
	print_string(provider, "username", 1);
	print_string(provider, "password", 1);

	if (cfg_size(provider, "alias")) {
		printf("  alias ");

		for (i = 0; i < cfg_size(provider, "alias"); i++)
			printf("%s%s", i != 0 ? ", " : "",
			       cfg_getnstr(provider, "alias", i));

		printf("\n");
	}
}

static void print_systems(cfg_t *cfg)
{
	size_t i;

	for (i = 0; i < cfg_size(cfg, "provider"); i++)
		print_system(cfg_getnsec(cfg, "provider", i));
}

cfg_t *parse_conf(char *conf)
{
	cfg_opt_t provider_opts[] = {
		CFG_STR     ("username",  0, CFGF_NONE),
		CFG_STR     ("password",  0, CFGF_NONE),
		CFG_STR_LIST("alias",     0, CFGF_NONE),
		CFG_END()
	};
	cfg_opt_t opts[] = {
		CFG_BOOL("syslog",	  cfg_false, CFGF_NONE),
		CFG_BOOL("wildcard",	  cfg_false, CFGF_NONE),
		CFG_STR ("bind",	  0, CFGF_NONE),
		CFG_INT ("period",	  60, CFGF_NONE),
		CFG_INT ("startup-delay", 0, CFGF_NONE),
		CFG_INT ("forced-update", 720000, CFGF_NONE),
		CFG_SEC ("provider", provider_opts, CFGF_MULTI | CFGF_TITLE),
		CFG_END()
	};
	cfg_t *cfg = cfg_init(opts, CFGF_NONE);

	switch (cfg_parse(cfg, conf)) {
	case CFG_FILE_ERROR:
		fprintf(stderr, "Cannot read configuration file %s: %s\n", conf, strerror(errno));

	case CFG_PARSE_ERROR:
		return NULL;

	case CFG_SUCCESS:
		break;
	}

    return cfg;
}

int main (int argc, char *argv[])
{
	cfg_t *cfg;

	cfg = parse_conf(argc > 1 ? argv[1] : "/etc/example.conf");
	if (!cfg)
		return 1;

	print_bool   (cfg, "syslog");
	print_integer(cfg, "period");
	print_integer(cfg, "startup-delay");
	print_integer(cfg, "forced-update");
	print_bool   (cfg, "wildcard");
	print_string (cfg, "bind", 0);
	print_systems(cfg);

	cfg_free(cfg);

	return 0;
}

/**
 * Local Variables:
 *  compile-command: "LDLIBS=-lconfuse make confuse && ./confuse example.conf"
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
