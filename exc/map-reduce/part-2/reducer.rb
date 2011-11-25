#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# Create a hash with a default value that has empty counts.
words = Hash.new do |hash, key|
  hash[key] = Array.new(2, 0)
end

# Naive Bayes
#
# Loop through each tuple in the input and keep a count of how many times it
# has appeared in upper and lower case words.
#
ARGF.each do |line|
  feature, uppercase, count = line.chomp.split

  # Change the strings to an integers.
  uppercase = uppercase.to_i
  count = count.to_i

  words[feature][uppercase] += count
end

# Output the probabilities of the words being upper or lower case in the
# format:
#
# <feature><tab><lower probability><tab><upper probability>
#
words.each_key do |key|
  sum = words[key][0] + words[key][1]
  lower = words[key][0].to_f/sum
  upper = words[key][1].to_f/sum

  puts "#{key}\t#{lower}\t#{upper}"
end
