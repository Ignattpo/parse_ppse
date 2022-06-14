#include <aio.h>

struct candidate_t
{
    u_int8_t *id;
    size_t id_size;
    u_int8_t prior;
};

struct candidates_t
{
    struct candidates_t *next;
    struct candidate_t *candidate;
};
struct candidates_list
{
    struct candidates_t *root;
    struct candidates_t *last;
};

void candidates_list_init(struct candidates_list *candidates);
struct candidate_t *create_candidate(u_int8_t *id, size_t id_size, u_int8_t prior);
int candidates_list_add(struct candidates_list *candidates, struct candidate_t *candidate);
void candidates_list_print(struct candidates_list *candidates);
void candidates_sort(struct candidates_list *candidates);
void candidates_list_free(struct candidates_list *candidates);