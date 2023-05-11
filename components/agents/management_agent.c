// SPDX-License-Identifier: BSD-3-Clause OR Apache-2.0
#include "agents/management_agent.h"

#include "ud3tn/bundle_processor.h"
#include "ud3tn/common.h"
#include "ud3tn/config.h"
#include "ud3tn/eid.h"

#include "platform/hal_io.h"
#include "platform/hal_time.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct management_agent_params {
	const char *local_eid;
	bool allow_remote_configuration;
};

static void callback(struct bundle_adu data, void *param,
		     const void *bp_context)
{
	(void)bp_context;

	struct management_agent_params *const ma_param = param;

	if (!ma_param->allow_remote_configuration) {
		char *const node_id = get_node_id(data.source);

		if (!node_id || strncmp(ma_param->local_eid, node_id,
					strlen(ma_param->local_eid)) != 0) {
			LOGF("MgmtAgent: Dropped config message from foreign endpoint",
			     data.source);
			free(node_id);
			return;
		}
		free(node_id);
	}

	if (data.length < 1) {
		LOG("MgmtAgent: Received payload without a command.");
		bundle_adu_free_members(data);
		return;
	}

	switch ((enum management_command)data.payload[0]) {
	default:
		LOG("MgmtAgent: Received invalid management command.");
		break;
	case MGMT_CMD_SET_TIME:
		if (data.length == 9) {
			const uint64_t t = (
				(uint64_t)data.payload[1] << 56 |
				(uint64_t)data.payload[2] << 48 |
				(uint64_t)data.payload[3] << 40 |
				(uint64_t)data.payload[4] << 32 |
				(uint64_t)data.payload[5] << 24 |
				(uint64_t)data.payload[6] << 16 |
				(uint64_t)data.payload[7] << 8 |
				(uint64_t)data.payload[8]
			);

			hal_time_init(t);
			LOGF("MgmtAgent: Updated time to DTN ts: %llu", t);
		} else {
			LOG("MgmtAgent: Received invalid time command.");
		}
		break;
	}

	bundle_adu_free_members(data);
}

int management_agent_setup(QueueIdentifier_t bundle_processor_signaling_queue,
			   const char *local_eid,
			   bool allow_remote_configuration)
{
	struct management_agent_params *const ma_param = malloc(
		sizeof(struct management_agent_params)
	);
	ma_param->local_eid = local_eid;
	ma_param->allow_remote_configuration = allow_remote_configuration;
	const int is_ipn = get_eid_scheme(local_eid) == EID_SCHEME_IPN;

	return bundle_processor_perform_agent_action(
		bundle_processor_signaling_queue,
		BP_SIGNAL_AGENT_REGISTER,
		is_ipn ? AGENT_ID_MANAGEMENT_IPN : AGENT_ID_MANAGEMENT_DTN,
		callback,
		ma_param,
		false
	);
}
