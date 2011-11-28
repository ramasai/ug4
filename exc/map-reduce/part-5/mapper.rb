#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

ARGF.each do |line|
  word, upper = line.split
  upper = (upper == 1)
  
  word.capitalize if upper
  
  puts word
end