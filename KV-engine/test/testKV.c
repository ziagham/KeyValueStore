#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "keystore-without-poet.h"

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])

int main() {
	db_t *root = db_new();
	char keys[][64] = {"aAiFPRqVdIqubAtTPGAw", "artist", "cuck", "cunny", "dog", "zoo", "zoom", "zone", "way", 
                        "wood", "meat", "sir", "food", "ear", "math", "debt", "mall", "hat", "dad", "oven", 
                        "week", "disk", "menu", "role", "army"};

	char values[][256] = {"It's always a good idea to seek shelter from the evil gaze of the sun.", 
                        "When nobody is around, the trees gossip about the people who have walked under them.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "Eating eggs on Thursday for choir practice was recommended.", 
                        "Even with the snow falling outside, she felt it appropriate to wear her bikini.", 
                        "Every manager should be able to recite at least ten nursery rhymes backward.", 
                        "He waited for the stop sign to turn to a go sign.", 
                        "The crowd yells and screams for more memes.",
                        "The glacier came alive as the climbers hiked closer.", 
                        "Dan took the deep dive down the rabbit hole.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned.", 
                        "The changing of down comforters to cotton bedspreads always meant the squirrels had returned."};

    for (int i = 0; i < ARRAY_SIZE(keys); i++)
        db_put(root, keys[i], values[i]);

	printf("%s --- %s\n", "aAiFPRqVdIqubAtTPGAw", db_get(root, "aAiFPRqVdIqubAtTPGAw"));
    printf("%s --- %s\n", "cuck", db_get(root, "cuck"));
    printf("%s --- %s\n", "dog", db_get(root, "dog"));
    printf("%s --- %s\n", "zoom", db_get(root, "zoom"));
    printf("%s --- %s\n", "wood", db_get(root, "wood"));
    printf("%s --- %s\n", "oven", db_get(root, "oven"));
    printf("%s --- %s\n", "army", db_get(root, "army"));

}