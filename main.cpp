#include <iostream>
#include <source_location>
#include <stdint.h>
#include <vector>
struct Person
{
	int age;

	Person()
	{
		std::cout << "Hello World";
	}
};

template<auto ptr>
const char* getFunctionSig()
{
	return std::source_location::current().function_name();
}


template<typename T>
struct Fields
{
#ifdef __GNUC__
	static inline union {
		T obj;
		std::aligned_storage<sizeof(Person), alignof(Person)> wildcard{};
	} value;
	static consteval auto& getObj()
	{
		return value.obj;
	}
#endif // __GNUC__

#ifdef _MSC_VER
	static inline std::aligned_storage<sizeof(Person), alignof(Person)> obj;
	static consteval auto& getObj()
	{
		return obj;
	}
#endif // MSVC
};


template<typename T>
concept FieldConcept = requires(T & obj) { [](T& obj) { const auto& [a] = obj; }(obj); };
template<uint32_t index, FieldConcept T>
static consteval auto getField()
{
	const auto& [a] = (T&)Fields<T>::getObj();
	return &std::get<index>(std::tie(a));
}
static consteval auto fieldCount()
{
	return 1;
}

template<uint32_t index, typename T>
void addFields(std::vector<const char*>& names)
{
	if constexpr (fieldCount() > index)
	{
		names.push_back(getFunctionSig<getField<index, T>()>());
		addFields<index + 1, T>(names);
	}
}

template<typename T>
void getFields(std::vector<const char*>& names)
{
	addFields<0, T>(names);
}

int main()
{
	std::vector<const char*> names;
	getFields<Person>(names);
	for (auto curr : names)
		std::cout << curr << "\n";
}
