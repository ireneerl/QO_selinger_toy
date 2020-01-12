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
  std::cout << "printing the performance";
  long diff = (end.tv_sec - begin.tv_sec) * 1000 * 1000
                + (end.tv_usec - begin.tv_usec);
  printf(" lat:%7ld usec\n", diff);
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
  std::cout << "relation -> " ;
  for (int i = 0; i < input.size(); i++)
    std::cout << char('A' + input.at(i)) << " ";
  std::cout << '\n';
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
  // std::cout << "the length of relation pos -> " << pos << " -> " << state.size() << '\n';
  int best_cost = 1000000;
  int total_relations = relations.size();

  for(int i = 0; i < total_relations; ++i)
    {
      // std::cout << "joined " << (joined & (1 << i)) << " -> " << (1 << i) << '\n';
    if(i == pos || (joined & (1 << i)))
      continue;
    auto [join_cost, sol] = selinger(relations, i, joined | (1 << i), state, solution);
    int cost = (i == total_relations-1)? join_cost : relations[pos][i] + join_cost;
    solution.push_back(pos);

    // std::cout << "solution pos -> " << pos << "joined -> " << joined << '\n';


    if(cost < state[pos][joined]){
      state[pos][joined] = cost;
        if(pos == 0){
          if(cost < best_cost){
            best_cost = cost;
            temp.clear();
            for (int x=0; x<solution.size(); x++) {
              temp.push_back(solution[x]);
              // std::cout << "solution -> " << solution[x]<< '\n';
            }
            std::cout <<  '\n';
          }
        }
      }
    }
    return {state[pos][joined], temp};
}

void DP(int n) {
  std::vector<std::vector<int>> relations;
  int first_join_min = 100000000;
  int relation_joined[2];

  //making first layer memoization
  for(int i=0; i<n; i++){
    std::vector <int> rRow;
    int tmp = 0, max_V = 0;
    for(int j=0; j<n; j++){
      auto [l_tuples, l_V] = getCost(tables[i]);
      auto [r_tuples, r_V] = getCost(tables[j]);
      max_V = std::max(l_V, r_V);
      tmp = (i == j)?  0 : (l_tuples * r_tuples)/max_V;
      std::cout << "the left " << l_tuples <<  l_V << "the right " << r_tuples <<  r_V << "the max " << max_V << " = " << tmp << "value float -> " << ((l_tuples * r_tuples)/max_V) <<  "or "<< ((20 * 20)/18) <<'\n';
      rRow.push_back( tmp );
    }
    relations.push_back(rRow);
  }

  for(int i=0; i<n; i++){
    for (int j=0;j<n;j++){
      if (relations[i][j] < first_join_min && i != j){
        relation_joined[0] = i;
        relation_joined[1] = j;
        first_join_min = relations[i][j];
      }
    }
  }
  std::cout << "joined relation" << relation_joined[0] << relation_joined[1] << '\n';
  print_first_memoization(relations);

  //building the second memoization onward
  std::vector<std::vector<int>> state(relations.size());
  for(auto& neighbors : state)
    neighbors = std::vector<int>((1 << relations.size()) - 1, INT_MAX);
    std::vector<int> vect;
    // vect.push_back(relation_joined[1]);
    auto [join_cost, path] = selinger(relations, relation_joined[0], 1, state, vect);
    std::cout << "minimum tupple generated: " << std::endl << join_cost << std::endl;
    print_rel(path);
}

void relative_DP(int n){

  std::vector<int> joined;
  int first_join_min;
  int tmp, l_tuples_1, l_V_1, max_V, total_join, tmp_total_join;
  for(int observed=0; observed < n; observed++){  //the number of R
    if (observed == 0){ //first relation to be joined
      int Ra,Rb;
      for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
          auto [l_tuples, l_V] = getCost(tables[i]);
          auto [r_tuples, r_V] = getCost(tables[j]);
          max_V = std::max(l_V, r_V);
          tmp = (i == j)?  0 : (l_tuples * r_tuples)/max_V;
          if (i == 0 && j != i){
            first_join_min = tmp;
            Ra = i;
            Rb = j;
            // std::cout << "first observed, first order, i and j" << i << j << '\n';
          }if (first_join_min >= tmp && j != i){
            first_join_min = tmp;
            l_tuples_1 = tmp;
            l_V_1 = max_V;
            Ra = i;
            Rb = j;
            // std::cout << "first observed, second order, i and j" << i << j << '\n';
          }
        }
      }
      joined.push_back(Ra);
      joined.push_back(Rb);
      total_join = first_join_min;
      print_rel(joined);
    }else{
      int Rc, compare = 0;
      // std::cout << "observed ->" <<observed << '\n';
      for(int j=0; j<n; j++){
        auto [r_tuples, r_V] = getCost(tables[j]);
        max_V = std::max(l_V_1, r_V);
        // std::cout << "observed J " << j << " the left " << observed;
        if(std::find(joined.begin(), joined.end(), j) == joined.end()) {
          // std::cout << "not found! checking! the J->" << j << '\n';
          tmp = (observed == j)?  0 : (l_tuples_1 * r_tuples)/max_V;
          tmp_total_join = total_join + tmp;
          std::cout << "the total -> " << tmp_total_join << "the total" << first_join_min << '\n';

          if (compare == 0 && j != observed){
            // std::cout << "the left " << l_tuples_1 <<  l_V_1 << "the right " << r_tuples <<  r_V << "the max " << max_V << " = " << tmp_total_join << "value float -> " << ((l_tuples_1 * r_tuples)/max_V) <<  " or "<< ((20 * 20)/18) <<'\n';
            first_join_min = tmp_total_join;
            Rc = j;
            compare = 1;
          }if (first_join_min >= tmp_total_join && observed != j){
            first_join_min = tmp_total_join;
            l_tuples_1 = tmp_total_join;
            l_V_1 = max_V;
            Rc = j;
          }
        }
      }
      if(std::find(joined.begin(), joined.end(), Rc) == joined.end()){
        joined.push_back(Rc);
        total_join = first_join_min;
      }
    }
  }
  print_rel(joined);
  std::cout << "minimum tupple generated: " << std::endl << total_join << std::endl;
}

int* getOrder_naive(int n) {
  int* order = new int[n];
  int tmp_min = 0, tmp = 0, max_V = 0;
  std::vector<bool> R_taken(n, false);
  int total_joined_Row = 0, max_vID_joined_row = 0;

  for(int idx = 1; idx < n ; idx++){
    // std::cout << "idx" << idx << '\n' << "tmp_min " << tmp_min << std::endl;
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


int* getOrder_greedy(int n) {
  int* order = new int[n];
  int tmp_min = 0, tmp = 0, max_V = 0;
  std::vector<bool> R_taken(n, false);
  int total_joined_Row = 0, max_vID_joined_row = 0;

  for(int idx = 1; idx < n ; idx++){
    // std::cout << "idx" << idx << '\n' << "tmp_min " << tmp_min << std::endl;
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

char nth_letter(int n)
{
  n = n+1;
  assert(n >= 1 && n <= 26);
    return "abcdefghijklmnopqrstuvwxyz"[n-1];
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
    DP(n);
    end = cur_time();
    print_performance(begin, end);
    begin = cur_time();
    relative_DP(n);
    end = cur_time();
    print_performance(begin, end);

    std::cout << "naive" << '\n';
    begin = cur_time();
    int* tablesOrder = getOrder_naive(n);
    end = cur_time();
    print_performance(begin, end);
    for (int i = 0; i<n; i++){
      std::cout << " " << nth_letter(tablesOrder[i]) << ';';
    }
  }
}
