
#include <vector>
#include <iostream>
#include <limits.h>
#include <tuple>

void print_rel(std::vector<int> const &input)
{
	for (int i = 0; i < input.size(); i++) {
		std::cout << input.at(i) << " ";
	}
}

std::tuple<int, std::vector<int>> tsp(const std::vector<std::vector<int>>& relations, int pos, int joined, std::vector<std::vector<int>>& state, std::vector<int> &solution)
{
  if(joined == ((1 << relations.size()) - 1)){
    solution.push_back(pos);
    return {relations[pos][0], solution};
  }

  if(state[pos][joined] != INT_MAX){
    return {state[pos][joined], solution};
  }
  std::vector<int> temp;
  solution.clear();
  int best_dist = 10000;

  for(int i = 0; i < relations.size(); ++i)
    {
    if(i == pos || (joined & (1 << i)))
      continue;
    auto [dis, sol] = tsp(relations, i, joined | (1 << i), state, solution);
    int join_cost = relations[pos][i] + dis;
    solution.push_back(pos);

    if(join_cost < state[pos][joined]){
      state[pos][joined] = join_cost;
        if(pos == 0){
          if(join_cost < best_dist){
            best_dist = join_cost;
            temp.clear();
            for (int i=0; i<solution.size(); i++) {
              temp.push_back(solution[i]);
            }
          }
        }
      }
    }
    return {state[pos][joined], temp};
}

int main()
{
    std::vector<std::vector<int>> relations = { { 0, 2, 9, 10 },
                                   { 1, 0, 6, 4 },
                                   { 15, 7, 0, 8 },
                                   { 6, 3, 12, 0 }
                                 };
    std::vector<std::vector<int>> state(relations.size());

for(auto& neighbors : state)
        neighbors = std::vector<int>((1 << relations.size()) - 1, INT_MAX);

        std::vector<int> vect;
        auto [dis, path] = tsp(relations, 0, 1, state, vect);

    std::cout << "minimum: " << std::endl << dis << std::endl;
    print_rel(path);
return 0;
}
