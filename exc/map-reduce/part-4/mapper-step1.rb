#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# There are two different formats of data that will be coming into this mapper.
# 
# Feature Probabilities:
#   <feature> <lower probability> <upper probability>
#
# Word Features:
#   <word> <feature>
#

ARGF.each do |line|
  tokens = line.split "\t"
  
  case tokens.length
  when 2
    puts tokens.reverse.join "\t"
  when 3
    puts line
  end
end