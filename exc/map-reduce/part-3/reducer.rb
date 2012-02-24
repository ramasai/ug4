#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

# Simple bloom filter implementation
#
# The hashing is dreadful and wrong but it works well enough.
class BloomFilter
  def initialize(size)
    @size = size
    @buffer = Array.new(size, false)
  end
  
  def insert(element)
    hash(element).each { |i| @buffer[i] = true}
  end
  
  def maybe_include?(element)
    hash(element).map { |i| @buffer[i] }.inject(:&)
  end
  
  private :hash
  def hash(element)
    letters = element.chars.map { |e| ?e }
    
    [
      letters.inject(:+),
      letters.inject(:*)
    ].map { |h| h % @size }
  end
end

bloom_filter = BloomFilter.new(2000)
used_words = []

# We want to make sure that the features of a word are only outputted once
# even though there may be multiple occurences of the word in the text.
#
# We use a bloom filter to check if we have seen the line before. This prevents
# the O(n) check of the inclusion of the line in an used_words array. Since
# a bloom filter is not 100% accurate we must check the array if the bloom filter
# thinks we have seen the line before.
ARGF.each do |line|
  if !bloom_filter.maybe_include?(line)
    puts line
    bloom_filter.insert(line)
    used_words << line
  else
    unless used_words.include? line
      puts line
      used_words << line
    end
  end
end
