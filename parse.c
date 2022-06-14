#include "parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HEAD 0x6F
#define TAG_FCI_PT 0xA5
#define TAG_FCI_IDD_1 0xBF
#define TAG_FCI_IDD_2 0x0C
#define TAG_AT 0x61
#define TAG_AID 0x4F
#define TAG_API 0x87
#define TAG_LAST 0x90
#define TAG_UNKNOWN_1 0x9F
#define TAG_UNKNOWN_2 0x2A
#define SIZE_RID 5

static bool if_candidate(u_int8_t *id, size_t id_size)
{
    if (id == NULL || id_size < SIZE_RID)
    {
        return false;
    }
    static u_int8_t rid[SIZE_RID] = {0xA0, 0x00, 0x00, 0x06, 0x58};
    for (int i = 0; i < SIZE_RID; i++)
    {
        if (id[i] != rid[i])
        {
            return false;
        }
    }
    return true;
}

static int parse_at(u_int8_t *package, struct candidates_list *candidates)
{
    u_int8_t *tag = package;
    u_int8_t *id = NULL;
    size_t id_size = 0;
    u_int8_t prior = 0xFF;
next_tag:
    switch (*tag)
    {
    case 0x9F:
    {
        tag++;
        if (*tag != 0x2A)
        {
            fprintf(stderr, "Package error TAG_UNKNOWN\n");
            return -1;
        }
        tag++;
        size_t size = *tag;
        tag++;
        tag += size;
        goto next_tag;
    }
    case TAG_AID:
    {
        tag++;
        size_t size = *tag;
        tag++;
        id_size = size;
        id = malloc(size);
        if (id == NULL)
        {
            fprintf(stderr, "Package error malloc\n");
            return -1;
        }
        memcpy(id, tag, size);
        tag += size;
        goto next_tag;
    }
    case TAG_API:
    {
        tag++;
        size_t size = *tag;
        tag++;
        prior = *tag;
        tag += size;
        goto next_tag;
    }
    }

    bool is_candidate = if_candidate(id, id_size);
    if (!is_candidate)
    {
        free(id);
        return 0;
    }

    struct candidate_t *candidate = create_candidate(id, id_size, prior);
    candidates_list_add(candidates, candidate);
    candidates_sort(candidates);

    return 0;
}

static int parse_dci(u_int8_t *package, struct candidates_list *candidates)
{
    u_int8_t *tag = package;
    if (*tag != TAG_FCI_IDD_1)
    {
        fprintf(stderr, "Package error TAG_FCI_IDD\n");
        return -1;
    }
    tag++;
    if (*tag != TAG_FCI_IDD_2)
    {
        fprintf(stderr, "Package error TAG_FCI_IDD\n");
        return -1;
    }
    tag++;

    size_t package_size = *tag;
    tag++;
next_tag:
    switch (*tag)
    {
    case TAG_AT:
    {
        tag++;
        size_t size = *tag;
        tag++;
        int res = parse_at(tag, candidates);
        if (res != 0)
        {
            fprintf(stderr, "Package error TAG_AT\n");
            return -1;
        }
        tag += size;
        goto next_tag;
    }
    }
    return 0;
}

int parse_package(u_int8_t *package, struct candidates_list *candidates)
{
    u_int8_t *tag = package;
    int res = 0;
next_tag:
    switch (*tag)
    {
    case TAG_FCI_PT:
    {
        tag++;
        size_t size = *tag;
        tag++;
        res = parse_dci(tag, candidates);
        if (res != 0)
        {
            fprintf(stderr, "Package error TAG_FCI_PT\n");
            return -1;
        }
        tag += size;
        goto next_tag;
    }
    case HEAD:
    {
        tag++;
        tag++;
        goto next_tag;
    }

    case TAG_LAST:
    {
        break;
    }
    default:
    {
        tag++;
        size_t size = *tag;
        tag++;
        tag += size;
        goto next_tag;
    }
    }
    return 0;
}
