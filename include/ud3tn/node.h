// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include "ud3tn/bundle.h"
#include "ud3tn/result.h"

#include <stdint.h>

struct routed_bundle_list {
	struct bundle *data;
	struct routed_bundle_list *next;
};

struct contact {
	struct node *node;
	uint64_t from_ms;
	uint64_t to_ms;
	uint32_t bitrate_bytes_per_s;
	uint32_t total_capacity_bytes;
	int32_t remaining_capacity_p0;
	int32_t remaining_capacity_p1;
	int32_t remaining_capacity_p2;
	struct endpoint_list *contact_endpoints;
	struct routed_bundle_list *contact_bundles;
	int8_t active;
};

struct contact_list {
	struct contact *data;
	struct contact_list *next;
};

enum node_flags {
	NODE_FLAG_NONE = 0,
	NODE_FLAG_INTERNET_ACCESS = 0x1
};

struct node {
	char *eid;
	char *cla_addr;
	enum node_flags flags;
	struct endpoint_list *endpoints;
	struct contact_list *contacts;
};

struct node_list {
	struct node *node;
	struct node_list *next;
};

#define CONTACT_CAPACITY(contact, p) \
	(p == 1 ? contact->remaining_capacity_p1 : \
	(p == 2 ? contact->remaining_capacity_p2 : \
	contact->remaining_capacity_p0))

struct node *node_create(char *eid);
struct contact *contact_create(struct node *node);

void free_contact(struct contact *contact);
void free_node(struct node *node);

struct endpoint_list *endpoint_list_free(struct endpoint_list *e);

struct endpoint_list *endpoint_list_union(
	struct endpoint_list *a, struct endpoint_list *b);
struct endpoint_list *endpoint_list_difference(
	struct endpoint_list *a, struct endpoint_list *b, const int free_b);

int contact_list_sorted(struct contact_list *cl, const int order_by_from);
struct contact_list *contact_list_free(struct contact_list *e);
struct contact_list *contact_list_union(
	struct contact_list *a, struct contact_list *b,
	struct contact_list **modified);
struct contact_list *contact_list_difference(
	struct contact_list *a, struct contact_list *b,
	struct contact_list **modified, struct contact_list **deleted);

struct endpoint_list *endpoint_list_strip_and_sort(struct endpoint_list *el);
int node_prepare_and_verify(struct node *node, uint64_t min_end_time_s);
void recalculate_contact_capacity(struct contact *contact);
int32_t contact_get_remaining_capacity_bytes(
	struct contact *contact, enum bundle_routing_priority prio,
	uint64_t time_ms);
int32_t contact_get_cur_remaining_capacity_bytes(
	struct contact *contact, enum bundle_routing_priority prio);
int add_contact_to_ordered_list(
	struct contact_list **list, struct contact *contact,
	const int order_by_from);
int remove_contact_from_list(
	struct contact_list **list, struct contact *contact);

#endif // NODE_H_INCLUDED
