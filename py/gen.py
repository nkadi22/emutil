import itertools

#len(list(itertools.combinations([item for item in range(1,50)], 5)))
#len(list(itertools.combinations([item for item in range(1,12)], 2)))

def all_keys():
  return list(itertools.combinations([item for item in range(1,50)], 5))

def all_even_odd(key):
  count_even = 0
  count_odd  = 0
  for key_elem in key:
  	if key_elem%2==0:
  	  count_even += 1
  	else:
  	  count_odd += 1

  return count_odd>=4 or count_even>=4


def all_low_high(key):
  count_low = 0
  count_high  = 0
  for key_elem in key:
  	if key_elem<26:
  	  count_low += 1
  	else:
  	  count_high += 1

  return count_low>=4 or count_high>=4


def three_or_more_cons(key):
  return  ( (key[0]+1==key[1] and key[1]+1==key[2]) or
            (key[1]+1==key[2] and key[2]+1==key[3]) or
            (key[2]+1==key[3] and key[3]+1==key[4])    )

def main():
  f = open("../ds/all_keys.ds", "w")
  keys = all_keys()
  print("%d combinations were computed.\n" % (len(keys)))

  #remove all even and all odds
  print("removing keys with only odds or only even numbers...\n")
  keys_without_all_odd_even = [x for x in keys if not all_even_odd(x)]
  print("%d keys were removed\n" % (len(keys)-len(keys_without_all_odd_even)))

  #remove all low and all high
  print("removing keys with only lows or only high numbers...\n")
  keys_without_all_low_high = [x for x in keys_without_all_odd_even if not all_low_high(x)]
  print("%d keys were removed\n" % (len(keys_without_all_odd_even)-len(keys_without_all_low_high)))

  #remove keys with 3 or more consecutive numbers
  print("removing keys with 3 or more consecutive numbers...\n")
  keys_with_3_or_more_cons = [x for x in keys_without_all_low_high if not three_or_more_cons(x)]
  print("%d keys were removed\n" % (len(keys_without_all_low_high)-len(keys_with_3_or_more_cons)))

  print("writing to file\n")
  for key in keys_with_3_or_more_cons:
    f.write("%s%d %s%d %s%d %s%d %s%d\n" %
              (
              	' ' if key[0]<10 else '',
              	key[0],
              	' ' if key[1]<10 else '',
              	key[1],
              	' ' if key[2]<10 else '',
              	key[2],
              	' ' if key[3]<10 else '',
              	key[3],
              	' ' if key[4]<10 else '',
              	key[4]
              )
           )

  f.close()
  print("%d combinations were written to file.\n" % (len(keys_with_3_or_more_cons)))

if __name__ == "__main__":
  main()