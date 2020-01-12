#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sys/time.h>
#include <limits.h>
#include <vector>

#define DEFAULT_TABLE_SIZE 1000
#define DEFAULT_NUM_SPACE 10000
#define MAX_TABLE_NUM 16

void print_performance(struct timeval begin, struct timeval end) {
  std::cout << "printing the performance" << '\n';
  long diff = (end.tv_sec - begin.tv_sec) * 1000 * 1000
                + (end.tv_usec - begin.tv_usec);
  printf("lat:%7ld usec\n", diff);
}

struct timeval
  cur_time(void) {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t;
}

typedef struct _RECORD{
  std::vector<int> attr;
}  TUPLE;

typedef struct return_DP {
  int minimum_cost;
  std::vector<int> path;
} DP_Param;

int numSpace;
int tableSize;
std::vector<std::vector<TUPLE*> > tables(MAX_TABLE_NUM);

TUPLE* alloc_tuple(int j){
  TUPLE *tuple;
  if(!(tuple = (TUPLE *) calloc(1, sizeof(TUPLE))))
    printf("ERROR! (%s): calloc failed\n", __func__);
  //by value
  tuple->attr.push_back(j); //id_key
  tuple->attr.push_back(rand() % numSpace);
  return tuple;
}

void init_database(int n){
  for(int i = 0; i < n; i++){
    for(int j = 0; j < tableSize; j++)
      tables[i].push_back(alloc_tuple(j));
  }
}

void print_database(int n){
  for(int i = 0 ; i < n; i++){
    std::cout << "Table " << i << '\n';
    for(long unsigned int j = 0; j < tables[i].size();j++){
      printf("[Row#: %ld] %d %d\n", j, tables[i][j]->attr[0], tables[i][j]->attr[1]);
    }
  }
}

void print_tuple(std::vector<TUPLE*>& data){
  for (int i=0; i < data.size(); i ++){
    for (int j = 0 ; j < data[1]->attr.size(); j++){
      std::cout << data[i]->attr[j]<< ";" ;
    }
    std::cout <<std::endl;
  }
}

int V(std::vector<TUPLE*> R){ //-> std::vector<TUPLE*>& R
	auto comp_ = [] ( const TUPLE *lts, const TUPLE *rts ) {return lts->attr[1] == rts->attr[1];};
	auto pred_ = []( const TUPLE *lts, const TUPLE *rts ) {return lts->attr[1] < rts->attr[1];};
	std::sort(R.begin(),R.end(),pred_);
	auto last_ = std::unique(R.begin(), R.end(),comp_);
	R.erase(last_, R.end());
  return R.size();
}

int row_R(std::vector<TUPLE*>& R){
  return R.size();
}

std::tuple<int, int> getCost(std::vector<TUPLE*>& table_observed){
  int row_r = row_R(table_observed);
  int v_r = V(table_observed);
  // std::cout << "the V -> " << v_r << '\n';
  return {row_r, v_r};
}

void print_rel(std::vector<int> const &input){
  std::cout << "relation -> " << '\n';
	for (int i = 0; i < input.size(); i++)
		std::cout << char('A' + input.at(i)) << " ";

}

void print_first_memoization(std::vector<std::vector<int>> relations){
  for(int i=0; i<relations.size(); i++){
    for(int j=0; j<relations[0].size(); j++)
      std::cout << relations[i][j]<< " ";
  std::cout << "" << '\n';
  }
}

std::tuple<int, std::vector<int>> selinger(const std::vector<std::vector<int>>& relations, int pos, int joined, std::vector<std::vector<int>>& state, std::vector<int> &solution)
{
  if(joined == ((1 << relations.size()) - 1)){
    solution.push_back(pos);
    return {relations[pos][0], solution}; //the last relation
  }

  if(state[pos][joined] != INT_MAX){
    return {state[pos][joined], solution};
  }
  std::vector<int> temp;
  solution.clear();
  std::cout << "the length of relation pos -> " << pos << " -> " << state.size() << '\n';
  int best_cost = 1000000;

  int total_relations = relations.size();

  for(int i = 0; i < total_relations; ++i)
    {
    if(i == pos || (joined & (1 << i)))
      continue;
    auto [join_cost, sol] = selinger(relations, i, joined | (1 << i), state, solution);
    // int cost = relations[pos][i] + join_cost;
    // std::cout << "relation[pos][i]" << relations[pos][i] << "join cost" << join_cost << '\n';
    int cost = (i == total_relations-1)? join_cost : relations[pos][i] + join_cost;
    solution.push_back(pos);

    if(cost < state[pos][joined]){
      state[pos][joined] = cost;
       //push the smallest cost to relation plan
        if(pos == 0){
          if(cost < best_cost){
            best_cost = cost;
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

void DP(int n) {
  std::vector<std::vector<int>> relations;
  int original_table = 0;

  //making first layer memoization
  for(int i=0; i<n; i++){
    std::vector <int> rRow;
    int tmp = 0, max_V = 0;
    for(int j=0; j<n; j++){
      auto [l_tuples, l_V] = getCost(tables[i]);
      auto [r_tuples, r_V] = getCost(tables[j]);
      max_V = std::max(l_V, r_V);
      tmp = (i == j)?  0 : (l_tuples * r_tuples)/max_V;
      // std::cout << "the left " << l_tuples <<  l_V << "the right " << r_tuples <<  r_V << "the max " << max_V << " = " << tmp << "value float -> " << ((l_tuples * r_tuples)/max_V) <<  "or "<< ((20 * 20)/18) <<'\n';
      rRow.push_back( tmp );
    }
    relations.push_back(rRow);
  }
  print_first_memoization(relations);
  int min_cost = relations[0][0];
  for(int i=0; i<n; i++){
    for(int j=0; j<n; j++){
      if(relations[i][j] < min_cost){
        original_table = i;
      }
    }
  }

  std::cout << "original table " << original_table << '\n';

  //building the second memoization onward
  std::vector<std::vector<int>> state(relations.size());
  for(auto& neighbors : state)
    neighbors = std::vector<int>((1 << relations.size()) - 1, INT_MAX);
    std::vector<int> vect;
    auto [join_cost, path] = selinger(relations, original_table, 1, state, vect);
    std::cout << "minimum tupple generated: " << std::endl << join_cost << std::endl;
    print_rel(path);
}

int main(int argc, char *argv[]){
  tableSize = DEFAULT_TABLE_SIZE;
  numSpace = DEFAULT_NUM_SPACE;

  struct timeval begin, end;

  if (argc >= 2) tableSize = atoi(argv[1]); //total rows
  if (argc == 3) numSpace = atoi(argv[2]); //the key

  unsigned int n;
  std::cout << "Number of tables:";
  std::cin >> n;
  tables.resize(n);

  if(n > MAX_TABLE_NUM || n < 2){
    printf("Invalid number of tables = %d, either too much or too little\n", n);
  }else{
    init_database(n);
    print_database(n);
    begin = cur_time();
    DP(n);
    end = cur_time();
    print_performance(begin, end);
  }
}
