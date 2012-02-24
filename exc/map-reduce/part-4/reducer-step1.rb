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

# Output the current list of saved words to STDOUT.
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
        
    # If this is a new feature then we should reset the state of the
    # script and output all of the saved words.
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
    
    # If this is a new feature then we should reset the state of the
    # script and output all of the saved words.
    if feature != current_feature[:name]
      dump_saved_words(saved_words, current_feature)
      saved_words.clear
    end
    
    # Set the current feature to the new one.
    current_feature = {
      :name => feature,
      :lower => lower,
      :upper => upper
    }
  end
end

# Make sure that the last word in the file is also outputted.
dump_saved_words(saved_words, current_feature)