#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "split.hpp"

using namespace std;
using namespace physycom;

string line;
string separators;
std::vector<std::string> tokens, result;

TEST_CASE( "Splitting function" ) {
	line = "string_to_split";
	separators = "_";
    SECTION( "Simple case, compress off, single separator\nstring = " + line + "\nseparators = " + separators ) {
		result = vector<string>({"string", "to", "split"});
		split(tokens, line, separators);

		REQUIRE( tokens.size() == result.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == result[i] );		
		}
	}
	line = "__string_to__split__";
	separators = "_";
    SECTION( "Empty tokens, compress off, single separator\nstring = " + line + "\nseparators = " + separators ) {
		result = vector<string>({"", "", "string", "to", "", "split", "", ""});
		split(tokens, line, separators);

		REQUIRE( tokens.size() == result.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == result[i] );		
		}
    }
	line = "string_to_split";
	separators = "_";
    SECTION( "Simple case, compress on, single separator\nstring = " + line + "\nseparators = " + separators ) {
		result = vector<string>({"string", "to", "split"});
		split(tokens, line, separators, token_compress_on);

		REQUIRE( tokens.size() == result.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == result[i] );		
		}
	}
	line = "__string_to__split__";
	separators = "_";
    SECTION( "Empty tokens, compress on, single separator\nstring = " + line + "\nseparators = " + separators ) {
		result = vector<string>({"string", "to", "split"});
		split(tokens, line, separators, token_compress_on);

		REQUIRE( tokens.size() == result.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == result[i] );		
		}
	}
}
