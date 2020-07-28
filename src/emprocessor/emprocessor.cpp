
#include "emprocessor/emprocessor.hpp"

emprocessor::emprocessor(char *filename): filename(filename)
{
  this->read_dataset();
}

bool emprocessor::read_dataset()
{
  FILE *fp;
  char line[LINE_SIZE];
  char separator[] = " ";
  char slash[] = "/";

  fp = fopen(filename, "r");
  if (!fp)
  {
    _DEBUG(printf( "Error opening file\n" ));
    return false;
  }

  while(fgets(line, LINE_SIZE, fp) != nullptr)
  {
    char *ptr = strtok(line, separator);

    if (ptr == nullptr)
    {
      continue;
    }

    char draw_date[11];
    strcpy(draw_date, ptr);


    ptr = strtok(nullptr, separator);

    short keys[5];
    keys[0] = atoi(ptr);

    ptr = strtok(nullptr, separator);
    keys[1] = atoi(ptr);

    ptr = strtok(nullptr, separator);
    keys[2] = atoi(ptr);

    ptr = strtok(nullptr, separator);
    keys[3] = atoi(ptr);

    ptr = strtok(nullptr, separator);
    keys[4] = atoi(ptr);

    short stars[2];

    ptr = strtok(nullptr, separator);
    stars[0] = static_cast<short>(atoi(ptr));

    ptr = strtok(nullptr, separator);
    stars[1] = static_cast<short>(atoi(ptr));


    char *date_ptr;
    date_ptr = strtok(draw_date, slash);

    short day = static_cast<short>(atoi(date_ptr));

    date_ptr = strtok(nullptr, slash);
    short month = static_cast<short>(atoi(date_ptr));

    date_ptr = strtok(nullptr, slash);
    short year = static_cast<short>(atoi(date_ptr));

    date ddate = {day, month, year};


    em_key *new_em_key = (em_key *)malloc(sizeof(em_key));
    new_em_key->draw_date = ddate;
    memcpy(new_em_key->main_numbers, keys, sizeof(keys));
    memcpy(new_em_key->stars, stars, sizeof(stars));

    ensure_sorted( *new_em_key );

    all_keys.push_back(new_em_key);

    ptr = strtok(nullptr, separator);

    if (ptr != nullptr)
    {
      fprintf(stderr, "corrupted file, bye\n");
      return false;
    }
  }

  sort(all_keys.begin(), all_keys.end(), 
        [](const em_key *a, const em_key *b) -> bool
        { 
          return *a > *b;
        }
      );

  fclose(fp);

  _DEBUG(fprintf(stdout, "read %d lines\n", static_cast<int>(all_keys.size())));

  return true;
}

bool emprocessor::validate_key(const em_key &key)
{
  if (key.draw_date.month>12 || key.draw_date.month < 1)
  {
  	return false;
  }

  if ((key.draw_date.year%4)==0)
  {
    if (key.draw_date.month==2 && (key.draw_date.day>29 || key.draw_date.day<1))
      return false;
  }
  else
  {
    if (key.draw_date.month==2 && (key.draw_date.day>28 || key.draw_date.day<1))
      return false;
  }

  switch (key.draw_date.month)
  {
  	case 1:
  	case 3:
  	case 5:
  	case 7:
  	case 8:
  	case 10:
  	case 12:
  	  if (key.draw_date.day>31 || key.draw_date.day<1)
  	    return false;
  	  break;
  	case 4:
  	case 6:
  	case 9:
  	case 11:
  	  if (key.draw_date.day>30 || key.draw_date.day<1)
  	    return false;
  	  break;
  	default:
  	  return false;
  }

  for (int i = 0; i < 5; ++i)
  {
  	if (key.main_numbers[i]>50 || key.main_numbers[i]<1)
  	{
  	  return false;
  	}

  	if (i<2 && (key.stars[i]>12 || key.stars[i]<1))
  	{
  	  return false;
  	}
  }

  return true;
}

void emprocessor::sort_array(short *array, short len)
{
  if (len < 2)
  {
  	return;
  }

  for (int i = 0; i < len-1; ++i)
  {
  	for (int j = i+1; j < len; ++j)
  	{
  	  if (array[i] > array[j])
  	  {
  	  	short aux = array[i];
  	  	array[i] = array[j];
  	  	array[j] = aux;
  	  }
  	}
  }
}

void emprocessor::ensure_sorted(em_key &key)
{
  sort_array(key.main_numbers, 5);
  sort_array(key.stars, 2);
}

bool emprocessor::record_exists(const em_key &key)
{
  for (const auto &each : all_keys)
  {
    if (*each == key)
    {
      return true;
    }
  }
  return false;
}

em_key *emprocessor::key_exists(const em_key &key)
{
  for (const auto &each : all_keys)
  {
    if ((*each).numbers_and_stars_match(key))
    {
      return &(*each);
    }
  }
  return nullptr;
}

void emprocessor::insert_key(em_key &key)
{
  ensure_sorted(key);

  if (record_exists(key))
  {
  	_DEBUG(printf("record not inserted, already exists.\n"));
  	return;
  }

  std::vector<em_key *>::iterator it;

  it = all_keys.begin();

  while ( key < (**it) )
  	++it;

  em_key *k = (em_key *)malloc(sizeof(em_key));
  memcpy(k, &key, sizeof(em_key));

  all_keys.insert(it, k);


  std::string out_filename(filename);
  out_filename += ".temp";

  FILE *file = fopen(out_filename.data(), "w");
  if (!file)
  {
    _DEBUG(printf( "Error opening file\n" ));
    return;
  }

  //bool first = true;
  for (const auto &each : all_keys)
  {
  	/*if (!first)
  	{
  	  fprintf(file, "\n");
  	}
  	else
  	{
  	  first = false;
  	}*/

  	fprintf(file, "%s%u/%s%u/%s%u %s%u %s%u %s%u %s%u %s%u %s%u %s%u\n",
      (each->draw_date.day<10) ? "0" : "",
      each->draw_date.day,
      (each->draw_date.month<10) ? "0" : "",
      each->draw_date.month,
      "",
      each->draw_date.year,
      (each->main_numbers[0]<10) ? " " : "",
      each->main_numbers[0],
      (each->main_numbers[1]<10) ? " " : "",
      each->main_numbers[1],
      (each->main_numbers[2]<10) ? " " : "",
      each->main_numbers[2],
      (each->main_numbers[3]<10) ? " " : "",
      each->main_numbers[3],
      (each->main_numbers[4]<10) ? " " : "",
      each->main_numbers[4],
      (each->stars[0]<10) ? " " : "",
      each->stars[0],
      (each->stars[1]<10) ? " " : "",
      each->stars[1]);
  }

  fclose(file);

  if (remove(filename) == 0)
  {
    _DEBUG(printf("file '%s' was deleted successfully.\n", filename));
  }
  else
  {
   	_DEBUG(printf("error: unable to delete file '%s'", filename));
  }

  if (rename(out_filename.data(), filename) == 0)
  {
  	_DEBUG(printf("successfully renamed file from '%s' to '%s'.\n", out_filename.data(), filename));
  }
  else
  {
  	_DEBUG(printf("error: unable to rename file from '%s' to '%s'\n", out_filename.data(), filename));
  }

}

std::vector<em_key *> emprocessor::check_for_duplicates_ever()
{
  std::vector<em_key *> ret;

  if (all_keys.size() < 2)
  {
    return ret;
  }

  bool found = false;
  float percent = 0.0;
  for (std::vector<em_key *>::size_type i = 0; i < all_keys.size() - 1; i++)
  {
  	percent = (static_cast<float>(i)/(all_keys.size() - 1)) * 100.00;
  	printf("\rsearching: %.2f%%", percent);
  	fflush(stdout);
  	sleep(0.5);
    for (std::vector<em_key *>::size_type j = i+1; j < all_keys.size(); j++)
    {
      if (all_keys[i]->numbers_and_stars_match(*all_keys[j]))
      {
      	ret.push_back(all_keys[j]);
      	found = true;
      }
    }

    if (found)
    {
      ret.push_back(all_keys[i]);
    }

    found = false;
  }

  if (all_keys.size() > 0)
  {
  	percent = 100.00;
  	printf("\rsearching: %.2f%%", percent);
  }

  sort(ret.begin(), ret.end(), 
        [](const em_key *a, const em_key *b) -> bool
        { 
          return *a > *b;
        }
      );

  printf("\n");

  return ret;
}

short emprocessor::generate_random(short lower, short upper)
{
  return (rand() % (upper - lower + 1)) + lower;
}

em_key *emprocessor::generate_key()
{
  em_key *key = (em_key *)malloc(sizeof(em_key));

  short main_numbers[] = {-1, -1, -1, -1, -1};
  short stars[] = {-1, -1};

  std::set<short> set_numbers;
  std::set<short> set_stars;

  // Use current time as  
  // seed for random generator 
  srand(time(0));

  while (set_numbers.size() < 5)
  {
    set_numbers.insert(generate_random(1, 50));
  }

  while (set_stars.size() < 2)
  {
    set_stars.insert(generate_random(1, 12));
  }

  int i = 0;
  for (auto each : set_numbers)
  {
  	main_numbers[i++] = each;
  }

  i = 0;
  for (auto each : set_stars)
  {
    stars[i++] = each;
  }

  memcpy(key->main_numbers, main_numbers, sizeof(short)*5);
  memcpy(key->stars, stars, sizeof(short)*2);

  ensure_sorted(*key);

  return key;
}

em_key *emprocessor::generate_non_existent_key()
{
  constexpr short NUM_TRIES = 100;

  for (int i = 0; i < NUM_TRIES; ++i)
  {
  	em_key *key = generate_key();
  	if (key_exists(*key))
  	{
  	  printf("generated key: \"%s%u %s%u %s%u %s%u %s%u * %s%u %s%u\" exists.\n",
  	    (key->main_numbers[0]<10) ? " " : "",
        key->main_numbers[0],
        (key->main_numbers[1]<10) ? " " : "",
        key->main_numbers[1],
        (key->main_numbers[2]<10) ? " " : "",
        key->main_numbers[2],
        (key->main_numbers[3]<10) ? " " : "",
        key->main_numbers[3],
        (key->main_numbers[4]<10) ? " " : "",
        key->main_numbers[4],
        (key->stars[0]<10) ? " " : "",
        key->stars[0],
        (key->stars[1]<10) ? " " : "",
        key->stars[1]);
  	  free(key);
  	  key = nullptr;
  	}
  	else
  	{
  	  return key;
  	}
  }
  return nullptr;
}

emprocessor::~emprocessor()
{
  for (auto &value : all_keys)
  {
  	free(value);
  }
}