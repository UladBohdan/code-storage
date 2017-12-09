#ifndef COMMON_BASE_CPP_
#define COMMON_BASE_CPP_

#include <cmath>
#include <iomanip>
#include <stdlib.h>
#include <utility>
#include <vector>

#include "constants.cc"

using namespace std;

// The very common interface for any random generator (basic, discrete, continuous).
class RandomGenerator {
public:
  virtual double Next() = 0;

  // The distribution.
  virtual double GetExpectation() = 0;
  virtual double GetDispersion() = 0;
  virtual double DistributionFunc(double) = 0;

  bool TestMoments() {
    int sz = x.size();
    double m = 0;
    for (int i = 0; i < sz; i++) {
      m += x[i];
    }
    m /= sz;
    cout << "Expectation: " << m << endl;
    double s2 = 0;
    for (int i = 0; i < sz; i++) {
      s2 += (x[i] - m) * (x[i] - m);
    }
    s2 /= (sz - 1);
    cout << "S2: " << s2 << endl;

    bool expTest = abs(m - GetExpectation()) < EPS;
    bool dispTest = abs(s2 - GetDispersion()) < EPS;

    cout << "math expectation test... " <<
      (expTest ? "passed." : "failed.") << endl;
    cout << "dispersion test..." <<
      (dispTest ? "passed." : "failed.") << endl;

    return expTest && dispTest;
  }

  bool TestPearson() {
    int hsz = histogram.size();
    vector<int> v(hsz, 0);
    for (int i = 0; i < x.size(); i++) {
      if (x[i] == 1.) {
        v[hsz-1]++;
        continue;
      }
      for (int j = 0; j < hsz; j++) {
        if (x[i] >= histogram[j].first && x[i] < histogram[j].second) {
          v[j]++;
          break;
        }
      }
    }
    cout << "HIST: ";
    for (int i = 0; i < v.size(); i++) {
      cout << v[i] << " ";
    }
    cout << endl;

    double hi2 = 0;
    int sz = x.size();
    for (int i = 0; i < hsz; i++) {
      double p = DistributionFunc(histogram[i].second) - DistributionFunc(histogram[i].first);
      hi2 += ((v[i] - sz * p) * (v[i] - sz * p) * 1. / (sz * p));
    }
    cout << "hi2 value: " << hi2 << endl;
    return hi2 < PEARSON_THRESHOLD;
  }

  void RunTests(double sz = DEFAULT_TEST_SIZE) {
    GenerateValues(sz);

    string name = "Moments test";
    PrintIntro(name);
    PrintTestResult(name, TestMoments());

    name = "Pearson test";
    PrintIntro(name);
    PrintTestResult(name, TestPearson());
  }

  virtual ~RandomGenerator() {}

protected:
  vector<double> x;
  // Set of pairs [a, b) defining the histogram. Must be initialized before
  // running TestPearson.
  vector<pair<double,double>> histogram;

  // UTILS. ////////////////////////////////
  void GenerateValues(int sz) {
    x = vector<double>(sz);
    for (int i = 0; i < sz; i++) {
      x[i] = Next();
    }
    /*cout << "GENERATED:" << endl;
    for (int i = 0; i < sz; i++) {
      cout << x[i] << endl;
    }*/
  }
  void PrintIntro(string name) {
    cout << "===== Running " << name << " =====" << endl;
  }
  void PrintTestResult(string name, bool result) {
    cout << setw(TEST_NAME_OUTPUT_WIDTH) << left << name <<
      (result ? COLOUR_GREEN + "PASSED" : COLOUR_RED + "FAILED")
        << COLOUR_DEFAULT << endl;
  }
};

class BaseRandomGenerator : public RandomGenerator {
public:
  BaseRandomGenerator() {
    histogram = vector<pair<double,double>>(10);
    for (int i = 0; i < 10; i++) {
      histogram[i] = make_pair(i/10., (i+1)/10.);
    }
  }

  virtual double Next() = 0;

  double GetExpectation() { return 0.5; }
  double GetDispersion() { return 1./12; }
  double DistributionFunc(double p) { return p; }

  bool TestKolmogorov() {
    int sz = x.size();
    double mx = 0;
    vector<double> v(x.begin(), x.end());
    sort(v.begin(), v.end());
    for (int i = 0; i < v.size(); i++) {
      mx = max(mx, (i * 1. / sz) - v[i]);
    }
    mx *= sqrt(sz);
    cout << "Kolmogorov value: " << mx << endl;
    return mx < KOLMOGOROV_THRESHOLD;
  }

  void RunTests(int sz = DEFAULT_TEST_SIZE) {
    RandomGenerator::RunTests(sz);

    // Base Random Generator specific tests.
    string name = "Kolmogorov test";
    PrintIntro(name);
    PrintTestResult(name, TestKolmogorov());
  }
};

class StandartRandomGenerator : public BaseRandomGenerator {
// Remember to run srand() in your main() function.
public:
  StandartRandomGenerator() : BaseRandomGenerator() { }
  double Next() {
    return (rand() % (M+1)) / (M * 1.);
  }
};

class MultiplicativeCongruentialGenerator : public BaseRandomGenerator {
public:
  MultiplicativeCongruentialGenerator(int x0, int a, int M) :
    BaseRandomGenerator(), a(a), M(M), cur(x0) { }
  double Next() {
    cur = (a * cur) % M;
    return cur * 1. / M;
  }
private:
  long long a, M;
  long long cur;
};

class MacLarenMarsagliaGenerator : public BaseRandomGenerator {
public:
  MacLarenMarsagliaGenerator(int k) : BaseRandomGenerator(), k(k), pos(0) {
    xx = vector<double>(k);
    y = vector<double>(k);
    v = vector<double>(k);

    MultiplicativeCongruentialGenerator mcg(4099, 4099, 2147483647);
    StandartRandomGenerator srg;

    for (int i = 0; i < k; i++) {
      xx[i] = mcg.Next();
      v[i] = xx[i];
    }

    for (int i = 0; i < k; i++) {
      y[i] = srg.Next();
    }
  }
  double Next() {
    int i = y[pos] * k;
    double r = v[i];
    v[i] = xx[(pos + k) % k];
    pos++;
    return r;
  }
private:
  int k;
  vector<double> xx, y, v;
  int pos;
};

#endif  // COMMON_BASE_CPP_
