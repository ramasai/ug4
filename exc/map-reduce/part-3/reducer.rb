#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# To stop the word's features being repeated we only allow a word to print 5
# features before refusing to print any more.

current_word = nil
feature_count = 0

ARGF.each do |line|
  word, feature = line.split

  if current_word.nil? || current_word != word
    current_word = word
    feature_count = 0
  end

  if feature_count < 5
    puts line
  end

  feature_count += 1
end
