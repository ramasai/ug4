#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# This reducer is responsible for creating tuples of the form:
#
#   <word> <feature> <upper probability> <lower probability>
#
# For further processing in the next stage.
#

current_feature = {
  :name => nil,
  :lower => 0,
  :upper => 0
}

saved_words = []

def dump_saved_words(saved_words, feature)
  return if feature[:lower].nil?
  
  saved_words.each do |word|
    puts "#{word}\t#{feature[:name]}\t#{feature[:lower]}\t#{feature[:upper]}"
  end
end

ARGF.each do |line|
  tokens = line.split
  
  if tokens.length == 2
    feature, word = tokens
        
    if feature != current_feature[:name]
      dump_saved_words(saved_words, current_feature)
      saved_words.clear
      current_feature = {
        :name => feature,
        :lower => nil,
        :upper => nil
      }
    end
    
    saved_words << word
  else
    feature, lower, upper = tokens
    
    if feature != current_feature[:name]
      dump_saved_words(saved_words, current_feature)
      saved_words.clear
    end
    
    current_feature = {
      :name => feature,
      :lower => lower,
      :upper => upper
    }
  end
end

dump_saved_words(saved_words, current_feature)