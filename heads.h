#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <math.h>
#include <map>
#include <vector>
#include <algorithm>

#define MAX_STRING 1000
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40
#define MAX_DOC_PER_TAG 100
#define MIN_WORDS 3
#define SQR(x) ((x)*(x))

#define check printf("here is ok\n")
#define debug(...) fprintf(stderr, __VA_ARGS__)

using namespace std;

typedef float real;                    // Precision of float numbers
typedef char Char;
typedef pair<unsigned, unsigned> Upair;
typedef map<unsigned, unsigned> Umap;
typedef real* Embeds;
typedef vector<real *> AllEmbeds;


char train_file[MAX_STRING], output_file[MAX_STRING];
char save_vocab_file[MAX_STRING], read_vocab_file[MAX_STRING];

int vocab_hash_size = 3000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary
int dict_hash_size = 10000;
int binary = 0, cbow = 1, debug_mode = 2, window = 5, min_count = 5, num_threads = 12, min_reduce = 1;

long long max_w = 20;
long long vocab_max_size = 1000, vocab_size = 0, layer1_size = 100;
long long train_words = 0, word_count_actual = 0, iter = 5, file_size = 0, classes = 0;
real alpha = 0.025, starting_alpha, sample = 1e-3;
real *syn0, *syn1, *syn1neg, *expTable;

int hs = 0, negative = 5;
const int table_size = 1e8;
int *table;

void ReadWord(Char *word, FILE *f) {
	int a = 0;
	while (1) {
		word[a] = fgetc(f);
		if (!isalpha(word[a])) break;
		if (isupper(word[a])) word[a] = word[a] - 'A' + 'a';
		if ((a < max_w) && (word[a] != '\n')) a++;
	}
	word[a] = 0;
}
