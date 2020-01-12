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
  int minimum_distance;
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
    for(int j = 0; j < tableSize; j++){
      tables[i].push_back(alloc_tuple(j));
    }
  }
}

void print_database(int n){
  for(int i = 0 ; i < n; i++){
    for(long unsigned int j = 0; j < tables[i].size();j++){
      printf("[Table#: %d, Row#: %ld] %d %d\n",
        i, j, tables[i][j]->attr[0], tables[i][j]->attr[1]);
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
  return {row_r, v_r};
}

int* getOrder_greedy(int n) {
  int* order = new int[n];
  int tmp_min = 0, tmp = 0, max_V = 0;
  std::vector<bool> R_taken(n, false);
  int total_joined_Row = 0, max_vID_joined_row = 0;

  for(int idx = 1; idx < n ; idx++){
    std::cout << "idx" << idx << '\n' << "tmp_min " << tmp_min << std::endl;
    std::vector<int> r_number = {-1, -1};
    for (int i = 0; i < n; i++){
      if(!R_taken[i]){
        bool firstCompare = true;
        auto [l_tuples, l_V] = (idx == 1 && !R_taken[i]) ? getCost(tables[i]) : std::tuple<int, int> {total_joined_Row, max_vID_joined_row};
        int init = idx == 1 ? i+1 : 0 ;
        for (int j = init; j < n; j++){
          if(!R_taken[j]){
            std::cout << "comparing between " << i << " and " << j << ' ';
            auto [r_tuples, r_V] = getCost(tables[j]);
            max_V = std::max(l_V, r_V);
            tmp = (l_tuples * r_tuples)/max_V;
            if (firstCompare || (tmp < tmp_min)){
              tmp_min = tmp; firstCompare = false;
              if (idx == 1)r_number = {i,j};
              else
                r_number = {j,-1};
            }
          }
        }
      }
    }
    if (r_number[0] != -1) {
      R_taken[r_number[0]] = true;
      order[idx] = r_number[0];
    }
    if (r_number[1] != -1) {
      R_taken[r_number[1]] = true;
      order[idx-1] = r_number[1]; //assign to the first node left deep tree
    }
    std::cout << "i " << r_number[0] << "; j " << r_number[1] << '\n';
    total_joined_Row = tmp_min;
    max_vID_joined_row = max_V;
    tmp_min = 0 ; tmp = 0 ;
  }
  return order;
}
void print_rel(std::vector<int> const &input)
{
	for (int i = 0; i < input.size(); i++) {
		std::cout << input.at(i) << " ";
	}
}

void print_first_memoization(std::vector<std::vector<int>> relations){
  for(int i=0; i<relations.size(); i++){
    for(int j=0; j<relations[0].size(); j++){
      std::cout << relations[i][j]<< " ";
  }
  std::cout << "" << '\n';
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
  int best_dist = 1000000;

  for(int i = 0; i < relations.size(); ++i)
    {
    if(i == pos || (joined & (1 << i)))
      continue;
    auto [dis, sol] = tsp(relations, i, joined | (1 << i), state, solution);
    int distance = relations[pos][i] + dis;
    solution.push_back(pos);

    if(distance < state[pos][joined]){
      state[pos][joined] = distance;
        if(pos == 0){
          if(distance < best_dist){
            best_dist = distance;
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
  for(int i=0; i<n; i++){
    std::vector <int> rRow;
    int tmp = 0, max_V = 0;
    for(int j=0; j<n; j++){
      auto [l_tuples, l_V] = getCost(tables[i]);
      auto [r_tuples, r_V] = getCost(tables[j]);
      max_V = std::max(l_V, r_V);
      tmp = (i == j)?  0 : (l_tuples * r_tuples)/max_V;
      std::cout << "the left " << l_tuples <<  l_V << "the right " << r_tuples <<  r_V << "the max " << max_V << " = " << tmp <<'\n';
      rRow.push_back( tmp );
    }
    relations.push_back(rRow);
  }
  print_first_memoization(relations);
  std::vector<std::vector<int>> state(relations.size());
  std::cout << "state -> " << state.size()<< "state -> " << state[0].size()<< '\n';
  for(auto& neighbors : state)
          neighbors = std::vector<int>((1 << relations.size()) - 1, INT_MAX);

          std::vector<int> vect;
          auto [dis, path] = tsp(relations, 0, 1, state, vect);

      std::cout << "minimum tupple generated: " << std::endl << dis << std::endl;
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
    // print_database(n);
    begin = cur_time();
    int* tablesOrder = getOrder_greedy(n);
    end = cur_time();
    print_performance(begin, end);
    for (int i = 0; i<n; i++){
      std::cout << " " << tablesOrder[i] << ';';
    }
    // //Query optimizer Dynamic Programming
    // DP(n);
  }
}
