#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <future>
#include <random>
#include <stdlib.h>
#include "stack.hpp"

using namespace std;

struct multiplicator
{
  threadsafe_stack<vector<float>*> chunks;
  int thread_num;
  atomic<bool> end_of_data;
  vector<thread> threads;
  promise<int> p;
  int threads_running;

  multiplicator(int _thread_num, vector<vector<float>>& v)
    :thread_num(_thread_num)
    ,end_of_data(false)
    ,threads_running(0)
  {

    for (int i = 0; i < v.size(); i++)
      chunks.push(&v[i]);

    for (int i = 0; i < thread_num; i++)
    {
      threads.push_back(thread(&multiplicator::run_mult, this));
      ++threads_running;
    }
  }
  ~multiplicator()
  {
    for (int i = 0; i < threads.size(); i++)
    {
      threads[i].join();
    }
  }
  future<int> future()
  {
    return p.get_future();
  }
  void try_mult()
  {
    try {
      vector<float>* a;
      chunks.pop(a);
      
      auto it = a->begin();
      auto end = a->end();
      for (;it != end; it++)
      {
        *it *= 2;
        *it *= 2;
        *it += 1;
      }
    }
    catch(empty_stack& e)
    {
      end_of_data = true;
    }
  }
  void run_mult()
  {
    while(!end_of_data)
    {
      try_mult();
      this_thread::yield();
    }
    --threads_running;
    if (threads_running == 0) {
      p.set_value(1);
    }
  }
};

void h(vector<vector<float>>& b)
{
  stack<vector<float>*> v;
  v.push(&b[0]);
  vector<float>* temp = v.top();
  (*temp)[0] = float{123};
}

vector<float> make_vector(const int length)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<float> distribution(1,60);
    auto dice = std::bind ( distribution, generator );
    vector<float> v;
    for (int i = 0; i < length; i++) {
        v.push_back(float{dice()});
    }
    return v;
}

int main(int argc, char** argv)
{
  if (argv[1] == NULL || argv[2] == NULL)
  {
    cerr << "Need two arguments." << endl;
    exit(1);
  }

  int p1 = strtol(argv[1], nullptr, 10);
  int p2 = strtol(argv[2], nullptr, 10);

  auto start = chrono::steady_clock::now();
  vector<vector<float>> a = {
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
    make_vector(int{p1}),
  };
  auto end = chrono::steady_clock::now();

  cout << "Memory allocated in: " << " " << chrono::duration <double, milli> (end - start).count()  << endl;

  // for (float i : a[0])
  //   cout << i << endl;
  // cout << endl;

  cout << "Available concurrency: " << thread::hardware_concurrency() << endl;
  for (int i = 1; i <= p2; i++)
  {
    auto start = chrono::steady_clock::now();
    multiplicator m(i, a);
    m.future().wait();
    auto end = chrono::steady_clock::now();

    cout << "Threads: " << i << " " << chrono::duration <double, milli> (end - start).count()  << endl;
  }

  //for (float i : a[0])
  //  cout << i << endl;
}
