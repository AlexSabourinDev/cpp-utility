#pragma once

#include "../common/UtilityMacros.h"
#include <memory>
#include <utility>

MIST_NAMESPACE

namespace Detail {
	class AnyImpl;
}

// The Any class holds a variable of any type.
// To retrieve the value however, the type requested must match the type of the value exactly.
class Any {

public:

	// -Public API-

	// Attempt to cast the value within the Any
	template< typename CastType >
	friend bool TryCast(const Any& any, CastType** cast);

	// Hard cast the value
	// @Detail: Returns null if the type differs
	template< typename CastType >
	friend CastType* Cast(const Any& any);


	// Set the any value
	// @Detail: overrides the previous value set
	template< typename NewType >
	void Set(NewType type);

	// Determine if the inner value matches the ValueType
	template< typename ValueType >
	bool Is();


	// -Structors-

	// Initialize to a value
	template< typename ValueType >
	Any(ValueType value);

	// Initialize to nothing, allowing for later setting
	Any();
	
	~Any() = default;

	Any(Any&) = delete;
	Any& operator=(Any&) = delete;

	Any(Any&& move);
	Any& operator=(Any&& move);

private:

	std::unique_ptr<Detail::AnyImpl> m_Data;

};


namespace Detail {

	class AnyImpl {
	public:
		virtual ~AnyImpl() {}
	};

	template< typename DataType >
	class AnyData : public AnyImpl {

		static_assert(std::is_reference<DataType>::value == false, "DataType must be value type");

	public:

		// -Public API-

		DataType* Get();

		// -Structors-

		explicit AnyData(DataType data);

		~AnyData() override = default;

	private:

		DataType m_Data;

	};

}

// -Implementation-

// Attempt to cast the value within the Any
template< typename CastType >
bool TryCast(const Any& any, CastType** cast) {

	if (any.Is<CastType>() == false) {
		return false;
	}

	*cast = dynamic_cast<Detail::AnyData<CastType>*>(any.m_Data.get())->Get();
	return true;
}

// Hard cast the value
// @Detail: Returns null if the type differs
template< typename CastType >
CastType* Cast(const Any& any) {

	return dynamic_cast<Detail::AnyData<CastType>*>(any.m_Data.get())->Get();
}

// Set the any value
// @Detail: overrides the previous value set
template< typename NewType >
void Any::Set(NewType type) {

	m_Data.reset(new Detail::AnyData<NewType>(type));
}

// Determine if the inner value matches the ValueType
template< typename ValueType >
bool Any::Is() {
	
	return dynamic_cast<Detail::AnyData<ValueType>*>(m_Data.get()) != nullptr;
}

// Initialize to a value
template< typename ValueType >
Any::Any(ValueType value) {
	
	Set(value);
}

// Initialize to nothing, allowing for later setting
Any::Any() : m_Data(nullptr) {}

Any::Any(Any&& move) : m_Data(std::move(move.m_Data)) {}

Any& Any::operator=(Any&& move) {

	m_Data = std::move(move.m_Data);
	return *this;
}

namespace Detail {

	template< typename DataType >
	typename DataType* AnyData<DataType>::Get() {

		return &m_Data;
	}

	template< typename DataType >
	AnyData<DataType>::AnyData(DataType data) : m_Data(data) {}

}

MIST_NAMESPACE_END
