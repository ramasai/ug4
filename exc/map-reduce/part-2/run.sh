#/usr/bin/env sh

hadoop jar contrib/streaming/hadoop-0.20.2-streaming.jar \
              -input /user/s0824586/coursework/input-train \
              -output /user/s0824586/coursework/output-part2 \
              -mapper mapper.rb \
              -file mapper.rb \
              -reducer reducer.rb \
              -file reducer.rb \
              -jobconf mapred.job.name="s0824586 - Part 2"