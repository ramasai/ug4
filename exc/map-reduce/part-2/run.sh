#/usr/bin/env sh

hadoop jar contrib/streaming/hadoop-0.20.2-streaming.jar \
              -input $1 \
              -output $2 \
              -mapper mapper.rb \
              -file mapper.rb \
              -reducer reducer.rb \
              -file reducer.rb \
              -jobconf mapred.job.name="s0824586 - Part 2"