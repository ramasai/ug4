#!/usr/bin/env ruby
TRAINING_FILE = "../data/small.txt"
TRAINING_WORDS = File.new(TRAINING_FILE).read.scan(/\w+/).reject { |w| w.match /\d/ }

class String
  def uppercase?
    self[0].match /[A-Z]/
  end
  
  def features
    [self.downcase, self[1..-1].downcase]
  end
end

class BayesClassifier
  attr_reader :words, :counts
  
  def initialize
    @words = Hash.new
    @counts = Hash.new(0)
  end
  
  def add(object)
    klass = object.uppercase? ? :upper : :lower
    
    object.features.each do |feature|
      if @words[feature].nil?
        @words[feature] = Hash.new(0)
      end
      
      @words[feature][klass] += 1
      @counts[klass] += 1
      @counts[:total] += 1
    end
  end
  
  def size
    @words.size
  end
  
  def prior(klass)
    @counts[klass].to_f/@counts[:total]
  end
  
  def class_for?(word)
    features = word.features  
    
    upper = 1
    words.keys.each do |word|
      x = words[word][:upper].to_f/counts[:upper]
      
      next if x == 0
      
      if features.include?(word)
        upper *= x
      else
        upper *= (1-x)
      end
    end
    
    lower = 1
    words.keys.each do |word|
      x = words[word][:lower].to_f/counts[:lower]
      
      next if x == 0
      
      if features.include?(word)
        lower *= x
      else
        lower *= (1-x)
      end
    end
    
    is_upper = (upper * prior(:upper)) / ((upper * prior(:upper)) + (lower * prior(:lower))) > 0.5
    
    if is_upper
      :upper
    else
      :lower
    end
  end
end

bayes = BayesClassifier.new
TRAINING_WORDS.each { |word| bayes.add(word) }

def capitalise(bayes, word)
  if bayes.class_for?(word).eql?(:upper)
    word.capitalize
  else
    word.downcase
  end
end

ARGF.each do |line|
  puts capitalise(bayes, line)
end