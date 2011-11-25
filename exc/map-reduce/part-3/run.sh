#/usr/bin/env sh

hadoop jar /opt/hadoop/hadoop-0.20.2/contrib/streaming/hadoop-0.20.2-streaming.jar \
              -jobconf mapred.job.name="s0824586 - Part 3" \
              -input /user/s0824586/coursework/input-test \
              -output /user/s0824586/coursework/output-part3 \
              -mapper mapper.rb \
              -file mapper.rb \
              -reducer reducer.rb \
              -file reducer.rb \