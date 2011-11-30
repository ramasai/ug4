#/usr/bin/env sh

hadoop fs -rmr /user/s0824586/coursework/output-part2

hadoop jar /opt/hadoop/hadoop-0.20.2/contrib/streaming/hadoop-0.20.2-streaming.jar \
              -jobconf mapred.job.name="s0824586 - Part 2" \
              -input /user/s0824586/coursework/input-train-small \
              -output /user/s0824586/coursework/output-part2 \
              -mapper mapper.rb \
              -file mapper.rb \
              -reducer reducer.rb \
              -file reducer.rb \
              
