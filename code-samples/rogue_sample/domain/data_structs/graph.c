#include "graph.h"

vector *dist_and_next_pos_to_target(const object_t *start, const object_t *target, const level_t *level)
{
    vector *path = NULL;
    if (start == target)
        return path;

    queue *q = create_queue();
    enqueue(q, start);
    
    static const int dx[] = { 0, 0, -1, 1 };
    static const int dy[] = { -1, 1, 0, 0 };
    directions_e dirs[] = { FORWARD, BACK, LEFT, RIGHT };
    const int count_directions = sizeof(dirs) / sizeof(*dirs);
    
    int visit[ROOMS_IN_HEIGHT * REGION_HEIGHT][ROOMS_IN_WIDTH * REGION_WIDTH] = { 0 };
    int dist[ROOMS_IN_HEIGHT * REGION_HEIGHT][ROOMS_IN_WIDTH * REGION_WIDTH] = { 0 };
    directions_e dir_parent[ROOMS_IN_HEIGHT * REGION_HEIGHT][ROOMS_IN_WIDTH * REGION_WIDTH] = { 0 };
    object_t parent[ROOMS_IN_HEIGHT * REGION_HEIGHT][ROOMS_IN_WIDTH * REGION_WIDTH];
    
    
    visit[start->coordinates[Y]][start->coordinates[X]] = true;
    while (!is_empty(q))
    {
        object_t current = dequeue(q);
        int current_dist = dist[current.coordinates[Y]][current.coordinates[X]];
        
        for (size_t i = 0; (signed) i < count_directions; i++)
        {
            object_t coords = current;
            coords.coordinates[X] += dx[i];
            coords.coordinates[Y] += dy[i];
            
            if (!visit[coords.coordinates[Y]][coords.coordinates[X]] && !check_outside_border(&coords, level) && check_unoccupied_level(&coords, level))
            {
                enqueue(q, &coords);
                dist[coords.coordinates[Y]][coords.coordinates[X]] = current_dist + 1;
                visit[coords.coordinates[Y]][coords.coordinates[X]] = true;
                parent[coords.coordinates[Y]][coords.coordinates[X]] = current;
                dir_parent[coords.coordinates[Y]][coords.coordinates[X]] = dirs[i];
            }
        }
    }

    if (visit[target->coordinates[Y]][target->coordinates[X]])
    {
        path = create_vector();
        object_t coords = *target;
        while (!check_equal_coords(parent[coords.coordinates[Y]][coords.coordinates[X]].coordinates, start->coordinates))
        {
            push_back(path, dir_parent[coords.coordinates[Y]][coords.coordinates[X]]);
            coords = parent[coords.coordinates[Y]][coords.coordinates[X]];
        }
        push_back(path, dir_parent[coords.coordinates[Y]][coords.coordinates[X]]);
        reverse_vector(path);
    }
    
    free(q);
    return path;
}
