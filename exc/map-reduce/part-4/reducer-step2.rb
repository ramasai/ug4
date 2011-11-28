#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

current_word = nil
lower_product = 1
upper_product = 1

ARGF.each do |line|
  word, feature, lower, upper = line.split
  current_word = word if current_word.nil?
  
  unless current_word == word
    is_upper = upper_product > lower_product
    puts "#{current_word}\t#{is_upper ? 1 : 0}"
    
    current_word = word
    upper_product = 1
    lower_product = 1
  end
  
  lower_product *= lower.to_f
  upper_product *= upper.to_f
end

is_upper = upper_product > lower_product
puts "#{current_word}\t#{is_upper ? 1 : 0}"