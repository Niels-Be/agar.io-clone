/*
 * File JSONValue.h part of the SimpleJSON Library - http://mjpa.in/json
 *
 * Copyright (C) 2010 Mike Anchor
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _JSONVALUE_H_
#define _JSONVALUE_H_

#include <vector>
#include <string>

#include "JSON.h"

class JSON;
namespace JSONTypes {
template< class T, class Enable = void >
struct TypeTraits;
}

enum JSONType { JSONType_Null, JSONType_String, JSONType_Bool, JSONType_Number, JSONType_Array, JSONType_Object };

class JSONValue
{
	friend class JSON;

	public:
		JSONValue(/*NULL*/);
		JSONValue(const char *m_char_value);
		JSONValue(const std::string &m_string_value);
		JSONValue(bool m_bool_value);
		JSONValue(double m_number_value);
		JSONValue(const JSONArray &m_array_value);
		JSONValue(const JSONObject &m_object_value);

		~JSONValue();

		bool IsNull() const;
		bool IsString() const;
		bool IsBool() const;
		bool IsNumber() const;
		bool IsArray() const;
		bool IsObject() const;

		const std::string &AsString() const;
		bool AsBool() const;
		double AsNumber() const;
		const JSONArray &AsArray() const;
		const JSONObject &AsObject() const;

		template<class T>
		void Cast(T& d) const {
			JSONTypes::TypeTraits<T>::toType(d, this);
		}

		template <class T>
		static JSONValue* From(const T& d) {
			return JSONTypes::TypeTraits<T>::toValue(d);
		}

		std::size_t CountChildren() const;
		bool HasChild(std::size_t index) const;
		JSONValue *Child(std::size_t index);
		bool HasChild(const char* name) const;
		JSONValue *Child(const char* name);
		std::vector<std::string> ObjectKeys() const;

		std::string Stringify(bool const prettyprint = false) const;

	protected:
		static JSONValue *Parse(const char **data);

	private:
		static std::string StringifyString(const std::string &str);
		std::string StringifyImpl(size_t const indentDepth) const;
		static std::string Indent(size_t depth);

		JSONType type;
		std::string string_value;
		bool bool_value;
		double number_value;
		JSONArray array_value;
		JSONObject object_value;
};


namespace JSONTypes {
template<bool T> using enable_if_t=typename std::enable_if<T>::type;

template<> struct TypeTraits<bool> {
	static JSONValue* toValue(const bool& d) {
		return new JSONValue(d);
	}
	static void toType(bool& d, const JSONValue* val) {
		assert(val->IsBool());
		d = val->AsBool();
	}
};
template<class T> struct TypeTraits<T, enable_if_t<std::is_arithmetic<T>::value> > {
	static JSONValue* toValue(const T& d) {
		return new JSONValue(static_cast<double>(d));
	}
	static void toType(T& d, const JSONValue* val) {
		assert(val->IsNumber());
		d = static_cast<T>(val->AsNumber());
	}
};
template<> struct TypeTraits<std::string> {
	static JSONValue* toValue(const std::string& d) {
		return new JSONValue(d);
	}
	static void toType(std::string& d, const JSONValue* val) {
		assert(val->IsString());
		d = val->AsString();
	}
};
template<class T> struct TypeTraits<T, enable_if_t<std::is_same<typename std::decay<T>::type, char*>::value> > {
	static JSONValue* toValue(const T& d) {
		return new JSONValue(d);
	}
	static void toType(T& d, const JSONValue* val) {
		assert(val->IsString());
		d = val->AsString().c_str();
	}
};
template <> struct TypeTraits<JSONArray> {
	static JSONValue* toValue(const JSONArray& d) {
		return new JSONValue(d);
	}
	static void toType(JSONArray& d, const JSONValue* val) {
		assert(val->IsArray());
		d = val->AsArray();
	}
};
template<class T> struct TypeTraits<T, enable_if_t<std::is_same<T, std::vector< typename T::value_type, typename T::allocator_type > >::value> > {
	static JSONValue* toValue(const T& d) {
		JSONArray arr;
		arr.reserve(d.size());
		for(auto& it : d)
			arr.push_back(JSONValue::From(it));
		return new JSONValue(arr);
	}
	static void toType(T& d, const JSONValue* val) {
		assert(val->IsArray());
		JSONArray arr = val->AsArray();
		d.reserve(arr.size());
		for(JSONValue* it : arr) {
			typename T::value_type data;
			it->Cast(data);
			d.push_back(data);
		}
	}
};
}


#define EVAL0(...) __VA_ARGS__
#define EVAL1(...) EVAL0 (EVAL0 (EVAL0 (__VA_ARGS__)))
#define EVAL2(...) EVAL1 (EVAL1 (EVAL1 (__VA_ARGS__)))
#define EVAL(...) EVAL2 (EVAL2 (EVAL2 (__VA_ARGS__)))
//#define EVAL(...) EVAL3 (EVAL3 (EVAL3 (__VA_ARGS__)))
//#define EVAL(...)  EVAL4 (EVAL4 (EVAL4 (__VA_ARGS__)))

#define MAP_END(...)
#define MAP_OUT

#define MAP_GET_END() 0, MAP_END
#define MAP_NEXT0(test, next, ...) next MAP_OUT
#define MAP_NEXT1(test, next) MAP_NEXT0 (test, next, 0)
#define MAP_NEXT(test, next)  MAP_NEXT1 (MAP_GET_END test, next)

#define MAP0(f, a1, a2, x, peek, ...) f(a1, a2, x) MAP_NEXT (peek, MAP1) (f, a1, a2, peek, __VA_ARGS__)
#define MAP1(f, a1, a2, x, peek, ...) f(a1, a2, x) MAP_NEXT (peek, MAP0) (f, a1, a2, peek, __VA_ARGS__)
#define MAP(f, a1, a2, ...) EVAL (MAP1 (f, a1, a2,  __VA_ARGS__, (), 0))


#define DECLARE_JSON_TO_VALUE_IMPL(obj, elem, entry) \
	obj[#entry] = JSONValue::From(elem . entry);

#define DECLARE_JSON_FROM_VALUE_IMPL(obj, elem, entry) \
	obj[#entry]->Cast(elem . entry);

#define DECLARE_JSON_TO_VALUE(obj, elem, ...) MAP(DECLARE_JSON_TO_VALUE_IMPL, obj, elem, __VA_ARGS__)
#define DECLARE_JSON_FROM_VALUE(obj, elem, ...) MAP(DECLARE_JSON_FROM_VALUE_IMPL, obj, elem, __VA_ARGS__)

#define DECLARE_JSON_STRUCT(Type, ...) \
namespace JSONTypes { \
template<> struct TypeTraits<Type> { \
static JSONValue* toValue(const Type& d) { \
	JSONObject obj; \
	DECLARE_JSON_TO_VALUE(obj, d, __VA_ARGS__) \
	return new JSONValue(obj); \
} \
static void toType(Type& d, const JSONValue* val) { \
	assert(val->IsObject()); \
	JSONObject obj = val->AsObject(); \
	DECLARE_JSON_FROM_VALUE(obj, d, __VA_ARGS__) \
}};}




#endif
