#include <stdlib.h>
#include <stdio.h>

typedef struct ss_part_struct 
{
    struct ss_part_struct* next;
    unsigned char value;
}ss_part;

void init_game_seed(int seed){
	srand(seed);
}

unsigned char get_random_char(){
	return rand() % 256;
}

static ss_part* generate_part(){
    ss_part* new_part = (ss_part*)malloc(sizeof(ss_part));
    new_part->next = NULL;
    new_part->value = get_random_char();
	return new_part;
}

ss_part* generate_new_part(ss_part* main_part){
    ss_part* new_part = generate_part();
	ss_part* current_part = main_part;
    if (current_part == NULL){
        return new_part;
    }
    while(current_part->next != NULL){
        current_part = current_part->next;
    }
    current_part->next = new_part;
    return main_part;
}

void delete_part(ss_part* main_part){
    while(main_part != NULL){
        ss_part* prev_part = main_part;
        main_part = main_part->next;
        free(prev_part);
    }
}