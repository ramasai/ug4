#/usr/bin/env sh

# Delete the output folder
hadoop fs -rmr /user/s0824586/coursework/output-part5

# Run the task
hadoop jar /opt/hadoop/hadoop-0.20.2/contrib/streaming/hadoop-0.20.2-streaming.jar \
              -jobconf mapred.job.name="s0824586 - Part 5" \
              -input /user/s0824586/coursework/output-part4-step-2 \
              -output /user/s0824586/coursework/output-part5 \
              -mapper mapper.rb \
              -file mapper.rb \
              -reducer cat
