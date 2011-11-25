first_file = File.new(ARGV[0]).read.split("\n")
second_file = File.new(ARGV[1]).read.split("\n")

count = 0
good = 0
bad = 0

first_file.zip(second_file).each do |a, b|
  same_word = a[0] == b[0]
  if a.length > 0
    same_word = same_word && a[1..-1].downcase == b[1..-1].downcase
  end
  if same_word
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