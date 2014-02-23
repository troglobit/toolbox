/* Example parsing an inadyn JSON file with Jansson
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

#include <jansson.h>

static void print_bool(json_t *json, char *key)
{
	json_t *obj;

	obj = json_object_get(json, key);
	if (!obj) return;

	printf("%s\n", json_boolean(obj) ? key : "");
}

static void print_integer(json_t *json, char *key)
{
	json_t *obj;

	obj = json_object_get(json, key);
	if (!obj) return;

	printf("%s %" JSON_INTEGER_FORMAT "\n", key, json_integer_value(obj));
}

static void print_string(json_t *json, char *key, int indent)
{
	json_t *obj;

	obj = json_object_get(json, key);
	if (!obj) return;

	if (!strcmp("type", key)) {
		indent = 0;
		key = "system";
	}

	printf("%s%s %s\n", indent ? "  ": "", key, json_string_value(obj));
}

static void print_system(json_t *sys)
{
	size_t i;
	json_t *alias;

	printf("\n");
	print_string(sys, "type", 1);
	print_string(sys, "username", 1);
	print_string(sys, "password", 1);

	alias = json_object_get(sys, "alias");
	if (!alias) return;

	printf("  alias ");
	for (i = 0; i < json_array_size(alias); i++) {
		json_t *obj = json_array_get(alias, i);

		if (obj && json_is_string(obj))
			printf("%s%s", i != 0 ? ", " : "", json_string_value(obj));
	}
	printf("\n");
}

static void print_systems(json_t *json)
{
	size_t i;
	json_t *array;

	array = json_object_get(json, "system");
	if (!array) return;

	for (i = 0; i < json_array_size(array); i++)
		print_system(json_array_get(array, i));
}

int main (int argc, char *argv[])
{
	json_t *json;
	json_error_t error;

	json = json_load_file(argv[1], 0, &error);
	if (!json) {
		fprintf(stderr, "Failed loading %s:%d pos %d %s %s\n",
			argv[1], error.line, error.column, error.source, error.text);

		return 1;
	}

	print_bool   (json, "syslog");
	print_integer(json, "period");
	print_integer(json, "startup-delay");
	print_integer(json, "forced-update");
	print_bool   (json, "wildcard");
	print_string (json, "bind", 0);
	print_systems(json);

	return 0;
}

/**
 * Local Variables:
 *  version-control: t
 *  compile-command: "LDLIBS=-ljansson make jansson && ./jansson example.json"
 *  indent-tabs-mode: t
 *  c-file-style: "linux"
 * End:
 */
