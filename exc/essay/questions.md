# Assignment

The goal of this assignment is to write a 3000-word essay (roughly, 10
pages of text) on the infrastructure and uses of cloud computing. More
specifically, you are given a description of a computing problem. You
have to analyse this decription further, identify how it relates to the
issues touched upon during the lectures, and then propose a computing
infrastucture to address the problem at hand.

The proposed infrastructure can be in the form of an architectural
diagram, or a simple bulleted list of points. There are no requirements
set in stone, choose whichever form you think makes more sense for your
presentation style.

Also, this being an essay, means that there is no right or wrong answer.
Of course, some answers make more sense than others (for instance,
saying "I would buy 10,000 monkeys to write random code for me" is an
interesting experiment but does not make too much sense) but the basic
rule of thumb is that if you can argue about what you have written, in
the sense of exhibiting a clear way of thinking and providing supporting
arguments, then the result is perfectly acceptable.

## Context

Dropcycle is a new startup company that is active in the area of
cloud-based computation. In particular, they aim to be the equivalent of
Dropbox1 for compute cycles and infrastructure. Their vision is that
compute cycles and infrastructure should be commodity. Any user should
have access to as many resources as he or she needs (or is willing to
pay for) in order to get their job done.

What they want to do is allow user programs to seamlessly execute on
their cloud when they run out of local resources. They have devised
a simple service that detects when a user-level application is nearing
the point of exhausting local resources, by they computation resources,
memory, disk space, or what not. Once such an event is detected, the
local process should migrate to the Dropcycle cloud and to some
processing node of the cloud that is capable of satisfying its
requirements. Once in the Dropcycle cloud, this process should continue:
resources should be added to the process (e.g., by migrating to
a machine with a larger memory pool, or by allowing it access to more
disk drives and thus more secondary storage). There are, of course, some
hard limits that a user's process can never reach; let us assume for now
that Dropcycle have a way to detect such (potentially malicious) cases
and will kill those processes. Dropcycle will provide infrastructure
similarly to other companies in the area. That is, users will register
for the service and depending on how much they are willing to pay, they
will receive an upper bound for the computation and/or resources they
can consume.

Dropcycle have the following rough set of specifications/requirements
for the service they aim to provide:

* Applications and/or data that is dropcycle'd (that's the term they are
  using) should be placed on some appropriate location on the user's
  machine. That location (typically a locally mounted directory) is
  monitored by Dropcycle. Once a process is started from that directory,
  it is continuously monitored for resource usage. Once it reaches
  certain limits, it migrates to the Dropcycle cloud.

* To keep maintenance costs low, all Dropcycle nodes in the company's
  cloud run the same version of Linux.

* Dropcycle is platform-independent. They should not care about the user
  operating system; regardless of whether the user uses Windows, or
  Linux, or Mac OS X, his or her process should be able to be executed
  on the Dropcycle cloud.

* All aspects of a process should be monitored. Apart from the typical
  resource monitoring, other aspects include I/O access patterns, number
  of parallel threads being executed, and so on. Dropcycle will use all
  this information when deciding what infrastructure they should migrate
  the process to.

* When in the Dropcycle cloud, a process should still have access to
  local resources (e.g., dropcycle'd files).

* The user should be able to manage the process as if it were local; he
  or she should be able to communicate with it for user input, or stop
  it if it takes too long, or retrieve output data as if they were local
  even though they reside on the Dropcycle cloud.

* The user should be able to set per-process limits that are different
  than his or her paid-for limits. For instance, say the user has paid
  for at most 64GB of main memory; he should be able to set a lower
  limit on a specific dropcycle'd process, say 4GB. That way, he or she
  is able to run 16 concurrent processes, each not exceeding its 4GB
  budget. It should also be possible to dynamically alter those limits.

Being impressed with you having taken Extreme Computing and your
excellent mark in the course, they have decided to put you in charge of
infrastructure. What they want you to do is come up with a proposal for
the architecture and technology of the underlying system they will build
and use. Specifically, the investors have given the company an
infrastructure budget to be spent over the next two years in any way the
company sees fit -- at which point the investors in the startup will
evaluate whether Dropcycle will receive further funding, or they will
pull the plug. You responsibility in Dropcycle is to make the best use
of this budget -- in addition to providing the overarching system design
and identifying where the technical effort should be spent.

## Questions to think about:

What sort of technology does Dropcycle need? What capabilities does that
technology have and what parts of the envisioned functionality can it
provide?

Given that they are operating under a budget, should they build their
own private cloud, or rent it from one of the existing cloud providers?
You can make assumptions about the budget (say, a total of B million
pounds to be spent over two years) and how much renting a cloud costs
(e.g., x pounds/GB/day, or along those lines) or whatever else you deem
important in your computations. Are some of the goals unreachable? If
so, which goals should be dropped from the specification and why? How
far can they go in automating the provisioning of resources? Will they
need dedicated personnel to do that, or is there a clear way to do this
with the chosen infrastructure? Is a completely self-tunable system
utopic? Structure Below is a potential (and rough) structure for your
essay. You do not need to follow it, but it might be helpful in terms of
organising your thoughts:

* Introduction
* Design considerations (what are the salient aspects of the problem,
  and what technology currently exists to support them?)
* Alternatives (including advantages and disadvantages of each)
* Proposed solution (including arguments of why you think it best fits
  the problem at hand)
* Conclusion

## Marking guidelines

There is a total of 100 marks available. The marks will be awarded as
follows:

* 20 marks for writing quality (so please proofread your essay before
  submission);

* 30 marks for commenting on the use cases and identifying the common
  ptterns that lead you to your proposal; note that you need to focus
  not only on what is already possible, but also on what needs to be
  done and what should be dropped as a goal;

* 20 marks for argumentation;

* 20 marks for the proposed solution and how well it aligns to the
  presented arguments;

* 10 marks discrectionary (e.g., general style of writing, exhibiting
  off-the-beaten-track thinking, etc.). Submission The submission is
  electronic only and the deadline is Wednesday, 2 November, 4:00 pm.

You will only need to submit a single PDF file of your essay (note: no
word documents, or open-document format files). Use the submit program
to make the submission. Name your file exc-essay.pdf; you can submit it
from the command line as:

    submit exc 1 exc-essay.pdf

That is all. For any questions, contact me.
