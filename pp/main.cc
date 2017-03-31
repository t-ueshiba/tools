#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;

class indenter
{
  private:
    int		get_lev() const
		{
		    return accumulate(levels.begin(), levels.end(), 0);
		}

    string	level_up(const char c, const int index)
		{
		    string ret;

		    ret.push_back('\n');
		    ret.append(get_lev(), '\t');
		    levels[index]++;
		    ret = (ret+c)+'\n';
		    ret.append(get_lev(), '\t');

		    return ret;
		}

    string	level_down(const char c, const int index)
		{
		    string ret;

		    ret.push_back('\n');
		    levels[index]--;
		    ret.append(get_lev(), '\t');
		    ret.push_back(c);

		    return ret;
		}

  public:

		indenter(const string& target_list)
		    :target_list(target_list), levels(target_list.size()/2)
		{
		}

    string	convert(const string& src)
		{
		    fill(levels.begin(), levels.end(), 0);

		    string ret;

		    string::const_iterator i = src.begin();
		    for(; i != src.end(); ++i)
		    {
			for(int j = 0; j < target_list.size(); j+=2)
			{
			    if(*i == target_list[j])
			    {
				ret+=level_up(target_list[j], j/2);
				goto contiunue;
			    }
			    if(*i == target_list[j+1])
			    {
				ret += level_down(target_list[j+1], j/2);
				goto contiunue;
			    }
			}

			switch(*i)
			{
			  case ',':
			    ret.append(string(",\n"));
			    ret.append(get_lev(), '\t');
			    break;

			  default:
			    ret.push_back((*i));
			    break;
			}

		      contiunue:;
		    }

		    return ret;
		}

  private:
    string	target_list;
    vector<int>	levels;
};

int main()
{
    string line;

    indenter conv("<>()[]");

    while(getline(cin, line))
    {
	cout << conv.convert(line) << endl;
    }
}
