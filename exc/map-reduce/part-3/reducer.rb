#!/usr/bin/env ruby

# Make sure that Ruby doesn't try and `help` us by buffering the output at all.
STDOUT.sync = true

class String
  def chars
    chars = []
    0.upto(self.length-1) { |i| chars << self[i] }
    chars
  end
end

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

