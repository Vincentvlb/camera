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

#include "json.h"
#include <algorithm>
#include <sstream>
#include <fstream>

using namespace std;

/* Null value */
const JSONNull JSON::null = (void *)0;

/* Constructors */
JSONValue::JSONValue()                       : m_type( JSON_UNDEFINED ) {}
JSONValue::JSONValue( bool b )               : m_type( JSON_BOOLEAN ), m_boolean( b ) {}
JSONValue::JSONValue( int i )                : m_type( JSON_INTEGER ), m_integer( i ) {}
JSONValue::JSONValue( double d )             : m_type( JSON_REAL    ), m_real   ( d ) {}
JSONValue::JSONValue( const char * s )       : m_type( JSON_STRING  ), m_string ( s ) {}
JSONValue::JSONValue( const string & s )     : m_type( JSON_STRING  ), m_string ( s ) {}
JSONValue::JSONValue( const JSONArray & a )  : m_type( JSON_ARRAY   ), m_array  ( a ) {}
JSONValue::JSONValue( const JSONObject & o ) : m_type( JSON_OBJECT  ), m_object ( o ) {}
JSONValue::JSONValue( const JSONNull & n )   : m_type( JSON_NULL    ) {}
JSONValue::JSONValue( const JSONValue & v ) :
	m_type   ( v.m_type    ),
	m_boolean( v.m_boolean ),
	m_integer( v.m_integer ),
	m_real   ( v.m_real    ),
	m_string ( v.m_string  ),
	m_array  ( v.m_array   ),
	m_object ( v.m_object  ) {}

/* Type checkers */
bool JSONValue::isBoolean() const { return ( m_type == JSON_BOOLEAN ); }
bool JSONValue::isNumber()  const { return ( m_type == JSON_INTEGER || m_type == JSON_REAL  ); }
bool JSONValue::isString()  const { return ( m_type == JSON_STRING  ); }
bool JSONValue::isArray()   const { return ( m_type == JSON_ARRAY   ); }
bool JSONValue::isObject()  const { return ( m_type == JSON_OBJECT  ); }
bool JSONValue::isNull()    const { return ( m_type == JSON_NULL    ); }
bool JSONValue::isInteger() const { return ( m_type == JSON_INTEGER ); }
bool JSONValue::isReal()    const { return ( m_type == JSON_REAL    ); }

/* Extraction */
bool       JSONValue::asBool()   const { if ( m_type == JSON_BOOLEAN ) return m_boolean; else throw JSONException( "JSON: Value is not boolean." ); }
int        JSONValue::asInt()    const { if ( m_type == JSON_INTEGER ) return m_integer; else throw JSONException( "JSON: Value is not integer." ); }
double     JSONValue::asDouble() const { if ( m_type == JSON_REAL )  return m_real;
				else if ( m_type == JSON_INTEGER ) return (double)m_integer;
				else throw JSONException( "JSON: Value is not double." ); }
string     JSONValue::asString() const { if ( m_type == JSON_STRING )  return m_string;  else throw JSONException( "JSON: Value is not string." );  }
JSONArray  JSONValue::asArray()  const { if ( m_type == JSON_ARRAY  )  return m_array;   else throw JSONException( "JSON: Value is not array." );   }
JSONObject JSONValue::asObject() const { if ( m_type == JSON_OBJECT )  return m_object;  else throw JSONException( "JSON: Value is not object." );  }

/* Type name */
const char * JSONValue::getType() const
{
	switch ( m_type )
	{
		case JSON_NULL:
			return "null";
		case JSON_BOOLEAN:
			return "boolean";
		case JSON_INTEGER:
		case JSON_REAL:
			return "number";
		case JSON_STRING:
			return "string";
		case JSON_ARRAY:
			return "array";
		case JSON_OBJECT:
			return "object";
		case JSON_UNDEFINED:
		default:
			return "undefined";
	}
}

/* Assignment */
JSONValue & JSONValue::operator= ( bool b )               { clear(); m_type = JSON_BOOLEAN; m_boolean = b; return *this; }
JSONValue & JSONValue::operator= ( int i )                { clear(); m_type = JSON_INTEGER; m_integer = i; return *this; }
JSONValue & JSONValue::operator= ( double d )             { clear(); m_type = JSON_REAL;    m_real    = d; return *this; }
JSONValue & JSONValue::operator= ( const char * s )       { clear(); m_type = JSON_STRING;  m_string  = s; return *this; }
JSONValue & JSONValue::operator= ( const string & s )     { clear(); m_type = JSON_STRING;  m_string  = s; return *this; }
JSONValue & JSONValue::operator= ( const JSONArray & a )  { clear(); m_type = JSON_ARRAY;   m_array   = a; return *this; }
JSONValue & JSONValue::operator= ( const JSONObject & o ) { clear(); m_type = JSON_OBJECT;  m_object  = o; return *this; }
JSONValue & JSONValue::operator= ( const JSONNull & n )   { clear(); m_type = JSON_NULL;                   return *this; }

/* Extraction */
JSONValue& JSONValue::operator>> ( bool & b )         { b = m_boolean;          return *this; }
JSONValue& JSONValue::operator>> ( int & i )          { i = m_integer;          return *this; }
JSONValue& JSONValue::operator>> ( double & d )       { d = m_real;             return *this; }
JSONValue& JSONValue::operator>> ( const char * & s ) { s = m_string.c_str();   return *this; }
JSONValue& JSONValue::operator>> ( string & s )       { s = m_string;           return *this; }
JSONValue& JSONValue::operator>> ( JSONArray & a )    { a = m_array;            return *this; }
JSONValue& JSONValue::operator>> ( JSONObject & o )   { o = m_object;           return *this; }
JSONValue& JSONValue::operator>> ( JSONValue & v )    { v = JSONValue( *this ); return *this; }

/* Insertion */
JSONValue& JSONValue::operator<< ( const JSONValue & v ) { push_back( v ); return *this; }

/* Array access */
JSONValue & JSONValue::operator[] ( int i )
{
	if ( m_type == JSON_UNDEFINED ) m_type = JSON_ARRAY;
	if ( m_type == JSON_ARRAY ) return m_array.at( i );
	else throw JSONException( "JSON: Value is not array." );
}

/* Object access */
JSONValue & JSONValue::operator[] ( const string & s )
{
	if ( m_type == JSON_UNDEFINED ) m_type = JSON_OBJECT;
	if ( m_type == JSON_OBJECT ) return m_object[ s ];
	else throw JSONException( "JSON: Value is not object." );
}

/* Array or object size */
int JSONValue::size() const
{
	switch ( m_type )
	{
		case JSON_ARRAY:
			return m_array.size();
		case JSON_OBJECT:
			return m_object.size();
		default:
			throw JSONException( "JSON: Value is not array or object." );
	}
}

/* Empty value */
bool JSONValue::empty() const
{
	switch ( m_type )
	{
		case JSON_BOOLEAN:
			return ( m_boolean == false );
		case JSON_INTEGER:
			return ( m_integer == 0 );
		case JSON_REAL:
			return ( m_real == 0.0 );
		case JSON_STRING:
			return m_string.empty();
		case JSON_ARRAY:
			return m_array.empty();
		case JSON_OBJECT:
			return m_object.empty();
		case JSON_NULL:
		case JSON_UNDEFINED:
		default:
			return true;
	}
}

/* Array insert */
void JSONValue::insert( int i, const JSONValue & v )
{
	switch ( m_type )
	{
		case JSON_UNDEFINED:
			m_type = JSON_ARRAY;
		case JSON_ARRAY:
			m_array.insert( m_array.begin() + i, v );
			break;
		default:
			throw JSONException( "JSON: Value is not array." );
	}
}

/* Array append */
void JSONValue::push_back( const JSONValue & v )
{
	switch ( m_type )
	{
		case JSON_UNDEFINED:
			m_type = JSON_ARRAY;
		case JSON_ARRAY:
			m_array.push_back( v );
			break;
		default:
			throw JSONException( "JSON: Value is not array." );
	}
}

/* Array element erase */
void JSONValue::erase( int i )
{
	if ( m_type == JSON_ARRAY )
		m_array.erase( m_array.begin() + i );
	else
		throw JSONException( "JSON: Value is not array." );
}

/* Object child erase */
bool JSONValue::erase( const string & key )
{
	if ( m_type == JSON_OBJECT )
		return m_object.erase( key );
	else
		throw JSONException( "JSON: Value is not object." );
}

/* Value clear */
void JSONValue::clear()
{
	m_boolean = false;
	m_integer = 0;
	m_real = 0.0;
	m_string.clear();
	m_array.clear();
	m_object.clear();
}

/* Decode from string */
void JSONValue::decode( const string & json )
{
	istringstream in( json );
	decode( in );
}

/* Decode from stream */
void JSONValue::decode( istream & in )
{
	char c;

	clear();
	in >> skipws >> c;

	switch ( c )
	{
		case '"':
			decodeString( in );
			break;
		case '[':
			decodeArray( in );
			break;
		case '{':
			decodeObject( in );
			break;
		default:
			in.unget();
			decodeValue( in );
			break;
	}
}

void JSONValue::decodeString( istream & in )
{
	ostringstream out;
	char c;

	for ( in >> noskipws >> c; c != '"'; in >> c )
	{
		if ( c == '\\' )
			in >> c;
		out << c;
	}
	m_string = out.str();
	m_type = JSON_STRING;
}

void JSONValue::decodeArray( istream & in )
{
	char c;

	in >> skipws >> c;
	if ( c != ']' )
	{
		in.unget();
		for ( c = ','; c == ','; in >> skipws >> c )
		{
			JSONValue value;

			value.decode( in );
			m_array.push_back( value );
		}
		if ( c != ']' )
			throw JSONDecodeException( in, "']' excepted" );
	}
	m_type = JSON_ARRAY;
}

void JSONValue::decodeObject( istream & in )
{
	char c;

	in >> skipws >> c;
	if ( c != '}' )
	{
		in.unget();
		for ( c = ','; c == ','; in >> skipws >> c )
		{
			JSONValue value;
			ostringstream out;

			in >> skipws >> c;
			if ( c != '"' )
				throw JSONDecodeException( in, "'\"' excepted" );

			for ( in >> noskipws >> c; c != '"'; in >> c )
			{
				if ( c == '\\' )
					in >> c;
				out << c;
			}

			in >> skipws >> c;
			if ( c != ':' )
				throw JSONDecodeException( in, "':' excepted" );

			value.decode( in );
			m_object[ out.str() ] = value;
		}
		if ( c != '}' )
			throw JSONDecodeException( in, "'}' excepted" );
	}
	m_type = JSON_OBJECT;
}

void JSONValue::decodeValue( istream & in )
{
	string s;
	char c;

	for ( c = in.peek(); !isdelim( c ); c = in.peek() )
	{
		in >> noskipws >> c;
		s += c;
	}

	if ( strtolower( s ) == "null" )
	{
		m_type = JSON_NULL;
	}
	else
	{
		istringstream isub( s );
		if ( isub >> boolalpha >> m_boolean )
		{
			m_type = JSON_BOOLEAN;
		}
		else
		{
			isub.clear();
			isub.str( s );
			if ( isub >> m_integer && isub.eof() )
			{
				m_type = JSON_INTEGER;
			}
			else
			{
				isub.clear();
				isub.str( s );
				if ( isub >> m_real && isub.eof() )
					m_type = JSON_REAL;
				else
					throw JSONDecodeException( in, "syntax error" );
			}
		}
	}
}

/* Encode to string */
string JSONValue::encode() const
{
	ostringstream out;
	encode( out );
	return out.str();
}

/* Encode to ostream */
void JSONValue::encode( ostream & out ) const
{
	JSONObject::const_iterator it;
	unsigned int i;

	switch ( m_type )
	{
		case JSON_NULL:
			out << "null";
			break;
		case JSON_BOOLEAN:
			out << boolalpha << m_boolean;
			break;
		case JSON_INTEGER:
			out << m_integer;
			break;
		case JSON_REAL:
			out << m_real;
			break;
		case JSON_STRING:
			out << '"' << escape( m_string ) << '"';
			break;
		case JSON_ARRAY:
			out << '[';
			for ( i = 0; i < m_array.size(); i++ )
			{
				out << m_array.at( i ).encode();
				if ( i < m_array.size() - 1 )
					out << ",\n";
			}
			out << ']';
			break;
		case JSON_OBJECT:
			out << "{\n";
			i = 0;
			for ( it = m_object.begin(); it != m_object.end(); it++ )
			{
				out << "\"" << escape( (*it).first ) << "\": " << (*it).second.encode();
				if ( ++i < m_object.size() )
					out << ",\n";
			}
			out << "\n}";
			break;
		case JSON_UNDEFINED:
		default:
			break;
	}
}

/* Load from file */
void JSONValue::load( const string & file )
{
	ifstream in;

	in.open( file.c_str() );
	decode( in );
	in.close();
}

/* Save to file */
void JSONValue::save( const string & file ) const
{
	ofstream out;

	out.open( file.c_str() );
	encode( out );
	out.close();
}

/* Add slashes */
string JSONValue::escape( string s ) const
{
	string::size_type offset;

	offset = 0;
	while ( ( offset = s.find( '"', offset ) ) != string::npos )
	{
		s.insert( offset, "\\" );
		offset += 2;
	}
	return s;
}

/* Lower case */
string JSONValue::strtolower( string s ) const
{
	transform( s.begin(), s.end(), s.begin(), JSONTolower() );
	return s;
}

/* Delimiters */
bool JSONValue::isdelim( const char c ) const
{
	return isspace( c ) || c == ',' || c == ']' || c == '}';
}

/* Static decode */
JSONValue JSON::decode( const string & json )
{
	JSONValue value;

	value.decode( json );
	return value;
}

/* Static encode */
string JSON::encode( const JSONValue & value )
{
	return value.encode();
}

/* Static load */
JSONValue JSON::load( const string & file )
{
	JSONValue value;

	value.load( file );
	return value;
}

/* Static save */
void JSON::save( const JSONValue & value, const string & file )
{
	value.save( file );
}

JSONDecodeException::JSONDecodeException( istream & in, const char * message )
{
	char c;
	int pos, i = 0, line = 1;

	pos = in.tellg();
	in.clear();
	in.seekg( 0 );
	while ( ++i < pos )
	{
		in >> noskipws >> c;
		if ( c == '\n' ) line++;
	}

	sprintf( m_text, "JSON: decoding error: %s, line %d.", message, line );
	m_what = m_text;
}

