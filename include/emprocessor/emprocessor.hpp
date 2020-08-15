
#ifndef __EM_PROCESSOR_H__
#define __EM_PROCESSOR_H__

#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <string>
#include <unistd.h>
#include <set>
#include <ctime>

struct date
{
  short day;
  short month;
  short year;

  bool operator==(const date &d) const
  {
    return day == d.day && month == d.month && year == d.year;
  }

  bool operator<(const date &d) const
  {
    if (year != d.year)
    {
      return year < d.year;
    }

    if (month != d.month)
    {
      return month < d.month;
    }

    return day < d.day;
  }

  bool operator>(const date &d) const
  {
    if (year != d.year)
    {
      return year > d.year;
    }

    if (month != d.month)
    {
      return month > d.month;
    }

    return day > d.day;
  }
};

struct em_key
{
  date draw_date;
  short main_numbers[5];
  short stars[2];

  bool operator==(const em_key &key) const
  {
    return draw_date == key.draw_date &&
           this->numbers_and_stars_match(key);
           
  }

  bool operator<(const em_key &key) const
  {
    return draw_date < key.draw_date;
  }

  bool operator>(const em_key &key) const
  {
    return draw_date > key.draw_date;
  }

  bool numbers_and_stars_match(const em_key &key) const
  {
  	return main_numbers[0] == key.main_numbers[0] &&
           main_numbers[1] == key.main_numbers[1] &&
           main_numbers[2] == key.main_numbers[2] &&
           main_numbers[3] == key.main_numbers[3] &&
           main_numbers[4] == key.main_numbers[4] &&
           stars[0] == key.stars[0] &&
           stars[1] == key.stars[1];
  }
};

extern bool _verbose;

#define _DEBUG(statement)	\
  if (_verbose)				\
  	statement;

class emprocessor
{
  private:
    std::vector<em_key *> all_keys;
    char *filename;
    static constexpr short LINE_SIZE = 40;

  public:
    emprocessor(char *filename);
    static bool validate_key(const em_key &key);
    static void sort_array(short *array, short len);
    static void ensure_sorted(em_key &key);
    void insert_key(em_key &key);
    std::vector<em_key *> check_for_duplicates_ever();
    static short generate_random(short lower, short upper);
    static em_key *generate_key();
    em_key *generate_non_existent_key(bool odd_even = false, int qte_odd = 0, int qte_even = 0);
    em_key *key_exists(const em_key &key);
    float percentage_odd_even(int qte_odd, int qte_even);
    float percentage_low_high(int qte_low, int qte_high);
    ~emprocessor();

  private:
    bool read_dataset();
    bool record_exists(const em_key &key);
};

#endif