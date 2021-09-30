#include "mmasm.h"

void usage() {
	printf("Usage: mmasm [options] filename\n");
	printf("Options:\n");
		printf("\t -h\t\t\t\tshow this help\n");
		printf("\t -o outfile\t\t\twrite output to outfile\n");
}

int main(int argc, char** argv) {
	if (argc < 2) {
		usage();
		return -1;
	}

	int err = 0;
	char* input_file = NULL;
	char* output_filename = "a.out";
	int opt = 0;
	while ((opt = getopt(argc, argv, "ho:")) != -1) {
		switch (opt) {
			case 'h': usage(); return 0;
			case 'o':
				output_filename = optarg;
#ifdef DEBUG
				printf("Output file set to %s\n", output_filename);
#endif
				break;
		};
	}

	if (optind < argc)
		input_file = argv[optind];
	else {
		printf("Missing input file!\n");
		usage();
		return -1;
	}

	initialize_instructions();
	if (handle_file(input_file, output_filename) != 0)
		err = -1;
	exit_instructions();

	return err;
}
