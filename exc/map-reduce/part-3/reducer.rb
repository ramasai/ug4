#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# To stop the word's features being repeated we only allow a word to print 5
# features before refusing to print any more.

current_word = nil
current_features = []

ARGF.each do |line|
  word, feature = line.split
  current_word = word if word.nil?
    
  unless word == current_word
    current_features.uniq.each do |feature|
      puts "#{word}\t#{feature}"
    end
    
    current_word = word
    current_features.clear
  end
  
  current_features << feature
end

current_features.uniq.each do |feature|
  puts "#{current_word}\t#{feature}"
end