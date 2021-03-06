#pragma once

#include "child.h"
#include "types.h"

extern unsigned int seed;
unsigned int init_seed(unsigned int seed);
void set_seed(struct childdata *child);
void reseed(void);
unsigned int new_seed(void);

unsigned int rand_bool(void);
unsigned int rand32(void);
u64 rand64(void);
unsigned int rand_range(unsigned int min, unsigned int max);

void generate_random_page(char *page);

void check_page_rand_redzone(void);

void init_page_rand(void);
