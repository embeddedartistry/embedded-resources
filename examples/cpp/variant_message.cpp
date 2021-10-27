#include <cstdio>
#include <variant>
#include <array>
#include <string_view>

// I rearranged name/value here to give the prototype a more reasonable representation
struct fieldInfo_t
{
	std::string_view name;
	std::variant<int, float, char> value;
};

struct genericMsg_t
{
	static constexpr int MAX_FIELDS = 10;
	std::string_view msgName;
	int numFields;
	std::array<fieldInfo_t, MAX_FIELDS> fields;
};

static_assert(std::is_trivially_copyable_v<genericMsg_t>,
			  "DANGER! message type is not serializable!");

// Takes raw parameters or fieldInfo_t
template<typename... T>
constexpr genericMsg_t make_genericMsg(std::string_view name, T... values)
{
	// If we're all fieldInfo_t values, sizeof() is the # of parameters
	// Else, assume raw values are used and divide by 2
	// I don't know how to do this count with a mix of values or fieldInfo_t structs
	using TisFieldInfo = std::conjunction<std::is_same<T, fieldInfo_t>...>;
	constexpr std::size_t count = TisFieldInfo::value ? sizeof...(T) : sizeof...(T) >> 1;

	// Clang warns us about missing braces with values... here. But this will break
	// initialization in the temp1 example.
	return genericMsg_t{name, count, {{values...}}};
}

int main(int argc, char* argv[])
{
	constexpr int NUM_MESSAGES = 2;

	// We can call our make_ function with raw values
	constexpr genericMsg_t temp1 = make_genericMsg("Temp msg", "Temp", 4.5f, "Samples", 10);

	// Or we can pass field info structs as elements
	constexpr genericMsg_t loc1 = make_genericMsg(
		"Loc msg", fieldInfo_t{.name = "Lat", .value = 33.4567f},
		fieldInfo_t{.name = "N/S", .value = 'N'}, fieldInfo_t{.name = "Long", .value = 124.8724f},
		fieldInfo_t{.name = "E/W", .value = 'E'});

	constexpr std::array<genericMsg_t, NUM_MESSAGES> messages{temp1, loc1};

	for(const auto& message: messages)
	{
		// The printf %.*s format string is used because we can't get a standard C string out of
		// std::string_view
		printf("---%.*s---\n", static_cast<int>(message.msgName.size()), message.msgName.data());
		for(int field_idx = 0; field_idx < message.numFields; ++field_idx)
		{
			const auto thisField = message.fields[field_idx];
			printf("%.*s: ", static_cast<int>(thisField.name.size()), thisField.name.data());
			std::visit(
				[](auto&& v) {
					using T = std::decay_t<decltype(v)>;
					if constexpr(std::is_same_v<T, int>)
						printf("%d\n", v);
					else if constexpr(std::is_same_v<T, float>)
						printf("%f\n", v);
					else if constexpr(std::is_same_v<T, char>)
						printf("%c\n", v);
				},
				thisField.value);
		}
		printf("\n");
	}
}
