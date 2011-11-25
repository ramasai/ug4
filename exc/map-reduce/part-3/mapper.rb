#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# Returns the different features of the word.
#
# These are:
#
# * The word itself.
# * The first 2 letters of the word.
# * The first 3 letters of the word.
# * The last 2 letters of the word.
# * The last 3 letters of the word.
#
def features(word)
  [
    word,
    word[0..1],
    word[0..2],
    word[-2..-1],
    word[-3..-1]
  ].map do |w|
    w ||= ""
    w.downcase
  end
end

ARGF.each do |line|
  words = line.chomp.split

  words.each do |word|
    word_features = features(word)

    word_features.each do |feature|
      puts "#{word}\t#{feature}"
    end
  end
end
