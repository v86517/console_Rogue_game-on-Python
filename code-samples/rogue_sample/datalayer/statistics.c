#include "statistics.h"

static session_stat get_stat_from_json(struct json_object *stat_json)
{
    session_stat stat;
    struct json_object *treasures;
    struct json_object *level;
    struct json_object *enemies;
    struct json_object *food;
    struct json_object *elixirs;
    struct json_object *scrolls;
    struct json_object *attacks;
    struct json_object *missed;
    struct json_object *moves;

    json_object_object_get_ex(stat_json, "treasures", &treasures);
    json_object_object_get_ex(stat_json, "level",     &level);
    json_object_object_get_ex(stat_json, "enemies",   &enemies);
    json_object_object_get_ex(stat_json, "food",      &food);
    json_object_object_get_ex(stat_json, "elixirs",   &elixirs);
    json_object_object_get_ex(stat_json, "scrolls",   &scrolls);
    json_object_object_get_ex(stat_json, "attacks",   &attacks);
    json_object_object_get_ex(stat_json, "missed",    &missed);
    json_object_object_get_ex(stat_json, "moves",     &moves);

    stat.treasures = json_object_get_int(treasures);
    stat.level     = json_object_get_int(level);
    stat.enemies   = json_object_get_int(enemies);
    stat.food      = json_object_get_int(food);
    stat.elixirs   = json_object_get_int(elixirs);
    stat.scrolls   = json_object_get_int(scrolls);
    stat.attacks   = json_object_get_int(attacks);
    stat.missed    = json_object_get_int(missed);
    stat.moves     = json_object_get_int(moves);

    return stat;
}

session_stat get_current_stat(const char *filename)
{
    char *buffer = read_file_to_str(filename);
    struct json_object *stat_json = json_tokener_parse(buffer);
    session_stat stat = get_stat_from_json(stat_json);

    free(buffer);
    json_object_put(stat_json);
    return stat;
}

void save_session_stat(const session_stat *stat, const char *filename)
{
    struct json_object *stat_json = json_object_new_object();
    json_object_object_add(stat_json, "treasures", json_object_new_int(stat->treasures));
    json_object_object_add(stat_json, "level",     json_object_new_int(stat->level));
    json_object_object_add(stat_json, "enemies",   json_object_new_int(stat->enemies));
    json_object_object_add(stat_json, "food",      json_object_new_int(stat->food));
    json_object_object_add(stat_json, "elixirs",   json_object_new_int(stat->elixirs));
    json_object_object_add(stat_json, "scrolls",   json_object_new_int(stat->scrolls));
    json_object_object_add(stat_json, "attacks",   json_object_new_int(stat->attacks));
    json_object_object_add(stat_json, "missed",    json_object_new_int(stat->missed));
    json_object_object_add(stat_json, "moves",     json_object_new_int(stat->moves));
    
    FILE *save = fopen(filename, "w");
    fprintf(save, "%s", json_object_to_json_string_ext(stat_json, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
    fclose(save);
    json_object_put(stat_json);
}

void update_statistics(const char *path_scoreboard, const char *path_stats)
{
    char *buffer_score = read_file_to_str(path_scoreboard);
    struct json_object *all_stat_json = json_tokener_parse(buffer_score);

    struct json_object *array_sessions;
    json_object_object_get_ex(all_stat_json, "sessionStats", &array_sessions);

    char *buffer_stat = read_file_to_str(path_stats);
    struct json_object *stat_json = json_tokener_parse(buffer_stat);

    json_object_array_add(array_sessions, stat_json);

    FILE *stat_file = fopen(path_scoreboard, "w");
    fprintf(stat_file, "%s", json_object_to_json_string_ext(all_stat_json, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));

    free(buffer_score);
    free(buffer_stat);
    fclose(stat_file);
    json_object_put(all_stat_json);
}

session_stat *get_session_stat_array(const char *path_scoreboard, size_t *count_sessions)
{
    char *buffer = read_file_to_str(path_scoreboard);
    struct json_object *parsed_json = json_tokener_parse(buffer);

    struct json_object *array_json;
    json_object_object_get_ex(parsed_json, "sessionStats", &array_json);

    *count_sessions = json_object_array_length(array_json);
    session_stat *array = malloc(*count_sessions * sizeof(session_stat));
    for (size_t i = 0; i < *count_sessions; i++)
    {
        struct json_object *cur_session = json_object_array_get_idx(array_json, i);
        array[i] = get_stat_from_json(cur_session);
    }

    free(buffer);
    json_object_put(parsed_json);

    return array;
}

int compare_session_stats(const void *first_ptr, const void *second_ptr)
{
    const session_stat *first = first_ptr;
    const session_stat *second = second_ptr;
    return second->treasures - first->treasures;
}
