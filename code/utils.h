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
struct DArray
{
	T *data;
	usize count;
	usize capacity;

	DArray()
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

	void push(T value)
	{
		if (count == capacity)
		{
			capacity = max(2, capacity + (capacity / 2));
			data = (T *)realloc(data, capacity * sizeof(T));
		}
		data[count++] = value;
	}

	void pop()
	{
		assert(count);
		count--;
	}

	T &back()
	{
		assert(count);
		return data[count - 1];
	}

	~DArray()
	{
		//free(data);
	}
};


template<typename T>
DArray<T> make_darray(usize count, T *data = 0)
{
	DArray<T> result;

	result.count = count;
	result.capacity = count;
	result.data = (T *)malloc(count * sizeof(T));
	if (data)
		memcpy(result.data, data, count * sizeof(T));
	return result;
}

struct String
{
	char 	*data;
	uint32_t size;
};