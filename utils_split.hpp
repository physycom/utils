#ifndef PHYSYCOM_UTILS_SPLIT_HPP
#define PHYSYCOM_UTILS_SPLIT_HPP

#include <vector>

namespace physycom{

	enum{
		token_compress_on,
		token_compress_off
	};

	template<typename E, typename C> inline bool belongs_to(const E& e, const C& c) {
		for (size_t i = 0; i < c.size(); i++) { if (e == c.at(i)) return true; }
		return false;
	}

	void split(std::vector<std::string>& tok, const std::string& str, const std::string& sep, const int& mode = token_compress_off){
		tok.clear();
		if( str.size() ){
			auto start = str.begin();
			for( auto it = str.begin(); it != str.end(); it++ ){
				if( belongs_to(*it, sep) ){
					tok.push_back( std::string(start, it) );
					start = it + 1;
				}
			}
			tok.push_back( std::string(start, str.end() ) );

			if( mode == token_compress_on ){
				auto it = tok.begin();
				while( it != tok.end() ){
					if( *it == "" ) it = tok.erase(it);
					else it++;
				}
			} 
		}
	}

}   // end namespace physycom

#endif // PHYSYCOM_SPLIT_HPP
