#include <physycom/combinatoric.hpp>

using namespace std;
using namespace physycom;

int main()
{
  combinations comb;

  // integers
  vector<int> elem {1,2,3,4};
  auto tuple_i = comb(elem, 3);
  for(int i = 0; i < tuple_i.size(); ++i) {
    cout << i+1 << ") ";
    for(auto e : tuple_i[i]) cout << e;
    cout << endl;
  }

  // strings
  string s = "abcde";
  auto tuple_s = comb(s, 3);
  for(int i = 0; i < tuple_s.size(); ++i) cout << i+1 << ") " << tuple_s[i] << endl;

  return 0;
}
