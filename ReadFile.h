int read_file(char *in_path, char *out_path) {
    FILE *f;
    f = fopen(out_path,"a");
	Char word[MAX_STRING];
	FILE *fin = fopen(in_path, "r");
	if (fin == NULL) return 0;
	while (1) {
		if (fgetc(fin) == '\n'){
			char ch = fgetc(fin);
			if (ch == '<' || ch == '\n') break;
		}
	}
	while (1) {
			if (feof(fin)) break;
			ReadWord(word, fin);
			if (strlen(word) < MIN_WORDS) continue;
			fprintf(f, "%s ", word);
	}


	fclose(fin);
    fclose(f);
    return 0;
}
