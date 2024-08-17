
struct State
{
	v3i p;
	// -1 => can't jump unless i'm on the ground
	// 0 
	int jump;
	int fscore;

	bool operator==(const State &rhs) const;
    bool operator<(const State &rhs) const;

};

struct StateHasher
{
    std::size_t operator()(const State& k) const;
};

#define ASTART_CELL_DIM (0.8)
