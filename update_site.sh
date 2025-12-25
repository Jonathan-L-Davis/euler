#!/usr/bin/bash

file=$(ls -l primes/ | tail -n 1 | sed -e 's/.* //')
num=$(echo $file | sed -e 's/primes_//' -e 's/.bin//')

prime=$((16#$( tail -c $num "primes/$file" | xxd -g $num -c $num | tail -n 1 | sed -e 's/.*: //' -e 's/ .*//') ))

i=$((1));
num_primes=$((0));

for size in $(ls -l primes/ | tail -n +2 | sed -e 's/^[-rwx]* [0-9] [a-zA-Z0-9]* [a-zA-Z0-9]* [ ]*//' -e 's/ .*//'); do
    num_primes=$(($num_primes+$(($size/$i))));
    i=$(($i+1));
done

mkdir tmp

prime=$(printf "%'.3f\n" $prime | sed -e 's/\..*//')
num_primes=$(printf "%'.3f\n" $num_primes | sed -e 's/\..*//')

sed home.html -e "s/\${biggest_prime}/${prime}/" > tmp/home.html
sed tmp/home.html -e "s/\${num_primes}/${num_primes}/" > /etc/site/home.html

rm -rf tmp
