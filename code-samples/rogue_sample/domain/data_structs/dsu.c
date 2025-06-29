#include "dsu.h"

void make_sets(int *parent, int *rank, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        parent[i] = i;
        rank[i] = 0;
    }
}

int find_set(int v, int *parent)
{
    if (v == parent[v])
        return v;
    return parent[v] = find_set(parent[v], parent);
}

void union_sets(int v, int u, int *parent, int *rank)
{
    v = find_set(v, parent);
    u = find_set(u, parent);
    
    if (u != v)
    {
        if (rank[u] >= rank[v])
            parent[v] = u;
        else
            parent[u] = v;
        
        if (rank[u] == rank[v])
            rank[u]++;
    }
}
