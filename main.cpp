#include <myers.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
  if (argc != 3) {
    cerr << "usage " << argv[0] << " filex filey" << endl;
  }
  vector<string> filex, filey;
  {
    ifstream stream(argv[1]);
    string line;
    for (std::string line; std::getline(stream, line);)
      filex.push_back(line);
  }
  {
    ifstream stream(argv[2]);
    string line;
    for (std::string line; std::getline(stream, line);)
      filey.push_back(line);
  }

  std::vector<Diff> summary;
  diff(summary, filex, filey);

  for (size_t i = 0; i < summary.size(); i++) {
    switch (summary[i].op) {
    case BOTH: {
      for (size_t px = summary[i].begx, py = summary[i].begy; px < summary[i].endx; px++, py++) {
        cout << setw(5) << px << ":" << py << " == " << filex[px] << endl;
      }
      break;
    }
    case FILEX: {
      for (size_t px = summary[i].begx, py = summary[i].begy; px < summary[i].endx; px++) {
        cout << setw(5) << px << ":" << py << " ++ " << filex[px] << endl;
      }
      break;
    }
    case FILEY: {
      for (size_t px = summary[i].begx, py = summary[i].begy; py < summary[i].endy; py++) {
        cout << setw(5) << px << ":" << py << " -- " << filey[py] << endl;
      }
      break;
    }
    }
  }
  //*/
  return 0;
}
