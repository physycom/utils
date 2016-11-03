#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"

#include "split.hpp"

using namespace std;
using namespace physycom;

string line;
string separators;
std::vector<std::string> tokens, right_tokens;

TEST_CASE( "Splitting function" ) {
	line = "string_to_split";
	separators = "_";
    SECTION( "Simple case, compress off, single separator\nstring = " + line + "\nseparators = " + separators ) {
		right_tokens = vector<string>({"string", "to", "split"});
		split(tokens, line, separators);

		REQUIRE( tokens.size() == right_tokens.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == right_tokens[i] );		
		}
	}
	line = "__string_to__split__";
	separators = "_";
    SECTION( "Empty tokens, compress off, single separator\nstring = " + line + "\nseparators = " + separators ) {
		right_tokens = vector<string>({"", "", "string", "to", "", "split", "", ""});
		split(tokens, line, separators);

		REQUIRE( tokens.size() == right_tokens.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == right_tokens[i] );		
		}
    }
	line = "string_to_split";
	separators = "_";
    SECTION( "Simple case, compress on, single separator\nstring = " + line + "\nseparators = " + separators ) {
		right_tokens = vector<string>({"string", "to", "split"});
		split(tokens, line, separators, token_compress_on);

		REQUIRE( tokens.size() == right_tokens.size() );
		REQUIRE( tokens[0] == right_tokens[0] );
		REQUIRE( tokens[1] == right_tokens[1] );
		REQUIRE( tokens[2] == right_tokens[2] );
	}
	line = "__string_to__split__";
	separators = "_";
    SECTION( "Empty tokens, compress on, single separator\nstring = " + line + "\nseparators = " + separators ) {
		right_tokens = vector<string>({"string", "to", "split"});
		split(tokens, line, separators, token_compress_on);

//		for(auto& t : tokens) cout << "-" << t << "-  ";
//			cout << endl;

		REQUIRE( tokens.size() == right_tokens.size() );
		for(int i=0; i<tokens.size(); i++){
			REQUIRE( tokens[i] == right_tokens[i] );		
		}
	}
}
