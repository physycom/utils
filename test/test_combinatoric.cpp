#include <physycom/combinatoric.hpp>

using namespace std;
using namespace physycom;

int main()
{
  // PERMUTATION
  // integers
  vector<int> elem {1,2,3,4};
  auto perm_int = permutations(elem);
  for(int i = 0; i < perm_int.size(); ++i) {
    cout << i+1 << ") ";
    for(auto e : perm_int[i]) cout << e;
    cout << endl;
  }

  // strings
  string s = "abcde";
  auto perm_s = permutations(s);
  for(int i = 0; i < perm_s.size(); ++i)
    cout << i+1 << ") " << perm_s[i] << endl;

  // COMBINATIONS
  // integers
  auto comb_int = combinations(elem, 3);
  for(int i = 0; i < comb_int.size(); ++i) {
    cout << i+1 << ") ";
    for(auto e : comb_int[i]) cout << e;
    cout << endl;
  }

  // strings
  auto comb_s = combinations(s, 3);
  for(int i = 0; i < comb_s.size(); ++i)
    cout << i+1 << ") " << comb_s[i] << endl;

  return 0;
}
