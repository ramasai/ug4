#/usr/bin/env sh

# Delete the output folder
hadoop fs -rmr /user/s0824586/coursework/output-part4-step-1
hadoop fs -rmr /user/s0824586/coursework/output-part4-step-2

# Run the task
hadoop jar /opt/hadoop/hadoop-0.20.2/contrib/streaming/hadoop-0.20.2-streaming.jar \
              -jobconf mapred.job.name="s0824586 - Part 4 - Step 1" \
              -input /user/s0824586/coursework/output-part2 \
              -input /user/s0824586/coursework/output-part3 \
              -output /user/s0824586/coursework/output-part4-step-1 \
              -mapper mapper-step1.rb \
              -file mapper-step1.rb \
              -reducer reducer-step1.rb \
              -file reducer-step1.rb \

# Run the task
hadoop jar /opt/hadoop/hadoop-0.20.2/contrib/streaming/hadoop-0.20.2-streaming.jar \
              -jobconf mapred.job.name="s0824586 - Part 4 - Step 2" \
              -input /user/s0824586/coursework/output-part4-step-1 \
              -output /user/s0824586/coursework/output-part4-step-2 \
              -mapper cat \
              -reducer reducer-step2.rb \
              -file reducer-step2.rb \