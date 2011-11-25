#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# This is just an identity reducer that prints out anything
# that it gets via STDIN.

ARGF.each do |line|
  puts line
end