#ifndef COLOURER_H
#define COLOURER_H

namespace nbody
{
	enum class ColourerType
	{
		SOLID,
		VELOCITY,
		N_TYPES,
		INVALID = -1
	};

	struct ColourerProperties
	{
		ColourerProperties(ColourerType type, char const* name, char const* tooltip, char cols_used)
			: type(type),
			  name(name),
			  tooltip(tooltip),
			  cols_used(cols_used)
		{
		}

		ColourerType const type;
		char const* name;
		char const* tooltip;
		char cols_used;
	};

	size_t constexpr MAX_COLS_PER_COLOURER = 2;

	class Colourer
	{

	};
}

#endif // COLOURER_H