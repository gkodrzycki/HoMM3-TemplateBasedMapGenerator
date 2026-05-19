#pragma once

#include "Global.hpp"

struct GridParent {
    int3 p{};
    bool hasParent = false;
};

inline vector<int3> reconstruct_path_xy(const int3 &start, const int3 &goal,
                                        const vector<vector<GridParent>> &parent) {
    vector<int3> path;
    int3 cur = goal;

    if (cur != start && !parent[cur.x][cur.y].hasParent)
        return {};

    while (cur != start) {
        path.push_back(cur);
        cur = parent[cur.x][cur.y].p;

        if (cur != start && !parent[cur.x][cur.y].hasParent)
            return {};
    }

    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

/**
 * BFS: zwraca ścieżkę start->goal
 * visited[x][y], parent[x][y]
 *
 * neighbors(pos) -> iterable po int3 (np. array<int3,4>, vector<int3>)
 * passable(pos)  -> bool
 */
template <typename NeighFn, typename PassableFn>
inline vector<int3> bfs_path_xy(int W, int H, const int3 &start, const int3 &goal,
                                NeighFn neighbors, PassableFn passable) {
    vector<vector<bool>> visited(W, vector<bool>(H, false));
    vector<vector<GridParent>> parent(W, vector<GridParent>(H));

    if (!isInside(0, 0, W, H, start) || !isInside(0, 0, W, H, goal))
        return {};
    if (!passable(start) || !passable(goal))
        return {};

    queue<int3> q;
    visited[start.x][start.y] = true;
    q.push(start);

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        if (cur == goal)
            break;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            if (visited[nxt.x][nxt.y])
                continue;

            visited[nxt.x][nxt.y] = true;
            parent[nxt.x][nxt.y]  = GridParent{cur, true};
            q.push(nxt);
        }
    }

    return reconstruct_path_xy(start, goal, parent);
}
/**
 * Multi-source BFS "claim" component
 */
template <typename NeighFn, typename PassableFn>
inline vector<int3> bfs_claim_component(int W, int H, int3 source, NeighFn neighbors,
                                        PassableFn passable) {
    vector<vector<int>> visited(W, vector<int>(H, 0));
    queue<int3> q;
    vector<int3> out;

    visited[source.x][source.y] = 1;
    q.push(source);

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        out.push_back(cur);

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            if (visited[nxt.x][nxt.y] != 0)
                continue;

            visited[nxt.x][nxt.y] = 1;
            q.push(nxt);
        }
    }

    return out;
}

/**
 * Multi-source BFS "claim": claim[x][y] = id (0 = nieprzypisane)
 */
template <typename NeighFn, typename PassableFn>
inline vector<vector<int>> bfs_claim_xy(int W, int H, const vector<pair<int, int3>> &sources,
                                        NeighFn neighbors, PassableFn passable) {
    vector<vector<int>> claim(W, vector<int>(H, 0));
    queue<int3> q;

    for (const auto &[id, s] : sources) {
        if (!isInside(0, 0, W, H, s))
            continue;
        if (!passable(s))
            continue;
        if (claim[s.x][s.y] != 0)
            continue;

        claim[s.x][s.y] = id;
        q.push(s);
    }

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        int id = claim[cur.x][cur.y];

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            if (claim[nxt.x][nxt.y] != 0)
                continue;

            claim[nxt.x][nxt.y] = id;
            q.push(nxt);
        }
    }

    return claim;
}

/**
 * Multi-source BFS "claim" w 2 warstwach z (z=0/1):
 * claim[x][y][z] = id
 *
 * Idealne do ZonePlacer::claimTiles, gdzie masz currentClaim[..][..][2].
 */
struct NoOpClaim {
    void operator()(int, const int3 &) const {}
};
template <typename NeighFn, typename PassableFn, typename ClaimFn>
inline vector<vector<array<int, 2>>>
bfs_claim_xyz2(int W, int H, const vector<pair<int, int3>> &sources, NeighFn neighbors,
               PassableFn passable, ClaimFn claimFn = NoOpClaim{}) {
    vector<vector<array<int, 2>>> claim(W, vector<array<int, 2>>(H, {0, 0}));
    queue<int3> q;

    for (const auto &[id, s] : sources) {
        if (!isInside(0, 0, W, H, s))
            continue;
        if (s.z < 0 || s.z > 1)
            continue;
        if (!passable(s))
            continue;
        if (claim[s.x][s.y][s.z] != 0)
            continue;

        claim[s.x][s.y][s.z] = id;
        q.push(s);
    }

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        int id = claim[cur.x][cur.y][cur.z];

        if (claimFn(id, cur) == false) {
            continue;
        }

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (nxt.z < 0 || nxt.z > 1)
                continue;
            if (!passable(nxt))
                continue;
            if (claim[nxt.x][nxt.y][nxt.z] != 0)
                continue;

            claim[nxt.x][nxt.y][nxt.z] = id;
            q.push(nxt);
        }
    }

    return claim;
}
/**
 * BFS zbierający wszystkie tile do dystansu <= maxDepth
 * depth[x][y]
 */
template <typename NeighFn, typename PassableFn>
inline vector<int3> bfs_collect_depth_xy(int W, int H, const vector<int3> &sources, int maxDepth,
                                         NeighFn neighbors, PassableFn passable) {
    vector<vector<int>> depth(W, vector<int>(H, -1));
    queue<int3> q;
    vector<int3> out;

    out.reserve((int)sources.size() * 4);

    for (const auto &s : sources) {
        if (!isInside(0, 0, W, H, s))
            continue;
        if (!passable(s))
            continue;
        if (depth[s.x][s.y] != -1)
            continue;

        depth[s.x][s.y] = 0;
        q.push(s);
        out.push_back(s);
    }

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        int d = depth[cur.x][cur.y];
        if (d >= maxDepth)
            continue;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            if (depth[nxt.x][nxt.y] != -1)
                continue;

            depth[nxt.x][nxt.y] = d + 1;
            q.push(nxt);
            out.push_back(nxt);
        }
    }

    return out;
}

/**
 * Dijkstra: dist[x][y], parent[x][y]
 * cost(a,b) >= 0
 */
template <typename NeighFn, typename PassableFn, typename CostFn>
inline vector<int3> dijkstra_path_xy(int W, int H, const int3 &start, const int3 &goal,
                                     NeighFn neighbors, PassableFn passable, CostFn cost) {
    using Dist     = int;
    const Dist INF = numeric_limits<Dist>::max();

    vector<vector<Dist>> dist(W, vector<Dist>(H, INF));
    vector<vector<GridParent>> parent(W, vector<GridParent>(H));

    struct Node {
        Dist d;
        int3 p;
    };
    struct Cmp {
        bool operator()(const Node &a, const Node &b) const { return a.d > b.d; }
    };
    priority_queue<Node, vector<Node>, Cmp> pq;

    if (!isInside(0, 0, W, H, start) || !isInside(0, 0, W, H, goal))
        return {};
    if (!passable(start) || !passable(goal))
        return {};

    dist[start.x][start.y] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        auto cur = pq.top();
        pq.pop();
        if (cur.d != dist[cur.p.x][cur.p.y])
            continue;
        if (cur.p == goal)
            break;

        for (const auto &nxt : neighbors(cur.p)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;

            Dist nd = cur.d + (Dist)cost(cur.p, nxt);
            if (nd < dist[nxt.x][nxt.y]) {
                dist[nxt.x][nxt.y]   = nd;
                parent[nxt.x][nxt.y] = GridParent{cur.p, true};
                pq.push({nd, nxt});
            }
        }
    }

    return reconstruct_path_xy(start, goal, parent);
}

/**
 * A*: g[x][y], parent[x][y]
 * h(pos) >= 0 ; jeśli h==0 to Dijkstra
 */
template <typename NeighFn, typename PassableFn, typename CostFn, typename HeuFn>
inline vector<int3> astar_path_xy(int W, int H, const int3 &start, const int3 &goal,
                                  NeighFn neighbors, PassableFn passable, CostFn cost, HeuFn h) {
    using Dist     = int;
    const Dist INF = numeric_limits<Dist>::max();

    vector<vector<Dist>> g(W, vector<Dist>(H, INF));
    vector<vector<GridParent>> parent(W, vector<GridParent>(H));

    struct Node {
        Dist f;
        Dist g;
        int3 p;
    };
    struct Cmp {
        bool operator()(const Node &a, const Node &b) const { return a.f > b.f; }
    };
    priority_queue<Node, vector<Node>, Cmp> pq;

    if (!isInside(0, 0, W, H, start) || !isInside(0, 0, W, H, goal))
        return {};
    if (!passable(start) || !passable(goal))
        return {};

    g[start.x][start.y] = 0;
    pq.push({(Dist)h(start), 0, start});

    while (!pq.empty()) {
        auto cur = pq.top();
        pq.pop();
        if (cur.g != g[cur.p.x][cur.p.y])
            continue;
        if (cur.p == goal)
            break;

        for (const auto &nxt : neighbors(cur.p)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;

            Dist ng = cur.g + (Dist)cost(cur.p, nxt);
            if (ng < g[nxt.x][nxt.y]) {
                g[nxt.x][nxt.y]      = ng;
                parent[nxt.x][nxt.y] = GridParent{cur.p, true};
                Dist f               = ng + (Dist)h(nxt);
                pq.push({f, ng, nxt});
            }
        }
    }

    return reconstruct_path_xy(start, goal, parent);
}

/**
 * BFS liczący dystans
 *
 */
template <typename Node, typename NeighFn>
inline unordered_map<Node, int> bfs_distances(const Node &start, NeighFn neighbors) {
    unordered_map<Node, int> dist;
    queue<Node> q;

    dist[start] = 0;
    q.push(start);

    while (!q.empty()) {
        Node cur = q.front();
        q.pop();
        int d = dist[cur];

        for (const auto &nxt : neighbors(cur)) {
            if (nxt == cur)
                continue;
            if (dist.find(nxt) != dist.end())
                continue;

            dist[nxt] = d + 1;
            q.push(nxt);
        }
    }

    return dist;
}

/*
 * Dijkstra reachability
 */
template <typename NeighFn, typename PassableFn, typename CostFn>
inline unordered_map<int3, pair<int3, int>>
dijkstra_reachability(int W, int H, int3 &start, NeighFn neighbors, PassableFn passable,
                      CostFn cost) {
    using Dist     = int;
    const Dist INF = numeric_limits<Dist>::max();

    unordered_map<int3, pair<int3, int>> reachability;

    for (int i = 0; i < W; i++) {
        for (int j = 0; j < H; j++) {
            for (int k = 0; k < 2; k++) {
                reachability[int3(i, j, k)] = {int3(-1, -1, -1), INF};
            }
        }
    }

    struct Node {
        Dist d;
        int3 p;
    };

    struct Cmp {
        bool operator()(const Node &a, const Node &b) const { return a.d > b.d; }
    };

    priority_queue<Node, vector<Node>, Cmp> pq;

    if (!isInside(0, 0, W, H, start))
        return {};
    if (!passable(start))
        return {};

    reachability[start] = {int3(-1, -1, -1), 0};
    pq.push({0, start});

    while (!pq.empty()) {
        auto cur = pq.top();
        pq.pop();
        if (cur.d != reachability[cur.p].second)
            continue;

        for (const auto &nxt : neighbors(cur.p)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;

            Dist nd = cur.d + (Dist)cost(cur.p, nxt);
            if (nd < reachability[nxt].second) {
                reachability[nxt] = {cur.p, nd};
                pq.push({nd, nxt});
            }
        }
    }

    return reachability;
}
