#define Kilobyte(x) (1024ULL * x)
#define Megabyte(x) (1024ULL * Kilobyte(x))
#define GigaByte(x) (1024ULL * Megabyte(x))

template<typename T>
T min(T a, T b)
{
	return (a < b ? a : b);
}

template<typename T>
T max(T a, T b)
{
	return (a > b ? a : b);
}

template<typename T>
void swap(T &a, T &b)
{
	T tmp = a;

	a = b;
	b = tmp;
}

template<typename T>
struct Array
{
	T *data;
	usize count;
	usize capacity;

	Array()
	{
		data = 0;
		count = capacity = 0;
	}

	T &operator[](int index)
	{
		assert(index >= 0 && index < count);
		return data[index];
	}

	T &operator[](usize index)
	{
		assert(index < count);
		return data[index];
	}

	void push(const T &value)
	{
		assert(count < capacity);
		data[count++] = value;
	}

	T &back()
	{
		assert(count);
		return data[count - 1];
	}

	~Array()
	{
		//free(data);
	}
};


template<typename T>
Array<T> make_array_max(Arena *arena, usize capacity)
{
	Array<T> result;

	result.count = 0;
	result.capacity = capacity;
	result.data = (T *)arena_alloc(arena, capacity * sizeof(T));
	return result;
}

template<typename T>
Array<T> make_array(Arena *arena, usize count, const T *data = 0)
{
	Array<T> result = make_array_max<T>(arena, count);

	result.count = count;
	if (data)
		memcpy(result.data, data, count * sizeof(T));
	return result;
}

template<typename T>
Array<T> make_zero_array(Arena *arena, usize count)
{
	Array<T> result = make_array<T>(arena, count);
	memset(result.data, 0, sizeof(T) * count);
	return result;
}

// TODO: what if we have an array of strings?
template<typename T>
Array<T> clone_array(Arena *arena, Array<T> &array)
{
	Array<T> result;

	result.data = (T *)arena_alloc(arena, array.capacity * sizeof(T));
	memcpy(result.data, array.data, sizeof(T) * array.count);
	result.count = array.count;
	result.capacity = array.capacity;
	return result;
}



// template<>
// bool operator==(const Array<char> &a, const Array<char> &b)
// {
// 	return true;
// }

using String = Array<char>;
String make_string(Arena *arena, usize count, const char *data = 0) {return make_array<char>(arena, count, data);}
b32 strings_equal(const String &a, const String &b)
{
	if (a.count != b.count)
		return false;
	for (usize i = 0; i < a.count; i++)
		if (a.data[i] != b.data[i])
			return false;
	return true;
}

String concact_string(Arena *arena, String a, String b)
{
	String result = make_string(arena, a.count + b.count);

	memcpy(result.data, a.data, a.count);
	memcpy(result.data + a.count, b.data, b.count);
	return result;
}
#define str_format(str) (int)str.count, str.data