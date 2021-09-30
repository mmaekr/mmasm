#include "arm.h"
#include "parse.h"
#include "pass.h"
#include "symtab.h"

static struct pass_ctx* initialize_pass_ctx(FILE* out_fd) {
	struct pass_ctx* ctx = malloc(sizeof(struct pass_ctx));
	ctx->pass = FIRST_PASS;
	ctx->symbol_table = symtab_create();
	ctx->address = 0;
	ctx->line_number = 0;
	ctx->out_fd = out_fd;
	return ctx;
}

static void destroy_pass_ctx(struct pass_ctx* ctx) {
	symtab_destroy(ctx->symbol_table);
	free(ctx);
	ctx = NULL;
}

static int do_pass(FILE* fd, struct pass_ctx* ctx) {
	char line[256];
	while (fgets(line, sizeof(line), fd)) {
		if (parse_line(line, ctx) != 0)
			return 1;
	}
	return 0;
}

int handle_file(const char* input_file, const char* out_file) {
	FILE* fd = fopen(input_file, "r");
	if (fd == NULL) {
		printf("Error opening input file!\n");
		return 1;
	}

	FILE* ofd = fopen(out_file, "wb");
	if (ofd == NULL) {
		printf("Error opening output file!\n");
		return 1;
	}

	// I am lazy so I made a two pass assembler, maybe next time I will do one pass
	struct pass_ctx* ctx = initialize_pass_ctx(ofd);

	// Pass 1
	if (do_pass(fd, ctx) != 0) {
		destroy_pass_ctx(ctx);
		fclose(fd);
		fclose(ofd);
		return 1;
	}

	// Reset file back to start for pass 2
	if (fseek(fd, 0, SEEK_SET) != 0) {
		printf("Failed to reset file!\n");
		return 1;
	}
	ctx->pass = SECOND_PASS;
	ctx->line_number = 0;

#if DEBUG
	if (symtab_entry_count(ctx->symbol_table)) {
		printf("Symbol Table:\n");
		symtab_print(ctx->symbol_table);
	}
#endif

	// Pass 2
	if (do_pass(fd, ctx) != 0) {
		destroy_pass_ctx(ctx);
		fclose(fd);
		fclose(ofd);
		return 1;
	}

	destroy_pass_ctx(ctx);
	fclose(fd);
	fclose(ofd);
	return 0;
}
