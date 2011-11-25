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

# Takes a file and returns a list of all of the words that are in the file.
def parse_file(path)
  File.new(path).read.split
end

# Show some help if the user gets it wrong.
unless ARGV.length == 2
  puts "Usage: #{$0} [training-file] [testing-file]"
  exit 1
end

# Create some lists of the training and testing words.
TRAINING_WORDS = parse_file(ARGV[0])
TESTING_WORDS = parse_file(ARGV[1])

# Create a hash with a default value that has empty counts.
words = Hash.new do |hash, key|
  hash[key] = Array.new(2, 0)
end

# Loop through each of the training words and increment the counts for each
# feature of the word.
TRAINING_WORDS.each do |word|
  uppercase = word.match(/[A-Z]/) ? 1 : 0
  word_features = features(word)

  word_features.each do |feature|
    words[feature][uppercase] += 1
  end
end

# Let's create a new hash to store the probabilities of the case given the
# feature of the word.
probabilities = Hash.new do |hash, key|
  hash[key] = Array.new(2, 0)
end

# And then we'll loop through our counts to create the model for using with
# Naive Bayes.
words.each_key do |key|
  sum = words[key].inject(:+)

  (0..1).each do |i|
    value = words[key][i].to_f/sum
    probabilities[key][i] = value
  end
end

# Okay, so we have our model. Lets get capitalising some words! This function
# takes a word and then words out if it should be upper or lower case. If it
# should be upper case then it returns 1 and if lower case then a 0. It also
# takes the model that it should base these predicions off.
#
def word_case(word, model)
  word_features = features(word)

  # The probabilities of each feature being lower or upper case.
  lower_prob = []
  upper_prob = []

  # Retrieve the probabilities.
  word_features.each do |feature|
    # If we've never seen this feature before in our model.
    if !model.has_key?(feature)
      lower_prob << 1
      upper_prob << 1
      next
    end

    lower_prob << model[feature][0]
    upper_prob << model[feature][1]
  end

  # Calculate the product of them.
  lower = lower_prob.inject(:*)
  upper = upper_prob.inject(:*)

  # Return the class of the larger one.
  lower > upper ? 0 : 1
end

# For each of the words in the test data we now classify it and print it out.
TESTING_WORDS.each do |word|
  # Should it be upper case?
  upper = word_case(word, probabilities) > 0

  # Capitalise the word if it should be uppercase
  word.capitalize! if upper

  # We're done! Output the word.
  puts word
end
