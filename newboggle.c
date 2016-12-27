#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BENCHITTER 100000

#ifdef BENCHMARK
#include <time.h>
#endif
#define BOARDX 4
#define BOARDY 4

#define MAXWORD 16 //should be boardx * boardy or some shit


#define TRUE 1
#define FALSE 0


#define ISLOWER(char) ( ((char) >='a' && (char) <= 'z') )
#define ISUPPER(char) ( ((char) >='A' && (char) <= 'Z') )


typedef struct wordtrie_s {
	struct wordtrie_s * nexts[26];
	uint8_t isend;
} wordtrie_t;

char board[BOARDX][BOARDY] = {	"enen",
				"vfao",
				"txer",
				"cayu"};
char boardsign[BOARDX][BOARDY] = {0};




wordtrie_t root = {0};


//maybe add wordlength as an arg so i dont have to recalc
int addtotrie(const char * word, const size_t wordlen, wordtrie_t * const r){
	int i, num_created = 0;
//	size_t wordlen = strlen(word);

	wordtrie_t * cur = r;
	for(i = 0; i < wordlen; i++){
		char curc = word[i];
		if(ISLOWER(curc)) curc -= 'a';
		else if(ISUPPER(curc)) curc -= 'A';
		else {/*printf("invalid word %s %c %i\n", word, curc, curc); */return -1;}
		if(curc == 'q'-'a' && (word[i+1] == 'u' || word[i+1] == 'U')) i++;

		if(cur->nexts[curc]){
			cur = cur->nexts[curc];
		} else {
			cur->nexts[curc] = malloc(sizeof(wordtrie_t));
			cur = cur->nexts[curc];
			memset(cur, 0, sizeof(wordtrie_t));
			num_created++;
		}
	}
	cur->isend = TRUE;
	return num_created;
}


int jdepth = 0;
char jeremy[MAXWORD] = {0};

int search(int x, int y, wordtrie_t *cur){
	if(boardsign[x][y]) return 0;
	char dil = board[x][y];
	wordtrie_t *next =cur->nexts[dil];
	if(!next) return 0;
	boardsign[x][y] = 1;

	jeremy[jdepth] = dil+'a';
	jdepth++;
	if(dil == 'q'){
		jeremy[jdepth] = dil+'u';
		jdepth++;
	}
	if(next->isend){
		#ifndef BENCHMARK
		 printf("%s\n", jeremy);
		#endif
	}
	if(y > 0){
		if(x > 0) search(x-1, y-1, next);
		search(x, y-1, next);
		if(x < BOARDX-1) search(x+1, y-1, next);
	}
	if(x > 0) search(x-1, y, next);
	if(x < BOARDX-1) search(x+1, y, next);
	if(y < BOARDY-1){
		if(x > 0) search(x-1, y+1, next);
		search(x, y+1, next);
		if(x < BOARDX-1) search(x+1, y+1, next);
	}
	boardsign[x][y] = 0;
	jeremy[jdepth] = 0;
	jdepth--;
	return 1;
}



int main(const int argc, const char ** argv){
	FILE *f;
	if(argc < 2){
		printf("need more args\n");
		return 1;
	}
	f = fopen(argv[1], "r");
	if(!f){
		printf("unable to load %s\n", argv[1]);
	}

	int x, y;
	for(x = 0; x < BOARDX; x++)
	for(y = 0; y < BOARDY; y++)
		board[x][y] -= 'a';


	char *line = 0;
	ssize_t len = 0;
	ssize_t read = 0;
	while ((read = getline(&line, &len, f)) != -1) {
//		printf("%s", line);
		addtotrie(line, read-1, &root);
	}
	if(line) free(line);

#ifdef BENCHMARK
	struct timespec tstart = {0}, tend = {0};
	clock_gettime(CLOCK_MONOTONIC, &tstart);
	int k;
	for(k = 0; k < BENCHITTER; k++){
#endif

	for(x = 0; x < BOARDX; x++)
	for(y = 0; y < BOARDY; y++)
		search(x, y, &root);
#ifdef BENCHMARK
	}
	clock_gettime(CLOCK_MONOTONIC, &tend);
	double time = ((double)tend.tv_sec + 1.0e-9 * tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9 * tstart.tv_nsec);
	printf("time to do search %.8f\n", time);
#endif


	return 0;
}
