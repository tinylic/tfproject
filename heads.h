#ifndef _TFPROJECT_HEADS_H_
#define _TFPROJECT_HEADS_H_

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
#include <cstdint>
#include <cassert>
#include <iostream>
#include <thread>

#define MAX_THREADS 4
#define DIS_INF 1000
#define MAX_STRING 1000
#define EXP_TABLE_SIZE 1000
#define MAX_EXP 6
#define MAX_DOC_WORDS 800
#define MAX_SENTENCE_LENGTH 1000
#define MAX_CODE_LENGTH 40
#define MAX_DOC_PER_TAG 100
#define MIN_WORDS 3
#define MAX_TAGS 20
#define MAX_KEY_WORDS 2000
#define QUERY_DOC 500
#define THRESHOLD_K 50
#define ITER 10
#define vocab_hash_size 30000000 // Maximum 30 * 0.7 = 21M words in the vocabulary
#define max_w 200   //dimensionality that represent document - cluster number
#define layer1_size 300

#define MAX_DOCS ((MAX_TAGS) * (MAX_DOC_PER_TAG))
#define check perror("here is ok\n")
#define debug(...) fprintf(stderr, __VA_ARGS__)

using namespace std;

typedef float real;                    // Precision of float numbers
typedef char Char;
typedef pair<unsigned, unsigned> Upair;
typedef map<unsigned, unsigned> Umap;
typedef real* Embeds;
typedef vector<Embeds> AllEmbeds;

#endif
