#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# Really simple script that takes a tuple and uppercases the first
# element if the second element is 1.
ARGF.each do |line|
  word, upper = line.split
  upper = (upper == "1")
  
  word.capitalize! if upper
  
  puts word
end