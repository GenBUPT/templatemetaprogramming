#include<memory>
#include<type_traits>
namespace NSVarTypeDict
{
	struct NullParameter;
	template<size_t N, template<typename...> class TCont, typename ...T>
	struct Create_
	{
		using type = typename Create_<N - 1, TCont, NullParameter, T...>::type;
	};
	template<template<typename...> class TCont, typename...T>
	struct Create_<0, TCont, T...>
	{
		using type = TCont<T...>;
	};
}
namespace NSMultiTypeDict
{
	/*
	* 
	* 简单的递归循环检查变量类型是否匹配
	*/
	template<typename...tags>
	constexpr size_t Tag2ID = 0;
	template<typename TTag, typename tag, typename...tags>
	constexpr size_t Tag2ID<TTag, tag, tags...> = std::is_same<TTag, tag>() ? 0 : Tag2ID<TTag, tags...>+1;

	template<typename TVal, size_t N, size_t M, \
		typename TProcessedTypes, \
		typename...TRemainTypes>
		struct NewTupleType_;



	template<typename TVal, size_t N, size_t M, \
		template<typename...> class TCont, \
		typename...TModifiedTypes,
		typename TCurType,
		typename...TRemainTypes>
		struct NewTupleType_<TVal, N, M, TCont<TModifiedTypes...>, \
		TCurType, TRemainTypes...>
	{
		using type = typename NewTupleType_<TVal, N, M + 1, TCont<TModifiedTypes..., TCurType>, TRemainTypes...>::type;
	};




	template<typename TVal, size_t N, template<typename...> class TCont, \
		typename...TModifiedTypes, \
		typename TCurType,
		typename... TRemainTypes>
		struct NewTupleType_<TVal, N, N, TCont<TModifiedTypes...>, \
		TCurType, TRemainTypes...>
	{
		using type = TCont<TModifiedTypes..., TVal, TRemainTypes...>;
	};

	template<typename TVal, size_t TagPos, typename TCont, typename...TRemainTypes>
	using NewTupleType = typename NewTupleType_<TVal, TagPos, 0, TCont, TRemainTypes...>::type;


	template<size_t pos, size_t cur, \
		typename TProcessedTypes, \
		typename...TRemainTypes>
		struct GetTupleType_;

	template<size_t pos, size_t cur, \
		template<typename...>class TCont, \
		typename...TModifiedTypes, \
		typename TCurType, \
		typename...TRemainTypes>
		struct GetTupleType_<pos, cur, TCont<TModifiedTypes...>, \
		TCurType, TRemainTypes...>
	{
		using type = typename GetTupleType_<pos, cur + 1, TCont<TModifiedTypes..., TCurType>, TRemainTypes...>::type;
	};

	template<size_t pos, template<typename...>class TCont, \
		typename...TModifiedTypes, \
		typename TCurType,
		typename...TRemainTypes>
		struct GetTupleType_<pos, pos, TCont<TModifiedTypes...>, \
		TCurType, TRemainTypes...>
	{
		using type = TCurType;
	};
	template<size_t pos, size_t cur, typename TCont, typename...TRemainTypes>
	using GetTupleType = typename GetTupleType_<pos, cur, TCont, TRemainTypes...>::type;

}
template <typename ...TParameters>
struct VarTypeDict
{
	template<typename ... TTypes>
	struct Values
	{
		Values() = default;
		Values(std::shared_ptr<void>(&& input)[sizeof...(TTypes)])
		{
			for (size_t i = 0; i < sizeof...(TTypes); ++i)
			{
				m_tuple[i] = std::move(input[i]);
			}
		}
		template<typename TTag, typename TVal>
		auto Set(TVal&& val)&&
		{
			using namespace NSMultiTypeDict;
			constexpr static size_t TagPos = Tag2ID<TTag, TParameters...>;
			using rawVal = std::decay_t<TVal>;
			rawVal* tmp = new rawVal(std::forward<TVal>(val));
			m_tuple[TagPos] = std::shared_ptr<void>(tmp, [](void* ptr)
				{
					rawVal* nptr = static_cast<rawVal*>(ptr);
					delete nptr;
				});
			using new_type = NewTupleType<rawVal, TagPos, Values<>, TTypes...>;
			return new_type(std::move(m_tuple));
		}
		template<typename TTag>
		const auto& Get() const
		{
		/*
		* 借鉴NewTupleType的思路，通过TCont来区分已经遍历过的变量类型、当前的变量类型和未遍历的变量类型
		*
		*/
			using namespace NSMultiTypeDict;
			const static size_t TagPos = Tag2ID<TTag, TParameters...>;
			using type = GetTupleType<TagPos, 0, Values<>, TTypes...>;
			type* tmp = (type*)m_tuple[TagPos].get();
			return *tmp;
		}
	private:
		std::shared_ptr<void> m_tuple[sizeof...(TTypes)];
	};
	static auto Create()
	{
		using namespace NSVarTypeDict;
		using type = typename Create_<sizeof...(TParameters), Values>::type;
		return type{};
	}

};