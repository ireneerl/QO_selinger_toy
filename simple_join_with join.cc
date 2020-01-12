#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sys/time.h>
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

TUPLE* join_record(TUPLE *l, TUPLE *r){
  TUPLE *tuple;
  if (!(tuple = (TUPLE *)calloc(1, sizeof(TUPLE))))
    printf("ERROR (%s): calloc failed. \n ",__func__);
  for(long unsigned int i = 0 ; i < l->attr.size(); i++){
    tuple->attr.push_back(l->attr[i]);
  }
  for(long unsigned int j = 0; j < r->attr.size(); j++){
    tuple->attr.push_back(r->attr[j]);
  }
  return tuple;
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

int* getOrder(int n) {
  int* order = new int[n];
  int tmp_min = 0, tmp = 0, max_V = 0;
  std::vector<bool> R_taken(n, false);
  int total_joined_Row = 0, max_vID_joined_row = 0;

  for(int idx = 1; idx < n ; idx++){
    std::cout << "idx" << idx << '\n' << "tmp_min " << tmp_min << std::endl;
    std::vector<int> r_number = {-1, -1};
    for (int i = 0; i < n; i++){
      // std::cout << "R_taken[i] -> " << R_taken[i] << " the status -> " << !R_taken[i] << '\n';
      if(!R_taken[i]){
        bool firstCompare = true;
        auto [l_tuples, l_V] = (idx == 1 && !R_taken[i]) ? getCost(tables[i]) : std::tuple<int, int> {total_joined_Row, max_vID_joined_row};
        int init = idx == 1 ? i+1 : 0 ;
        for (int j = init; j < n; j++){
          if(!R_taken[j]){
            // std::cout << "hi" << j <<'\n';
            std::cout << "comparing between " << i << " and " << j << ' ';
            auto [r_tuples, r_V] = getCost(tables[j]);
            max_V = std::max(l_V, r_V);
            tmp = (l_tuples * r_tuples)/max_V;
            std::cout << "l tuple " << l_tuples << " the temp value = " << tmp << '\n';
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
  for (int i = 0; i<n; i++){
    // std::cout << " " << R_taken[i] << ';';
    std::cout << " " << order[i] << ';';
    // order[i] = 0+i;
  }
  return order;
}
//
// std::vector<TUPLE*>* simple_nested_loop_join(
//   std::vector<TUPLE*>& left, std::vector<TUPLE*>& right){ //left join
//     std::vector<TUPLE*> *join_result;
//     TUPLE *leftRec;
//     TUPLE *rightRec;
//     if (!(join_result = (std::vector<TUPLE*>*) calloc(1, sizeof(std::vector<TUPLE*>))))
//       printf("ERROR (%s); calloc failed for calloc join_result\n",__func__ );
//
//     for (long unsigned int i = 0; i < left.size(); i++){
//       leftRec = left[i];
//       for (long unsigned int j = 0; j < right.size(); j++){
//         rightRec = right[j];
//         if (leftRec -> attr[1] == rightRec->attr[1]){
//           join_result->push_back(join_record(leftRec, rightRec));
//         }
//       }
//     }
//     return join_result;
// }

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
    std::vector<TUPLE*> *tmp;

    //query optimizer
    int* tablesOrder = getOrder(n);
    // query plan execution
    // if(n > 2){
    //   tmp = simple_nested_loop_join(tables[tablesOrder[0]], tables[tablesOrder[1]]);
    //   for (unsigned int i = 2; i < n; i++){
    //     tmp = simple_nested_loop_join(*tmp, tables[i]);
    //   }
    // }else{
    //   tmp = simple_nested_loop_join(tables[tablesOrder[0]], tables[tablesOrder[1]]);
    // }
    end = cur_time();
    // print_tuple(*tmp);
    printf("Number of matched rows: %ld\n", tmp->size());
    print_performance(begin, end);
  }
}
