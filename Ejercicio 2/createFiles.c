#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char rangeLow[8 + 1];
    char rangeHigh[8 + 1];
    unsigned char len;
    int id;
} range_t;

typedef struct {
    char label[12 + 1];
    int id;
} card_t;

int main() {
    FILE *ranges;
    ranges = fopen("ranges.dat", "w");
    range_t reg1 = {"10000000", "20000000", 15, 540};
    range_t reg2 = {"20000000", "30000000", 15, 541};
    fwrite(&reg1, sizeof(range_t), 1, ranges);
    fwrite(&reg2, sizeof(range_t), 1, ranges);
    fclose(ranges);
    FILE *cards;
    cards = fopen("cards.dat", "w");
    card_t card1 = {"LABELNUMBER1", 540};
    card_t card2 = {"LABELNUMBER2", 541};
    fwrite(&card1, sizeof(card_t), 1, cards);
    fwrite(&card2, sizeof(card_t), 1, cards);
    fclose(cards);
}