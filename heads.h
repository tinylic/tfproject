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
#include <pthread.h>

#define MAX_THREADS 1
#define DIS_INF 100
#define MAX_STRING 1000
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
#define MAX_DOC_LENGTH 100
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40
#define MAX_DOC_PER_TAG 100
#define MIN_WORDS 3
#define MAX_TAGS 20
#define MAX_KEY_WORDS 1000
#define QUERY_DOC 500

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

int vocab_hash_size = 30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary
int dict_hash_size = 100000;
int binary = 0, cbow = 1, debug_mode = 2, window = 5, min_count = 5, min_reduce = 1;

long long max_w = 50;
long long vocab_max_size = 1000, vocab_size = 0, layer1_size = 100;
long long train_words = 0, word_count_actual = 0, iter = 5, file_size = 0, classes = 0;
real alpha = 0.025, starting_alpha, sample = 1e-3;
real *syn0, *syn1, *syn1neg, *expTable;
real **cost;

char totalinput[] = "input.txt";


int hs = 0, negative = 5;
const int table_size = 1e8;
int *table;

real SQR(real x) {
	return x * x;
}
real WordDistance(const Embeds &a, const Embeds &b) {
	real result = 0;
	if (a == NULL || b == NULL) return DIS_INF;
	for (int i = 0; i < layer1_size; i++)
		result += SQR(a[i] - b[i]);
	return sqrt(result);
}
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
