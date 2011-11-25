#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# To stop the word's features being repeated we only allow a word to print 5
# features before refusing to print any more.

used_lines = []

ARGF.each do |line|  
  next if used_lines.include? line
  puts line
  used_lines << line
end
