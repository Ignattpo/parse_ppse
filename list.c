#include "list.h"

#include <stdlib.h>
#include <stdio.h>

void candidates_list_init(struct candidates_list *candidates)
{
    candidates->root = NULL;
    candidates->last = NULL;
}

struct candidate_t *create_candidate(u_int8_t *id, size_t id_size, u_int8_t prior)
{
    struct candidate_t *entry = malloc(sizeof(struct candidate_t));
    if (!entry)
    {
        fprintf(stderr, "create_candidate malloc\n");
        return NULL;
    }
    entry->id = id;
    entry->id_size = id_size;
    entry->prior = prior;

    return entry;
}

int candidates_list_add(struct candidates_list *candidates, struct candidate_t *candidate)
{
    struct candidates_t *entry = malloc(sizeof(struct candidates_t));
    if (!entry)
    {
        fprintf(stderr, "candidates_list_add malloc\n");
        return -1;
    }
    entry->candidate = candidate;

    if (!candidates->root)
    {
        candidates->root = entry;
    }
    else
    {
        candidates->last->next = entry;
    }
    candidates->last = entry;

    return 0;
}
void candidates_sort(struct candidates_list *candidates)
{
    struct candidates_t *c = candidates->root;

    if (c != NULL)
    {
        while (c->next != NULL)
        {
            struct candidates_t *p = c->next;

            do
            {
                if (p->candidate->prior < c->candidate->prior)
                {
                    u_int8_t *id = p->candidate->id;
                    p->candidate->id = c->candidate->id;
                    c->candidate->id = id;

                    size_t id_size = p->candidate->id_size;
                    p->candidate->id_size = c->candidate->id_size;
                    c->candidate->id_size = id_size;

                    u_int8_t prior = p->candidate->prior;
                    p->candidate->prior = c->candidate->prior;
                    c->candidate->prior = prior;
                }

                p = p->next;
            } while (p != NULL);

            c = c->next;
        }
    }
}

void candidates_list_print(struct candidates_list *candidates)
{
    struct candidates_t *curr = candidates->root;
    if (!curr)
    {
        printf("No found candidates\n");
    }
    while (curr)
    {
        for (int i = 0; i < curr->candidate->id_size; i++)
        {
            printf("%x", curr->candidate->id[i]);
        }
        printf("\n");
        curr = curr->next;
    }
}

void candidates_list_free(struct candidates_list *candidates)
{
    struct candidates_t *curr = candidates->root;
    while (curr)
    {
        free(curr->candidate->id);
        struct candidates_t *next = curr->next;
        free(curr);
        curr = next;
    }
}