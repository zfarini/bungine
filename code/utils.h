template <typename T> struct Array {
    T *data;
    usize count;
    usize capacity;

    T &operator[](int index) {
        assert(index >= 0 && index < count);
        return data[index];
    }

    T &operator[](usize index) {
        assert(index < count);
        return data[index];
    }

    void push(const T &value) {
        assert(count < capacity);
        data[count++] = value;
    }

    T &back() {
        assert(count);
        return data[count - 1];
    }

};

template <typename T> Array<T> make_array_max(Arena *arena, usize capacity) {
    Array<T> result;

    result.count = 0;
    result.capacity = capacity;
    result.data = (T *)arena_alloc(arena, capacity * sizeof(T));
    return result;
}

template <typename T>
Array<T> make_array(Arena *arena, usize count, const T *data = 0) {
    Array<T> result = make_array_max<T>(arena, count);

    result.count = count;
    if (data && count)
        memcpy(result.data, data, count * sizeof(T));
    return result;
}

template <typename T> Array<T> make_zero_array(Arena *arena, usize count) {
    Array<T> result = make_array<T>(arena, count);
    memset(result.data, 0, sizeof(T) * count);

    return result;
}

template <typename T> Array<T> make_array(T *data, usize count) {
    Array<T> result;

    result.capacity = result.count = count;
    result.data = data;
    return result;
}

// TODO: what if we have an array of strings?
template <typename T> Array<T> clone_array(Arena *arena, Array<T> &array) {
    Array<T> result;

    result.data = (T *)arena_alloc(arena, array.capacity * sizeof(T));
    memcpy(result.data, array.data, sizeof(T) * array.count);
    result.count = array.count;
    result.capacity = array.capacity;
    return result;
}

struct String {
    char *data;
    usize count;

    String();
    String(const char *cstr);
    char &operator[](int index);
    bool operator==(const String &other) const;
};

struct StringHasher {
    std::size_t operator()(const String& s) const
    {
        // TODO: pushcleanup
        std::size_t hash = 5381;
        for (int i = 0; i < s.count; ++i) {
            hash = ((hash << 5) + hash) + s.data[i];
        }
        return hash;
    }
};

#define str_format(str) (int)str.count, str.data

enum LogType {
	LOG_TYPE_DEBUG,
	LOG_TYPE_INFO,
	LOG_TYPE_WARN,
	LOG_TYPE_ERROR,
	LOG_TYPE_FATAL
};

// TODO: cleanup
#define LOG_FILENAME "build/log.txt"
// TODO: this should work on multiple threads!
void _log(LogType log_type, const char *fmt, va_list args1, va_list args2)
{
	static FILE *log_file = fopen(LOG_FILENAME, "w");

	const char *log_type_str[] = {
		"debug",
		"info",
		"warn",
		"error",
		"fatal"
	};
	const char *log_type_color[] = {
		"\033[032m",
		"\033[035m",
		"\033[033m",
		"\033[031m",
		"\033[031m",
	};
	printf("%s[%s]:\033[0m ", log_type_color[log_type], log_type_str[log_type]);
	vprintf(fmt, args1);
	printf("\n");

	fprintf(log_file, "[%s]: ", log_type_str[log_type]);
	vfprintf(log_file, fmt, args2);
	fprintf(log_file, "\n");
}

#define DEFINE_LOG_TYPE(name) void LOG_##name(const char *fmt, ...) {\
		va_list args1, args2; \
		va_start(args1, fmt); \
		va_start(args2, fmt); \
		_log(LOG_TYPE_##name, fmt, args1, args2); \
		va_end(args1); \
		va_end(args2); \
	}

DEFINE_LOG_TYPE(DEBUG);
DEFINE_LOG_TYPE(INFO);
DEFINE_LOG_TYPE(WARN);
DEFINE_LOG_TYPE(ERROR);
DEFINE_LOG_TYPE(FATAL);
