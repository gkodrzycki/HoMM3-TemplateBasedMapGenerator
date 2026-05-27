#pragma once

#include "Global.hpp"

// ---------------------------------------------------------------------------
// GridSearchContext
//
// Preallocated buffers for all BFS/Dijkstra/A* operations.
// Create one instance per map (e.g. as a field of the Map class) and pass
// it by reference to the functions below.
//
// Epoch trick: instead of clearing the entire buffer O(W*H) before each call,
// we increment an `epoch` counter. A cell is "clean" when stamp[i] != epoch.
// Only touched cells are reset — O(visited) instead of O(W*H).
// ---------------------------------------------------------------------------
struct GridSearchContext {
    int W = 0, H = 0;

    // flat index: i = y*W + x  (row-major, cache-friendly)
    vector<int> stamp;   // W*H   — epoch stamp (sentinel for "unvisited")
    vector<int> dist;    // W*H   — distances (Dijkstra / BFS depth)
    vector<int> parentX; // W*H   — parent.x  (-1 = no parent)
    vector<int> parentY; // W*H   — parent.y
    vector<int> claim;   // W*H   — zone id (multi-source BFS claim)
    // for bfs_claim_xyz2 (2 z-layers):
    vector<int> stamp2; // W*H*2
    vector<int> claim2; // W*H*2
    // for dijkstra_reachability:
    vector<int> reachDist;    // W*H
    vector<int> reachParentX; // W*H
    vector<int> reachParentY; // W*H

    int epoch  = 0;
    int epoch2 = 0; // separate epoch for the *2 buffers

    // queue reused across calls (always empty on entry)
    queue<int3> q;

    GridSearchContext() = default;

    explicit GridSearchContext(int W_, int H_) { resize(W_, H_); }

    void resize(int W_, int H_) {
        W      = W_;
        H      = H_;
        int n  = W * H;
        int n2 = n * 2;
        stamp.assign(n, -1);
        dist.assign(n, 0);
        parentX.assign(n, -1);
        parentY.assign(n, -1);
        claim.assign(n, 0);
        stamp2.assign(n2, -1);
        claim2.assign(n2, 0);
        reachDist.assign(n, 0);
        reachParentX.assign(n, -1);
        reachParentY.assign(n, -1);
        epoch  = 0;
        epoch2 = 0;
    }

    inline int idx(int x, int y) const { return y * W + x; }
    inline int idx(const int3 &p) const { return p.y * W + p.x; }
    inline int idx2(int x, int y, int z) const { return (y * W + x) * 2 + z; }
    inline int idx2(const int3 &p) const { return (p.y * W + p.x) * 2 + p.z; }

    // primary arena — BFS/Dijkstra/A*
    inline void newSearch() { ++epoch; }
    inline bool fresh(int i) const { return stamp[i] != epoch; }
    inline void touch(int i) { stamp[i] = epoch; }

    // arena for 2-layer buffers
    inline void newSearch2() { ++epoch2; }
    inline bool fresh2(int i) const { return stamp2[i] != epoch2; }
    inline void touch2(int i) { stamp2[i] = epoch2; }
};

// ---------------------------------------------------------------------------
// Internal helper — reconstructs path from parentX/parentY buffers in ctx
// ---------------------------------------------------------------------------
inline vector<int3> reconstruct_path(const GridSearchContext &ctx, const int3 &start,
                                     const int3 &goal) {
    vector<int3> path;
    int3 cur = goal;

    while (cur != start) {
        path.push_back(cur);
        int ci = ctx.idx(cur);
        if (ctx.parentX[ci] == -1)
            return {};
        cur = int3(ctx.parentX[ci], ctx.parentY[ci], cur.z);
    }
    path.push_back(start);
    reverse(path.begin(), path.end());
    return path;
}

// ---------------------------------------------------------------------------
// BFS — returns path start -> goal
//
// neighbors(pos) -> iterable<int3>
// passable(pos)  -> bool
// ---------------------------------------------------------------------------
template <typename NeighFn, typename PassableFn>
inline vector<int3> bfs_path_xy(GridSearchContext &ctx, const int3 &start, const int3 &goal,
                                NeighFn neighbors, PassableFn passable) {
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;

    if (!isInside(0, 0, W, H, start) || !isInside(0, 0, W, H, goal))
        return {};
    if (!passable(start) || !passable(goal))
        return {};

    auto &q = ctx.q;
    int si  = ctx.idx(start);
    ctx.touch(si);
    ctx.parentX[si] = -1;
    ctx.parentY[si] = -1;
    q.push(start);

    bool found = false;
    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        if (cur == goal) {
            found = true;
            break;
        }

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            int ni = ctx.idx(nxt);
            if (!ctx.fresh(ni))
                continue;

            ctx.touch(ni);
            ctx.parentX[ni] = cur.x;
            ctx.parentY[ni] = cur.y;
            q.push(nxt);
        }
    }

    // drain queue in case we broke out early
    while (!q.empty())
        q.pop();

    if (!found)
        return {};
    return reconstruct_path(ctx, start, goal);
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

// ---------------------------------------------------------------------------
// BFS claim component — returns all tiles reachable from source
// ---------------------------------------------------------------------------
template <typename NeighFn, typename PassableFn>
inline vector<int3> bfs_claim_component(GridSearchContext &ctx, int3 source, NeighFn neighbors,
                                        PassableFn passable) {
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;

    auto &q = ctx.q;
    vector<int3> out;

    if (!isInside(0, 0, W, H, source) || !passable(source))
        return out;

    int si = ctx.idx(source);
    ctx.touch(si);
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
            int ni = ctx.idx(nxt);
            if (!ctx.fresh(ni))
                continue;

            ctx.touch(ni);
            q.push(nxt);
        }
    }

    return out;
}

// ---------------------------------------------------------------------------
// Multi-source BFS claim — claim[x][y] = id (0 = unclaimed)
//
// Result written to ctx.claim. Returns a reference to the buffer (no copy).
// ---------------------------------------------------------------------------
template <typename NeighFn, typename PassableFn>
inline const vector<int> &bfs_claim_xy(GridSearchContext &ctx,
                                       const vector<pair<int, int3>> &sources, NeighFn neighbors,
                                       PassableFn passable) {
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;
    auto &q = ctx.q;

    for (const auto &[id, s] : sources) {
        if (!isInside(0, 0, W, H, s))
            continue;
        if (!passable(s))
            continue;
        int si = ctx.idx(s);
        if (!ctx.fresh(si))
            continue;

        ctx.touch(si);
        ctx.claim[si] = id;
        q.push(s);
    }

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        int ci = ctx.idx(cur);
        int id = ctx.claim[ci];

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            int ni = ctx.idx(nxt);
            if (!ctx.fresh(ni))
                continue;

            ctx.touch(ni);
            ctx.claim[ni] = id;
            q.push(nxt);
        }
    }

    return ctx.claim;
}

// Convenience wrapper that copies the result into a 2D vector.
// Prefer the reference-returning version above in hot paths.
template <typename NeighFn, typename PassableFn>
inline vector<vector<int>> bfs_claim_xy_copy(GridSearchContext &ctx,
                                             const vector<pair<int, int3>> &sources,
                                             NeighFn neighbors, PassableFn passable) {
    const auto &flat = bfs_claim_xy(ctx, sources, neighbors, passable);
    int W = ctx.W, H = ctx.H;
    vector<vector<int>> out(W, vector<int>(H, 0));
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++) {
            int i = ctx.idx(x, y);
            if (ctx.stamp[i] == ctx.epoch)
                out[x][y] = flat[i];
        }
    return out;
}

// ---------------------------------------------------------------------------
// Multi-source BFS claim with 2 z-layers (z=0/1)
// claim2[idx2(x,y,z)] = id
//
// Result in ctx.claim2. claimFn(id, pos) -> bool (false = stop expansion for that zone)
// ---------------------------------------------------------------------------
struct NoOpClaim {
    bool operator()(int, const int3 &) const { return true; }
};

template <typename NeighFn, typename PassableFn, typename ClaimFn = NoOpClaim>
inline const vector<int> &bfs_claim_xyz2(GridSearchContext &ctx,
                                         const vector<pair<int, int3>> &sources, NeighFn neighbors,
                                         PassableFn passable, ClaimFn claimFn = NoOpClaim{}) {
    ctx.newSearch2();
    int W = ctx.W, H = ctx.H;
    auto &q = ctx.q;

    for (const auto &[id, s] : sources) {
        if (!isInside(0, 0, W, H, s))
            continue;
        if (s.z < 0 || s.z > 1)
            continue;
        if (!passable(s))
            continue;
        int si = ctx.idx2(s);
        if (!ctx.fresh2(si))
            continue;

        ctx.touch2(si);
        ctx.claim2[si] = id;
        q.push(s);
    }

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        int ci = ctx.idx2(cur);
        int id = ctx.claim2[ci];

        if (!claimFn(id, cur))
            continue;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (nxt.z < 0 || nxt.z > 1)
                continue;
            if (!passable(nxt))
                continue;
            int ni = ctx.idx2(nxt);
            if (!ctx.fresh2(ni))
                continue;

            ctx.touch2(ni);
            ctx.claim2[ni] = id;
            q.push(nxt);
        }
    }

    return ctx.claim2;
}

// Helper to read bfs_claim_xyz2 result (instead of [x][y][z])
inline int claim2_get(const GridSearchContext &ctx, int x, int y, int z) {
    int i = ctx.idx2(x, y, z);
    return (ctx.stamp2[i] == ctx.epoch2) ? ctx.claim2[i] : 0;
}
inline int claim2_get(const GridSearchContext &ctx, const int3 &p) {
    return claim2_get(ctx, p.x, p.y, p.z);
}

// ---------------------------------------------------------------------------
// BFS collect — gathers all tiles within distance <= maxDepth
// Returns tiles in BFS order
// ---------------------------------------------------------------------------
template <typename NeighFn, typename PassableFn>
inline vector<int3> bfs_collect_depth_xy(GridSearchContext &ctx, const vector<int3> &sources,
                                         int maxDepth, NeighFn neighbors, PassableFn passable) {
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;
    auto &q = ctx.q;
    vector<int3> out;
    out.reserve(sources.size() * 4);

    for (const auto &s : sources) {
        if (!isInside(0, 0, W, H, s))
            continue;
        if (!passable(s))
            continue;
        int si = ctx.idx(s);
        if (!ctx.fresh(si))
            continue;

        ctx.touch(si);
        ctx.dist[si] = 0;
        q.push(s);
        out.push_back(s);
    }

    while (!q.empty()) {
        int3 cur = q.front();
        q.pop();
        int ci = ctx.idx(cur);
        int d  = ctx.dist[ci];
        if (d >= maxDepth)
            continue;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;
            int ni = ctx.idx(nxt);
            if (!ctx.fresh(ni))
                continue;

            ctx.touch(ni);
            ctx.dist[ni] = d + 1;
            q.push(nxt);
            out.push_back(nxt);
        }
    }

    return out;
}

// ---------------------------------------------------------------------------
// Dijkstra — returns path start -> goal
//
// cost(a, b) -> int >= 0
// ---------------------------------------------------------------------------
template <typename NeighFn, typename PassableFn, typename CostFn>
inline vector<int3> dijkstra_path_xy(GridSearchContext &ctx, const int3 &start, const int3 &goal,
                                     NeighFn neighbors, PassableFn passable, CostFn cost) {
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;
    const int INF = numeric_limits<int>::max();

    struct Node {
        int d;
        int3 p;
        bool operator>(const Node &o) const { return d > o.d; }
    };
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    if (!isInside(0, 0, W, H, start) || !isInside(0, 0, W, H, goal))
        return {};
    if (!passable(start) || !passable(goal))
        return {};

    int si = ctx.idx(start);
    ctx.touch(si);
    ctx.dist[si]    = 0;
    ctx.parentX[si] = -1;
    ctx.parentY[si] = -1;
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, cur] = pq.top();
        pq.pop();
        int ci = ctx.idx(cur);

        if (d != ctx.dist[ci])
            continue;
        if (cur == goal)
            break;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;

            int ni      = ctx.idx(nxt);
            int curDist = ctx.fresh(ni) ? INF : ctx.dist[ni];
            int nd      = d + cost(cur, nxt);

            if (nd < curDist) {
                ctx.touch(ni);
                ctx.dist[ni]    = nd;
                ctx.parentX[ni] = cur.x;
                ctx.parentY[ni] = cur.y;
                pq.push({nd, nxt});
            }
        }
    }

    // check whether goal was actually reached
    int gi = ctx.idx(goal);
    if (ctx.fresh(gi))
        return {};
    return reconstruct_path(ctx, start, goal);
}

// ---------------------------------------------------------------------------
// A* — returns path start -> goal
//
// cost(a, b) -> int >= 0
// h(pos)     -> int >= 0  (admissible heuristic)
// ---------------------------------------------------------------------------
template <typename NeighFn, typename PassableFn, typename CostFn, typename HeuFn>
inline vector<int3> astar_path_xy(GridSearchContext &ctx, const int3 &start, const int3 &goal,
                                  NeighFn neighbors, PassableFn passable, CostFn cost, HeuFn h) {
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;
    const int INF = numeric_limits<int>::max();

    struct Node {
        int f, g;
        int3 p;
        bool operator>(const Node &o) const { return f > o.f; }
    };
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    if (!isInside(0, 0, W, H, start) || !isInside(0, 0, W, H, goal))
        return {};
    if (!passable(start) || !passable(goal))
        return {};

    int si = ctx.idx(start);
    ctx.touch(si);
    ctx.dist[si]    = 0; // stores g-cost
    ctx.parentX[si] = -1;
    ctx.parentY[si] = -1;
    pq.push({h(start), 0, start});

    while (!pq.empty()) {
        auto [f, g, cur] = pq.top();
        pq.pop();
        int ci = ctx.idx(cur);

        if (g != ctx.dist[ci])
            continue;
        if (cur == goal)
            break;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;

            int ni   = ctx.idx(nxt);
            int curG = ctx.fresh(ni) ? INF : ctx.dist[ni];
            int ng   = g + cost(cur, nxt);

            if (ng < curG) {
                ctx.touch(ni);
                ctx.dist[ni]    = ng;
                ctx.parentX[ni] = cur.x;
                ctx.parentY[ni] = cur.y;
                pq.push({ng + h(nxt), ng, nxt});
            }
        }
    }

    int gi = ctx.idx(goal);
    if (ctx.fresh(gi))
        return {};
    return reconstruct_path(ctx, start, goal);
}

// ---------------------------------------------------------------------------
// BFS distances — BFS distances from source, for small graphs (e.g. zone graph)
//
// Generic over any hashable Node type — no ctx needed (graph is tiny).
// Used e.g. to compute inter-zone distances.
// ---------------------------------------------------------------------------
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
            if (dist.count(nxt))
                continue;
            dist[nxt] = d + 1;
            q.push(nxt);
        }
    }

    return dist;
}

// ---------------------------------------------------------------------------
// Dijkstra reachability — full distance map from start across the entire grid
//
// Result written to ctx.reachDist / ctx.reachParentX / ctx.reachParentY.
// Read values via reach_dist(ctx, x, y) and reach_parent(ctx, x, y).
// Unreachable cells have reachDist = INT_MAX.
// ---------------------------------------------------------------------------
inline int reach_dist(const GridSearchContext &ctx, int x, int y) {
    int i = ctx.idx(x, y);
    return ctx.reachDist[i];
}
inline int reach_dist(const GridSearchContext &ctx, const int3 &p) {
    return reach_dist(ctx, p.x, p.y);
}
inline int3 reach_parent(const GridSearchContext &ctx, int x, int y) {
    int i = ctx.idx(x, y);
    return int3(ctx.reachParentX[i], ctx.reachParentY[i], 0);
}
inline int3 reach_parent(const GridSearchContext &ctx, const int3 &p) {
    return reach_parent(ctx, p.x, p.y);
}

template <typename NeighFn, typename PassableFn, typename CostFn>
inline void dijkstra_reachability(GridSearchContext &ctx, const int3 &start, NeighFn neighbors,
                                  PassableFn passable, CostFn cost) {
    // Uses separate reachDist/reachParentXY buffers with the primary epoch stamp,
    // so it can coexist with other searches without clobbering their results.
    ctx.newSearch();
    int W = ctx.W, H = ctx.H;
    const int INF = numeric_limits<int>::max();

    auto setReach = [&](int i, int d, int px, int py) {
        ctx.touch(i);
        ctx.reachDist[i]    = d;
        ctx.reachParentX[i] = px;
        ctx.reachParentY[i] = py;
    };

    auto getDist = [&](int i) -> int { return ctx.fresh(i) ? INF : ctx.reachDist[i]; };

    struct Node {
        int d;
        int3 p;
        bool operator>(const Node &o) const { return d > o.d; }
    };
    priority_queue<Node, vector<Node>, greater<Node>> pq;

    if (!isInside(0, 0, W, H, start) || !passable(start))
        return;

    int si = ctx.idx(start);
    setReach(si, 0, -1, -1);
    pq.push({0, start});

    while (!pq.empty()) {
        auto [d, cur] = pq.top();
        pq.pop();
        int ci = ctx.idx(cur);

        if (d != getDist(ci))
            continue;

        for (const auto &nxt : neighbors(cur)) {
            if (!isInside(0, 0, W, H, nxt))
                continue;
            if (!passable(nxt))
                continue;

            int ni = ctx.idx(nxt);
            int nd = d + cost(cur, nxt);
            if (nd < getDist(ni)) {
                setReach(ni, nd, cur.x, cur.y);
                pq.push({nd, nxt});
            }
        }
    }
}
