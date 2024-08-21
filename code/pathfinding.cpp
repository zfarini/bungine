v3i get_cell(v3 p) {
    v3i res;

    res.x = roundf(p.x / (ASTART_CELL_DIM));
    res.y = roundf(p.y / (ASTART_CELL_DIM));
    res.z = roundf(p.z / (ASTART_CELL_DIM));
    return res;
}

#define MAX_CELL_POW 10
#define MAX_CELL (1 << MAX_CELL_POW)

uint64_t pack_cell(v3i c) {
    assert(abs(c.x) < MAX_CELL / 2 && abs(c.y) < MAX_CELL / 2 &&
           abs(c.z) < MAX_CELL / 2);
    return ((uint64_t)(c.x + MAX_CELL / 2)) |
           ((c.y + MAX_CELL / 2) << MAX_CELL_POW) |
           ((c.z + MAX_CELL / 2) << (MAX_CELL_POW * 2));
}

v3i unpack_cell(uint64_t x) {
    v3i res;

    res.x = (x >> 0) & (MAX_CELL - 1);
    res.y = (x >> MAX_CELL_POW) & (MAX_CELL - 1);
    res.z = (x >> (MAX_CELL_POW * 2)) & (MAX_CELL - 1);
    return res - V3i(MAX_CELL / 2, MAX_CELL / 2, MAX_CELL / 2);
}

bool State::operator==(const State &rhs) const {
    return p.x == rhs.p.x && p.y == rhs.p.y && p.z == rhs.p.z &&
           jump == rhs.jump;
}

bool State::operator<(const State &rhs) const {
    if (fscore == rhs.fscore) {
        auto p1 = pack_cell(p);
        auto p2 = pack_cell(rhs.p);
        if (p1 == p2)
            return jump < rhs.jump;
        return p1 < p2;
    }
    return fscore < rhs.fscore;
}

std::size_t StateHasher::operator()(const State &k) const {
    std::size_t h1 = std::hash<int>{}(k.p.x);
    std::size_t h2 = std::hash<int>{}(k.p.y);
    std::size_t h3 = std::hash<int>{}(k.p.z);
    std::size_t h4 = std::hash<int>{}(k.jump);

    return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
}

v3 get_closest_point_in_cell(v3i cell, v3 p) {
    v3 box_min = V3(cell) * ASTART_CELL_DIM - 0.5f * V3(ASTART_CELL_DIM);
    v3 box_max = V3(cell) * ASTART_CELL_DIM + 0.5f * V3(ASTART_CELL_DIM);

    v3 result;
    result.x = (p.x >= box_min.x && p.x <= box_max.x
                    ? p.x
                    : (p.x <= box_min.x ? box_min.x : box_max.x));
    result.y = (p.y >= box_min.y && p.y <= box_max.y
                    ? p.y
                    : (p.y <= box_min.y ? box_min.y : box_max.y));
    result.z = (p.z >= box_min.z && p.z <= box_max.z
                    ? p.z
                    : (p.z <= box_min.z ? box_min.z : box_max.z));
    return result;
}

void render_cell(v3i x, float s = 1, v3 color = V3(1, 1, 0)) {
    // push_cube_outline(V3(x) * ASTART_CELL_DIM, V3(ASTART_CELL_DIM*0.5f * s),
    // color);
}

int cell_dist(v3i a, v3i b) {
    return roundf(10.f *
                  sqrtf((b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y) +
                        (b.z - a.z) * (b.z - a.z)));
}

#define MAX_JUMP_CELL_COUNT 4

void get_state_childs(World &world, State state, State *childs, int &count) {
    bool on_ground =
        world.occupied.count(pack_cell(state.p + V3i(0, 0, -1))) != 0;

    if (on_ground)
        state.jump = 0;
    count = 0;

    if (state.jump == -1) {
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++) {
                State s = state;

                s.p = s.p + V3i(dx, dy, -1);
                childs[count++] = s;
            }
        return;
    }

    if (!on_ground) {
        // for (int dx = -1; dx <= 1; dx++)
        // for (int dy = -1; dy <= 1; dy++) {
        // 	State s = state;
        // 	s.p = s.p + V3i(0, 0, -1);
        // 	s.jump = -1;
        // 	childs[count++] = s;
        // }
        State s = state;
        s.p = s.p + V3i(0, 0, -1);
        s.jump = -1;
        childs[count++] = s;
    }
    if (state.jump < MAX_JUMP_CELL_COUNT) {
        for (int dx = -1; dx <= 1; dx++)
            for (int dy = -1; dy <= 1; dy++) {
                State s = state;
                s.p = s.p + V3i(dx, dy, +1);
                s.jump++;
                childs[count++] = s;
            }
    }
    for (int dx = -1; dx <= 1; dx++)
        for (int dy = -1; dy <= 1; dy++) {
            if (!dx && !dy)
                continue;
            State s = state;
            s.p = s.p + V3i(dx, dy, 0);
            if (!on_ground)
                s.jump = -1;
            else
                s.jump = 0;
            childs[count++] = s;
        }
}

v3 find_path_astar(World &world, Entity &e, v3 target_p) {
    v3i target_cell = get_cell(target_p);
    v3i start_cell = get_cell(e.position);

    State istate;
    istate.p = start_cell;
    if (!e.can_jump && e.dp.z < 0)
        istate.jump = -1;
    else {
        if (e.can_jump)
            istate.jump = 0;
        else {
            istate.jump =
                roundf((e.position.z - e.last_jump_z) / ASTART_CELL_DIM);
        }
        if (istate.jump < 0)
            istate.jump = 0;
    }
    Arena *temp = begin_temp_memory();

    std::unordered_map<State, int, StateHasher> visited;
    std::unordered_map<State, int, StateHasher> gScore;
    std::unordered_map<State, State, StateHasher> parent;

    gScore[istate] = 0;
    istate.fscore = cell_dist(istate.p, target_cell);

    State best_cell = istate;
    float best_length_sq = istate.fscore;

    std::set<State> set;
    set.insert(istate);
    visited[istate] = true;

    const int max_iterations = 4096;

    for (int itr = 0; itr < max_iterations && !set.empty(); itr++) {
        auto state = *set.begin();

        set.erase(set.begin());

        if (state.p.x == target_cell.x && state.p.y == target_cell.y &&
            state.p.z == target_cell.z) {
            // break ;
        }
        // render_cell(state.p, 0.2);

        int state_score = gScore[state];
        State childs[36];
        int child_count;

        get_state_childs(world, state, childs, child_count);
        for (int i = 0; i < child_count; i++) {
            if (!world.occupied.count(pack_cell(childs[i].p))) {
                //					v3 p =
                // get_closest_point_in_cell(childs[i].p, t
                int tentative_gScore =
                    state_score + cell_dist(childs[i].p, state.p); //+1
                if (!gScore.count(childs[i]) ||
                    tentative_gScore < gScore[childs[i]]) {
                    parent[childs[i]] = state;
                    gScore[childs[i]] = tentative_gScore;
                    childs[i].fscore =
                        tentative_gScore + cell_dist(childs[i].p, target_cell);
                    set.insert(childs[i]);
                }
                if (cell_dist(childs[i].p, target_cell) < best_length_sq) {
                    best_length_sq = cell_dist(childs[i].p, target_cell);
                    best_cell = childs[i];
                }
            }
        }
    }
    v3 result_dir = {};
    if (parent.count(best_cell)) {
        auto path = make_array_max<v3i>(temp, 1024);

        State curr = best_cell;

        while (1) {
            path.push(curr.p);
            if (!parent.count(curr))
                break;
            curr = parent[curr];
        }
        for (int i = 0; i < path.count; i++) {
            v3 color = V3(0, 1, 0);
            if (world.occupied.count(pack_cell(path[i])))
                color = V3(1, 0, 0);
            render_cell(path[i], 0.9f, V3(0, 1, 0));
        }

        // if (path[path.count - 2].z > start_cell.z)
        //     jump = true;
        result_dir = V3(path[path.count - 2]) * ASTART_CELL_DIM -
                     V3(path[path.count - 1]) * ASTART_CELL_DIM;
    } else
        result_dir = target_p - e.position;
    end_temp_memory();
    return result_dir;
}