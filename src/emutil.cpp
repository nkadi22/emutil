#include "emutil.hpp"

/* Prints usage information for this program to STREAM (tipically 
   stdout or stderr), and exit the program with EXIT_CODE. Does not 
   return. */
void print_usage( char *program_name, FILE *stream, int exit_code )
{
	fprintf( stream, "USAGE: %s [-v] [-h] [-f file]\n", program_name );
	fprintf( stream,
		"  -v  --verbose       Display messages in a verbose fashion.\n"
		"  -h  --help          Display this usage information.\n"
		"  -f  --file          Provide the dataset filename.\n");
	exit( exit_code );
}

std::pair<char*, bool>
read_command_line_parameters( int argc, char **argv )
{
	int next_option;

	const char* const short_options = "vhf:";

	const struct option long_options[] = {
		{ "verbose",	0, 		NULL, 		'v' },
		{ "help",		0, 		NULL, 		'h' },
		{ "file", 		1, 		NULL, 		'f' },
		{ NULL, 		0,		NULL, 		0   } /* Required at end of array. */
	};

	/* Whether to display verbose messages. */
	bool verbose = false;

	/* Remember the name of the program, to incorporate in messages.
		The name is stored in argv[0] */
	char *program_name = argv[0];

	char *filename = nullptr;
	do
	{
		next_option = getopt_long( argc, argv, short_options, long_options, NULL );

		switch( next_option )
		{
			case 'h':	/* -h or --help */
				/* User has requested usage information. Print it to standard 
				   output, and exit with code zero (normal termination). */
				print_usage( program_name, stdout, 0 );
				break;

			case 'f':	/* -i or --instance */
				/* User is providing the file name. Read it */
				filename = (char *)malloc(sizeof(char)*strlen(optarg) + 1);
				strcpy(filename, optarg);
				break;

			case 'v':	/* -v or --verbose */
				verbose = true;
				break;

			case '?':	/* User specified an invalid option */
				/* Print usage information to standard error, and exit with exit
				   code one (indicating abnormal termination). */
				print_usage( program_name, stderr, 1 );
				break;

			case -1:	/* Done with options */
				break;

			default:	/* Something else: unexpected */
				abort();
		}
	} while ( next_option != -1 );

	return std::pair<char*, bool>(filename, verbose);
}


extern bool _verbose;

int main(int argc, char *argv[])
{
  auto options = read_command_line_parameters(argc, argv);

  //printf("The filename is '%s'. Verbose='%s'\n", options.first? options.first : "no_file_provided", options.second? "true" : "false");

  char *filename = options.first? options.first : const_cast<char *>(DEFAULT_INPUT_FILE);
  _verbose = options.second;

  sleep(1);

  if (options.first)
  {
  	_DEBUG(printf("using dataset file '%s'\n", filename));
  }
  else
  {
    _DEBUG(printf("using default dataset file\n"));	
  }

  char *file_path = nullptr;
  if ( filename[0] == '/' )
  {
  	/*user provided an absolute path*/
  	file_path = filename;
  }
  else if ( strstr(filename, "/") != nullptr )
  {
    if (options.first)
    {
      free(options.first);
    }
  	fprintf(stderr, "invalid data set file path, bye.\n");
  	return 2;
  }
  else
  {
  	/*user provided a relative path*/
  	size_t file_path_len;
  	file_path = (char *)malloc( file_path_len=(sizeof(char)*strlen(filename)+1) );
  	snprintf(file_path, file_path_len+14, "../../ds/%s", filename);
  }
  
  /*check whether the dataset file exists*/
  
  if ( access(file_path, F_OK) == -1 )
  {
    if (file_path)
    {
      free(file_path);
    }

  	if (options.first)
    {
      free(options.first);
    }

  	printf("file not found. bye.\n");
  	return 1;
  }

  /*Read file*/
  emprocessor facade(file_path);

  bool doexit = false;
  while(true && doexit==false)
  {
    /*display menu*/
    printf("**********Euro Millions utility***********\n");
    printf("1. Add new key\n");
    printf("2. Check for duplicates keys on dataset\n");
    printf("3. Generate new key\n");
    printf("4. Generate non-existent key\n");
    printf("5. Check if a key has ever lead to a jackpot\n");
    printf("6. Exit\n");
    printf("******************************************\n");

    int option;
    scanf("%d", &option);

    switch (option)
    {
      case 1:
      {
        em_key key;
        while ((getchar()) != '\n');
        char buffer[40];
        printf("key:\n");
        //scanf("%s", new_key_str);
        if (fgets(buffer, 40, stdin) != nullptr)
        {
          int count = sscanf(buffer, "%hd/%hd/%hd %hd %hd %hd %hd %hd %hd %hd",
            &key.draw_date.day, &key.draw_date.month, &key.draw_date.year,
            &key.main_numbers[0], &key.main_numbers[1], &key.main_numbers[2],
            &key.main_numbers[3], &key.main_numbers[4], &key.stars[0], &key.stars[1]);
          printf("count: %d\n", count);

          if (count != 10)
          {
          	fprintf(stderr, "error: wrong arguments\n");
          	continue;
          }

          if (!emprocessor::validate_key(key))
          {
          	fprintf(stderr, "error: wrong arguments\n");
          	continue;
          }

          facade.insert_key(key);
        }
        break;
      }
      case 2:
      {
        auto results = facade.check_for_duplicates_ever();
        if (results.size() == 0)
        {
          printf("no duplicates found\n");
        }
        else
        {
          for (const auto &each : results)
          {
            fprintf(stdout, "%s%u/%s%u/%s%u %s%u %s%u %s%u %s%u %s%u %s%u %s%u\n",
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
      	}
        break;
      }
      case 3:
      {
        em_key *key = facade.generate_key();
        fprintf(stdout, "%s%u %s%u %s%u %s%u %s%u * %s%u %s%u\n",
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
        break;
      }
      case 4:
      {
      	em_key *key = facade.generate_non_existent_key();
      	if (key)
      	{
      	  fprintf(stdout, "%s%u %s%u %s%u %s%u %s%u * %s%u %s%u\n",
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
      	}
      	else
      	{
      	  fprintf(stdout, "unable to find a non-existent key. try again :)\n" );
      	}
        break;
      }
      case 5:
      {
      	em_key key;
        while ((getchar()) != '\n');
        char buffer[40];
        printf("key(7 numbers space-separated):\n");
        //scanf("%s", new_key_str);
        if (fgets(buffer, 40, stdin) != nullptr)
        {
          int count = sscanf(buffer, "%hd %hd %hd %hd %hd %hd %hd",
            &key.main_numbers[0], &key.main_numbers[1], &key.main_numbers[2],
            &key.main_numbers[3], &key.main_numbers[4], &key.stars[0], &key.stars[1]);
          printf("count: %d\n", count);

          /*ficticious date, just to pass validation*/
          key.draw_date.day = 27;
          key.draw_date.month = 7;
          key.draw_date.year = 2020;

          if (count != 7)
          {
          	fprintf(stderr, "error: wrong arguments\n");
          	continue;
          }

          if (!emprocessor::validate_key(key))
          {
          	fprintf(stderr, "error: wrong arguments\n");
          	continue;
          }

          em_key *exist = facade.key_exists(key);

          fprintf(stdout, "key%s found.\n", exist? "" : " not"); 
          if (exist)
          {
          	fprintf(stdout, "%s%u/%s%u/%s%u %s%u %s%u %s%u %s%u %s%u %s%u %s%u\n",
              (exist->draw_date.day<10) ? "0" : "",
              exist->draw_date.day,
              (exist->draw_date.month<10) ? "0" : "",
              exist->draw_date.month,
              "",
              exist->draw_date.year,
              (exist->main_numbers[0]<10) ? " " : "",
              exist->main_numbers[0],
              (exist->main_numbers[1]<10) ? " " : "",
              exist->main_numbers[1],
              (exist->main_numbers[2]<10) ? " " : "",
              exist->main_numbers[2],
              (exist->main_numbers[3]<10) ? " " : "",
              exist->main_numbers[3],
              (exist->main_numbers[4]<10) ? " " : "",
              exist->main_numbers[4],
              (exist->stars[0]<10) ? " " : "",
              exist->stars[0],
              (exist->stars[1]<10) ? " " : "",
              exist->stars[1]);
          }
        }
        break;
      }
      case 6:
        doexit = true;
        break;
      default:
        break;
    }
  }

  fprintf(stdout, "exiting...\n");
  sleep(0.5);
  
  if (options.first)
  {
    //free(filename);
    free(options.first);
  }

  if (file_path)
  {
  	free(file_path);
  }

  fprintf(stdout, "bye.\n");
  
  return 0;
}
