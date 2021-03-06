#include "skynet.h"

#include <stdio.h>
#include <stdlib.h>

struct logger {
	FILE * handle;
	int close;
};

struct logger *
logger_create(void) {
	struct logger * inst = malloc(sizeof(*inst));
	inst->handle = NULL;
	inst->close = 0;
	return inst;
}

void
logger_release(struct logger * inst) {
	if (inst->close) {
		fclose(inst->handle);
	}
	free(inst);
}

static void
_logger(struct skynet_context * context, void *ud, int session, const char * uid, const void * msg, size_t sz) {
	struct logger * inst = ud;
	fprintf(inst->handle, "[%s] ",uid);
	fwrite(msg, sz , 1, inst->handle);
	fprintf(inst->handle, "\n");
}

int
logger_init(struct logger * inst, struct skynet_context *ctx, const char * parm) {
	if (parm) {
		inst->handle = fopen(parm,"w");
		if (inst->handle == NULL) {
			return 1;
		}
		inst->close = 1;
	} else {
		inst->handle = stdout;
	}
	if (inst->handle) {
		skynet_callback(ctx, inst, _logger);
		skynet_command(ctx, "REG", ".logger");
		return 0;
	}
	return 1;
}
