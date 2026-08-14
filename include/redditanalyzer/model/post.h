#ifndef REDDITANALYZER_MODEL_POST_H
#define REDDITANALYZER_MODEL_POST_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct
{
    char *id;
    char *title;
    char *author;

    int64_t score;
    uint64_t comments;

    time_t created_at;

    bool is_video;
    bool is_self;
} Post;

Post *post_create(void);
void post_destroy(Post *post);

#endif /* REDDITANALYZER_MODEL_POST_H */