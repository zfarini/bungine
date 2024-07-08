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
Array<T> make_array_max(usize capacity)
{
	Array<T> result;

	result.count = 0;
	result.capacity = capacity;
	result.data = (T *)malloc(capacity * sizeof(T));
	return result;
}

template<typename T>
Array<T> make_array(usize count, T *data = 0)
{
	Array<T> result = make_array_max<T>(count);

	result.count = count;
	if (data)
		memcpy(result.data, data, count * sizeof(T));
	return result;
}

struct String
{
	char 	*data;
	usize size;

	String()
	{
		size = 0;
		data = 0;
	}
	String(const char *c_str)
	{
		data = (char *)c_str;
		size = 0;
		while (c_str[size])
			size++;
	}

	bool operator==(const String &other)
	{
		if (size != other.size)
			return false;
		for (usize i = 0; i < size; i++)
			if (data[i] != other.data[i])
				return false;
		return true;
	}
};

String StringDup(const char *c_str)
{
	String s;

	s.size = 0;
	while (c_str[s.size])
		s.size++;
	s.data = (char *)malloc(s.size);
	for (usize i = 0; i < s.size; i++)
		s.data[i] = c_str[i];
	return s;
}

#define str_format(s) (int)s.size, s.data

