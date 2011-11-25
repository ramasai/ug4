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
  ]
end

# Naive Bayes
#
# Loop through the words in the input and split them up into their features. We
# then emit a tuple with the feature, whether or not the word is uppercase or
# not, and the count. The count may just be 1 or we could cache some of these
# in the mapper and send them in blocks.
#
ARGF.each do |line|
  words = line.chomp.split

  words.each do |word|
    uppercase = word.match(/[A-Z]/) ? 1 : 0
    word_features = features(word)

    word_features.each do |feature|
      next if feature.nil?
      puts "#{feature}\t#{uppercase}\t1"
    end
  end
end
