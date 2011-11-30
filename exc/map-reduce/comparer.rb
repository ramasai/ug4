first_file = File.new(ARGV[0]).read.split("\n").reject { |r| r == "" }
second_file = File.new(ARGV[1]).read.split("\n").reject { |r| r == "" }

count = 0
good = 0
bad = 0

def same_word(a, b)
  same = a[0] == b[0]
  if a.length > 0
    same = same && a[1..-1].downcase.strip == b[1..-1].downcase.strip
  end
end

first_file.zip(second_file).each do |a, b|
  if same_word(a, b)
    good += 1
    puts "GOOD"
  else
    print "BAD - "
    puts "#{a} != #{b}"
    bad += 1
  end

  count += 1
end

puts "Good: #{(good.to_f/count)*100}"
puts "Bad: #{(bad.to_f/count)*100}"
