/**
Librairie JSON simple
Lightweight JSON library
Version 2010.11

Copyright (c) 2010 Charly LERSTEAU

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef JSON_H
#define JSON_H

#include <string>
#include <vector>
#include <map>
#include <istream>
#include <exception>

class JSONValue;

typedef std::vector<JSONValue>          JSONArray;
typedef std::map<std::string,JSONValue> JSONObject;
typedef void *                          JSONNull;

/**
 * JSON Exception
 */
class JSONException : public std::exception
{
public:
	JSONException() : m_what( "JSON: Unknown exception." ) {}
	JSONException( const char * what ) : m_what( what ) {}
	const char * what() const throw() { return m_what; }
protected:
	const char * m_what;
};

/**
 * JSON Decode Exception
 */
class JSONDecodeException : public JSONException
{
public:
	JSONDecodeException( std::istream & in, const char * message );
private:
	char m_text[ 128 ];
};

/**
 * JSON Value
 */
class JSONValue
{
public:
	JSONValue();
	JSONValue( bool b );
	JSONValue( int i );
	JSONValue( double d );
	JSONValue( const char * s );
	JSONValue( const std::string & s );
	JSONValue( const JSONArray & a );
	JSONValue( const JSONObject & o );
	JSONValue( const JSONValue & v );
	JSONValue( const JSONNull & n );

	bool isBoolean() const;
	bool isNumber()  const;
	bool isString()  const;
	bool isArray()   const;
	bool isObject()  const;
	bool isNull()    const;
	bool isInteger() const;
	bool isReal()    const;

	bool        asBool()   const;
	int         asInt()    const;
	double      asDouble() const;
	std::string asString() const;
	JSONArray   asArray()  const;
	JSONObject  asObject() const;

	const char * getType() const;

	JSONValue & operator= ( bool b );
	JSONValue & operator= ( int i );
	JSONValue & operator= ( double d );
	JSONValue & operator= ( const char * s );
	JSONValue & operator= ( const std::string & s );
	JSONValue & operator= ( const JSONArray & a );
	JSONValue & operator= ( const JSONObject & o );
	JSONValue & operator= ( const JSONNull & n );

	JSONValue& operator>> ( bool & b );
	JSONValue& operator>> ( int & i );
	JSONValue& operator>> ( double & d );
	JSONValue& operator>> ( const char * & s );
	JSONValue& operator>> ( std::string & s );
	JSONValue& operator>> ( JSONArray & a );
	JSONValue& operator>> ( JSONObject & o );
	JSONValue& operator>> ( JSONValue & v );

	JSONValue & operator[] ( int i );
	JSONValue & operator[] ( const std::string & s );

	JSONValue& operator<< ( const JSONValue & v );

	int size() const;
	bool empty() const;
	void insert( int i, const JSONValue & v );
	void push_back( const JSONValue & v );
	void erase( int i );
	bool erase( const std::string & key );
	void clear();

	void decode( const std::string & json );
	std::string encode() const;

	void load( const std::string & file );
	void save( const std::string & file ) const;

private:
	typedef enum
	{
		JSON_UNDEFINED,
		JSON_NULL,
		JSON_BOOLEAN,
		JSON_INTEGER,
		JSON_REAL,
		JSON_STRING,
		JSON_ARRAY,
		JSON_OBJECT
	}
	JSONType;

	struct JSONTolower
	{
		char operator() ( char c ) const
		{
			return tolower( static_cast<unsigned char>( c ) );
		}
	};

	JSONType    m_type;
	bool        m_boolean;
	int         m_integer;
	double      m_real;
	std::string m_string;
	JSONArray   m_array;
	JSONObject  m_object;

	void decode      ( std::istream & in );
	void decodeString( std::istream & in );
	void decodeArray ( std::istream & in );
	void decodeObject( std::istream & in );
	void decodeValue ( std::istream & in );
	void encode      ( std::ostream & out ) const;

	std::string escape( std::string s ) const;
	std::string strtolower( std::string s ) const;
	bool isdelim( const char c ) const;
};

/**
 * JSON
 */
class JSON
{
public:
	static JSONValue decode( const std::string & json );
	static std::string encode( const JSONValue & value );
	static JSONValue load( const std::string & file );
	static void save( const JSONValue & value, const std::string & file );
	static const JSONNull null;
};

#endif

